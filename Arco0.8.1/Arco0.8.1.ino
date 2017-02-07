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
#define NUMBLOCKS       8    //number of blocks
#define MAXVEL         30
#define FADESPEED      0.14
#define INITBLOCKSIZE   25


#define IDLEDELAY      60000   // milliseconds to wait before going idle


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
unsigned int heldSince;
float blockPos[NUMBLOCKS];
int lifeTime [NUMBLOCKS];   // how long it's been alive
unsigned long lifeStart [NUMBLOCKS];  // time that the life started
bool alive[NUMBLOCKS];               // is it alive
bool front[NUMBLOCKS];       // is it in front

float saturation[NUMBLOCKS];
float hue[NUMBLOCKS];
float brightness[NUMBLOCKS];
bool exploding[NUMBLOCKS];
float blockSize[NUMBLOCKS];


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

// Idle state
bool idle = true;
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
//    float b = i*0.01*bright;
//    for(int j=0; j<NUMLEDS; j++){
//      pixels1[j] = CHSV(0,0,b);
//      pixels2[j] = CHSV(0,0,b);
//    }
    FastLED.show();
    delay(1);
  }
  

  

  // Set initial max and min brightness for each LDR
  for (int i=0; i<NUMLDRS; i++){
    maxBright[i] = initMaxBright;
    minBright[i] = initMinBright;
  }

  

  FastLED.setBrightness(bright);

  Serial.println("Setup done, send 'i' for info");
}




//---------------------------start of loop---------------------------------

