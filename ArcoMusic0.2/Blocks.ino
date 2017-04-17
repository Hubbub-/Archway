
// blocks
void blocksUpdate(){
  for (int i=0; i<NUMBLOCKS; i++){     // cycle through blocks
    blockPos[i] += vel[i];
    if(blockPos[i] > NUMPIXELS){
      blockPos[i] = 0;
    }
    if(blockPos[i] < 0){
      blockPos[i] = NUMPIXELS;
    }

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

    
    // Update Velocities
    // limit targetVel
    if(abs(targetVel[i])<5) targetVel[i]=0;  // make targetvel 0 if it's really small

    
    //if the absolute velocity is slower than the absolute target, change quickly to the target
    if(abs(vel[i])<abs(targetVel[i]) && targetVel[i]!=0){
      vel[i]=fade(vel[i],targetVel[i],targetVel[i]*0.06);
    }
    //if the absolute velocity is faster than absolute target, change slowly to the target
    //and set the target to 0 if it isn't already
    else if (abs(vel[i])>=abs(targetVel[i])){
      vel[i]=fade(vel[i],targetVel[i],vel[i]*0.05);
    }
  

    
    

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






void blocksRender(){
  if(printBlocks) Serial.println(" ");
  
  // reset pixel used
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

    
    //strobe
    // 1 minute/BPM = beat length (I used 2 minutes so we could do half time)
    int strobePer = 240000/BPM/strobeSpeed[i];           // convert frequency (strobes per beat) to period

    // if blackout part of strobe 1 (50/50)
    if((millis()-beatStart) % strobePer > strobePer/2 && strobeType[i]==1){
      
    }
    // if blackout part of strobe 2 (7/93)
    else if((millis()-beatStart) % strobePer > strobePer*0.07 && strobeType[i]==2){
      
    }
    // if first blackout part of strobe 3 (50/7/43 syncopated)
    else if((millis()-beatStart) % strobePer < strobePer*0.5 && strobeType[i]==3){
      
    }
    // if second blackout part of strobe 3 (50/7/43 syncopated)
    else if((millis()-beatStart) % strobePer > strobePer*0.57 && strobeType[i]==3){
      
    }

    // if blackout part of strobe 4 (37.5/62.5)
    else if((millis()-beatStart) % strobePer > strobePer*0.375 && strobeType[i]==4){
      
    }
    // if first blackout part of strobe 5 (50/37.5/12.5 syncopated)
    else if((millis()-beatStart) % strobePer < strobePer*0.5 && strobeType[i]==5){
      
    }
    // if second blackout part of strobe 5 (50/37.5/12.5 syncopated)
    else if((millis()-beatStart) % strobePer > strobePer*0.875 && strobeType[i]==5){
      
    }

    // if first blackout part of strobe 6 (12.5/75/12/5)
    else if((millis()-beatStart) % strobePer < strobePer*0.125 && strobeType[i]==6){
      
    }
    // if second blackout part of strobe 6 (12.5/75/12.5)
    else if((millis()-beatStart) % strobePer > strobePer*0.875 && strobeType[i]==6){
      
    }

    // if blackout part of strobe 7 (44/12/44)
    else if((millis()-beatStart) % strobePer > strobePer*0.44 && (millis()-beatStart) % strobePer < strobePer*0.56  && strobeType[i]==7){
      
    }

    // if first blackout part of strobe 8 (20->10<-70)
    else if((millis()-beatStart) % strobePer < strobePer*0.2 && strobeType[i]==8){
      
    }
    // if second blackout part of strobe 8 (20->10<-70)
    else if((millis()-beatStart) % strobePer > strobePer*0.3 && strobeType[i]==8){
      
    }
    // if first blackout part of strobe 9 (70->10<-20)
    else if((millis()-beatStart) % strobePer < strobePer*0.7 && strobeType[i]==9){
      
    }
    // if second blackout part of strobe 9 (70->10<-20)
    else if((millis()-beatStart) % strobePer > strobePer*0.8 && strobeType[i]==9){
      
    }

    // if first blackout part of strobe 10 (40->20<-40)
    else if((millis()-beatStart) % strobePer < strobePer*0.4 && strobeType[i]==10){
      
    }
    // if second blackout part of strobe 10 (40->20<-40)
    else if((millis()-beatStart) % strobePer > strobePer*0.6 && strobeType[i]==10){
      
    }
   
    
    else {    // If not in blackout part of strobe
      for(int j=-blockWidth[i]/2; j<blockWidth[i]/2; j++){
        
        int pos = blockPos[i]+j;
        if(pos > NUMPIXELS) pos -= NUMPIXELS;
        if(pos < 0) pos += NUMPIXELS;
        if(pos>0 && pos<NUMPIXELS){
          applyColour(pos, hue[i], saturation[i], brightness[i]);
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

// initiate a block
void initBlock(int blockNum){
  // Replace selected block
  blockPos[blockNum] = (blockNum*NUMPIXELS)+(NUMPIXELS/2);  // put in correct place
  hue[blockNum] = random(190,270);              // a random blue/purple colour
  saturation[blockNum] = 255;                   // start with no saturation (to fade in)
  brightness[blockNum] = 255;                   // start with no brightness (to fade in)
  blockWidth[blockNum] = 100;          // the initial size of the block
  preset(blockNum,2);
}

