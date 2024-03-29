int ch2, pot;
int s2s_current_position;
int s2s_target_position;
int s2s_position_difference;

void side_to_side()
{
  ch2 = sbus_rx.data().ch[CH_DRIVE_S2S];
  int s2soffset = map(sbus_rx.data().ch[15], RC_MIN, RC_MAX, -50, 50);

  s2s_target_position = map(ch2, RC_MIN, RC_MAX, S2S_MAX_ANGLE, -S2S_MAX_ANGLE);

  // Calculate error
  s2s_position_difference = s2s_target_position - s2s_current_position;

  // Avoid any strange zero condition
  if ( s2s_position_difference != 0.00 )
  {
    s2s_current_position += s2s_position_difference * S2S_EASING;
  }

  Setpoint2 = s2s_current_position;

  // Rolling average
  total = total - readings[readIndex];
  readings[readIndex] = analogRead(S2S_POT_PIN) + S2S_OFFSET + s2soffset;
  total = total + readings[readIndex];
  readIndex = readIndex + 1;

  // Wrap to beginning
  if (readIndex >= numReadings)
  {
    readIndex = 0;
  }

  // calculate the average:
  average = (float)total / (float)numReadings;
  pot = average;


  Input2 = (roll + rollOffset) * -1;
  //  Input2 = roll * -1;
  Setpoint2 = constrain(Setpoint2, -S2S_MAX_ANGLE, S2S_MAX_ANGLE);
  PID2.Compute();

  Setpoint1 = Output2;

  Input1 = map(pot, 0, 4096, 255, -255);

  Input1 = constrain(Input1, -S2S_MAX_ANGLE, S2S_MAX_ANGLE);
  Setpoint1 = constrain(Setpoint1, -S2S_MAX_ANGLE, S2S_MAX_ANGLE);
  Setpoint1 = map(Setpoint1, S2S_MAX_ANGLE, -S2S_MAX_ANGLE, -S2S_MAX_ANGLE, S2S_MAX_ANGLE);

  // Update PK1 from RC control
  Pk1 = get_pk1();
  PID1.SetTunings(Pk1, Ik1, Dk1);
  PID1.Compute();

  if (Output1 < 0)
  {
    s2sController.TurnLeft(abs(Output1));
    #ifdef S2S_DEBUG
      Serial.print("Left ");
    #endif
  }
  else if (Output1 > 0)
  {
    s2sController.TurnRight(abs(Output1));
    #ifdef S2S_DEBUG
      Serial.print("Right ");
    #endif
  }
  else
  {
    s2sController.Stop();
    
    #ifdef S2S_DEBUG
      Serial.print("Stop ");
    #endif
  } 
  
  #ifdef S2S_DEBUG
    Serial.print(s2s_current_position);
    Serial.print(" ");
    Serial.print(s2s_target_position);
    Serial.print(" ");
    Serial.print(pot);
    Serial.print(" ");
    Serial.print(s2soffset);    
    Serial.print(" |Pk1 ");
    Serial.print(Pk1);
    Serial.print(" ");
    Serial.print(sbus_rx.data().ch[CH_ROLL_OFFSET]);
    Serial.print(" |1 ");
    Serial.print(Input1);
    Serial.print(" ");
    Serial.print(Setpoint1);
    Serial.print(" "); 
    Serial.print(Output1);
    Serial.print(" |2 ");
    Serial.print(Input2);
    Serial.print(" ");  
    Serial.print(Setpoint2);
    Serial.print(" "); 
    Serial.print(Output2);
    Serial.print(" ");
  #endif
  
}
