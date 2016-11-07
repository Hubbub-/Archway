/* 
  Arco
  by Hubbub
  (Reuben Poharama, Braeden Foster and Martin Hill)
  
  1. Measure light cast on array of LDRs (light dependant resistors)
  2. Infer movement of shadows
  3. Use placement and movement to control LED strips


  Makes use of the Adafruit NeoPixel library and the HSBColor Library
*/

#include <Adafruit_NeoPixel.h>
#include <HSBColor.h>
#include <EEPROM.h>


#define NUMLDRS 14

// Define pin and pixel number for LEDs
#define PIXELPIN1      3
#define PIXELPIN2      4
#define NUMLEDS        300   //Number of LEDs per strip
#define NUMPIXELS      600   //Total number of LEDs (both strips)
#define NUMLEADS       20    //number of lead pixels
#define MAXVEL         30


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


#define brightAddress 0

//setup the NeoPixel library, tell it how many pixels, and which pin to use.
Adafruit_NeoPixel pixels1 = Adafruit_NeoPixel(NUMPIXELS, PIXELPIN1, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel pixels2 = Adafruit_NeoPixel(NUMPIXELS, PIXELPIN2, NEO_GRB + NEO_KHZ800);

byte red[NUMPIXELS];
byte green[NUMPIXELS];
byte blue[NUMPIXELS];

//initial pixel colours
int pColour[NUMLEADS][3] = {
  {0,255,255},
  {0,255,255},
  {0,255,255},
  {0,255,255},
  {0,255,255},
  {0,255,255},
  {0,255,255},
  {0,255,255},
  {0,255,255},
  {0,255,255},
  {0,255,255},
  {0,255,255},
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
float leadPixelPos[NUMLEADS];
int lifeTime [NUMLEADS];   // how long it's been alive
unsigned long lifeStart [NUMLEADS];  // time that the life started
bool alive [NUMLEADS];               // is it alive
int sensorHold [NUMLEADS];           // holds the value from the LDR when triggered
float saturation[NUMLEADS];
float hue[NUMLEADS];


//maximum and minimum brightness to use for scaling
int initMaxBright = 300;
int initMinBright = 100;
int maxBright[NUMLDRS];
int minBright[NUMLDRS];

//saves the last time each LDR was triggered
unsigned long trigTime [NUMLDRS];


//velocity of the moving pixel
float vel[NUMLEADS];
float targetVel[NUMLEADS];
float targetVelHigh[NUMLEADS];


//Mapping when spawning new pixel
int LEDSpawnNum[] = {22,64,107,150,193,236,279,322,364,407,450,493,536,579};


//whether or not to print information
bool printLDRs;
bool printTrigs;
bool printPixels;
bool printMoves;
byte bright;  //how bright the pixels are

String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete


// PIR sensors
bool movement = false;
int PIRpin[] = {1,2,5,6};



//---------------------------start of setup---------------------------------

void setup() {
  // Read EEPROM
  bright = EEPROM.read(brightAddress);
  if(bright<0) bright = 0;           // Limit the volume between 0 and 100
  if(bright>255) bright = 255;

  // Start Serial
  Serial.begin(115200);

  // Setup pins
  for(int i=0; i<4; i++){
    pinMode(PIRpin[i], INPUT);
  }
  
  pinMode(shiftClear, OUTPUT);
  pinMode(shiftClock, OUTPUT);
  pinMode(shiftLive, OUTPUT);
  pinMode(data, OUTPUT);
  pinMode(LDRpin, INPUT);
  digitalWrite(shiftClear, HIGH);

  // Start Neo Pixels and make them fade to white
  pixels1.begin();
  pixels2.begin();
  for(int i=0; i<100; i++){
    float b = i*0.01*bright;
    for(int j=0; j<NUMPIXELS; j++){
      pixels1.setPixelColor(j, pixels1.Color(b,b,b));
      pixels2.setPixelColor(j, pixels2.Color(b,b,b));
    }
    pixels1.show();
    pixels2.show();
    delay(1);
  }
  

  // Set max and min brightness (These change constantly)
  for (int i=0; i<NUMLDRS; i++){
    maxBright[i] = initMaxBright;
    minBright[i] = initMinBright;
  }

  
  
  // Set all printing to false
  printLDRs = false;
  printPixels = false;
  printTrigs = false;
  printMoves = false;

  //Initiate Lead Pixels
//  for(int i=0; i<NUMLEADS; i++){
//    initLead(i);
//  }

  Serial.println("Setup done, send 'i' for info");
}




//---------------------------start of loop---------------------------------

void loop() {
  serialEvent();  // Handles incoming serial

  // Check for movement
  checkMovement();
  
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
      if (abs(heldVal[i] - LDR[i]) > middle*0.12+10 && !alive[i]) {  // if there's a change in reading and the pixel isn't alive
        trig(i);                  // trigger the pixel spawn
        sensorHold[i] = LDR[i];   // store the sensor value
      }
//      if (abs(sensorHold[i] - LDR[i]) > middle*0.12){  // if the hand isn't there anymore
//        alive[i] = false;
//      }
      heldVal[i] = LDR[i];  // store the value
    }

    
    // bring closer together if far apart
    if(difference > middle*0.1){
      maxBright[i] -= 0.001;    
      minBright[i] += 0.001;
    }
    if(LDR[i] > maxBright[i]) maxBright[i] = LDR[i];
    if(LDR[i] < minBright[i]) minBright[i] = LDR[i];
  }






  
  for (int i=0; i<NUMLEADS; i++){     // cycle through lead pixels
    
    // -- TIMING --
    if (alive[i]){
      lifeTime[i] = millis() - lifeStart[i];    // update life time
    }

    
    
    if (lifeTime[i] < 100){       // first 0.1 seconds of life
      float shortLifeTime = lifeTime[i];
      saturation[i] = mapFloat(shortLifeTime,0.0,100.0,0,0.6);  // increase saturation
    }
    else if (lifeTime[i] > 100 && lifeTime[i] < 120){  // between 0.1 and 0.12 seconds
      float shortLifeTime = lifeTime[i];
      saturation[i] = mapFloat(shortLifeTime,100,120,0.6,1.0);
    }
    else if (lifeTime[i] > 150 && lifeTime[i] < 1500){    // more than 0.12 seconds
      saturation[i] = 1.0;
    }
    else if (lifeTime[i] > 1500 && lifeTime[i] < 2500){
      float shortLifeTime = lifeTime[i];
      saturation[i] = mapFloat(shortLifeTime,1500,2500,1.0,0.0);
    }
    else if (lifeTime[i] > 2500){
      delLead(i);
    }

    if(printPixels){
      Serial.print("lifeTime ");
      Serial.print(i);
      Serial.print(": ");
      Serial.print(lifeTime[i]);
      Serial.print(" | ");
    }


    

    
    
    leadPixelPos[i] += vel[i];              // add the velocity
    
    if (leadPixelPos[i] >= NUMPIXELS || leadPixelPos[i] < 0){   // if the pixel gets to the end
      delLead(i);
    }
  }
  if(printPixels) Serial.println(" ");
  




  // update velocities
  for(int i=0; i<NUMLEADS; i++){
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
  }
  

  // print the LDR values if wanted
  if(printLDRs){
    for(int i=0; i<NUMLDRS; i++){
      Serial.print(i);
      Serial.print(": ");
      Serial.print(LDR[i]);
      Serial.print(" | ");  
    }
    Serial.println(" ");
  }



  for(int i=0; i<NUMPIXELS; i++){
    applyColour(i,255,255,255);
  }
  
  // apply colour to the Lead pixels and surrounding LEDs
  for(int i=0; i<NUMLEADS; i++){
    if(saturation[i] > 0){
      H2R_HSBtoRGBfloat(hue[i], saturation[i], bright, pColour[i]);
      for(int j=-25; j<25; j++){
        applyColour(leadPixelPos[i]+j, pColour[i][0], pColour[i][1], pColour[i][2]);
      }
    }
  }
  
  pixels1.show(); // This sends the updated pixel color to the hardware.
  pixels2.show();

}







