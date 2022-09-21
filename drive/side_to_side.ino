int ch2, pot;
int s2s_current_position;
int s2s_target_position;
int s2s_position_difference;

void side_to_side()
{
  ch2 = sbus_rx.ch()[CH_DRIVE_S2S];

  s2s_target_position = map(ch2, RC_MIN, RC_MAX, S2S_MAX_ANGLE, -S2S_MAX_ANGLE);

  // Calculate error
  s2s_position_difference = s2s_target_position - s2s_current_position;

  // Avoid any strange zero condition
  if ( s2s_position_difference != 0.00 )
  {
    s2s_current_position += s2s_position_difference * S2S_EASING;
  }

  Setpoint2 = s2s_current_position;

  // TODO: Rolling average
  total = total - readings[readIndex];
  readings[readIndex] = analogRead(S2S_POT_PIN) + S2S_OFFSET;
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

  //  pot = analogRead(S2S_POT_PIN) + S2S_OFFSET;

  Input2 = (roll + rollOffset) * -1;
  //  Input2 = roll * -1;
  Setpoint2 = constrain(Setpoint2, -S2S_MAX_ANGLE, S2S_MAX_ANGLE);
  PID2.Compute();

  Setpoint1 = Output2;

  Input1 = map(pot, 0, 1024, -255, 255);

  Input1 = constrain(Input1, -S2S_MAX_ANGLE, S2S_MAX_ANGLE);
  Setpoint1 = constrain(Setpoint1, -S2S_MAX_ANGLE, S2S_MAX_ANGLE);
  Setpoint1 = map(Setpoint1, S2S_MAX_ANGLE, -S2S_MAX_ANGLE, -S2S_MAX_ANGLE, S2S_MAX_ANGLE);

  // Update PK1 from RC control
  Pk1 = get_pk1();
  PID1.SetTunings(Pk1, Ik1, Dk1);
  PID1.Compute();

//   Serial.print(motorsEnabled);
//   Serial.print('\t');
//   Serial.println(Output1);

  if (Output1 < 0)
  {
    Output1a = abs(Output1);
    s2sController.TurnLeft(Output1a);
  }
  else if (Output1 > 0)
  {
    Output1a = abs(Output1);
    s2sController.TurnRight(Output1a);
  }
  else
  {
    s2sController.Stop();
  }
}
