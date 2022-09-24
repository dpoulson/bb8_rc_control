

float mapfloat(long x, long in_min, long in_max, long out_min, long out_max)
{
  return (float)(x - in_min) * (out_max - out_min) / (float)(in_max - in_min) + out_min;
}

/**
 *
 */
bool is_drive_enabled()
{
  return map(sbus_rx.ch()[CH_DRIVE_EN], RC_MIN, RC_MAX, 0, 1) == 1;
}

DriveMode get_drive_mode()
{
  int driveVal = sbus_rx.ch()[CH_DRIVE_EN];
  if (driveVal == RC_MAX)
  {
    return DriveMode::Enabled;
  }
  else if (driveVal == RC_MID)
  {
    return DriveMode::Static;
  }
  else
  {
    return DriveMode::Disabled;
  }
}

/**
   ReadIMU

   Leaning forward = +y
   Tilting left    = -z

   Pitch is leaning forwards/backwards - Y axis
   Roll is leaning sideways - Z axis
*/
void readIMU()
{
  imu::Vector<3> euler = bno.getVector(Adafruit_BNO055::VECTOR_EULER);
  pitch = euler.y();
  roll = euler.z(); // * get_roll_multiplier();
}

bool in_rc_deadband(int value)
{
  return value >= RC_DEADBAND_LOW && value <= RC_DEADBAND_HIGH;
}

void disable_drive()
{
  //flywheelController.Disable();
  s2sController.Disable();
  driveController.Disable();
}

void enable_drive()
{
  //flywheelController.Enable();
  s2sController.Enable();
  driveController.Enable();
}

bool is_main_drive_enabled()
{
  return driveMode == DriveMode::Enabled;
}

bool is_s2s_enabled()
{
  return driveMode == DriveMode::Enabled;
}

bool is_flywheel_enabled()
{
  return driveMode == DriveMode::Enabled;
}

bool is_dome_rotation_enabled()
{
  return driveMode == DriveMode::Enabled || driveMode == DriveMode::Static;
}

bool is_dome_movement_enabled()
{
  return driveMode == DriveMode::Enabled || driveMode == DriveMode::Static;
}

/**
   Maps the S1 pot to the S2S stability's PID Proportional value
*/
double get_pk1()
{
  return mapfloat(sbus_rx.ch()[CH_ROLL_OFFSET], RC_MIN, RC_MAX, 0, 30);
}

double get_roll_multiplier()
{
  return mapfloat(sbus_rx.ch()[CH_ROLL_OFFSET], RC_MIN, RC_MAX, 0, 1);
}

void debug_print()
{
  //  Serial.print("Drive Enabled: ");
  //  Serial.print(is_drive_enabled());
  //  Serial.print("\t");

  Serial.print("S2S: ");
  Serial.print(analogRead(S2S_POT_PIN));
  Serial.print("\t");

  Serial.print("S2S mapped: ");
  Serial.print(map(analogRead(S2S_POT_PIN), 0, 1024, -135, 135));
  Serial.print("\t");

  Serial.print("PK1: ");
  Serial.print(get_pk1());
  Serial.print("\t");
  //  Serial.print("PK2: ");
  //  Serial.print(get_pk2());
  //  Serial.print("\t");

  Serial.print("Pitch: ");
  Serial.print(pitch);
  Serial.print("\t");

  Serial.print("Roll: ");
  Serial.print(roll);
  Serial.print("\t");

  //  Serial.print("Roll Offset: ");
  //  Serial.print(get_roll_offset());
  //  Serial.print("\t");

  //  Serial.print("Roll Multiplier: ");
  //  Serial.print(get_roll_multiplier());
  //  Serial.print("\t");

  Serial.println();
}
