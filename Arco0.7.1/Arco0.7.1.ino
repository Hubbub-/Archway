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
#define NUMLEADS       10    //number of lead pixels
#define MAXVEL         30
#define FADESPEED      0.14
#define INITLEADSIZE   25


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
int pixelType[NUMPIXELS];

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
bool front[NUMLEADS];       // is it in front

float saturation[NUMLEADS];
float hue[NUMLEADS];
bool fadeUp[NUMLEADS];
bool exploding[NUMLEADS];
float leadSize[NUMLEADS];


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


//Mapping when spawning new pixel
int LEDSpawnNum[] = {16,58,107,150,193,236,279,318,360,403,442,491,540,584};


//whether or not to print information
bool printLDRs;
bool printTrigs;
bool printPixels;
bool printMoves;
bool printStrips;
byte bright;  //how bright the pixels are

String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete


// PIR sensors
bool movement = false;
int PIRpin[] = {1,2,5,6};
bool idle = true;


//---------------------------start of setup---------------------------------

void setup() {
  // Read EEPROM
  bright = EEPROM.read(brightAddress);
  if(bright<0) bright = 0;           // Limit the brightness between 0 and 255
  if(bright>255) bright = 255;

  // Start Serial
  Serial.begin(115200);

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
  

  // Set initial max and min brightness for each LDR
  for (int i=0; i<NUMLDRS; i++){
    maxBright[i] = initMaxBright;
    minBright[i] = initMinBright;
  }

  
  
  // Set all printing to false
  printLDRs = false;
  printPixels = false;
  printTrigs = false;
  printMoves = false;
  printStrips = false;



  Serial.println("Setup done, send 'i' for info");
}




//---------------------------start of loop---------------------------------

