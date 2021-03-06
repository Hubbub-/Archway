void midi(){
  usbMIDI.read(); // USB MIDI receive
}

void OnNoteOn(byte channel, byte note, byte velocity) {
  Serial.print("MIDI note=");
  Serial.print(note, DEC);
  Serial.println();
  midiTrig(note);
}

void OnControlChange(byte channel, byte control, byte value) {
  Serial.print("Slider value=");
  Serial.print(value, DEC);
  Serial.println();
  slideControl(value);

}

void midiTrig(int in){
  // Side buttons
  if(in == 2){    // top side button selects blueHue
    controlling = 0;
  }
  else if(in == 3){  // 2nd side button selects redHue
    controlling = 1; 
  }
  else if(in == 4){
    controlling = 2;  // 3rd side button selects whiteWarmth
  }
  else if(in == 5){   // 4th side button reverses direction
    reverseAll();
  }
  else if(in == 6){  // bottom side button resets timing
    beatStart = millis();
  }

  

  //top row
  else if(in == 44){
    setting[0]=0;
  }
  else if(in == 45){
    setting[0]=1; 
  }
  else if(in == 46){
    setting[0]=2;
  }
  else if(in == 47){
    setting[0]=3; 
  }

  //top big row
  else if(in == 40){
    setting[1]=0;
  }
  else if(in == 41){
    setting[1]=1;
  }
  else if(in == 42){
    setting[1]=2;
  }
  else if(in == 43){
    setting[1]=3;
  }
  
  // bottom big row
  else if(in == 36){
    setting[1]=4;
  }
  else if(in == 37){
    setting[1]=5;
  }
  else if(in == 38){
    setting[1]=6;
  }
  else if(in == 39){
    for(int i=0; i<NUMPIXELS; i++){
      applyColour(i,0,0,0);
      
    }
    FastLED.show();
    delay(4000);
    for(byte i=0; i<255; i++){
      for(int j=0; j<NUMPIXELS/2; j++){
        applyColour(j,blueHue,255,i);
      }
      for(int j=NUMPIXELS/2; j<NUMPIXELS; j++){
        applyColour(j,redHue,255,i);
      }
      FastLED.show();
      delay(1);
    }
  }

  //bottom row
  if(in == 32){
    setting[2]=0;
  }
  else if(in == 33){
    setting[2]=1;
  }
  else if(in == 34){
    setting[2]=2;
  }
  else if(in == 35){
    setting[2]=3;
  }

  // update preset when bottom row pressed
  if(in >= 32 && in <= 35) preset(settingMap[setting[0]][setting[1]][setting[2]]);
  Serial.println(" ");
}

void slideControl(int val){
  if(controlling==0){        // bluehue
    blueHue = map(val,0,127,GREENHUE,BLUEHUE);
    
    EEPROM.write(blueHueAddr, blueHue);
  }
  else if(controlling==1){    // redhue
    int r = map(val,0,127,REDHUE,255+ORANGEHUE);
    if(r>255) r-=255;
    redHue = r;
    EEPROM.write(redHueAddr, redHue);
  }
  else if(controlling==2){    // warmth
    whiteSat = map(val,0,127,0,WARMSAT);
    EEPROM.write(whiteSatAddr, whiteSat);
  }

  preset(settingMap[setting[0]][setting[1]][setting[2]]);
}

