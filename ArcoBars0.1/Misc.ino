void allBars(int p, byte h, byte s, byte v){
  for(byte i=0; i<4; i++){
    pixels[i][p] = CHSV(h,s,v);
  }
}

void allBars(byte h, byte s, byte v){
  for(byte i=0; i<4; i++){
    fullBar(i,h,s,v);
  }
}

void fullBar(byte b, byte h, byte s, byte v){
  for(int i=0; i<NUMLEDS; i++){
    pixels[b][i] = CHSV(h,s,v);
  }
}

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

    // For controlling blocks
    String blockString = inputString.substring(1,2);   // Get block number (2nd character)
    String commandString = inputString.substring(2,3); // Get block command character
    String numberString = inputString.substring(1,8);  // Get block command value
    int blockIn = blockString.toInt();
    int numberIn = numberString.toInt();        // Convert number string to int

    // For imitating midi commands
    String noteString = inputString.substring(1);  // Get midi Note
    int noteIn = noteString.toInt();

    // Get info by sending "i"
    if(inputString.startsWith("m")){  
      midiTrig(numberIn);
    }
    // clear the string:
    inputString = "";
    stringComplete = false;
  }
}
