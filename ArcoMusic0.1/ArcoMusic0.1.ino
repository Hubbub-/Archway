/* 
  Arco
  by Hubbub
  (Reuben Poharama, Braeden Foster and Martin Hill)
  
  2 Blocks: variable size, colour, position, velocity


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
#define NUMBLOCKS       2    //number of blocks



// Define eeprom addresses
#define printBlocksAddr 16
#define printStripsAddr 20

bool pixelUsed[NUMPIXELS];

CRGB pixels1[NUMLEDS];
CRGB pixels2[NUMLEDS];
byte numBlends[NUMPIXELS];

//initial block colours
int pColour[NUMBLOCKS][3] = {
  {255,255,255},
  {0,0,255},
};


float blockPos[NUMBLOCKS];
float saturation[NUMBLOCKS];
float hue[NUMBLOCKS];
float brightness[NUMBLOCKS];
float blockWidth[NUMBLOCKS];


//velocity of the block
float vel[NUMBLOCKS];
float targetVel[NUMBLOCKS];


//whether or not to print information
bool printBlocks;
bool printStrips;
byte bright;  //how bright the FastLEDs are

String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete




//---------------------------start of setup---------------------------------

void setup() {
  // Read EEPROM
  // Set print bools from eeprom
  printBlocks = EEPROM.read(printBlocksAddr);
  printStrips = EEPROM.read(printStripsAddr);


  // Start Serial
  Serial.begin(115200);

  usbMIDI.setHandleNoteOn(OnNoteOn);

  usbMIDI.setHandleControlChange(OnControlChange);



  // start fast leds and make them fade to white
  FastLED.addLeds<WS2812,PIXELPIN1,GRB>(pixels1, NUMLEDS);
  FastLED.addLeds<WS2812,PIXELPIN2,GRB>(pixels2, NUMLEDS);


  initBlock(0);
  initBlock(1);


  Serial.println("Setup done, send 'i' for info");
}




//---------------------------start of loop---------------------------------

void loop() {
  // reset blends
  for(int i=0; i<NUMPIXELS; i++){
    numBlends[i] = 0;
  }
  midi();  // read midi
  blocksUpdate();  //update blocks
  blocksRender();
  FastLED.show(); // This sends the updated pixel color to the hardware.
}







//--------------------------------end of loop---------------------------------


