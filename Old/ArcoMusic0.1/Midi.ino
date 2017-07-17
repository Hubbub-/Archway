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
    selected = 0;
  }
  else if(in == 3){  // 2nd side button selects block 2
    selected = 1; 
  }
  else if(in == 4){
    
  }
  else if(in == 5){
    
  }
  else if(in == 6){  // bottom side button resets timing
    for(int i=0; i<NUMBLOCKS; i++){
      strobeStart[i] = millis();
    }
  }

  // print information on what's selected
  if(in < 36 || in > 43){
    Serial.print("controlling block ");
    Serial.print(selected);
    Serial.print(" ");
  }

  
  //bottom row
  if(in == 32){
    controlling = 0;  // hue 
    Serial.println("hue");
  }
  else if(in == 33){
    controlling = 1;  // saturation 
    Serial.println("saturation");
  }
  else if(in == 34){
    controlling = 2;  // velocity 
    Serial.println("velocity");
  }
  else if(in == 35){
    controlling = 3;  // width 
    Serial.println("width");
  }

  //top row
  else if(in == 44){
    controlling = 4;  // strobeSpeed 
    Serial.println("strobeSpeed");
  }
  else if(in == 45){
    controlling = 1;  // saturation 
  }
  else if(in == 46){
    controlling = 2;  // velocity 
  }
  else if(in == 47){
    controlling = 3;  // width 
  }
  
  // big buttons for presets
  else if(in == 36){
    preset(0,1);
    preset(1,0); 
  }
  else if(in == 37){
    preset(0,2);
    preset(1,2); 
  }
  else if(in == 38){
    preset(0,3);
    preset(1,0);
  }
  else if(in == 39){
    preset(0,3);
    preset(1,3);
  }
  else if(in == 40){
    preset(0,4);
    preset(1,4);
  }
  Serial.println(" ");
}

void slideControl(int val){
  if(controlling==0){        // hue
    hue[selected] = map(val,0,127,190,255);
  }
  else if(controlling==1){    // saturation
    saturation[selected] = val*2;
  }
  else if(controlling==2){    // velocity
    targetVel[selected] = mapFloat(val,0,127,-20,20);
  }
  else if(controlling==3){    // width
    blockWidth[selected] = map(val,0,127,0,NUMPIXELS/2);
  }
  else if(controlling==4){    // strobe speed
    strobeSpeed[selected] = pow(2,map(val,0,127,0,4));
  }
}

