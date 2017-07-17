
// blocks
void blocksUpdate(){
  for (int i=0; i<NUMBLOCKS; i++){     // cycle through blocks
    
    // -- TIMING --
    if (alive[i] && mode == 3){
      lifeTime[i] = millis() - lifeStart[i];    // update life time
      
      
      if(!exploding[i]){                      // if the block is not exploding, pulse
        int pulseTimer = int(lifeTime[i]*pulseSpeed[i]) % 255;
        
//        saturation[i] = map(triwave8(pulseTimer),0,255,50,255);
//        if(brightness[i]<250){
//          brightness[i] +=5;
//        }
        brightness[i] = map(triwave8(pulseTimer),0,255,0,255);
        saturation[i] = 255;
      }

      else if (exploding[i]){   // if the block is exploding
        if(blockSize[i] > 300){  
          vel[i] = 0;            // limit size
        }
        blockSize[i] += vel[i]; // increase size
      }

      if(lifeTime[i] > 3000 && exploding[i]){                // if the block is old
        if(brightness[i] > WHITEBRIGHT) brightness[i] -= 1;  // fade brightness
        saturation[i] -= 3;                                  // fade saturation
        if(saturation[i] <= 0){  
          alive[i] = false;             // kill it off when faded
          initBlock(random(0,NUMLDRS));  // initiate another block somewhere
        }
      }
    }


    // Update Velocities
    //if the absolute velocity is slower than the absolute target, change quickly to the target
    if(abs(vel[i])<abs(targetVel[i]) && targetVel[i]!=0){
      vel[i]=fade(vel[i],targetVel[i],targetVel[i]*0.06);
    }
    //if the absolute velocity is faster than absolute target, change slowly to the target
    //and set the target to 0 if it isn't already
    else if (abs(vel[i])>=abs(targetVel[i])){
      vel[i]=fade(vel[i],targetVel[i],vel[i]*0.05);
      if(targetVel[i]!=0) targetVel[i]=0;
    }
    //limit the velocity
    if(targetVel[i]>MAXVEL) targetVel[i]=MAXVEL;
    else if(targetVel[i]<-MAXVEL) targetVel[i]=-MAXVEL;
  

    

    if(printBlocks){
      Serial.print("leadPos ");
      Serial.print(i);
      Serial.print(":");
      Serial.print(blockPos[i]);
      if(exploding[i]) Serial.print(" Exploded!");
      Serial.print(" | ");
    }
  }
  if(printBlocks) Serial.println(" ");
}



//---------------------------------------------------------------------






void blocksRender(){
  if(printBlocks) Serial.println(" ");
  
  // reset pixel types
  if(printStrips){
    for(int i=0; i<NUMPIXELS; i++){
      blockType[i] = 0;
    }
  }


  // Reset Pixel Colour to White BG
  for(int j=0; j<NUMLEDS; j++){
    pixels1[j] = CHSV(0,0,WHITEBRIGHT);
    pixels2[j] = CHSV(0,0,WHITEBRIGHT);
  }
  // apply colour to the blocks
  for(int i=0; i<NUMBLOCKS; i++){
    if(exploding[i]){ 
      for(int j=-blockSize[i]/2; j<blockSize[i]/2; j++){
        
        int pos = blockPos[i]+j;
        if(pos>0 && pos<NUMPIXELS){
          mixColour(pos, hue[i], saturation[i], brightness[i]);
        }
        // Apply pixel types for serial printing
        // If in range
        if (pos>=0 && pos<NUMPIXELS && alive[i]){
          //Led:nothing + Lead:Not exploding
          if(blockType[pos]==0 && !exploding[i]) blockType[pos] = 1;
          //Led:nothing + Lead:Exploding
          else if(blockType[pos]==0 && exploding[i]) blockType[pos] = 2;
          //Led:Not Exploding + Lead:Exploding
          else if(blockType[pos]==1 && exploding[i]) blockType[pos] = 3;
          //Led:Exploding + Lead:Not Exploding
          else if(blockType[pos]==2 && !exploding[i]) blockType[pos] = 3;
        }
      }
      
      // Edge of explosion has pure colours (no mixing)
      if(vel[i] != 0){
        for(int j=-blockSize[i]/2; j<-blockSize[i]/2 + 8; j++){
          int pos = blockPos[i]+j;
          if(pos>0 && pos<NUMPIXELS){
            applyColour(pos, hue[i], saturation[i], brightness[i]);
          } 
        }
        for(int j=blockSize[i]/2; j<blockSize[i]/2 - 8; j++){
          int pos = blockPos[i]+j;
          if(pos>0 && pos<NUMPIXELS){
            applyColour(pos, hue[i], saturation[i], brightness[i]);
          } 
        }
      }

      
    }
  }
  for(int i=0; i<NUMBLOCKS; i++){
    if(!exploding[i]){ 
      for(int j=-blockSize[i]/2; j<blockSize[i]/2; j++){
        
        int pos = blockPos[i]+j;
        if(pos>0 && pos<NUMPIXELS){
          applyColour(pos, hue[i], saturation[i], brightness[i]);
        }
        // Apply pixel types for serial printing
        // If in range
        if (pos>=0 && pos<NUMPIXELS && alive[i]){
          //Led:nothing + Lead:Not exploding
          if(blockType[pos]==0 && !exploding[i]) blockType[pos] = 1;
          //Led:nothing + Lead:Exploding
          else if(blockType[pos]==0 && exploding[i]) blockType[pos] = 2;
          //Led:Not Exploding + Lead:Exploding
          else if(blockType[pos]==1 && exploding[i]) blockType[pos] = 3;
          //Led:Exploding + Lead:Not Exploding
          else if(blockType[pos]==2 && !exploding[i]) blockType[pos] = 3;
        }
      }
    }
  }

  if(printStrips){
    for(int i=0; i<NUMPIXELS-4; i+=4){
      if (blockType[i] == 0) Serial.print("_");
      else if (blockType[i] == 1) Serial.print("-");
      else if (blockType[i] == 2) Serial.print("|");
      else if (blockType[i] == 3) Serial.print("+");
    }
    Serial.println(" ");
  }
}



//---------------------------------------------------------------------

// initiate a block
void initBlock(int pos){
  // Look through array of blocks for any that are dead
  int index = 0;
  while(alive[index]){  // Only look until a dead one is found
    index++;
    if(index >= NUMBLOCKS){        // If there are no dead ones...
      index = random(0,NUMBLOCKS); // choose one at random
      break;
    }
  }
  // Replace selected block
  blockPos[index] = LEDSpawnNum[pos];  // put in correct place
  alive[index] = true;                     // make alive
  lifeStart[index] = millis();             // give lifeStart time
  hue[index] = random(0,255);              // a random colour
  saturation[index] = 0;                   // start with no saturation (to fade in)
  brightness[index] = 0;                   // start with no brightness (to fade in)
  blockSize[index] = INITBLOCKSIZE;          // the initial size of the block
  exploding[index] = false;                // make sure it's not exploding to start with

}


//--------------------------------------------------------------------------

void explode(int target){
  targetVel[target] = MAXVEL;     // change the target velocity
  exploding[target] = true;       // make explode
  saturation[target] = 255;       // Full saturation
  brightness[target] = 255;
  lifeStart[target] = millis();   // Restart life clock
  if(printTrigs){
    Serial.print(" | targetVel of[");
    Serial.print(target);
    Serial.print("] now ");
    Serial.print(targetVel[target]);
  }
}
