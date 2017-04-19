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




/* 1A: 1  Half Blue, Half Red
 * 1B: 2  Half Blue, Half Red, switching
 * 2A1: 3 Half Green, Half Red, spinning slow
 * 2A2: 4 Half Green, Half Red, spinning fast
 * 2B: 5  Half Green, Half Red, switching with white strobing blocks
 * 3A1: 6  Red and White Quarters, switching
 * 3A2: 7  Red and White quarters, spinning slow
 * 3A3: 8  Red and White Quarters, spinning fast
 * 3B: 9   Red with White Strobing blocks slow
 * 3C: 10  Red with White Strobing blocks fast
 * 4A1: 11  Four white blocks spinning
 * 4A2: 12  Four blue eighths spinning
 * 4A3: 13  Four red eighths spinning
 * 4B1: 14  Seven white segments, strobing interchanging
 * 4B2: 15  Seven white segments, strobing interchanging faster
 * 4C1: 16  Half white spinning slow
 * 4C2: 17  Half White Spinning fast
 * 4C3: 18  Half White Spinning slow, blinking
 * 4C4: 19  Half White Spinning fast, with red segment in darkness
 * 4D1: 20  Two White Quarters, switching
 * 4D2: 21  Two White Quarters, switching, blinking
 * 4D3: 22  Two White Quarters, switching, blinking in middle
 * 4E1: 23  Half White, Switching, with red strobe in darkness
 * 4E2: 24  Half White, Switching, with blue strobe in darkness
 * 4E3: 25  Half Red, Switching, with white strobe in darkness
 * 4E4: 26  Half Blue, Switching, with white strobe in darkness
 * 4F1: 27  Two Small white segments, spinning opposite directions
 * 4F2: 28  Two Small red segments, spinning opposite directions
 * 4F3: 29  White segment and red segment, spinning opposite directions
 * 4G1: 30  Two White segments falling from top
 * 4G2: 31  Two Larger White segments falling from top
 * 4G3: 32  Two Larger Red segments falling from top
 * 4G3: 33  Two Larger Blue segments falling from top
 * 
 * change white warmth (pure to warm)
 * change blue (135 - 170)
 * change Red (Red-Orange)
 * */
 
 */