void loop() {
//  if(!movement) idlePulse();
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



  
  for (int i=0; i<NUMLEADS; i++){     // cycle through lead pixels
    
    // -- TIMING --
    if (alive[i]){
      lifeTime[i] = millis() - lifeStart[i];    // update life time
      
      if(!exploding[i]){                      // if the block is not exploding, pulse
        if(fadeUp[i]){                                // if it should fade up...
          saturation[i] += FADESPEED;                 // fade up
          if (saturation[i] >= 1) fadeUp[i] = false;  // if it reaches the top, start fading down
        }
        else{                                         // if it should fade down...
          saturation[i] -= FADESPEED;                 // fade down
          if (saturation[i] <= 0) fadeUp[i] = true;   // if it reaches the bottom, start fading up
        }
      }

      else if (exploding[i]){   // if the block is exploding
        if(leadSize[i] > 300){  
          vel[i] = 0;            // limit size
        }
        leadSize[i] += vel[i]; // increase size
      }

      if(lifeTime[i] > 5000){           // if the block is old
        saturation[i] -= 0.01;          // fade out
        if(saturation <= 0){  
          alive[i] = false;             // kill it off when faded
          initLead(random(0,NUMLDRS));  // initiate another block somewhere
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


    

    if(printPixels){
      Serial.print("leadPos ");
      Serial.print(i);
      Serial.print(":");
      Serial.print(leadPixelPos[i]);
      if(exploding[i]) Serial.print(" Exploded!");
      Serial.print(" | ");
    }


  }
  if(printPixels) Serial.println(" ");
  


  // Set Blueish White background
  for(int i=0; i<NUMPIXELS; i++){
    applyColour(i,200,200,255);
    pixelType[i] = 0;
  }
  
  // apply colour to the exploded lead pixels not in front
  for(int i=0; i<NUMLEADS; i++){
    if(exploding[i] && !front[i]){
      H2R_HSBtoRGBfloat(hue[i], saturation[i], bright, pColour[i]);
      for(int j=-leadSize[i]/2; j<leadSize[i]/2; j++){
        
        applyColour(leadPixelPos[i]+j, pColour[i][0], pColour[i][1], pColour[i][2]);
        int pos = leadPixelPos[i]+j;
        if (pos>=0 && pos<NUMPIXELS && alive[i]) pixelType[pos] = 1;
      }
    }
  }

  // apply colour to the Lead pixels in front
  for(int i=0; i<NUMLEADS; i++){
    if(exploding[i] && front[i]){
      H2R_HSBtoRGBfloat(hue[i], saturation[i], bright, pColour[i]);
      for(int j=-leadSize[i]/2; j<leadSize[i]/2; j++){
        
        applyColour(leadPixelPos[i]+j, pColour[i][0], pColour[i][1], pColour[i][2]);
        int pos = leadPixelPos[i]+j;
        if (pos>=0 && pos<NUMPIXELS && alive[i]) pixelType[pos] = 1;
      }
    }
  }

  // apply colour to the Lead pixels that have not exploded
  for(int i=0; i<NUMLEADS; i++){
    if(!exploding[i]){
      H2R_HSBtoRGBfloat(hue[i], saturation[i], bright, pColour[i]);
      for(int j=-leadSize[i]/2; j<leadSize[i]/2; j++){
        
        applyColour(leadPixelPos[i]+j, pColour[i][0], pColour[i][1], pColour[i][2]);
        int pos = leadPixelPos[i]+j;
        if (pos>=0 && pos<NUMPIXELS && alive[i]) pixelType[pos] = 2;
      }
    }
  }
  
  pixels1.show(); // This sends the updated pixel color to the hardware.
  pixels2.show();

  if(printStrips){
    for(int i=0; i<NUMPIXELS-4; i+=4){
      if (pixelType[i] == 0) Serial.print("-");
      else if (pixelType[i] == 1) Serial.print("|");
      else if (pixelType[i] == 2) Serial.print("+");
    }
    Serial.println(" ");
  }

}







//--------------------------------end of loop---------------------------------



// initiate a lead pixel
void initLead(int pos){
  // Look through array of lead pixels for any that are dead
  int index = 0;
  while(alive[index]){  // Only look until a dead one is found
    index++;
    if(index >= NUMLEADS){        // If there are no dead ones...
      index = random(0,NUMLEADS); // choose one at random
      break;
    }
  }
  // Replace selected pixel
  leadPixelPos[index] = LEDSpawnNum[pos];  // put in correct place
  alive[index] = true;                     // make alive
  lifeStart[index] = millis();             // give lifeStart time
  hue[index] = random(0,100)*0.01;         // a random colour
  saturation[index] = 0;                   // start with no saturation (to fade in)
  fadeUp[index] = true;                    // start lead pixel fading up
  leadSize[index] = INITLEADSIZE;          // the initial size of the lead pixel
  exploding[index] = false;                // make sure it's not exploding to start with
  front[index] = false;                    // It's already in front of the exploeded ones

}



void trig(int LDR) {

  unsigned long t = millis();
  trigTime[LDR] = t;  // set the initial trigger time
  
  if(printTrigs){
    Serial.print("Triggered LDR#");
    Serial.print(LDR);
  }

  if(leadPixelAt(LDR) < 0){    // If there's no lead pixel at the LDR
    initLead(LDR);             // make a new lead pixel
    if(printTrigs){
      Serial.print(" | made leadPixel at ");
      Serial.print(LDR);
    }
  }
  
  else{       // if there is a lead pixel at the LDR
    int target = leadPixelAt(LDR);    // select the lead pixel

    // Explode it
    if(lifeTime[target] > 500){       // only if it's old enough
      targetVel[target] = MAXVEL;     // change the target velocity
      exploding[target] = true;       // make explode
      saturation[target] = 1.0;       // Full saturation
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
  int separation = 10;            // how far the LDRs are separated (by LEDs)
  int LDRpos = LEDSpawnNum[LDR];  // Where on the LED strips is the LDR
  
  for(int i=0; i<NUMLEADS; i++){                      // cycle through leadPixels
    if(abs(leadPixelPos[i]-LDRpos) < separation/2){   // if the leadPixel is near the LDR
      if(alive[i]) selectedPixel = i;                 // if it's alive, choose this pixel
    }
  }
  return selectedPixel;
}




//--------------------------------------------------------------------------



void putInFront(int p){
  front[p] = true;
  for(int i=0; i<NUMLEADS; i++){
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
      Serial.print("printPixels(p): ");
      Serial.println(printLDRs);
      Serial.print("printLDRs(l): ");
      Serial.println(printPixels);
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

    // Control printing strips with "m"
    else if(inputString.startsWith("s")){
      if(printStrips) printStrips=0;
      else printStrips = 1;
      Serial.print("printStrips(s): ");
      Serial.println(printStrips);
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




