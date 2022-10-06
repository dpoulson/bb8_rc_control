enum DriveMode : uint8_t
{
  Disabled = 0, // No movement
  Static = 1,   // Head movement only, no drive or s2s
  Enabled = 2   // Full control
};

enum CalibrationMode : uint8_t
{
  CalOff = 0, // 
  CalEnabled = 1,   // 
  CalSave = 2   // 
};

typedef struct dome_message {
  bool psi; // Flash the PSI
  int effect; // Trigger an effect
};
