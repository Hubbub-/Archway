/* 
  Arco
  by Hubbub
  (Reuben Poharama, Braeden Foster and Martin Hill)
  
  1. Measure light cast on array of LDRs (light dependant resistors)
  2. Infer movement of shadows
  3. Use placement and movement to control LED strips


  Makes use of the EEPROM and FastLED library
*/



#include <EEPROM.h>
#include "FastLED.h"



#define NUMLDRS 14

// Define pin and pixel number for LEDs
#define PIXELPIN1      3
#define PIXELPIN2      4
#define NUMLEDS        300   //Number of LEDs per strip
#define NUMPIXELS      600   //Total number of LEDs (both strips)
#define NUMBLOCKS       9    //number of blocks
#define MAXVEL         30
#define FADESPEED      0.14
#define INITBLOCKSIZE   25


#define IDLEDELAY      60000   // milliseconds to wait before going idle
#define WHITEBRIGHT    200     // how bright the background white is /255


// pins for shift register                         _________
#define data 12           // pin 14 on chip1    Qb |1  U  16| V
#define shiftClear 7      // pin 10 on chips    Qc |2     15| Qa
#define shiftClock 13     // pin 11 on chips    Qd |3     14| SER (serial)
#define shiftLive 8       // pin 12 on chips    Qe |4     13| OE (Output Enable - Active Ground)
                          //                    Qf |5     12| RCLK (Register Clock)
                          //                    Qg |6     11| SRCLK (Serial Clock)
                          //                    Qh |7     10| SRCLR (Clear - Active Ground)
                          //                   GND |8      9| Qh' (Serial to next chip)

// pin to read the LDRs
#define LDRpin 14

// Define eeprom addresses
#define brightAddress 9
#define printLDRAddr 12
#define printTrigAddr 14
#define printBlocksAddr 16
#define printMovesAddr 18
#define printStripsAddr 20


int blockType[NUMPIXELS]; // 0:Nothing 1:Exploded 2:Not Exploded

CRGB pixels1[NUMLEDS];
CRGB pixels2[NUMLEDS];
byte numBlends[NUMPIXELS];

//initial block colours
int pColour[NUMBLOCKS][3] = {
  {0,255,255},
  {0,255,255},
  {0,255,255},
  {0,255,255},
  {0,255,255},
  {0,255,255},
  {0,255,255},
  {0,255,255}
};


float LDR[NUMLDRS];
float prevLDR[NUMLDRS];
float heldVal[NUMLDRS];
unsigned long heldSince;
float blockPos[NUMBLOCKS];
int lifeTime [NUMBLOCKS];   // how long it's been alive
unsigned long lifeStart [NUMBLOCKS];  // time that the life started
bool alive[NUMBLOCKS];               // is it alive


float saturation[NUMBLOCKS];
float hue[NUMBLOCKS];
float brightness[NUMBLOCKS];
bool exploding[NUMBLOCKS];
float blockSize[NUMBLOCKS];
float pulseSpeed[NUMBLOCKS];


//maximum and minimum brightness to use for scaling
int initMaxBright = 300;
int initMinBright = 100;
int maxBright[NUMLDRS];
int minBright[NUMLDRS];

//saves the last time each LDR was triggered
unsigned long trigTime [NUMLDRS];


//velocity of the expanding block
float vel[NUMBLOCKS];
float targetVel[NUMBLOCKS];


//Mapping when spawning new block
int LEDSpawnNum[] = {16,58,107,150,193,236,279,318,360,403,442,491,540,584};


//whether or not to print information
bool printLDRs;
bool printTrigs;
bool printBlocks;
bool printMoves;
bool printStrips;
byte bright;  //how bright the FastLEDs are

String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete


// PIR sensors
bool movement = false;
int PIRpin[] = {1,2,5,6};
bool PIRbool[4];

// Idle state
bool idle = false;
bool forceIdle = false;
byte idleHue = 170;
unsigned long lastActed = 0;  // The last time something happened
unsigned long idleStart;      // When the idle state started
unsigned long idleEnd;        // When the idle state ended


//---------------------------start of setup---------------------------------

