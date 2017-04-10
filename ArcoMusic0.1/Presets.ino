void preset(int block, int value){
  if(value == 0){
    blockWidth[block] = 0;
  }
  else if(value == 1){
    blockWidth[block] = NUMPIXELS;
    blockPos[block] = NUMPIXELS/2;
    targetVel[block] = 0;
    vel[block] = 0;
  }
  else if(value == 2){
    blockWidth[block] = NUMPIXELS/NUMBLOCKS;
    blockPos[block] = blockWidth[block]*block + blockWidth[block]/2;
    targetVel[block] = 0;
    vel[block] = 0;
  }
  else if(value == 3){
    targetVel[block] = 10;
    blockWidth[block] = 60;
  }
}
