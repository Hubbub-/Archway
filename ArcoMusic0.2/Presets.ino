void preset(int block, int value){
  for(int i=0; i<NUMBLOCKS; i++){
    blockWidth[i] = 0;
    blockPos[i] = 0;
    targetVel[i] = 0;
    vel[i] = 0;
    hue[i] = BLUEHUE;
    saturation[i] = 255;
    blockWidth[i] = 0;
    strobeType[i] = 0;
    switching[i] = false;
  }
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
    strobeType[block]++;
    if(strobeType[block] > 3) strobeType[block]=0;
  }
}

void preset(int value){
  if(value == 1){             // half blue, half red
    blockWidth[0] = NUMPIXELS/2;
    blockPos[0] = NUMPIXELS/4;
    hue[0] = BLUEHUE;
    saturation[0] = 255; 
    blockWidth[1] = NUMPIXELS/2;
    blockPos[1] = NUMPIXELS*0.75;
    hue[1] = REDHUE;
    saturation[1] = 255;
  }

  else if(value == 2){             // half blue, half red, switching
    blockWidth[0] = NUMPIXELS/2;
    blockPos[0] = NUMPIXELS/4;
    hue[0] = BLUEHUE; 
    switching[0] = true;
    blockWidth[1] = NUMPIXELS/2;
    blockPos[1] = NUMPIXELS*0.75;
    hue[1] = REDHUE;
    switching[1] = true;
    switchSpeed = 2;
  }

  else if(value == 3){            // half blue, half red, spinning
    blockWidth[0] = NUMPIXELS/2;
    blockPos[0] = NUMPIXELS/4;
    targetVel[0] = SPINSPEED;
    hue[0] = BLUEHUE;
    blockWidth[1] = NUMPIXELS/2;
    blockPos[1] = NUMPIXELS*0.75;
    targetVel[1] = SPINSPEED;
    hue[1] = REDHUE;
  }

  else if(value == 4){            // half blue, half red, switching, strobing white blocks alternating
    for(int b=0; b<4; b++){
      blockWidth[b] = NUMPIXELS/2;
    }
    blockPos[0] = NUMPIXELS/4;
    hue[0] = BLUEHUE; 
    switching[0] = true;
    blockPos[1] = NUMPIXELS*0.75;
    hue[1] = REDHUE;
    switching[1] = true;
    switchSpeed = 2;
    blockPos[2] = NUMPIXELS/4;
    saturation[2] = 0; 
    strobeType[2] = 2;
    strobeSpeed[2] = 1;
    blockPos[3] = NUMPIXELS*0.75;
    saturation[3] = 0;
    strobeType[3] = 3;
    strobeSpeed[3] = 1;
  }

  else if(value == 5){            // red and white quarters, switching
    hue[0] = REDHUE; 
    saturation[1] = 0;
    hue[2] = REDHUE; 
    saturation[3] = 0;
    switchSpeed = 2;
    for(int b=0; b<4; b++){
      blockWidth[b] = NUMPIXELS/4;
      blockPos[b] = NUMPIXELS*((b*2+1)/8);
      switching[b] = true;
    }
  }

  else if(value == 6){            // red and white quarters, spinning
    hue[0] = REDHUE; 
    saturation[1] = 0;
    hue[2] = REDHUE; 
    saturation[3] = 0;
    for(int b=0; b<4; b++){
      blockWidth[b] = NUMPIXELS/4;
      blockPos[b] = NUMPIXELS*((b*2+1)/8);
      targetVel[b] = SPINSPEED;
    }
  }

  else if(value == 7){            // four white eighths, spinning fast
    for(int b=0; b<4; b++){
      blockWidth[b] = NUMPIXELS/8;
      blockPos[b] = NUMPIXELS*((b*2+1)/8);
      targetVel[b] = SPINSPEED*2;
      saturation[b] = 0;
    }
  }

  else if(value == 8){            // four white eighths, spinning slow
    for(int b=0; b<4; b++){
      blockWidth[b] = NUMPIXELS/8;
      blockPos[b] = NUMPIXELS*((b*2+1)/8);
      targetVel[b] = SPINSPEED;
      saturation[b] = 0;
    }
  }

  else if(value == 9){            // red and white eighths, spinning fast
    for(int b=0; b<4; b++){
      blockWidth[b] = NUMPIXELS/4;
      blockPos[b] = NUMPIXELS*((b*2+1)/16);
      targetVel[b] = SPINSPEED*2;
    }
    for(int b=0; b<8; b+=2) hue[b] = REDHUE;
    for(int b=1; b<8; b+=2) saturation[b] = 0;
  }

  else if(value == 10){            // red and white eighths, spinning slow
    for(int b=0; b<4; b++){
      blockWidth[b] = NUMPIXELS/4;
      blockPos[b] = NUMPIXELS*((b*2+1)/16);
      targetVel[b] = SPINSPEED;
    }
    for(int b=0; b<8; b+=2) hue[b] = REDHUE;
    for(int b=1; b<8; b+=2) saturation[b] = 0;
  }

  else if(value == 11){           // three white eighths, strobing
    for(int b=0; b<3; b++){
      blockWidth[b] = NUMPIXELS/8;
      blockPos[b] = NUMPIXELS*0.25*(b+1);
      strobeType[b] = 1;
      strobeSpeed[b] = 2;
      saturation[b] = 0;
    }
  }

  else if(value == 12){            // white eighths, switching and strobing
    switchSpeed = 3;
    for(int b=0; b<8; b+=2){
      blockWidth[b] = NUMPIXELS/8;
      blockPos[b] = NUMPIXELS*((b*2+1)/16);
      saturation[b] = 0;
      switching[b] = true;
      strobeType[b] = 1;
      strobeSpeed[b] = 2;
    }
    for(int b=1; b<8; b+=2){
      blockWidth[b] = 0;
      blockPos[b] = NUMPIXELS*((b*2+1)/16);
      switching[b] = true;
    }
  }

  else if(value == 13){            // white quarters, switching
    switchSpeed = 3;
    for(int b=0; b<4; b+=2){
      blockWidth[b] = NUMPIXELS/8;
      blockPos[b] = NUMPIXELS*((b*2+1)/8);
      saturation[b] = 0;
      switching[b] = true;
    }
    for(int b=1; b<4; b+=2){
      blockWidth[b] = 0;
      blockPos[b] = NUMPIXELS*((b*2+1)/8);
      switching[b] = true;
    }
  }
  
  else if(value == 14){            // white quarters, switching and strobing
    switchSpeed = 3;
    for(int b=0; b<4; b+=2){
      blockWidth[b] = NUMPIXELS/8;
      blockPos[b] = NUMPIXELS*((b*2+1)/8);
      saturation[b] = 0;
      switching[b] = true;
      strobeSpeed[b] = 2;
      strobeType[b] = 1;
    }
    for(int b=1; b<4; b+=2){
      blockWidth[b] = 0;
      blockPos[b] = NUMPIXELS*((b*2+1)/8);
      switching[b] = true;
    }
  }

  else if(value == 15){            // red and white quarters, switching fast
    hue[0] = REDHUE; 
    saturation[1] = 0;
    hue[2] = REDHUE; 
    saturation[3] = 0;
    switchSpeed = 3;
    for(int b=0; b<4; b++){
      blockWidth[b] = NUMPIXELS/4;
      blockPos[b] = NUMPIXELS*((b*2+1)/8);
      switching[b] = true;
    }
  }

  else if(value == 16){            // red and white quarters, switching fast and strobing
    hue[0] = REDHUE; 
    saturation[1] = 0;
    hue[2] = REDHUE; 
    saturation[3] = 0;
    switchSpeed = 3;
    for(int b=0; b<4; b++){
      blockWidth[b] = NUMPIXELS/4;
      blockPos[b] = NUMPIXELS*((b*2+1)/8);
      switching[b] = true;
      strobeSpeed[b] = 3;
      strobeType[b] = 1;
    }
  }
}
