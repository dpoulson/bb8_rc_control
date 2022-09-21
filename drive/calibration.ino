void calibrate()
{
  setPitchOffset();
  setDomeTiltOffset();
  setS2SOffset();
}

void setPitchOffset()
{
  //  if (recIMUData.pitch < 0) {
  //    pitchOffset = abs(recIMUData.pitch);
  //  } else {
  //    pitchOffset = recIMUData.pitch * -1;
  //  }
  //  EEPROM.writeFloat(0, pitchOffset);

}

void setDomeTiltOffset()
{
  //#ifdef reverseDomeTiltPot
  //  if (map(analogRead(domeTiltPotPin), 0, 1024, -135, 135) < 0)
  //  {
  //    domeTiltPotOffset = abs(map(analogRead(domeTiltPotPin), 0, 1024, 135, -135));
  //  }
  //  else
  //  {
  //    domeTiltPotOffset = map(analogRead(domeTiltPotPin), 0, 1024, -135, 135) * -1;
  //  }
  //#else
  //  if (map(analogRead(domeTiltPotPin), 0, 1024, 135, -135) < 0)
  //  {
  //    domeTiltPotOffset = abs(map(analogRead(domeTiltPotPin), 0, 1024, 135, -135));
  //  }
  //  else
  //  {
  //    domeTiltPotOffset = map(analogRead(domeTiltPotPin), 0, 1024, 135, -135) * -1;
  //  }
  //#endif
  //  EEPROM.writeInt(10, domeTiltPotOffset);
}

void setS2SOffset()
{
  //    if(recIMUData.roll < 0){
  //      rollOffset = abs(recIMUData.roll);
  //    }else{
  //        rollOffset = recIMUData.roll * -1;
  //    }
  //
  //    if(S2Spot < 0){
  //      potOffsetS2S = abs(S2Spot);
  //    }else{
  //      potOffsetS2S = S2Spot * -1;
  //    }
  //    EEPROM.writeFloat(4,rollOffset);
  //    EEPROM.writeInt(8,potOffsetS2S);
}

void loadCalibration()
{
  pitchOffset = EEPROM.readFloat(0);
  rollOffset = EEPROM.readFloat(4);
  potOffsetS2S = EEPROM.readInt(8);

  if (isnan(pitchOffset))
  {
    pitchOffset = 0;
  }
  if (isnan(rollOffset))
  {
    rollOffset = 0;
  }
  if (isnan(potOffsetS2S))
  {
    potOffsetS2S = 0;
  }
}
