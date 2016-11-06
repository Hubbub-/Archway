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
#define NUMLEADS       4     //number of lead pixels


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
int pColour[NUMLEADS][3];


float LDR[NUMLDRS];
float prevLDR[NUMLDRS];
float heldVal[NUMLDRS];
unsigned int heldSince;
float leadPixelPos[NUMLEADS];


//maximum and minimum brightness to use for scaling
int initMaxBright = 800;
int initMinBright = 200;
int maxBright[NUMLDRS];
int minBright[NUMLDRS];

//saves the last time each LDR was triggered
unsigned long trigTime [NUMLDRS];


//velocity of the moving pixel
float vel[NUMLEADS];
float targetVel[NUMLEADS];
float targetVelHigh[NUMLEADS];


//Mapping when spawning new pixel
int LDRSpawnNum[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13};
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

  // Initialise pColour array
  for(int i=0; i<NUMLEADS; i++){
    for(int j=0; i<3; j++){
      pColour[i][j] = random(255);
    }
  }
  
  // Set all printing to false
  printLDRs = false;
  printPixels = false;
  printTrigs = false;
  printMoves = false;

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
    if (heldSince == 0) {                   // if "heldsince" is greater than 10
      if (abs(heldVal[i] - LDR[i]) > 50) {  // if the change in reading is greater than 50
        trig(i);                            // trigger the pixel movement
      }
      heldVal[i] = LDR[i];  // store the value
    }

    // change the min and max values
    int difference = abs(maxBright[i]-minBright[i]);
    int middle = minBright[i]+difference/2;
    // bring closer together if far apart
    if(difference > middle*0.1){
      maxBright[i] -= 0.001;    
      minBright[i] += 0.001;
    }
    if(LDR[i] > maxBright[i]) maxBright[i] = LDR[i];
    if(LDR[i] < minBright[i]) minBright[i] = LDR[i];
  }

  
  for (int i=0; i<NUMLEADS; i++){     // cycle through lead pixels
    leadPixelPos[i] += vel[i];              // add the velocity
    
    if (leadPixelPos[i] >= NUMPIXELS){   // if the pixel gets to the end
      leadPixelPos[i] = 0;               // put it at the start
      float hue = random(0,100)*0.01; // with a random colour
      H2R_HSBtoRGBfloat(hue, 1.0, bright, pColour[i]);
    }
    else if (leadPixelPos[i] <0){        // if the pixel gets to the start
      leadPixelPos[i] = NUMPIXELS;       // put it at the end
      float hue = random(0,100)*0.01; // with a random colour
      H2R_HSBtoRGBfloat(hue, 1.0, bright, pColour[i]);
    }
    if(printPixels){
      Serial.print("Pixel");
      Serial.print(i);
      Serial.print(": ");
      Serial.print(leadPixelPos[i]);
      Serial.print(" | ");
    }
  }
  if(printPixels) Serial.println(" ");
  




  // update velocities
  for(int i=0; i<NUMLEADS; i++){
    //if the absolute velocity is slower than the absolute target, change quickly to the target
    if(abs(vel[i])<abs(targetVel[i]) && targetVel[i]!=0){
      vel[i]=fade(vel[i],targetVel[i],targetVel[i]*0.03);
    }
    //if the absolute velocity is faster than absolute target, change slowly to the target
    //and set the target to 0 if it isn't already
    else if (abs(vel[i])>=abs(targetVel[i])){
      vel[i]=fade(vel[i],targetVel[i],vel[i]*0.005);
      if(targetVel[i]!=0) targetVel[i]=0;
    }
    //limit the velocity
    if(targetVel[i]>0.6) targetVel[i]=0.6;
    else if(targetVel[i]<-0.6) targetVel[i]=-0.6;
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

  //  Serial.print(" | max: ");
  //  Serial.print(maxBright);
  //  Serial.print(" | min: ");
  //  Serial.println(minBright);
//  Serial.print(" | targetVel: ");
//  Serial.print(targetVel);
//  Serial.print(" | vel: ");
//  Serial.print(vel);
//  Serial.print(" | pixel: ");
//  Serial.print(leadPixelPos[0]);




  
  // apply colour to only the Lead pixel (this needs to change)
  for(int i=0; i<NUMLEADS; i++){
    applyColour(leadPixelPos[0], pColour[i][0], pColour[i][1], pColour[i][2]);
  }
  pixels1.show(); // This sends the updated pixel color to the hardware.
  pixels2.show();

}







//--------------------------------end of loop---------------------------------









void trig(int LDR) {

// spawn a new lead pixel
//  makeLead(multiMap();
  
  unsigned long t = millis();
  trigTime[LDR] = t;  // set the initial trigger time
  // Select the lead pixel to move
  int leadP = leadPixelAt(LDR);
  
  if(printTrigs){
    Serial.print("Triggered LDR#");
    Serial.print(LDR);
    Serial.print(" trigTime:");
    Serial.print(trigTime[LDR]);
  }
  
  if(LDR>0){
    if(printTrigs){
      Serial.print(" prevTrigTime:");
      Serial.print(trigTime[LDR-1]);
    }
    if (t-trigTime[LDR-1] < 1000){
      targetVel[leadP] = vel[leadP]+mapFloat(t - trigTime[LDR-1], 600, 0, 0.0, 0.13);  // Set the target velocity 
      if(printTrigs){
        Serial.print("| set targetVel of [");
        Serial.print(leadP);
        Serial.print("] to: ");
        Serial.print(targetVel[leadP]);
      }
    }
  }

  else if(LDR<NUMLDRS){
    if(t-trigTime[LDR+1] <1000){
      targetVel[leadP] = vel[leadP]-mapFloat(t - trigTime[LDR+1], 600, 0, 0.0, 0.13);  // Set the target velocity
      if(printTrigs){
        Serial.print("| set targetVel to:");
        Serial.print(targetVel[leadP]);
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



// note: the _in array should have increasing values
int multiMap(int val, int* _in, int* _out, uint8_t size)
{
  // take care the value is within range
  // val = constrain(val, _in[0], _in[size-1]);
  if (val <= _in[0]) return _out[0];
  if (val >= _in[size-1]) return _out[size-1];

  // search right interval
  uint8_t pos = 1;  // _in[0] allready tested
  while(val > _in[pos]) pos++;

  // this will handle all exact "points" in the _in array
  if (val == _in[pos]) return _out[pos];

  // interpolate in the right segment for the rest
  return (val - _in[pos-1]) * (_out[pos] - _out[pos-1]) / (_in[pos] - _in[pos-1]) + _out[pos-1];
}


//--------------------------------------------------------------------------





// Apply pixel colours to the right strip
void applyColour(float pixelin, int red, int green, int blue){
  int pixel = (int) pixelin;
  // strip1
  if(pixel%2 == 0){
    int p = pixel/2;
    pixels1.setPixelColor(p, pixels1.Color(red,green,blue));
  }
  
  // strip2 
  if(pixel%2 == 1){
    int p = (pixel-1)/2;
    pixels2.setPixelColor(p, pixels2.Color(red,green,blue));
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
  int separation = 43;            // how far the LDRs are separated (by LEDs)
  int LDRpos = LEDSpawnNum[LDR];  // Where on the LED strips is the LDR
  
  for(int i=0; i<NUMLEADS; i++){                      // cycle through leadPixels
    if(abs(leadPixelPos[i]-LDRpos) < separation/2){   // if the leadPixel is near the LDR
      selectedPixel = i;                              // choose this pixel
    }
  }
  return selectedPixel;
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
        float newVel = random(-100, 100);
        targetVel[i] = newVel/100;
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




