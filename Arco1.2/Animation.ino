void animInit(){
  animLength = millis()+random(8000,15000);
  preset(random(1,27));
  Serial.println("Mode: Animation");
}


// blocks
void animUpdate(){
  if(millis() > animLength) animInit();  //reset animation every 10 seconds
  
  for (int i=0; i<NUMBLOCKS; i++){     // cycle through blocks
    

    int spinPer = 240000/BPM/spinSpeed[i];           // convert frequency (strobes per beat) to period
    if(spinning[i]){
      if(reverse[i]){
        blockPos[i] = initPos[i] - mapFloat((millis()-beatStart) % spinPer, 0, spinPer, 0, NUMPIXELS);
        if(downOnly[i] && blockPos[i] < 0) blockPos[i] += NUMPIXELS/2;
      }
      else{
        blockPos[i] = initPos[i] + mapFloat((millis()-beatStart) % spinPer, 0, spinPer, 0, NUMPIXELS);
        if(downOnly[i] && blockPos[i] > NUMPIXELS) blockPos[i] -= NUMPIXELS/2;
      }
    }

    if(blockPos[i] > NUMPIXELS) blockPos[i] -= NUMPIXELS;
    else if(blockPos[i] < 0) blockPos[i] += NUMPIXELS;
  
    

    if(printBlocks){
      Serial.print("blockPos ");
      Serial.print(i);
      Serial.print(":");
      Serial.print(blockPos[i]);
      Serial.print(" | ");
    }
  }
  if(printBlocks) Serial.println(" ");
}



//---------------------------------------------------------------------






void animRender(){
  if(printBlocks) Serial.println(" ");
  
  // reset pixel used
  bool pixelUsed[NUMPIXELS];
  if(printStrips){
    for(int i=0; i<NUMPIXELS; i++){
      pixelUsed[i] = false;
    }
  }


  // Reset Pixel Colour to Black BG
  for(int j=0; j<NUMLEDS; j++){
    pixels1[j] = CHSV(0,0,0);
    pixels2[j] = CHSV(0,0,0);
  }
  // apply colour to the blocks
  for(int i=0; i<NUMBLOCKS; i++){
    if(switching[i]) blockPos[i] = switchPos[i];
    // switching
    int switchPer = 120000/BPM/switchSpeed;           // convert frequency (strobes per beat) to period

    //if         it's time to switch            and it hasn't just switched   and it should be switching
    if((millis()-beatStart) % switchPer < 50 && (millis()-lastSwitch[i]) > 50 && switching[i]){
      switchPos[i] = blockPos[nextSwitching(i)];
      lastSwitch[i] = millis();
    }


    if(!blackout(i) || strobeType[i]==0) {  // If not in blackout part of strobe or not strobing
      for(int j=-blockSize[i]/2; j<blockSize[i]/2; j++){
        
        int pos = blockPos[i]+j;
        if(pos > NUMPIXELS) pos -= NUMPIXELS;
        if(pos < 0) pos += NUMPIXELS;
        if(pos>0 && pos<NUMPIXELS){
          applyColour(pos, hue[i], saturation[i], brightnes[i]);
        }
        // Apply pixel types for serial printing
        // If in range
        if (pos>=0 && pos<NUMPIXELS){
          pixelUsed[pos] = true;
        }
      }
    }
  }  


  if(printStrips){
    for(int i=0; i<NUMPIXELS-4; i+=4){
      if (pixelUsed[i] == false) Serial.print("_");
      else if (pixelUsed[i] == true) Serial.print("+");
    }
    Serial.println(" ");
  }
}




//---------------------------------------------------------------------


