/* 
  Arco (Music Video Version)
  by Hubbub
  (Reuben Poharama, Braeden Foster and Martin Hill)
  


  Makes use of the EEPROM and FastLED library

  Button mapping:
  +------+   +-------------+  +-------------+  +-------------+  +-------------+
  |      |   |  Setting 1  |  |  Setting 2  |  |  Setting 3  |  |  Setting 4  |
  +------+   +-------------+  +-------------+  +-------------+  +-------------+
  
  +------+   +-------------+  +-------------+  +-------------+  +-------------+
  |      |   |             |  |             |  |             |  |             |
  +------+   |  Setting 5  |  |  Setting 6  |  |  Setting 7  |  |  Setting 8  |
             |             |  |             |  |             |  |             |
  +------+   +-------------+  +-------------+  +-------------+  +-------------+
  |      |
  +------+   +-------------+  +-------------+  +-------------+  +-------------+
             |             |  |             |  |             |  |             |
  +------+   |  Setting 9  |  | Setting 10  |  | Setting 11  |  | Setting 12  |
  |      |   |             |  |             |  |             |  |             |
  +------+   +-------------+  +-------------+  +-------------+  +-------------+
  
  +------+   +-------------+  +-------------+  +-------------+  +-------------+
  |Reset |   | Setting 13  |  | Setting 14  |  | Setting 15  |  | Setting 16  |
  +------+   +-------------+  +-------------+  +-------------+  +-------------+
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


#define BPM   106


// Define eeprom addresses
#define printBlocksAddr 16
#define printMidiAddr   18
#define printStripsAddr 20

//Define hues
#define BLUEHUE  170
#define REDHUE   250
#define SPINSPEED 20

bool pixelUsed[NUMPIXELS];

CRGB pixels1[NUMLEDS];
CRGB pixels2[NUMLEDS];
byte numBlends[NUMPIXELS];

//initial block colours
int pColour[NUMBLOCKS][3] = {
  {255,0,0},
  {255,0,0},
  {255,0,0},
  {255,0,0},
  {255,0,0},
  {255,0,0},
  {255,0,0},
  {255,0,0},
};


float blockPos[NUMBLOCKS];
float saturation[NUMBLOCKS];
float hue[NUMBLOCKS];
float brightness[NUMBLOCKS];
float blockWidth[NUMBLOCKS];


//velocity of the block
float vel[NUMBLOCKS];
float targetVel[NUMBLOCKS];

//strobe
byte strobeType[NUMBLOCKS];    //0: no strobe  1: 50/50  2: 25/75  3: 25/75 syncopated
float strobeSpeed[NUMBLOCKS];
unsigned long beatStart;

//switch
bool switching[NUMBLOCKS];
float switchSpeed;
unsigned long lastSwitch;


//block control
byte selected = 0;
byte controlling = 0; // 0:hue 1:saturation 2:velocity 3: size


//whether or not to print information
bool printBlocks;
bool printStrips;
bool printMidi;

byte bright;  //how bright the FastLEDs are

String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete




//---------------------------start of setup---------------------------------

void setup() {
  // Read EEPROM
  // Set print bools from eeprom
  printBlocks = EEPROM.read(printBlocksAddr);
  printStrips = EEPROM.read(printStripsAddr);
  printMidi = EEPROM.read(printMidiAddr);


  // Start Serial
  Serial.begin(115200);

  usbMIDI.setHandleNoteOn(OnNoteOn);
  usbMIDI.setHandleControlChange(OnControlChange);



  // start fast leds
  FastLED.addLeds<WS2812,PIXELPIN1,GRB>(pixels1, NUMLEDS);
  FastLED.addLeds<WS2812,PIXELPIN2,GRB>(pixels2, NUMLEDS);

  // initiate 2 blocks
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


