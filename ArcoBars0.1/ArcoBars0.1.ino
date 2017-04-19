
#include <EEPROM.h>
#include "FastLED.h"


// Define pin and pixel number for LEDs
#define PIXELPIN1      3
#define PIXELPIN2      4
#define PIXELPIN3      5
#define PIXELPIN4      6
#define NUMLEDS        120   //Number of LEDs per strip
#define NUMPIXELS      480   //Total number of LEDs (all strips)

#define BPM   106

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

byte blueHue = BLUEHUE;
byte redHue = REDHUE;
byte whiteSat = WARMSAT;

CRGB pixels[4][NUMLEDS];

byte hue;
byte sat = 255;
byte bright = 255;
bool pulsing = false;
unsigned long beatStart;


byte controlling;

void setup() {
  // Start Serial
  Serial.begin(115200);

  usbMIDI.setHandleNoteOn(OnNoteOn);
  usbMIDI.setHandleControlChange(OnControlChange);

  // start fast leds
  FastLED.addLeds<WS2812,PIXELPIN1,GRB>(pixels[0], NUMLEDS);
  FastLED.addLeds<WS2812,PIXELPIN2,GRB>(pixels[1], NUMLEDS);
  FastLED.addLeds<WS2812,PIXELPIN3,GRB>(pixels[2], NUMLEDS);
  FastLED.addLeds<WS2812,PIXELPIN4,GRB>(pixels[3], NUMLEDS);

  blueHue = EEPROM.read(blueHueAddr);
  redHue = EEPROM.read(redHueAddr);
  whiteSat = EEPROM.read(whiteSatAddr);
  hue = blueHue;

  
  
  for(int i=0; i<NUMLEDS; i++){
    byte b = map(i,0,NUMLEDS,0,255);
    allBars(i,0,0,b);
    FastLED.show();
    delay(1);
  }
  for(int i=NUMLEDS-1; i>0; i--){
    allBars(i,0,0,255);
    FastLED.show();
    delay(1);
  }


}

void loop() {
  usbMIDI.read(); // USB MIDI receive
  barsUpdate();  //update blocks

  FastLED.show(); // This sends the updated pixel color to the hardware.

}




void barsUpdate(){
  if (pulsing) bright = 255-beat8(BPM,beatStart);
  allBars(hue,sat,bright);
}


