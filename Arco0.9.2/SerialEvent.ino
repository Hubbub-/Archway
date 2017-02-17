
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

    String numberString = inputString.substring(1,8);  // Get number following letter
    int numberIn = numberString.toInt();        // Convert number string to int

    // Get info by sending "i"
    if(inputString.startsWith("i")){  
      Serial.print("lastActed: ");
      Serial.println(lastActed);
      Serial.print("printBlocks(p): ");
      Serial.println(printLDRs);
      Serial.print("printLDRs(l): ");
      Serial.println(printBlocks);
      Serial.print("printTrigs(t): ");
      Serial.println(printTrigs);
      Serial.print("printMoves(m): ");
      Serial.println(printMoves);
      Serial.print("printStrips(s): ");
      Serial.println(printStrips);
      Serial.print("bright(b...): ");
      Serial.println(bright);
      Serial.println("force trigger(f...)");
      Serial.println("force idle(d)");
    }
    

    // Control printing pixel values with "p"
    else if(inputString.startsWith("p")){
      printBlocks=!printBlocks;
      byte eepromIn = printBlocks;
      EEPROM.write(printBlocksAddr, eepromIn);
      Serial.print("printBlocks(p): ");
      Serial.println(printBlocks);
    }

    // Control printing LDR values with "l"
    else if(inputString.startsWith("l")){
      printLDRs=!printLDRs;
      byte eepromIn = printLDRs;
      EEPROM.write(printLDRAddr, eepromIn);
      Serial.print("printLDRs(l): ");
      Serial.println(printLDRs);
    }

    // Control printing trigger values with "t"
    else if(inputString.startsWith("t")){
      printTrigs=!printTrigs;
      byte eepromIn = printTrigs;
      EEPROM.write(printTrigAddr, eepromIn);
      Serial.print("printTrigs(t): ");
      Serial.println(printTrigs);
    }
    
    // Control printing movement with "m"
    else if(inputString.startsWith("m")){
//      if(printMoves) printMoves=0;
//      else printMoves = 1;
      printMoves=!printMoves;
      byte eepromIn = printMoves;
      EEPROM.write(printMovesAddr, eepromIn);
      Serial.print("printMoves(m): ");
      Serial.println(printMoves);
    }

    // Control printing strips with "m"
    else if(inputString.startsWith("s")){
//      if(printStrips) printStrips=0;
//      else printStrips = 1;
      printStrips=!printStrips;
      byte eepromIn = printStrips;
      EEPROM.write(printStripsAddr, eepromIn);
      Serial.print("printStrips(s): ");
      Serial.println(printStrips);
    }

    
    // Control the brightness with string, examples: "b13" "b0" "b255"
    else if(inputString.startsWith("b")){  // Volume string
      bright = numberIn;              // Change the volume
      if(bright<0) bright = 0;           // Limit the brightness between 0 and 255
      if(bright>255) bright = 255;
      FastLED.setBrightness(bright);

//      byte eepromIn = bright;
      EEPROM.write(brightAddress, bright);  // Save the brightness to eeprom
      Serial.print("bright(b...): ");
      Serial.println(bright);
    }

    // Force trigger with "f..."
    else if(inputString.startsWith("f")){
      trig(numberIn);
    }

    // Force idle with "d"
    else if(inputString.startsWith("d")){
      forceIdle = !forceIdle;
      Serial.print("forceIdle:");
      Serial.println(forceIdle);
    }


    Serial.println("-----------------------");

    // clear the string:
    inputString = "";
    stringComplete = false;
  }
}
