int domeRaw;
int domeSpeed;
int targetNod;
int currentNod;
int diffNod;
int varServo1;
int varServo2;
int targetSide;
int diffSide;
int currentSide;
int ch3, ch4;

/**
   Dome spin is assigned to the self-centering pot of a 3-axis joystick.

   Turning the knob fully clockwise or counter-clockwise will continuously
   spin the dome. Otherwise, the dome will "point" and hold position
   in the general direction of the knob.
*/
void dome_spin()
{
  if (is_dome_rotation_enabled())
  {
    domeRaw = sbus_rx.ch()[CH_DOME_SPIN];

    if (domeRaw > RC_MIN + 100 && domeRaw < RC_MAX -100)
    {
      Setpoint4 = map(domeRaw, RC_MIN, RC_MAX, 0, 1024);
      Input4 = analogRead(DOME_POT_PIN) + DOME_POT_OFFSET;
      PID4.Compute();
      domeSpeed = constrain((int)Output4, -255, 255);
    }
    else if (domeRaw <= RC_MIN + 100)
    {
      domeSpeed = -255;
    }
    else if (domeRaw >= RC_MAX - 100)
    {
      domeSpeed = 255;
    }

    domeSpeed = constrain(domeSpeed, -DOME_SPIN_SPEED, DOME_SPIN_SPEED);

    if (domeSpeed >= 1)
    {
      analogWrite(DOME_SPIN_A_PIN, abs(domeSpeed));
      analogWrite(DOME_SPIN_B_PIN, 0);
    }
    else if (domeSpeed <= -1)
    {
      analogWrite(DOME_SPIN_A_PIN, 0);
      analogWrite(DOME_SPIN_B_PIN, abs(domeSpeed));
    }
    else
    {
      analogWrite(DOME_SPIN_A_PIN, 0);
      analogWrite(DOME_SPIN_B_PIN, 0);
    }
  }
  else
  {
    analogWrite(DOME_SPIN_A_PIN, 0);
    analogWrite(DOME_SPIN_B_PIN, 0);
  }

}

void dome_servos()
{
  if (is_dome_movement_enabled())
  {
    // Forwards-backwards
    ch3 = sbus_rx.ch()[CH_DOME_TILT_Y];
    targetNod = map(ch3, RC_MIN, RC_MAX, 180, 0);

    // TODO: pitch correction
    targetNod = targetNod + (pitch * 4.5);

    diffNod = targetNod - currentNod;

    // Avoid any strange zero condition
    if ( diffNod != 0.00 )
    {
      currentNod += diffNod * NOD_EASING;
    }

    varServo1 = currentNod;
    varServo2 = currentNod;

    // Left-right
    ch4 = sbus_rx.ch()[CH_DOME_TILT_X];
    targetSide = map(ch4, RC_MIN, RC_MAX, 60, -60);

    diffSide = targetSide - currentSide;

    // Avoid any strange zero condition
    if ( diffSide != 0.00 )
    {
      currentSide += diffSide * TILT_EASING;
    }

    varServo1 = varServo1 - currentSide;
    varServo2 = varServo2 + currentSide;

    // TODO: map properly
    varServo1 = constrain(varServo1, 0, 180);
    varServo2 = constrain(varServo2, 0, 180);

    // Reverse the servo value
    varServo2 = map(varServo2, 0, 180, 180, 0);

    varServo1 = map(varServo1, 0, 180, 1000, 2000);
    varServo2 = map(varServo2, 0, 180, 1000, 2000);

    servo1.writeMicroseconds(varServo1);
    servo2.writeMicroseconds(varServo2);
  }
  else
  {
    Serial.println("Dome movement is disabled");
  }
}
