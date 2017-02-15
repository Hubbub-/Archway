
// Read LDRs
void LDRs() {
  
// Read the LDR values using the shift Register
  shiftReg();  //function after loop
  heldSince ++;  //increase the "heldSince" value
  
  if(heldSince > 5){  //only check LDR values every 5 loops
    heldSince = 0;
  }
  

  for(int i=0; i<NUMLDRS; i++){   //cycle through LDRs
    // change the min and max values
    int difference = abs(maxBright[i]-minBright[i]);
    int middle = minBright[i]+difference/2;
    
    if (heldSince == 0) {                   // if "heldsince" is 0 (only happens every 5 loops)
      // if there's a significant change in reading and it hasn't just gone to idle
      if (abs(heldVal[i]-LDR[i]) > middle*0.12+10 && millis() > idleStart+1000) {  
        trig(i);                  // trigger the pixel spawn/explode
      }

      heldVal[i] = LDR[i];  // store the value (for comparison next time)
    }

    
    // bring max and min brightness closer together if far apart
    if(difference > middle*0.1){
      maxBright[i] -= 0.001;    
      minBright[i] += 0.001;
    }
    // set new max and min brightness
    if(LDR[i] > maxBright[i]) maxBright[i] = LDR[i];
    if(LDR[i] < minBright[i]) minBright[i] = LDR[i];

    // print the LDR values if wanted
    if(printLDRs){
      Serial.print(i);
      Serial.print(": ");
      Serial.print(LDR[i]);
      Serial.print(" | ");  
    }
  }
  if(printLDRs) Serial.println(" ");
}



//-----------------------------------------------------------------------------




// LDR Shift Register stuff
void shiftReg() {
  for(int i = 0; i<NUMLDRS; i++){       // Cycle through the LDRs
    if(i==0) digitalWrite(data, HIGH);  // Start the cycle with a high bit
    else digitalWrite(data, LOW);       // Otherwise a low bit
    digitalWrite(shiftClock, HIGH);     // Add the bit to the SR memory
    digitalWrite(shiftClock, LOW);
    digitalWrite(shiftLive, HIGH);      // Makes the SR "Live"
    digitalWrite(shiftLive, LOW);
    delay(1);
    // Read the resistance of the live LDR
//    if(i==0) LDR[NUMLDRS-1] = analogRead(LDRpin);  
//    else LDR[i-1] = analogRead(LDRpin);
    LDR[i] = analogRead(LDRpin);
  }
}
