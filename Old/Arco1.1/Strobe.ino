bool blackout(byte i){
  bool result;
  // 1 minute/BPM = beat length (I used 2 minutes so we could do half time)
  int strobePer = 240000/BPM/strobeSpeed[i];           // convert frequency (strobes per beat) to period

  // if blackout part of strobe 1 (50/50)
  if((millis()-beatStart) % strobePer > strobePer/2 && strobeType[i]==1){
    result = true;
  }
  // if blackout part of strobe 2 (7/93)
  else if((millis()-beatStart) % strobePer > strobePer*0.07 && strobeType[i]==2){
    result = true;
  }
  // if first blackout part of strobe 3 (50/7/43 syncopated)
  else if((millis()-beatStart) % strobePer < strobePer*0.5 && strobeType[i]==3){
    result = true;
  }
  // if second blackout part of strobe 3 (50/7/43 syncopated)
  else if((millis()-beatStart) % strobePer > strobePer*0.57 && strobeType[i]==3){
    result = true;
  }

  // if blackout part of strobe 4 (37.5/62.5)
  else if((millis()-beatStart) % strobePer > strobePer*0.375 && strobeType[i]==4){
    result = true;
  }
  // if first blackout part of strobe 5 (50/37.5/12.5 syncopated)
  else if((millis()-beatStart) % strobePer < strobePer*0.5 && strobeType[i]==5){
    
  }
  // if second blackout part of strobe 5 (50/37.5/12.5 syncopated)
  else if((millis()-beatStart) % strobePer > strobePer*0.875 && strobeType[i]==5){
    result = true;
  }

  // if first blackout part of strobe 6 (12.5/75/12/5)
  else if((millis()-beatStart) % strobePer < strobePer*0.125 && strobeType[i]==6){
    result = true;
  }
  // if second blackout part of strobe 6 (12.5/75/12.5)
  else if((millis()-beatStart) % strobePer > strobePer*0.875 && strobeType[i]==6){
    result = true;
  }

  // if blackout part of strobe 7 (44/12/44)
  else if((millis()-beatStart) % strobePer > strobePer*0.44 && (millis()-beatStart) % strobePer < strobePer*0.56  && strobeType[i]==7){
    result = true;
  }

  // if first blackout part of strobe 8 (20->10<-70)
  else if((millis()-beatStart) % strobePer < strobePer*0.2 && strobeType[i]==8){
    result = true;
  }
  // if second blackout part of strobe 8 (20->10<-70)
  else if((millis()-beatStart) % strobePer > strobePer*0.3 && strobeType[i]==8){
    result = true;
  }
  // if first blackout part of strobe 9 (70->10<-20)
  else if((millis()-beatStart) % strobePer < strobePer*0.7 && strobeType[i]==9){
    result = true;
  }
  // if second blackout part of strobe 9 (70->10<-20)
  else if((millis()-beatStart) % strobePer > strobePer*0.8 && strobeType[i]==9){
    result = true;
  }

  // if first blackout part of strobe 10 (40->20<-40)
  else if((millis()-beatStart) % strobePer < strobePer*0.4 && strobeType[i]==10){
    result = true;
  }
  // if second blackout part of strobe 10 (40->20<-40)
  else if((millis()-beatStart) % strobePer > strobePer*0.6 && strobeType[i]==10){
    result = true;
  }

  else result = false;

  return result;
}