//--------------------------------end of loop---------------------------------




void initLead(int pos){
  int index = 0;
  while(alive[index]){
    index++;
  }
  leadPixelPos[index] = LEDSpawnNum[pos];
  lifeTime[index] = 0;
  alive[index] = true;
  lifeStart[index] = millis();
  hue[index] = random(0,100)*0.01; // with a random colour
  saturation[index] = 0;
}

void delLead(int index){
  lifeTime[index] = 0;
  alive[index] = false;
  saturation[index] = 0;
}



void trig(int LDR) {


  unsigned long t = millis();
  trigTime[LDR] = t;  // set the initial trigger time
  int leadBefore = leadPixelAt(LDR-1);
  int leadAfter = leadPixelAt(LDR+1);
  
  if(printTrigs){
    Serial.print("Triggered LDR#");
    Serial.print(LDR);
    Serial.print(" leadBefore:");
    Serial.print(leadBefore);
    Serial.print(" leadAfter:");
    Serial.print(leadAfter);
  }
  
  if(LDR>0 && alive[leadBefore] && inPlace(leadBefore)){
    targetVel[leadBefore] = MAXVEL;
    if(printTrigs){
      Serial.print(" | moving ");
      Serial.print(leadBefore);
    }
  }
  else if(LDR<NUMLDRS-1 && alive[leadAfter] && inPlace(leadAfter)){
    targetVel[leadAfter] = -MAXVEL;
    if(printTrigs){
      Serial.print(" | moving ");
      Serial.print(leadAfter);
    }
  }

  else if (livingPixelsAt(LDR)<1){
    initLead(LDR);
    if(printTrigs){
      Serial.print(" | made leadPixel at ");
      Serial.print(LDR);
    }
  }

  if(printTrigs)Serial.println(" ");


}


