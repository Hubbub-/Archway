// Check for movement
bool moveDetected(){
  bool newMovement = false;
  bool movement = false;
  
  // store movement
  for(int i=0; i<4; i++){
    bool pinHigh = digitalRead(PIRpin[i]);

    if (pinHigh){
      movement = true;  //If any pin is high, there is movement
      lastMoved = millis();
    }
    
    // if there is movement but it's not stored
    if (pinHigh && !PIRbool[i]){
      PIRbool[i] = true;
      newMovement = true;
      animInit();
    }
    // if there's no movement but it's stored
    else if(!pinHigh && PIRbool[i]){
      PIRbool[i] = false;
    }
  }


  if(newMovement){
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
  if (millis() > pingTime + 50) {
    distIn = sonar.ping_cm();
    pingTime = millis();
  }
  if(printDistance){
    Serial.print("Distance: ");
    Serial.println(distIn);
  }
  if(distIn < 160 && distIn > 0){
    detected=true;
    lastDistTime = millis();
  }

  return detected;
}

