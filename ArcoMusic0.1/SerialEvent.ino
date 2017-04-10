
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

    String blockString = inputString.substring(1,2);   // Get number following letter (2nd character)
    String commandString = inputString.substring(2,3); // Get character following strip number
    String numberString = inputString.substring(3,8);  // Get number following cammand letter
    int blockIn = blockString.toInt();
    int numberIn = numberString.toInt();        // Convert number string to int

    // Get info by sending "i"
    if(inputString.startsWith("i")){  
      
      Serial.print("printBlocks(pb): ");
      Serial.println(printBlocks);
      Serial.print("printStrips(ps): ");
      Serial.println(printStrips);
      Serial.println("block commands(b-block#-variable-value)");
      Serial.println("eg. b0h170");
      Serial.print("midi commands(m): ");
    }
    

    // Control printing block info with "pb"
    else if(inputString.startsWith("pb")){
      printBlocks=!printBlocks;
      byte eepromIn = printBlocks;
      EEPROM.write(printBlocksAddr, eepromIn);
      Serial.print("printBlocks(p): ");
      Serial.println(printBlocks);
    }

    // Control printing strips with "ps"
    else if(inputString.startsWith("ps")){
      printStrips=!printStrips;
      byte eepromIn = printStrips;
      EEPROM.write(printStripsAddr, eepromIn);
      Serial.print("printStrips(s): ");
      Serial.println(printStrips);
    }

    else if(inputString.startsWith("b")){   // incoming block command
      if(commandString == "w") blockWidth[blockIn] = numberIn;   // change size of block
      if(commandString == "p") blockPos[blockIn] = numberIn;   // change position of block
      if(commandString == "v") targetVel[blockIn] = numberIn;   // change velocity of block
      if(commandString == "h") hue[blockIn] = numberIn;   // change hue of block
      if(commandString == "s") saturation[blockIn] = numberIn;   // change hue of block
    }

    else if(inputString.startsWith("m")){
      midiTrig(blockIn);
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