bool inPlace(int p){
  if(abs(leadPixelPos[p] - LEDSpawnNum[p]) < 5) return true;
  else return false;
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





// Apply pixel colours to the right strip
void applyColour(float pixelin, float red, float green, float blue){
  int pixel = (int) pixelin;
  int redOut = red*(bright/255.0);
  int greenOut = green*(bright/255.0);
  int blueOut = blue*(bright/255.0);
  // strip1
  if(pixel%2 == 0){
    int p = pixel/2;
    pixels1.setPixelColor(p, pixels1.Color(redOut,greenOut,blueOut));
  }
  
  // strip2 
  if(pixel%2 == 1){
    int p = (pixel-1)/2;
    pixels2.setPixelColor(p, pixels2.Color(redOut,greenOut,blueOut));
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

// Find the leadPixel near a certain LDR
int leadPixelAt(int LDR){
  int selectedPixel = -1;         // -1 means there's none
  int separation = 45;            // how far the LDRs are separated (by LEDs)
  int LDRpos = LEDSpawnNum[LDR];  // Where on the LED strips is the LDR
  
  for(int i=0; i<NUMLEADS; i++){                      // cycle through leadPixels
    if(abs(leadPixelPos[i]-LDRpos) < separation/2){   // if the leadPixel is near the LDR
      if(alive[i]) selectedPixel = i;                              // if it's alive, choose this pixel
    }
  }
  return selectedPixel;
}


// How many living pixels near a certain LDR
int livingPixelsAt(int LDR){
  int count = 0;
  int separation = 25;            // how far the LDRs are separated (by LEDs)
  int LDRpos = LEDSpawnNum[LDR];  // Where on the LED strips is the LDR
  
  for(int i=0; i<NUMLEADS; i++){
    if(abs(leadPixelPos[i]-LDRpos) < separation/2){   // if the leadPixel is near the LDR
      if(alive[i]) count += 1;                        // if it's alive count this pixel
    }
  }
  return count;
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
      Serial.print("printPixels(p): ");
      Serial.println(printLDRs);
      Serial.print("printLDRs(l): ");
      Serial.println(printPixels);
      Serial.print("printTrigs(t): ");
      Serial.println(printTrigs);
      Serial.print("printMoves(m): ");
      Serial.println(printMoves);
      Serial.print("bright(b): ");
      Serial.println(bright);
      Serial.println("randomise velocities(v)");
    }
    

    // Control printing pixel values with "p"
    else if(inputString.startsWith("p")){
      printPixels=!printPixels;
      Serial.print("printPixels(p): ");
      Serial.println(printPixels);
    }

    // Control printing LDR values with "l"
    else if(inputString.startsWith("l")){
      printLDRs=!printLDRs;
      Serial.print("printLDRs(l): ");
      Serial.println(printLDRs);
    }

    // Control printing trigger values with "t"
    else if(inputString.startsWith("t")){
      printTrigs=!printTrigs;
      Serial.print("printTrigs(t): ");
      Serial.println(printTrigs);
    }
    
    // Control printing movement with "m"
    else if(inputString.startsWith("m")){
      if(printMoves) printMoves=0;
      else printMoves = 1;
      Serial.print("printMoves(m): ");
      Serial.println(printMoves);
    }

    

    
    // Control the brightness with string, examples: "b13" "b0" "b255"
    else if(inputString.startsWith("b")){  // Volume string
      bright = numberIn;              // Change the volume
      if(bright<0) bright = 0;           // Limit the brightness between 0 and 255
      if(bright>255) bright = 255;
      pixels1.setBrightness(bright);
      pixels2.setBrightness(bright);
      EEPROM.write(brightAddress, byte(bright));  // Save the brightness to eeprom
      Serial.print("bright(b...): ");
      Serial.println(bright);
    }

    // Control the hue with string, examples: "h13" "h0" "h255"
    else if(inputString.startsWith("h")){  // Hue string
      int colour[3];
      float hueIn = numberIn;
      H2R_HSBtoRGBfloat(hueIn/360, 1.0, bright, colour);
      for(int i=0; i<NUMPIXELS; i++){
        applyColour(i,colour[0],colour[1],colour[2]); 
      }
      
      Serial.print("hue(h...): ");
      Serial.println(hueIn);
    }

    else if(inputString.startsWith("v")){
      for(int i=0; i<NUMLEADS; i++){
        float newVel = random(-MAXVEL, MAXVEL);
        targetVel[i] = newVel;
        Serial.print(" | targetVel[");
        Serial.print(i);
        Serial.print("]:");
        Serial.print(targetVel[i]);
      }
      Serial.println("");
    }


    
    Serial.println("-----------------------");

    // clear the string:
    inputString = "";
    stringComplete = false;
  }
}