void loop() {
  
  LDRs();  // Check LDR values
  blocks();  //update blocks


  if(idle){
    //check if it should still be idle
    if(millis() < lastActed + IDLEDELAY){
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
    if(millis() > lastActed + IDLEDELAY){
      idle = true;
      idleStart = millis();
    }

    
    if(printBlocks) Serial.println(" ");
    
    // reset pixel types
    if(printStrips){
      for(int i=0; i<NUMPIXELS; i++){
        blockType[i] = 0;
      }
    }
  

    // Reset Pixel Colour
    for(int j=0; j<NUMLEDS; j++){
      pixels1[j] = CHSV(0,0,0);
      pixels2[j] = CHSV(0,0,0);
    }
    // apply colour to the blocks
    for(int i=0; i<NUMBLOCKS; i++){ 
      for(int j=-blockSize[i]/2; j<blockSize[i]/2; j++){
        
        int pos = blockPos[i]+j;
        applyColour(pos, hue[i], saturation[i], brightness[i]);
        
        // Apply pixel types for serial printing
        // If in range
        if (pos>=0 && pos<NUMPIXELS && alive[i]){
          //Led:nothing + Lead:Not exploding
          if(blockType[pos]==0 && !exploding[i]) blockType[pos] = 1;
          //Led:nothing + Lead:Exploding
          else if(blockType[pos]==0 && exploding[i]) blockType[pos] = 2;
          //Led:Not Exploding + Lead:Exploding
          else if(blockType[pos]==1 && exploding[i]) blockType[pos] = 3;
          //Led:Exploding + Lead:Not Exploding
          else if(blockType[pos]==2 && !exploding[i]) blockType[pos] = 3;
        }
      }
    }

    
  
  
    if(printStrips){
      for(int i=0; i<NUMPIXELS-4; i+=4){
        if (blockType[i] == 0) Serial.print("_");
        else if (blockType[i] == 1) Serial.print("-");
        else if (blockType[i] == 2) Serial.print("|");
        else if (blockType[i] == 3) Serial.print("+");
      }
      Serial.println(" ");
    }
  
  }  // End of "else" (not idle)
  FastLED.show(); // This sends the updated pixel color to the hardware.
}







//--------------------------------end of loop---------------------------------


// blocks
void blocks(){
  for (int i=0; i<NUMBLOCKS; i++){     // cycle through blocks
    
    // -- TIMING --
    if (alive[i]){
      lifeTime[i] = millis() - lifeStart[i];    // update life time
      
      
      if(!exploding[i]){                      // if the block is not exploding, pulse
        int pulseTimer = int(millis()*0.1) % 255;
        saturation[i] = map(quadwave8(pulseTimer),0,255,50,255);
        if(brightness[i]<250){
          brightness[i] +=5;
        }
        
      }

      else if (exploding[i]){   // if the block is exploding
        if(blockSize[i] > 300){  
          vel[i] = 0;            // limit size
        }
        blockSize[i] += vel[i]; // increase size
      }

      if(lifeTime[i] > 5000){           // if the block is old
        brightness[i] -= 1;          // fade out
        if(brightness[i] <= 0){  
          alive[i] = false;             // kill it off when faded
          initBlock(random(0,NUMLDRS));  // initiate another block somewhere
          Serial.println("Pixel Died");
        }
      }
    }


    // Update Velocities
    //if the absolute velocity is slower than the absolute target, change quickly to the target
    if(abs(vel[i])<abs(targetVel[i]) && targetVel[i]!=0){
      vel[i]=fade(vel[i],targetVel[i],targetVel[i]*0.06);
    }
    //if the absolute velocity is faster than absolute target, change slowly to the target
    //and set the target to 0 if it isn't already
    else if (abs(vel[i])>=abs(targetVel[i])){
      vel[i]=fade(vel[i],targetVel[i],vel[i]*0.05);
      if(targetVel[i]!=0) targetVel[i]=0;
    }
    //limit the velocity
    if(targetVel[i]>MAXVEL) targetVel[i]=MAXVEL;
    else if(targetVel[i]<-MAXVEL) targetVel[i]=-MAXVEL;
  

    

    if(printBlocks){
      Serial.print("leadPos ");
      Serial.print(i);
      Serial.print(":");
      Serial.print(blockPos[i]);
      if(exploding[i]) Serial.print(" Exploded!");
      Serial.print(" | ");
    }
  }
  if(printBlocks) Serial.println(" ");
}

// Read LDRs
void LDRs() {
  
// Read the LDR values using the shift Register
  shiftReg();  //function after loop
  heldSince ++;  //increase the "heldSince" value
  
  if(heldSince > 5){  //only check LDR values every 5 loops
    heldSince = 0;
  }
  

  for(int i=0; i<NUMLDRS; i++){   //cycle through LDRs
    // change the min and max values
    int difference = abs(maxBright[i]-minBright[i]);
    int middle = minBright[i]+difference/2;
    
    if (heldSince == 0) {                   // if "heldsince" is greater than 
      if (abs(heldVal[i]-LDR[i]) > middle*0.12+10) {  // if there's a change in reading and the pixel isn't alive
        trig(i);                  // trigger the pixel spawn/explode
      }

      heldVal[i] = LDR[i];  // store the value (for comparison next time)
    }

    
    // bring max and min brightness closer together if far apart
    if(difference > middle*0.1){
      maxBright[i] -= 0.001;    
      minBright[i] += 0.001;
    }
    // set new max and min brightness
    if(LDR[i] > maxBright[i]) maxBright[i] = LDR[i];
    if(LDR[i] < minBright[i]) minBright[i] = LDR[i];

    // print the LDR values if wanted
    if(printLDRs){
      Serial.print(i);
      Serial.print(": ");
      Serial.print(LDR[i]);
      Serial.print(" | ");  
    }
  }
  if(printLDRs) Serial.println(" ");
}

// initiate a block
void initBlock(int pos){
  // Look through array of blocks for any that are dead
  int index = 0;
  while(alive[index]){  // Only look until a dead one is found
    index++;
    if(index >= NUMBLOCKS){        // If there are no dead ones...
      index = random(0,NUMBLOCKS); // choose one at random
      break;
    }
  }
  // Replace selected block
  blockPos[index] = LEDSpawnNum[pos];  // put in correct place
  alive[index] = true;                     // make alive
  lifeStart[index] = millis();             // give lifeStart time
  hue[index] = random(0,255);              // a random colour
  saturation[index] = 0;                   // start with no saturation (to fade in)
  brightness[index] = 0;                   // start with no brightness (to fade in)
  blockSize[index] = INITBLOCKSIZE;          // the initial size of the block
  exploding[index] = false;                // make sure it's not exploding to start with
  front[index] = false;                    // It's already in front of the exploeded ones

}



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
  }
  
  else{       // if there is a block at the LDR
    int target = blockAt(LDR);    // select the block

    // Explode it
    if(lifeTime[target] > 500){       // only if it's old enough
      targetVel[target] = MAXVEL;     // change the target velocity
      exploding[target] = true;       // make explode
      saturation[target] = 255;       // Full saturation
      lifeStart[target] = millis();   // Restart life clock
      putInFront(target);             // put it in front
      if(printTrigs){
        Serial.print(" | targetVel of[");
        Serial.print(target);
        Serial.print("] now ");
        Serial.print(targetVel[target]);
        Serial.print(" front=");
        Serial.print(front[target]);
      }
    }
  }


  if(printTrigs)Serial.println(" ");


}



