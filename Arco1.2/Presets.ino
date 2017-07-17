void preset(int value){
  Serial.print("Preset:");
  Serial.println(value);
  if(value > 0){
    hue1 = random(0,255);
    int hue2temp = hue1+random(70,120);
    if(hue2temp > 255) hue2temp -= 255;
    hue2 = hue2temp;
    for(int i=0; i<NUMBLOCKS; i++){
      blockSize[i] = 0;
      blockPos[i] = 0;
      brightnes[i] = 255;
      saturation[i] = 255;
      blockSize[i] = 0;
      strobeType[i] = 0;
      switching[i] = false;
      spinning[i] = false;
      spinSpeed[i] = 1;
      reverse[i] = false;
      downOnly[i] = false;
      hue[i] = hue1;
    }
  }

  if(value == 1){             // half blue, half red, switching
    blockSize[0] = NUMPIXELS/2;
    blockPos[0] = NUMPIXELS/4;
    hue[0] = hue1; 
    switching[0] = true;
    blockSize[1] = NUMPIXELS/2;
    blockPos[1] = NUMPIXELS*0.75;
    hue[1] = hue2;
    switching[1] = true;
    switchSpeed = 2;
  }

  else if(value == 2){            // half blue, half red, spinning slow
    blockSize[0] = NUMPIXELS/2;
    blockPos[0] = NUMPIXELS/4;
    spinning[0] = true;
    spinSpeed[0] = 1;
    initPos[0] = blockPos[0];
    hue[0] = hue1;
    blockSize[1] = NUMPIXELS/2;
    blockPos[1] = NUMPIXELS*0.75;
    hue[1] = hue2;
    spinning[1] = true;
    spinSpeed[1] = 1;
    initPos[1] = blockPos[1];
  }

  else if(value == 3){            // half blue, half red, spinning fast
    blockSize[0] = NUMPIXELS/2;
    blockPos[0] = NUMPIXELS/4;
    spinning[0] = true;
    spinSpeed[0] = 2;
    initPos[0] = blockPos[0];
    hue[0] = hue1;
    blockSize[1] = NUMPIXELS/2;
    blockPos[1] = NUMPIXELS*0.75;
    hue[1] = hue2;
    spinning[1] = true;
    spinSpeed[1] = 2;
    initPos[1] = blockPos[1];
  }

  else if(value == 4){            // half blue, half red, switching, strobing white blocks alternating
    for(int b=0; b<4; b++){
      blockSize[b] = NUMPIXELS/2;
    }
    blockPos[0] = NUMPIXELS/4;
    hue[0] = hue1; 
    switching[0] = true;
    blockPos[1] = NUMPIXELS*0.75;
    hue[1] = hue2;
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
    hue[0] = hue2; 
    saturation[1] = 0;
    hue[2] = hue2; 
    saturation[3] = 0;
    switchSpeed = 2;
    for(int b=0; b<4; b++){
      blockSize[b] = NUMPIXELS/4;
      blockPos[b] = ((b*2+1)*0.125)*NUMPIXELS;
      switching[b] = true;
    }
  }

  else if(value == 6){            // red and white quarters, spinning slow
    hue[0] = hue2; 
    saturation[1] = 0;
    hue[2] = hue2; 
    saturation[3] = 0;
    for(int b=0; b<4; b++){
      blockSize[b] = NUMPIXELS/4;
      blockPos[b] = ((b*2+1)*0.125)*NUMPIXELS;
      spinning[b] = true;
      spinSpeed[b] = 1;
      initPos[b] = blockPos[b];
    }
  }

  else if(value == 7){            // red and white quarters, spinning fast
    hue[0] = hue2; 
    saturation[1] = 0;
    hue[2] = hue2; 
    saturation[3] = 0;
    for(int b=0; b<4; b++){
      blockSize[b] = NUMPIXELS/4;
      blockPos[b] = ((b*2+1)*0.125)*NUMPIXELS;
      spinning[b] = true;
      spinSpeed[b] = 2;
      initPos[b] = blockPos[b];
    }
  }

  else if(value == 8){            // red with white blocks strobing
    for(int b=0; b<4; b++){
      blockSize[b] = NUMPIXELS/2;
    }
    blockPos[0] = NUMPIXELS/4;
    hue[0] = hue2; 
    switching[0] = true;
    blockPos[1] = NUMPIXELS*0.75;
    hue[1] = hue2;
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

  else if(value == 9){            // half blue, half red, switching, strobing white blocks alternating
    for(int b=0; b<4; b++){
      blockSize[b] = NUMPIXELS/2;
    }
    blockPos[0] = NUMPIXELS/4;
    hue[0] = hue1; 
    switching[0] = true;
    blockPos[1] = NUMPIXELS*0.75;
    hue[1] = hue2;
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

  else if(value == 10){            // four blue eighths, spinning 
    for(int b=0; b<3; b++){
      blockSize[b] = NUMPIXELS/16;
      blockPos[b] = ((b*2+1)*0.166)*NUMPIXELS;
      hue[b] = hue1;
      spinning[b] = true;
      spinSpeed[b] = 1.33;
      initPos[b] = blockPos[b];
    }
  }

  else if(value == 11){            // four red eighths, spinning 
    for(int b=0; b<3; b++){
      blockSize[b] = NUMPIXELS/16;
      blockPos[b] = ((b*2+1)*0.166)*NUMPIXELS;
      hue[b] = hue2;
      spinning[b] = true;
      spinSpeed[b] = 1.33;
      initPos[b] = blockPos[b];
    }
  }

  else if(value == 12){            // three eighths, switching, strobing
    switchSpeed=1;
    for(int b=0; b<7; b+=2){
      blockSize[b] = NUMPIXELS/16;
      blockPos[b] = ((b*2+1)*0.071)*NUMPIXELS;
      initPos[b] = blockPos[b];
      strobeType[b] = 4;
      strobeSpeed[b] = 2;
    }
    for(int b=1; b<7; b+=2){
      blockSize[b] = NUMPIXELS/16;
      blockPos[b] = ((b*2+1)*0.071)*NUMPIXELS;
      initPos[b] = blockPos[b];
      strobeType[b] = 5;
      strobeSpeed[b] = 2;
    }
  }
  
  else if(value == 13){            // three eighths, switching, strobing slow
    switchSpeed=1;
    for(int b=0; b<7; b+=2){
      blockSize[b] = NUMPIXELS/16;
      blockPos[b] = ((b*2+1)*0.071)*NUMPIXELS;
      initPos[b] = blockPos[b];
      strobeType[b] = 4;
      strobeSpeed[b] = 4;
    }
    for(int b=1; b<7; b+=2){
      blockSize[b] = NUMPIXELS/16;
      blockPos[b] = ((b*2+1)*0.071)*NUMPIXELS;
      initPos[b] = blockPos[b];
      strobeType[b] = 5;
      strobeSpeed[b] = 4;
    }
  }

  else if(value == 14){            // half, spinning slow
    blockSize[0] = NUMPIXELS/2;
    blockPos[0] = NUMPIXELS/4;
    spinning[0] = true;
    spinSpeed[0] = 1;
    initPos[0] = blockPos[0];
  }

  else if(value == 15){            // half, spinning fast
    blockSize[0] = NUMPIXELS/2;
    blockPos[0] = NUMPIXELS/4;
    spinning[0] = true;
    spinSpeed[0] = 2;
    initPos[0] = blockPos[0];
  }

  else if(value == 16){            // half, spinning slow, blinking
    blockSize[0] = NUMPIXELS/2;
    blockPos[0] = NUMPIXELS/4;
    spinning[0] = true;
    strobeType[0] = 6;
    strobeSpeed[0] = 4;
    spinSpeed[0] = 2;
    initPos[0] = blockPos[0];
  }

  else if(value == 17){            // half blue, spinning fast, with red segment in dark
    blockSize[0] = NUMPIXELS/2;
    blockPos[0] = NUMPIXELS/4;
    blockSize[1] = NUMPIXELS/24;
    blockPos[1] = NUMPIXELS*0.75;
    hue[1] = hue2;
    for(int b=0; b<2; b++){
      spinning[b] = true;
      spinSpeed[b] = 2;
      initPos[b] = blockPos[b];
    }
  }

  else if(value == 18){            // red and blue quarters, switching
    brightnes[0] = 0; 
    hue[1] = hue2;
    brightnes[2] = 0; 
    hue[3] = hue2;
    switchSpeed = 2;
    for(int b=0; b<4; b++){
      blockSize[b] = NUMPIXELS/4;
      blockPos[b] = ((b*2+1)*0.125)*NUMPIXELS;
      switching[b] = true;
    }
  }

  else if(value == 19){            // red and white quarters, switching, blinking
    brightnes[0] = 0; 
    hue[1] = hue1;
    brightnes[2] = 0; 
    hue[3] = hue2;
    switchSpeed = 2;
    strobeType[1] = 6;
    strobeSpeed[1] = 4;
    strobeType[3] = 6;
    strobeSpeed[3] = 4;
    for(int b=0; b<4; b++){
      blockSize[b] = NUMPIXELS/4;
      blockPos[b] = ((b*2+1)*0.125)*NUMPIXELS;
      switching[b] = true;
    }
  }

  else if(value == 20){            // red and white quarters, switching, blinking in middle
    brightnes[0] = 0; 
    hue[1] = hue1;
    brightnes[2] = 0; 
    hue[3] = hue2;
    switchSpeed = 2;
    strobeType[1] = 7;
    strobeSpeed[1] = 4;
    strobeType[3] = 7;
    strobeSpeed[3] = 4;
    for(int b=0; b<4; b++){
      blockSize[b] = NUMPIXELS/4;
      blockPos[b] = ((b*2+1)*0.125)*NUMPIXELS;
      switching[b] = true;
    }
  }

  else if(value == 21){            // blue half, switching, with red strobe in darkness
    brightnes[1] = 0; 
    switchSpeed = 2;
    for(int b=0; b<2; b++){
      blockSize[b] = NUMPIXELS/2;
      blockPos[b] = ((b*2+1)*0.25)*NUMPIXELS;
      switching[b] = true;
    }
    for(int b=2; b<4; b++){
      blockSize[b] = NUMPIXELS/8;
      strobeSpeed[b] = 2;
      hue[b] = hue2;
    }
    blockPos[2] = NUMPIXELS*0.25;
    blockPos[3] = NUMPIXELS*0.75;
    strobeType[2] = 9;
    strobeType[3] = 8;
  }


  else if(value == 22){            // coloured half, switching, with white strobe in darkness
    hue[0] = hue1;
    brightnes[1] = 0; 
    switchSpeed = 2;
    for(int b=0; b<2; b++){
      blockSize[b] = NUMPIXELS/2;
      blockPos[b] = ((b*2+1)*0.25)*NUMPIXELS;
      switching[b] = true;
    }
    for(int b=2; b<4; b++){
      blockSize[b] = NUMPIXELS/6;
      strobeSpeed[b] = 2;
      saturation[b] = 0;
    }
    blockPos[2] = NUMPIXELS*0.25;
    blockPos[3] = NUMPIXELS*0.75;
    strobeType[2] = 9;
    strobeType[3] = 8;
  }

  else if(value == 23){            // two segments, spinning opposite directions
    spinSpeed[0] = 1;
    spinSpeed[1] = 1;
    reverse[1] = true;
    for(int b=0; b<2; b++){
      blockSize[b] = NUMPIXELS/16;
      blockPos[b] = NUMPIXELS/2;
      spinning[b] = true;
      initPos[b] = blockPos[b];
    }
  }

  else if(value == 24){            // different coloured segments, spinning opposite directions
    spinSpeed[0] = 1;
    spinSpeed[1] = 1;
    reverse[1] = true;
    hue[0] = hue1;
    hue[1] = hue2;
    for(int b=0; b<2; b++){
      blockSize[b] = NUMPIXELS/16;
      blockPos[b] = NUMPIXELS/2;
      spinning[b] = true;
      initPos[b] = blockPos[b];
    }
  }

  else if(value == 25){            // two red segments falling from top
    spinSpeed[0] = 1;
    spinSpeed[1] = 1;
    reverse[1] = true;
    for(int b=0; b<2; b++){
      blockSize[b] = NUMPIXELS/8;
      blockPos[b] = NUMPIXELS/2;
      spinning[b] = true;
      hue[b] = hue2;
      initPos[b] = blockPos[b];
      downOnly[b] = true;
    }
  }

  else if(value == 26){            // two blue segments falling from top
    spinSpeed[0] = 1;
    spinSpeed[1] = 1;
    reverse[1] = true;
    for(int b=0; b<2; b++){
      blockSize[b] = NUMPIXELS/8;
      blockPos[b] = NUMPIXELS/2;
      spinning[b] = true;
      hue[b] = hue1;
      initPos[b] = blockPos[b];
      downOnly[b] = true;
    }
  }
  
  for(int i=0; i<NUMBLOCKS; i++){
    switchPos[i] = blockPos[i];
  }
}


//-----------------------------------------------------------------------------------------
