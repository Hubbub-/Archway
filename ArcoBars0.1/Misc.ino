void allBars(int p, byte h, byte s, byte v){
  for(byte i=0; i<4; i++){
    pixels[i][p] = CHSV(h,s,v);
  }
}

void allBars(byte h, byte s, byte v){
  for(byte i=0; i<4; i++){
    fullBar(i,h,s,v);
  }
}

void fullBar(byte b, byte h, byte s, byte v){
  for(int i=0; i<NUMLEDS; i++){
    pixels[b][i] = CHSV(h,s,v);
  }
}
