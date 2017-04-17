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
  if(in == 2){    // top side button selects block 0
    controlling = 0;
  }
  else if(in == 3){  // 2nd side button selects block 2
    controlling = 1; 
  }
  else if(in == 4){
    controlling = 2;
  }
  else if(in == 5){
    
  }
  else if(in == 6){  // bottom side button resets timing
    beatStart = millis();
  }

  // print information on what's selected
  if(in < 36){
    Serial.print("controlling variable ");
    Serial.print(selected);
    Serial.print(" ");
  }

  

  //top row
  else if(in == 44){
    preset(1);
  }
  else if(in == 45){
    preset(2); 
  }
  else if(in == 46){
    preset(3);
  }
  else if(in == 47){
    preset(4); 
  }

  //top big row
  else if(in == 40){
    preset(5);
  }
  else if(in == 41){
    preset(6);
  }
  else if(in == 42){
    preset(7);
  }
  else if(in == 43){
    preset(8);
  }
  
  // bottom big row
  else if(in == 36){
    preset(9);
  }
  else if(in == 37){
    preset(10);
  }
  else if(in == 38){
    preset(11);
  }
  else if(in == 39){
    preset(12);
  }

  //bottom row
  if(in == 32){
    preset(13);
  }
  else if(in == 33){
    preset(14);
  }
  else if(in == 34){
    preset(15);
  }
  else if(in == 35){
    preset(16);
  }
  Serial.println(" ");
}

void slideControl(int val){
  if(controlling==0){        // hue
    blueHue = map(val,0,127,GREENHUE,BLUEHUE);
    
    EEPROM.write(blueHueAddr, blueHue);
  }
  else if(controlling==1){    // saturation
    int r = map(val,0,127,REDHUE,255+ORANGEHUE);
    if(r>255) r-=255;
    redHue = r;
    EEPROM.write(redHueAddr, redHue);
  }
  else if(controlling==2){    // velocity
    whiteSat = map(val,0,127,0,WARMSAT);
    EEPROM.write(whiteSatAddr, whiteSat);
  }
  else if(controlling==4){    // width
    blockWidth[selected] = map(val,0,127,0,NUMPIXELS/2);
  }
  else if(controlling==5){    // strobe speed
    strobeSpeed[selected] = pow(2,map(val,0,127,0,4));
  }
}

