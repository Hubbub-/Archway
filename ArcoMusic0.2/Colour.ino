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

