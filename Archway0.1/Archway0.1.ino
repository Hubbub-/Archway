#include <Adafruit_NeoPixel.h>
#include <HSBColor.h>

#define NUMLDRS 4

// Define pin and pixel number for LEDs
#define PIN            3
#define NUMPIXELS      40
#define NUMLEADPIXEL   1

// pins for shift register
#define data 11
#define shiftClear 7
#define shiftClock 8
#define shiftLive 13

#define resRead 14

//setup the NeoPixel library, tell it how many pixels, and which pin to use.
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

int red;
int green;
int blue;

int pColour[] = {255,0,0};

float LDR[NUMLDRS];
float prevLDR[NUMLDRS];
float heldVal[NUMLDRS];
unsigned int heldSince;
float leadPixel[NUMLEADPIXEL];

int initMaxBright = 800;
int initMinBright = 200;
int maxBright[NUMLDRS];
int minBright[NUMLDRS];

unsigned long trigTime [NUMLDRS];

float triggerVal;

float vel;
float targetVel;
float targetVelHigh;


void setup() {

  // Start Serial
  Serial.begin(115200);

  // Setup pins
  pinMode(shiftClear, OUTPUT);
  pinMode(shiftClock, OUTPUT);
  pinMode(shiftLive, OUTPUT);
  pinMode(data, OUTPUT);
  pinMode(resRead, INPUT);
  digitalWrite(shiftClear, HIGH);

  // Start Neo Pixels
  pixels.begin();

  // Set max and min brightness (These change constantly)
  for (int i=0; i<NUMLDRS; i++){
    maxBright[i] = initMaxBright;
    minBright[i] = initMinBright;
  }
}

void loop() {
  // Read the LDR values using the shift Register
  shiftReg();  //function after loop
  heldSince ++;  //increase the "heldSince" value
  if(heldSince > 5){  //only check LDR values every 5 loops
    heldSince = 0;
  }
  

  for(int i=0; i<NUMLDRS; i++){   //cycle through LDRs
    if (heldSince == 0) {           // if "heldsince" is greater than 10
      if (heldVal[i] - LDR[i] > 50) {  // if the change in reading is greater than 25
        trig(i);                       // trigger the pixel movement
      }
      heldVal[i] = LDR[i];  // store the value
    }
  }
  for (int i=0; i<NUMLEADPIXEL; i++){
    leadPixel[i] += vel;
    if (leadPixel[i] >= NUMPIXELS){
      leadPixel[i] = 0;
      float hue = random(0,100)*0.01;
      H2R_HSBtoRGBfloat(hue, 1.0, 1.0, pColour);
    }
    else if (leadPixel[i] <0){
      leadPixel[i] = NUMPIXELS;
      float hue = random(0,100)*0.01;
      H2R_HSBtoRGBfloat(hue, 1.0, 1.0, pColour);
    } 
  }
  

  //  maxBright -= 0.01;
  //  minBright += 0.001;
  //  if(max(val1,val2) > maxBright) maxBright = max(val1,val2);
  //  if(min(val1,val2) < minBright) minBright = min(val1,val2);



  //if the absolute velocity is slower than the absolute target, change quickly to the target
  if(abs(vel)<abs(targetVel) && targetVel!=0){
    vel=fade(vel,targetVel,targetVel*0.03);
//    vel=targetVel;
  }
  //if the absolute velocity is faster than absolute target, change slowly to the target
  //and set the target to 0 if it isn't already
  else if (abs(vel)>=abs(targetVel)){
    vel=fade(vel,targetVel,vel*0.005);
    if(targetVel!=0) targetVel=0;
  }
  
  



  for(int i=0; i<NUMLDRS; i++){
//    Serial.print(i);
//    Serial.print(": ");
//    Serial.print(LDR[i]);
//    Serial.print(" | ");  
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
//  Serial.print(leadPixel[0]);



  // apply colour to the pixels
  for (int i = 0; i < NUMPIXELS; i++) {
    if (i == int(leadPixel[0])) {
      red = pColour[0];
      green = pColour[1];
      blue = pColour[2];
//      Serial.print("+");

    }

    else {
      red = 0;
      green = 0;
      blue = 5;
//      Serial.print("_");
    }
    
    pixels.setPixelColor(i, pixels.Color(red, green, blue));
  }
//  pixels.setPixelColor(int(leadPixel[0]), pixels.Color(pColour[0], pColour[1], pColour[2]));
  pixels.show(); // This sends the updated pixel color to the hardware.
//  Serial.println(" ");


}

void trig(int LDR) {
  Serial.print("Triggered LDR#");
  Serial.print(LDR);
  
  unsigned long t = millis();
  trigTime[LDR] = t;  // set the initial trigger time
  Serial.print(" trigTime:");
  Serial.print(trigTime[LDR]);
  
  if(LDR>0){
    Serial.print(" Not First ");
    if (t-trigTime[LDR-1] < 1000){
      targetVel = vel-mapFloat(t - trigTime[LDR-1], 600, 0, 0.0, 0.13);  // Set the target velocity 
      Serial.print("| set targetVel to:");
      Serial.print(targetVel);
    }
  }

  else if(LDR<NUMLDRS){
    Serial.print(" Not last ");
    if(t-trigTime[LDR+1] <1000){
      targetVel = vel+mapFloat(t - trigTime[LDR+1], 600, 0, 0.0, 0.13);  // Set the target velocity
      Serial.print("| set targetVel to:");
      Serial.print(targetVel);
    }
  }
  Serial.println(" ");
  //limit the velocity
  if(targetVel>0.6) targetVel=0.6;
  else if(targetVel<-0.6) targetVel=-0.6;


  
//  if (LDR == 1) {                   // if the second LDR is triggered
//    if (t < initTrigTime + 1000) {  // and it's been less than a second since the initial
//      targetVel = vel+mapFloat(t - initTrigTime, 600, 0, 0.0, 0.13);  // Set the target velocity
//      //limit the velocity
//      if(targetVel>0.6) targetVel=0.6;
//    }
//  }
}

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
    if(i==0) LDR[NUMLDRS-1] = analogRead(resRead);  
    else LDR[i-1] = analogRead(resRead);
  }
}


//map function
float mapFloat(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

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