//------------------------------------------------------------------------





// LDR Shift Register stuff
void shiftReg() {
  for(int i = 0; i<NUMLDRS; i++){       // Cycle through the LDRs
    if(i==0) digitalWrite(data, HIGH);  // Start the cycle with a high bit
    else digitalWrite(data, LOW);       // Otherwise a low bit
    digitalWrite(shiftClock, HIGH);     // Add the bit to the SR memory
    digitalWrite(shiftClock, LOW);
    digitalWrite(shiftLive, HIGH);      // Makes the SR "Live"
    digitalWrite(shiftLive, LOW);
    delay(1);
    // Read the resistance of the live LDR
//    if(i==0) LDR[NUMLDRS-1] = analogRead(LDRpin);  
//    else LDR[i-1] = analogRead(LDRpin);
    LDR[i] = analogRead(LDRpin);
  }
}



//--------------------------------------------------------------------------


// Apply pixel colours to the right strip
void applyColour(float pixelin, float h, float s, float v){
  int pixel = (int) pixelin;
  
  // strip1
  if(pixel%2 == 0){
    int p = pixel/2;
    pixels1[p] += CHSV(h,s,v);
  }
  
  // strip2 
  if(pixel%2 == 1){
    int p = (pixel-1)/2;
    pixels1[p] += CHSV(h,s,v);
  }
}


//--------------------------------------------------------------------------




