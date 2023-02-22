void debug_rc_inputs()
{
  for (int i = 0; i < 16; i++)
  {
    Serial.print(sbus_rx.data().ch[i]);
    Serial.print("\t");
  }
  Serial.println();
}

void debug_dome_outputs(int servo1, int servo2)
{
  Serial.print(servo1);
  Serial.print("\t");
  Serial.print(servo2);
  Serial.println();
}

void debug_main_drive_outputs(int speed, int output)
{
  Serial.print(speed);
  Serial.print("\t");
  Serial.print(output);
  Serial.println();
}
