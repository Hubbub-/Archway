
// Read from serial
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
    String numberString = inputString.substring(3,8);  // Get block command value
    int blockIn = blockString.toInt();
    int numberIn = numberString.toInt();        // Convert number string to int

    // For imitating midi commands
    String noteString = inputString.substring(1);  // Get midi Note
    int noteIn = noteString.toInt();

    // Get info by sending "i"
    if(inputString.startsWith("i")){  
      
      Serial.print("printBlocks(pb): ");
      Serial.println(printBlocks);
      Serial.print("printStrips(ps): ");
      Serial.println(printStrips);
      Serial.print("printStrips(pm): ");
      Serial.println(printMidi);
      Serial.println("block commands(b-block#-variable-value)");
      Serial.println("eg. b0h170");
      Serial.println("midi commands(m)");
      Serial.println("slider control(s)");
    }
    

    // Control printing block info with "pb"
    else if(inputString.startsWith("pb")){
      printBlocks=!printBlocks;
      byte eepromIn = printBlocks;
      EEPROM.write(printBlocksAddr, eepromIn);
      Serial.print("printBlocks(pb): ");
      Serial.println(printBlocks);
    }

    // Control printing strips with "ps"
    else if(inputString.startsWith("ps")){
      printStrips=!printStrips;
      byte eepromIn = printStrips;
      EEPROM.write(printStripsAddr, eepromIn);
      Serial.print("printStrips(ps): ");
      Serial.println(printStrips);
    }

    // Control printing strips with "pm"
    else if(inputString.startsWith("pm")){
      printMidi=!printMidi;
      byte eepromIn = printMidi;
      EEPROM.write(printMidiAddr, eepromIn);
      Serial.print("printMidi(pm): ");
      Serial.println(printMidi);
    }

    // Block commands
    else if(inputString.startsWith("b")){   // incoming block command
      if(commandString == "w") blockWidth[blockIn] = numberIn;   // change size of block
      if(commandString == "p") blockPos[blockIn] = numberIn;   // change position of block
      if(commandString == "v") targetVel[blockIn] = numberIn;   // change velocity of block
      if(commandString == "h") hue[blockIn] = numberIn;   // change hue of block
      if(commandString == "s") saturation[blockIn] = numberIn;   // change hue of block
    }

    else if(inputString.startsWith("m")){
      midiTrig(noteIn);
    }

    else if(inputString.startsWith("s")){
      slideControl(noteIn);
    }

    else if(inputString.startsWith("q")){
      FastLED.setBrightness(numberIn);
    }


    Serial.println("-----------------------");

    // clear the string:
    inputString = "";
    stringComplete = false;
  }
}