// Check for movement
void checkMovement(){
  bool newMovement = false;
  bool PIRbool[4];
  for(int i=0; i<4; i++){
    if (digitalRead(PIRpin[i]) == HIGH){
      PIRbool[i] = true;
      newMovement = true;
    }
    else PIRbool[i] = false;
  }
  if(newMovement != movement){
    movement = newMovement;  
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

// Find the block near a certain LDR
int blockAt(int LDR){
  int selectedPixel = -1;         // -1 means there's none
  int separation = 10;            // how far the LDRs are separated (by LEDs)
  int LDRpos = LEDSpawnNum[LDR];  // Where on the LED strips is the LDR
  
  for(int i=0; i<NUMBLOCKS; i++){                      // cycle through blocks
    if(abs(blockPos[i]-LDRpos) < separation/2){   // if the block is near the LDR
      if(alive[i]) selectedPixel = i;                 // if it's alive, choose this pixel
    }
  }
  return selectedPixel;
}

//--------------------------------------------------------------------------




// Idle pulse
void idlePulse(){
  unsigned long pulseTime = millis()-idleStart;
  int pulseTimer = int(pulseTime*0.05) % 255;
  int brightness = map(quadwave8(pulseTimer),0,255,50,255);
  for(int i=0; i<NUMLEDS; i++){
    pixels1[i] = CHSV(0,0,brightness);
    pixels2[i] = CHSV(0,0,brightness);
  }
  FastLED.show();
}


//--------------------------------------------------------------------------



void putInFront(int p){
  front[p] = true;
  for(int i=0; i<NUMBLOCKS; i++){
    if(i != p) front[i] = false;
  }
}


//---------------------------------------------------

// map function
float mapFloat(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}


//--------------------------------------------------------------------------


//fade function
float fade(float current, float target, float amount){
  float result;
  if(current < target){           // if the current value is less than the target
    result = current + abs(amount);    // add the amount
    if(result > target){          // 
      result = target;
    }
  }
  else if(current > target){
    result = current - abs(amount);
    if(result < target){
      result = target;
    }
  }
  else if(current == target){
    result = target;
  }
  return result;
}



//--------------------------------------------------------------------------




// Read from serial
void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is newline, set a flag
    // so the main loop can do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    }
  }
  
  // A newline arrives:
  if (stringComplete) {
    Serial.println("-----------------------");
    Serial.print("String in: ");
    Serial.println(inputString);

    String numberString = inputString.substring(1,8);  // Get number following letter
    int numberIn = numberString.toInt();        // Convert number string to int

    // Get info by sending "i"
    if(inputString.startsWith("i")){  
      Serial.print("lastActed: ");
      Serial.println(lastActed);
      Serial.print("printBlocks(p): ");
      Serial.println(printLDRs);
      Serial.print("printLDRs(l): ");
      Serial.println(printBlocks);
      Serial.print("printTrigs(t): ");
      Serial.println(printTrigs);
      Serial.print("printMoves(m): ");
      Serial.println(printMoves);
      Serial.print("printStrips(s): ");
      Serial.println(printStrips);
      Serial.print("bright(b...): ");
      Serial.println(bright);
      Serial.println("force trigger(f...)");
    }
    

    // Control printing pixel values with "p"
    else if(inputString.startsWith("p")){
      printBlocks=!printBlocks;
      byte eepromIn = printBlocks;
      EEPROM.write(printBlocksAddr, eepromIn);
      Serial.print("printBlocks(p): ");
      Serial.println(printBlocks);
    }

    // Control printing LDR values with "l"
    else if(inputString.startsWith("l")){
      printLDRs=!printLDRs;
      byte eepromIn = printLDRs;
      EEPROM.write(printLDRAddr, eepromIn);
      Serial.print("printLDRs(l): ");
      Serial.println(printLDRs);
    }

    // Control printing trigger values with "t"
    else if(inputString.startsWith("t")){
      printTrigs=!printTrigs;
      byte eepromIn = printTrigs;
      EEPROM.write(printTrigAddr, eepromIn);
      Serial.print("printTrigs(t): ");
      Serial.println(printTrigs);
    }
    
    // Control printing movement with "m"
    else if(inputString.startsWith("m")){
//      if(printMoves) printMoves=0;
//      else printMoves = 1;
      printMoves=!printMoves;
      byte eepromIn = printMoves;
      EEPROM.write(printMovesAddr, eepromIn);
      Serial.print("printMoves(m): ");
      Serial.println(printMoves);
    }

    // Control printing strips with "m"
    else if(inputString.startsWith("s")){
//      if(printStrips) printStrips=0;
//      else printStrips = 1;
      printStrips=!printStrips;
      byte eepromIn = printStrips;
      EEPROM.write(printStripsAddr, eepromIn);
      Serial.print("printStrips(s): ");
      Serial.println(printStrips);
    }

    
    // Control the brightness with string, examples: "b13" "b0" "b255"
    else if(inputString.startsWith("b")){  // Volume string
      bright = numberIn;              // Change the volume
      if(bright<0) bright = 0;           // Limit the brightness between 0 and 255
      if(bright>255) bright = 255;
      FastLED.setBrightness(bright);

      byte eepromIn = bright;
      EEPROM.write(brightAddress, bright);  // Save the brightness to eeprom
      Serial.print("bright(b...): ");
      Serial.println(bright);
    }

    // Force trigger with "f..."
    else if(inputString.startsWith("f")){
      trig(numberIn);
    }


    Serial.println("-----------------------");

    // clear the string:
    inputString = "";
    stringComplete = false;
  }
}




