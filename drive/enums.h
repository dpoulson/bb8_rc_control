enum DriveMode : uint8_t
{
  Disabled = 0, // No movement
  Static = 1,   // Head movement only, no drive or s2s
  Enabled = 2   // Full control
};