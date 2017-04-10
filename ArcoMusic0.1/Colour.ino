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
