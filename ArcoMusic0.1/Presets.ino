void preset(int block, int value){
  if(value == 0){               // disappear
    blockWidth[block] = 0;
  }
  else if(value == 1){          // full
    blockWidth[block] = NUMPIXELS;
    blockPos[block] = NUMPIXELS/2;
    targetVel[block] = 0;
    vel[block] = 0;
  }
  else if(value == 2){          // half
    blockWidth[block] = NUMPIXELS/NUMBLOCKS;
    blockPos[block] = blockWidth[block]*block + blockWidth[block]/2;
    targetVel[block] = 0;
    vel[block] = 0;
  }
  else if(value == 3){          // spinning
    targetVel[block] = 10;
    blockWidth[block] = 60;
  }
  else if(value == 4){          // strobe
    strobing[block]=!strobing[block];
  }
}
