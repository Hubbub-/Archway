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
