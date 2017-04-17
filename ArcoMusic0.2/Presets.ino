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
    strobeType[block]++;
    if(strobeType[block] > 3) strobeType[block]=0;
  }
}

void preset(int value){
  EEPROM.write(presetAddr, value);
  for(int i=0; i<NUMBLOCKS; i++){
    blockWidth[i] = 0;
    blockPos[i] = 0;
    targetVel[i] = 0;
    vel[i] = 0;
    hue[i] = WARMHUE;
    brightness[i] = 255;
    saturation[i] = 255;
    blockWidth[i] = 0;
    strobeType[i] = 0;
    switching[i] = false;
    spinning[i] = false;
    spinSpeed[i] = 1;
    reverse[i] = false;
  }
  if(value == 1){             // half blue, half red
    blockWidth[0] = NUMPIXELS/2;
    blockPos[0] = NUMPIXELS/4;
    hue[0] = blueHue;
    saturation[0] = 255; 
    blockWidth[1] = NUMPIXELS/2;
    blockPos[1] = NUMPIXELS*0.75;
    hue[1] = redHue;
    saturation[1] = 255;
  }

  else if(value == 2){             // half blue, half red, switching
    blockWidth[0] = NUMPIXELS/2;
    blockPos[0] = NUMPIXELS/4;
    hue[0] = blueHue; 
    switching[0] = true;
    blockWidth[1] = NUMPIXELS/2;
    blockPos[1] = NUMPIXELS*0.75;
    hue[1] = redHue;
    switching[1] = true;
    switchSpeed = 2;
  }

  else if(value == 3){            // half blue, half red, spinning slow
    blockWidth[0] = NUMPIXELS/2;
    blockPos[0] = NUMPIXELS/4;
    spinning[0] = true;
    spinSpeed[0] = 1;
    initPos[0] = blockPos[0];
    hue[0] = blueHue;
    blockWidth[1] = NUMPIXELS/2;
    blockPos[1] = NUMPIXELS*0.75;
    hue[1] = redHue;
    spinning[1] = true;
    spinSpeed[1] = 1;
    initPos[1] = blockPos[1];
  }

  else if(value == 4){            // half blue, half red, spinning fast
    blockWidth[0] = NUMPIXELS/2;
    blockPos[0] = NUMPIXELS/4;
    spinning[0] = true;
    spinSpeed[0] = 2;
    initPos[0] = blockPos[0];
    hue[0] = blueHue;
    blockWidth[1] = NUMPIXELS/2;
    blockPos[1] = NUMPIXELS*0.75;
    hue[1] = redHue;
    spinning[1] = true;
    spinSpeed[1] = 2;
    initPos[1] = blockPos[1];
  }

  else if(value == 5){            // half blue, half red, switching, strobing white blocks alternating
    for(int b=0; b<4; b++){
      blockWidth[b] = NUMPIXELS/2;
    }
    blockPos[0] = NUMPIXELS/4;
    hue[0] = blueHue; 
    switching[0] = true;
    blockPos[1] = NUMPIXELS*0.75;
    hue[1] = redHue;
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

  else if(value == 6){            // red and white quarters, switching
    hue[0] = redHue; 
    saturation[1] = whiteSat;
    hue[2] = redHue; 
    saturation[3] = whiteSat;
    switchSpeed = 2;
    for(int b=0; b<4; b++){
      blockWidth[b] = NUMPIXELS/4;
      blockPos[b] = ((b*2+1)*0.125)*NUMPIXELS;
      switching[b] = true;
    }
  }

  else if(value == 7){            // red and white quarters, spinning slow
    hue[0] = redHue; 
    saturation[1] = whiteSat;
    hue[2] = redHue; 
    saturation[3] = whiteSat;
    for(int b=0; b<4; b++){
      blockWidth[b] = NUMPIXELS/4;
      blockPos[b] = ((b*2+1)*0.125)*NUMPIXELS;
      spinning[b] = true;
      spinSpeed[b] = 1;
      initPos[b] = blockPos[b];
    }
  }

  else if(value == 8){            // red and white quarters, spinning fast
    hue[0] = redHue; 
    saturation[1] = whiteSat;
    hue[2] = redHue; 
    saturation[3] = whiteSat;
    for(int b=0; b<4; b++){
      blockWidth[b] = NUMPIXELS/4;
      blockPos[b] = ((b*2+1)*0.125)*NUMPIXELS;
      spinning[b] = true;
      spinSpeed[b] = 2;
      initPos[b] = blockPos[b];
    }
  }

  else if(value == 9){            // red with white blocks strobing
    for(int b=0; b<4; b++){
      blockWidth[b] = NUMPIXELS/2;
    }
    blockPos[0] = NUMPIXELS/4;
    hue[0] = redHue; 
    switching[0] = true;
    blockPos[1] = NUMPIXELS*0.75;
    hue[1] = redHue;
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

  else if(value == 10){            // half blue, half red, switching, strobing white blocks alternating
    for(int b=0; b<4; b++){
      blockWidth[b] = NUMPIXELS/2;
    }
    blockPos[0] = NUMPIXELS/4;
    hue[0] = redHue; 
    switching[0] = true;
    blockPos[1] = NUMPIXELS*0.75;
    hue[1] = redHue;
    switching[1] = true;
    switchSpeed = 2;
    blockPos[2] = NUMPIXELS/4;
    saturation[2] = 0; 
    strobeType[2] = 2;
    strobeSpeed[2] = 2;
    blockPos[3] = NUMPIXELS*0.75;
    saturation[3] = 0;
    strobeType[3] = 3;
    strobeSpeed[3] = 2;
  }
  
  else if(value == 11){            // three white eighths, spinning 
    for(int b=0; b<3; b++){
      blockWidth[b] = NUMPIXELS/16;
      blockPos[b] = ((b*2+1)*0.166)*NUMPIXELS;
      saturation[b] = whiteSat;
      spinning[b] = true;
      spinSpeed[b] = 1.33;
      initPos[b] = blockPos[b];
    }
  }

  else if(value == 12){            // four blue eighths, spinning 
    for(int b=0; b<3; b++){
      blockWidth[b] = NUMPIXELS/16;
      blockPos[b] = ((b*2+1)*0.166)*NUMPIXELS;
      hue[b] = blueHue;
      spinning[b] = true;
      spinSpeed[b] = 1.33;
      initPos[b] = blockPos[b];
    }
  }

  else if(value == 13){            // four red eighths, spinning 
    for(int b=0; b<3; b++){
      blockWidth[b] = NUMPIXELS/16;
      blockPos[b] = ((b*2+1)*0.166)*NUMPIXELS;
      hue[b] = redHue;
      spinning[b] = true;
      spinSpeed[b] = 1.33;
      initPos[b] = blockPos[b];
    }
  }

  else if(value == 14){            // three white eighths, switching, strobing
    switchSpeed=1;
    for(int b=0; b<7; b+=2){
      blockWidth[b] = NUMPIXELS/16;
      blockPos[b] = ((b*2+1)*0.071)*NUMPIXELS;
      saturation[b] = 0;
      initPos[b] = blockPos[b];
      strobeType[b] = 4;
      strobeSpeed[b] = 2;
    }
    for(int b=1; b<7; b+=2){
      blockWidth[b] = NUMPIXELS/16;
      blockPos[b] = ((b*2+1)*0.071)*NUMPIXELS;
      saturation[b] = 0;
      initPos[b] = blockPos[b];
      strobeType[b] = 5;
      strobeSpeed[b] = 2;
    }
  }
  
  else if(value == 15){            // three white eighths, switching, strobing
    switchSpeed=1;
    for(int b=0; b<7; b+=2){
      blockWidth[b] = NUMPIXELS/16;
      blockPos[b] = ((b*2+1)*0.071)*NUMPIXELS;
      saturation[b] = 0;
      initPos[b] = blockPos[b];
      strobeType[b] = 4;
      strobeSpeed[b] = 4;
    }
    for(int b=1; b<7; b+=2){
      blockWidth[b] = NUMPIXELS/16;
      blockPos[b] = ((b*2+1)*0.071)*NUMPIXELS;
      saturation[b] = 0;
      initPos[b] = blockPos[b];
      strobeType[b] = 5;
      strobeSpeed[b] = 4;
    }
  }

  else if(value == 16){            // half white, spinning slow
    blockWidth[0] = NUMPIXELS/2;
    blockPos[0] = NUMPIXELS/4;
    spinning[0] = true;
    spinSpeed[0] = 1;
    initPos[0] = blockPos[0];
    saturation[0] = whiteSat;
  }

  else if(value == 17){            // half white, spinning fast
    blockWidth[0] = NUMPIXELS/2;
    blockPos[0] = NUMPIXELS/4;
    spinning[0] = true;
    spinSpeed[0] = 2;
    initPos[0] = blockPos[0];
    saturation[0] = whiteSat;
  }

  else if(value == 18){            // half white, spinning slow, blinking
    blockWidth[0] = NUMPIXELS/2;
    blockPos[0] = NUMPIXELS/4;
    spinning[0] = true;
    strobeType[0] = 6;
    strobeSpeed[0] = 4;
    spinSpeed[0] = 2;
    initPos[0] = blockPos[0];
    saturation[0] = whiteSat;
  }

  else if(value == 19){            // half white, spinning fast, with red segment in dark
    blockWidth[0] = NUMPIXELS/2;
    blockPos[0] = NUMPIXELS/4;
    saturation[0] = whiteSat;
    blockWidth[1] = NUMPIXELS/24;
    blockPos[1] = NUMPIXELS*0.75;
    hue[1] = redHue;
    for(int b=0; b<2; b++){
      spinning[b] = true;
      spinSpeed[b] = 2;
      initPos[b] = blockPos[b];
    }
  }

  else if(value == 20){            // red and white quarters, switching
    brightness[0] = 0; 
    saturation[1] = whiteSat;
    brightness[2] = 0; 
    saturation[3] = whiteSat;
    switchSpeed = 2;
    for(int b=0; b<4; b++){
      blockWidth[b] = NUMPIXELS/4;
      blockPos[b] = ((b*2+1)*0.125)*NUMPIXELS;
      switching[b] = true;
    }
  }

  else if(value == 21){            // red and white quarters, switching
    brightness[0] = 0; 
    saturation[1] = whiteSat;
    brightness[2] = 0; 
    saturation[3] = whiteSat;
    switchSpeed = 2;
    strobeType[1] = 6;
    strobeSpeed[1] = 4;
    strobeType[3] = 6;
    strobeSpeed[3] = 4;
    for(int b=0; b<4; b++){
      blockWidth[b] = NUMPIXELS/4;
      blockPos[b] = ((b*2+1)*0.125)*NUMPIXELS;
      switching[b] = true;
    }
  }

  else if(value == 22){            // red and white quarters, switching
    brightness[0] = 0; 
    saturation[1] = whiteSat;
    brightness[2] = 0; 
    saturation[3] = whiteSat;
    switchSpeed = 2;
    strobeType[1] = 7;
    strobeSpeed[1] = 4;
    strobeType[3] = 7;
    strobeSpeed[3] = 4;
    for(int b=0; b<4; b++){
      blockWidth[b] = NUMPIXELS/4;
      blockPos[b] = ((b*2+1)*0.125)*NUMPIXELS;
      switching[b] = true;
    }
  }

  else if(value == 23){            // white half, switching, with red strobe in darkness
    saturation[0] = whiteSat;
    brightness[1] = 0; 
    switchSpeed = 2;
    for(int b=0; b<2; b++){
      blockWidth[b] = NUMPIXELS/2;
      blockPos[b] = ((b*2+1)*0.25)*NUMPIXELS;
      switching[b] = true;
    }
    for(int b=2; b<4; b++){
      blockWidth[b] = NUMPIXELS/8;
//      blockPos[b] = ((b*2+1)*0.25)*NUMPIXELS;
      strobeSpeed[b] = 2;
      hue[b] = redHue;
    }
    blockPos[2] = NUMPIXELS*0.25;
    blockPos[3] = NUMPIXELS*0.75;
    strobeType[2] = 9;
    strobeType[3] = 8;
  }

  else if(value == 24){            // white half, switching, with blue strobe in darkness
    saturation[0] = whiteSat;
    brightness[1] = 0; 
    switchSpeed = 2;
    for(int b=0; b<2; b++){
      blockWidth[b] = NUMPIXELS/2;
      blockPos[b] = ((b*2+1)*0.25)*NUMPIXELS;
      switching[b] = true;
    }
    for(int b=2; b<4; b++){
      blockWidth[b] = NUMPIXELS/8;
      strobeSpeed[b] = 2;
      hue[b] = blueHue;
    }
    blockPos[2] = NUMPIXELS*0.25;
    blockPos[3] = NUMPIXELS*0.75;
    strobeType[2] = 9;
    strobeType[3] = 8;
  }

  else if(value == 25){            // red half, switching, with white strobe in darkness
    hue[0] = redHue;
    brightness[1] = 0; 
    switchSpeed = 2;
    for(int b=0; b<2; b++){
      blockWidth[b] = NUMPIXELS/2;
      blockPos[b] = ((b*2+1)*0.25)*NUMPIXELS;
      switching[b] = true;
    }
    for(int b=2; b<4; b++){
      blockWidth[b] = NUMPIXELS/6;
//      blockPos[b] = ((b*2+1)*0.25)*NUMPIXELS;
      strobeSpeed[b] = 2;
      saturation[b] = 0;
    }
    blockPos[2] = NUMPIXELS*0.25;
    blockPos[3] = NUMPIXELS*0.75;
    strobeType[2] = 9;
    strobeType[3] = 8;
  }

  else if(value == 26){            // red half, switching, with white strobe in darkness
    hue[0] = blueHue;
    brightness[1] = 0; 
    switchSpeed = 2;
    for(int b=0; b<2; b++){
      blockWidth[b] = NUMPIXELS/2;
      blockPos[b] = ((b*2+1)*0.25)*NUMPIXELS;
      switching[b] = true;
    }
    for(int b=2; b<4; b++){
      blockWidth[b] = NUMPIXELS/6;
//      blockPos[b] = ((b*2+1)*0.25)*NUMPIXELS;
      strobeSpeed[b] = 2;
      saturation[b] = 0;
    }
    blockPos[2] = NUMPIXELS*0.25;
    blockPos[3] = NUMPIXELS*0.75;
    strobeType[2] = 9;
    strobeType[3] = 8;
  }

  else if(value == 27){            // two white eighths, spinning opposite directions
    spinSpeed[0] = 1;
    spinSpeed[1] = 1;
    reverse[1] = true;
    for(int b=0; b<2; b++){
      blockWidth[b] = NUMPIXELS/16;
      blockPos[b] = NUMPIXELS/2;
      saturation[b] = whiteSat;
      spinning[b] = true;
      initPos[b] = blockPos[b];
    }
  }

//  else if(value == 23){            // white half, switching, with red strobe in darkness
//    saturation[0] = whiteSat;
//    brightness[1] = 0; 
//    switchSpeed = 2;
//    for(int b=0; b<2; b++){
//      blockWidth[b] = NUMPIXELS/2;
//      blockPos[b] = ((b*2+1)*0.25)*NUMPIXELS;
//      switching[b] = true;
//    }
//    for(int b=2; b<4; b++){
//      blockWidth[b] = NUMPIXELS/8;
//      strobeSpeed[b] = 4;
//      hue[b] = redHue;
//      strobeType[b] = 10;
//    }
//    blockPos[2] = NUMPIXELS*0.25;
//    blockPos[3] = NUMPIXELS*0.75;
//  }




//  else if(value == 19){            // 7 white segments, strobing interchanging
//    for(int b=0; b<3; b++){
//      blockWidth[b] = NUMPIXELS/16;
//      blockPos[b] = ((b*2+1)*0.166)*NUMPIXELS;
//      saturation[b] = 0;
//      spinning[b] = true;
//      spinSpeed[b] = 1.33;
//      initPos[b] = blockPos[b];
//      strobeType[b] = 1;
//      strobeSpeed[b] = 3;
//    }
//  }
  
  
  for(int i=0; i<NUMBLOCKS; i++){
    switchPos[i] = blockPos[i];
  }
}
