/* 
  Arco (Music Video Version)
  by Hubbub
  (Reuben Poharama, Braeden Foster and Martin Hill)
  


  Makes use of the EEPROM and FastLED library

  Button mapping:
  +------+   +-------------+  +-------------+  +-------------+  +-------------+
  |BlueHu|   | Setting 1.. |  | Setting 2.. |  | Setting 3.. |  | Setting 4.. |
  +------+   +-------------+  +-------------+  +-------------+  +-------------+
  
  +------+   +-------------+  +-------------+  +-------------+  +-------------+
  |RedHue|   |             |  |             |  |             |  |             |
  +------+   | Setting .A. |  | Setting .B. |  | Setting .C. |  | Setting .D. |
             |             |  |             |  |             |  |             |
  +------+   +-------------+  +-------------+  +-------------+  +-------------+
  |Warmth|
  +------+   +-------------+  +-------------+  +-------------+  +-------------+
             |             |  |             |  |             |  |             |
  +------+   | Setting .E. |  | Setting .F. |  | Setting .G. |  |             |
  |Revers|   |             |  |             |  |             |  |             |
  +------+   +-------------+  +-------------+  +-------------+  +-------------+
  
  +------+   +-------------+  +-------------+  +-------------+  +-------------+
  |Reset |   | Setting ..1 |  | Setting ..2 |  | Setting ..3 |  | Setting ..4 |
  +------+   +-------------+  +-------------+  +-------------+  +-------------+
*/



#include <EEPROM.h>
#include "FastLED.h"


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
#define presetAddr      22
#define blueHueAddr     24
#define redHueAddr      26
#define whiteSatAddr    28

//Define hues
#define BLUEHUE  180
#define GREENHUE 100
#define REDHUE   250
#define ORANGEHUE 15
#define WARMHUE  15
#define WARMSAT  170

byte blueHue = GREENHUE;
byte redHue = REDHUE;
byte whiteSat = WARMSAT;


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


//strobe
byte strobeType[NUMBLOCKS];    //0: no strobe  1: 50/50  2: 25/75  3: 25/75 syncopated
float strobeSpeed[NUMBLOCKS];
unsigned long beatStart;

//switch
bool switching[NUMBLOCKS];
float switchSpeed;
unsigned long lastSwitch[NUMBLOCKS];
float switchPos[NUMBLOCKS];

//spin
bool spinning[NUMBLOCKS];
float spinSpeed[NUMBLOCKS];
float initPos[NUMBLOCKS];
bool reverse[NUMBLOCKS];
bool downOnly[NUMBLOCKS];


//block control
byte selected = 0;
byte controlling = 0; // 0:hue 1:saturation 2:velocity 3: size
byte setting[3];
byte settingMap[4][7][4];


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
  
  blueHue = EEPROM.read(blueHueAddr);
  redHue = EEPROM.read(redHueAddr);
  whiteSat = EEPROM.read(whiteSatAddr);


  // Start Serial
  Serial.begin(115200);

  usbMIDI.setHandleNoteOn(OnNoteOn);
  usbMIDI.setHandleControlChange(OnControlChange);



  // start fast leds
  FastLED.addLeds<WS2812,PIXELPIN1,GRB>(pixels1, NUMLEDS);
  FastLED.addLeds<WS2812,PIXELPIN2,GRB>(pixels2, NUMLEDS);

  // initiate with last preset
  preset(EEPROM.read(presetAddr));
  // mpa presets to midi buttons
  setSettingMap();


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


