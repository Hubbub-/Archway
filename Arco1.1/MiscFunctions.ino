//--------------------------------------------------------------------------

// Find the block near a certain LDR
int blockAt(int LDR){
  int selectedPixel = -1;         // -1 means there's none
  int separation = 10;            // how far the LDRs are separated (by LEDs)
  int LDRpos = LEDSpawnNum[LDR];  // Where on the LED strips is the LDR
  
  for(int i=0; i<NUMBLOCKS; i++){                      // cycle through blocks
    if(abs(blockPos[i]-LDRpos) < separation/2){   // if the block is near the LDR
      if(alive[i]) selectedPixel = i;                 // if it's alive, choose this pixel
    }
  }
  return selectedPixel;
}


//---------------------------------------------------

// map function
float mapFloat(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}


//--------------------------------------------------------------------------


//fade function
float fade(float current, float target, float amount){
  float result;
  if(current < target){           // if the current value is less than the target
    result = current + abs(amount);    // add the amount
    if(result > target){          // 
      result = target;
    }
  }
  else if(current > target){
    result = current - abs(amount);
    if(result < target){
      result = target;
    }
  }
  else if(current == target){
    result = target;
  }
  return result;
}



//--------------------------------------------------------------------------

int nextSwitching(int in){
  int result = in;
  for(int i=in+1; i<NUMBLOCKS+in; i++){
    int sel = i;
    if(sel >= NUMBLOCKS) sel -= NUMBLOCKS;   //wrap around
    if(switching[sel]){     // if it finds a switching block
      result = sel;         // save the result
      break;              // exit the for loop
    }
  }
  return result;
}

//--------------------------------------------------------------------------
 void reverseAll(){
  for(byte i=0; i<NUMBLOCKS; i++){
    if(spinning[i]){
      reverse[i] = !reverse[i];
      initPos[i] = blockPos[i];
      Serial.print("block");
      Serial.print("block");
    }
  }
 }

//--------------------------------------------------------------------------
// Apply pixel colours to the right strip
void applyColour(float pixelin, float h, float s, float v){
  int pixel = (int) pixelin;
  
  // strip1
  if(pixel%2 == 0){
    int p = pixel/2;
    pixels1[p] = CHSV(h,s,v);
  }
  
  // strip2 
  if(pixel%2 == 1){
    int p = (pixel-1)/2;
    pixels2[p] = CHSV(h,s,v);
  }
}

//--------------------------------------------------------------------------
void mixColour(float pixelin, float h, float s, float v){
  int pixel = (int) pixelin;
  
  float blendAmount = 255/(1+numBlends[pixel]);  // how much to blend with existing
  
  numBlends[pixel] ++;            // increase number of blends
  // strip1
  if(pixel%2 == 0){
    int p = pixel/2;
    pixels1[p] = nblend(pixels1[p],CHSV(h,s,v),blendAmount);
  }
  
  // strip2 
  if(pixel%2 == 1){
    int p = (pixel-1)/2;
    pixels2[p] = nblend(pixels2[p],CHSV(h,s,v),blendAmount);
  }
}
 

