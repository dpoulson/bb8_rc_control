int flywheelRaw, flywheelSpeed;
bool flywheelEnabled = false;

/**
   The flywheel is mapped to the self-centering pot knob of a 3-axis joystick.
*/
void flywheel()
{
  flywheelRaw = sbus_rx.ch()[CH_FLYWHEEL];
  flywheelRaw = constrain(flywheelRaw, RC_MIN, RC_MAX);
  flywheelSpeed = map(flywheelRaw, RC_MIN, RC_MAX, 255, -255);
  flywheelEnabled = sbus_rx.ch()[CH_FLYWHEEL_EN] == RC_MAX;

  if (!flywheelEnabled || in_rc_deadband(flywheelRaw))
  {
    //flywheelController.Stop();
  }
  else
  {
    if (flywheelSpeed < 0)
    {
      //flywheelController.TurnRight(abs(flywheelSpeed));
    }
    else
    {
      //flywheelController.TurnLeft(flywheelSpeed);
    }
  }

}
