// Check for movement
bool moveDetected(){
  bool newMovement = false;
  bool movement = false;
  
  // store movement
  for(int i=0; i<4; i++){
    bool pinHigh = digitalRead(PIRpin[i]);

    if (pinHigh) movement = true;  //If any pin is high, there is movement
    
    // if there is movement but it's not stored
    if (pinHigh && !PIRbool[i]){
      PIRbool[i] = true;
      newMovement = true;
    }
    // if there's no movement but it's stored
    else if(!pinHigh && PIRbool[i]){
      PIRbool[i] = false;
    }
  }


  if(newMovement){  
    lastMoved = millis();
    if(printMoves){
      Serial.print("movement: ");
      for(int i=0; i<4; i++){
        Serial.print(i);
        Serial.print(": ");
        Serial.print(PIRbool[i]);
        Serial.print(" | ");
      }
      Serial.println(" ");
    }
  }
  return movement;
}




//--------------------------------------------------------------------------


bool distanceDetected(){
  bool detected = false;
  // check distance sensor

  return detected;
}