void setup() {
  // Read EEPROM
  bright = EEPROM.read(brightAddress);
  if(bright<20) bright = 20;           // Limit the brightness between 0 and 255
  if(bright>255) bright = 255;
  // Set print bools from eeprom
  printLDRs = EEPROM.read(printLDRAddr);
  printTrigs = EEPROM.read(printTrigAddr);
  printBlocks = EEPROM.read(printBlocksAddr);
  printMoves = EEPROM.read(printMovesAddr);
  printStrips = EEPROM.read(printStripsAddr);


  // Start Serial
  Serial.begin(9600);

  // Setup pins
  for(int i=0; i<4; i++){      // PIR pins
    pinMode(PIRpin[i], INPUT);
  }
  // Shift register pins
  pinMode(shiftClear, OUTPUT);
  pinMode(shiftClock, OUTPUT);
  pinMode(shiftLive, OUTPUT);
  pinMode(data, OUTPUT);
  pinMode(LDRpin, INPUT);
  digitalWrite(shiftClear, HIGH);


  // start fast leds and make them fade to white
  FastLED.addLeds<WS2812,PIXELPIN1,GRB>(pixels1, NUMLEDS);
  FastLED.addLeds<WS2812,PIXELPIN2,GRB>(pixels2, NUMLEDS);
  for(int i=0; i<NUMLEDS; i++){
    int s = map(i,0,NUMLEDS,0,255);
    pixels1[i] = CHSV(s,255,bright);
    pixels2[i] = CHSV(s,255,bright);
    FastLED.show();
    delay(1);
  }
  

  

  // Set initial max and min brightness for each LDR
  for (int i=0; i<NUMLDRS; i++){
    maxBright[i] = initMaxBright;
    minBright[i] = initMinBright;
  }

  for (int i=0; i<NUMBLOCKS; i++){
    pulseSpeed[i] = random(80,130)/1000.00;
  }
  

  FastLED.setBrightness(bright);

  Serial.println("Setup done, send 'i' for info");
}




//---------------------------start of loop---------------------------------

void loop() {
  // reset blends
  for(int i=0; i<NUMPIXELS; i++){
    numBlends[i] = 0;
  }
  
  LDRs();  // Check LDR values
  blocksUpdate();  //update blocks


  if(idle){
    //check if it should still be idle
    if(millis() < lastActed + IDLEDELAY && !forceIdle){
      idle = false;
      idleEnd = millis();
    }
    idlePulse(); //pulse
    // Check for movement
    checkMovement();
  }



  // If it's not idle
  else{
    //check if it should be idle
    if(millis() > lastActed + IDLEDELAY || forceIdle){
      idle = true;
      idleStart = millis();
      idleHue = random(0,255);
    }
    blocksRender();
  
  }  // End of "else" (not idle)
  FastLED.show(); // This sends the updated pixel color to the hardware.
}







//--------------------------------end of loop---------------------------------




void trig(int LDR) {

  unsigned long t = millis();
  trigTime[LDR] = t;  // set the initial trigger time
  lastActed = t;
  
  if(printTrigs){
    Serial.print("Triggered LDR#");
    Serial.print(LDR);
  }

  if(blockAt(LDR) < 0){    // If there's no block at the LDR
    initBlock(LDR);             // make a new block
    if(printTrigs){
      Serial.print(" | made block at ");
      Serial.print(LDR);
    }
    int target = blockAt(LDR);    // select the block
    // Explode it
    explode(target);
  }
  
  else{       // if there is a block at the LDR
    int target = blockAt(LDR);    // select the block

    // Explode it
    if(lifeTime[target] > 500 && !exploding[target]){       // only if it's old enough
      explode(target);
    }
  }


  if(printTrigs)Serial.println(" ");


}



//------------------------------------------------------------------------

void explode(int target){
  targetVel[target] = MAXVEL;     // change the target velocity
  exploding[target] = true;       // make explode
  saturation[target] = 255;       // Full saturation
  brightness[target] = 255;
  lifeStart[target] = millis();   // Restart life clock
  if(printTrigs){
    Serial.print(" | targetVel of[");
    Serial.print(target);
    Serial.print("] now ");
    Serial.print(targetVel[target]);
  }
}



//--------------------------------------------------------------------------




// Check for movement
void checkMovement(){
  bool newMovement = false;
  
  // store movement
  for(int i=0; i<4; i++){
    bool pinHigh = digitalRead(PIRpin[i]);

    // if there is movement but it's not stored
    if (pinHigh && !PIRbool[i]){
      PIRbool[i] = true;
      newMovement = true;
    }
    // if there's no movement but it's stored
    if(!pinHigh && PIRbool[i]){
      PIRbool[i] = false;
    }
  }


  if(newMovement){
//    movement = newMovement;  
    lastActed = millis();
    if(printMoves){
      Serial.print("movement: ");
      for(int i=0; i<4; i++){
        Serial.print(i);
        Serial.print(": ");
        Serial.print(PIRbool[i]);
        Serial.print(" | ");
      }
      Serial.println(" ");
    }
  }
}




//--------------------------------------------------------------------------




// Idle pulse
void idlePulse(){
  unsigned long pulseTime = millis()-idleStart;
  int pulseTimer = int(pulseTime*0.08) % 255;
  int brightness = map(quadwave8(pulseTimer),0,255,150,255);
  for(int i=0; i<NUMLEDS; i++){
    pixels1[i] = CHSV(idleHue,brightness,brightness);
    pixels2[i] = CHSV(idleHue,brightness,brightness);
  }
  FastLED.show();
}


