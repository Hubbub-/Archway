void midi(){
  usbMIDI.read(); // USB MIDI receive
}

void OnNoteOn(byte channel, byte note, byte velocity) {
  Serial.print("Note On, ch=");
  Serial.print(channel, DEC);
  Serial.print(", note=");
  Serial.print(note, DEC);
  Serial.print(", velocity=");
  Serial.print(velocity, DEC);
  Serial.println();
  midiTrig(note-35);
}

void OnControlChange(byte channel, byte control, byte value) {
  Serial.print("Control Change, ch=");
  Serial.print(channel, DEC);
  Serial.print(", control=");
  Serial.print(control, DEC);
  Serial.print(", value=");
  Serial.print(value, DEC);
  Serial.println();
  FastLED.setBrightness(value*2);

}

void midiTrig(int in){
  if(in == 1){
    preset(0,1);
    preset(1,0); 
  }
  else if(in == 2){
    preset(0,2);
    preset(1,2); 
  }
  else if(in == 3){
    preset(0,3);
    preset(1,0);
  }
  else if(in == 4){
    preset(0,3);
    preset(1,3);
  }
}

