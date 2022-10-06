/**
   FrSky SBUs RC control board for Joe's Drive Mk3

   Author:
      Patrick Ryan
      pat.m.ryan@gmail.com
      @circuitBurn

   Pin Mapping:
      TODO:

   Serial Ports:
      Serial1 - DFPlayer Mini
      Serial2 - SBUS RC

   I2C:
      PWM Servo Board

   Resources:
      https://github.com/circuitBurn/BB-8
      https://bb8builders.club/
      https://www.facebook.com/groups/863379917081398
*/
#include "Arduino.h"
#include "sbus.h"
#include <SPort.h>
#include <esp_now.h>

#include "BTS7960.h"
#include <PID_v1.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <Adafruit_NeoPixel.h>
#include <Wire.h>
#include <ESP32Servo.h>
#include "DFRobotDFPlayerMini.h"
#include <HardwareSerial.h>

#include "constants.h"
#include "enums.h"
#include <EEPROM.h>

#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>


// PID1 is for the side to side tilt
double Pk1 = 14; // 15.0;
double Ik1 = 0.0;
double Dk1 = 0; // 0.5;
double Setpoint1, Input1, Output1, Output1a;
PID PID1(&Input1, &Output1, &Setpoint1, Pk1, Ik1, Dk1, DIRECT);

// PID2 for S2S stability
double Pk2 = 1.5; // 0.6;
double Ik2 = 0;
double Dk2 = 0.025; // 0.03;
double Setpoint2, Input2, Output2, Output2a;
PID PID2(&Input2, &Output2, &Setpoint2, Pk2, Ik2, Dk2, DIRECT);

// PID3 for the main drive
double Pk3 = 2.5;
double Ik3 = 0.0;
double Dk3 = 0.01;
double Setpoint3, Input3, Output3, Output3a;
PID PID3(&Input3, &Output3, &Setpoint3, Pk3, Ik3, Dk3, DIRECT);

// PID4 for dome rotation
double Pk4 = 2.0;
double Ik4 = 0.1;
double Dk4 = 0.01;
double Setpoint4, Input4, Output4;
PID PID4(&Input4, &Output4, &Setpoint4, Pk4, Ik4, Dk4, DIRECT);

// Offsets
float pitchOffset, rollOffset, potOffsetS2S;

bool motorsEnabled = true;

// Dome Servos
Servo servo1;
Servo servo2;

/* SbusRx object on Serial1 */
bfs::SbusRx sbus_rx(&Serial2);
SPortHub hub(0x12, 17);
SimpleSPortSensor tel_pitch(0x5900);
SimpleSPortSensor tel_roll(0x5901);

// Sound
DFRobotDFPlayerMini myDFPlayer;

// BTS7960 Motor Drivers
BTS7960 driveController(DRIVE_EN_PIN, DRIVE_L_PWM_PIN, DRIVE_R_PWM_PIN);
BTS7960 s2sController(S2S_EN_PIN, S2S_L_PWM_PIN, S2S_R_PWM_PIN);
BTS7960 flywheelController(DRIVE_EN_PIN, FLYWHEEL_L_PWM_PIN, FLYWHEEL_R_PWM_PIN);

// IMU
Adafruit_BNO055 bno = Adafruit_BNO055(-1, 0x28);
float pitch, roll;

// S2S pot smoothing
// TODO: rename
const int numReadings = 10;
int readings[numReadings] = {510, 510, 510, 510, 510, 510, 510, 510, 510, 510};
int readIndex = 0;
int total = 0;
int average = 0;

unsigned long lastMillis;

uint8_t dome_mac[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
struct dome_message currentDomeMessage;
esp_now_peer_info_t peerDome;

Adafruit_NeoPixel body = Adafruit_NeoPixel(NEOPIXEL_COUNT, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

DriveMode driveMode = DriveMode::Disabled;
CalibrationMode calibrationMode = CalibrationMode::CalOff;
std::array<int16_t, bfs::SbusRx::NUM_CH()> sbus_data;


/*********************************
 * Setup
 */
void setup()
{
  sbus_rx.Begin(16,17);

  Serial.begin(115200);

  randomSeed(analogRead(0));

  pinMode(DRIVE_R_PWM_PIN, OUTPUT);
  pinMode(DRIVE_L_PWM_PIN, OUTPUT);
  pinMode(DRIVE_EN_PIN, OUTPUT);

  pinMode(S2S_R_PWM_PIN, OUTPUT);
  pinMode(S2S_L_PWM_PIN, OUTPUT);
  pinMode(S2S_EN_PIN, OUTPUT);

  pinMode(FLYWHEEL_R_PWM_PIN, OUTPUT);
  pinMode(FLYWHEEL_L_PWM_PIN, OUTPUT);

  pinMode(DOME_SPIN_A_PIN, OUTPUT);
  pinMode(DOME_SPIN_B_PIN, OUTPUT);


  // Motor Drivers
  driveController.Enable();
  s2sController.Enable();
  flywheelController.Enable();

  // PID Setup - S2S servo
  PID1.SetMode(AUTOMATIC);
  PID1.SetOutputLimits(-255, 255);
  PID1.SetSampleTime(15);

  // PID Setup - S2S stability
  PID2.SetMode(AUTOMATIC);
  PID2.SetOutputLimits(-255, 255);
  PID2.SetSampleTime(15);

  // PID Setup - main drive motor
  PID3.SetMode(AUTOMATIC);
  PID3.SetOutputLimits(-255, 255);
  PID3.SetSampleTime(15);

  // PID Setup - dome motor
  PID4.SetMode(AUTOMATIC);
  PID4.SetOutputLimits(-255, 255);
  PID4.SetSampleTime(15);

  /* Initialise the IMU */
  if (!bno.begin())
  {
    Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
    while (1)
      ;
  }
  Serial.println("IMU Online....");

  // TODO: loadOffsets
  pitchOffset = 4; //-0.55; //4.4;
  rollOffset = 5;//-3.6; // Positive tilts left
  potOffsetS2S = 3;

  // Fill the smoothing readings with the initial pot offset
  for (int thisReading = 0; thisReading < numReadings; thisReading++)
  {
    //    readings[thisReading] = analogRead(S2S_POT_PIN) + S2S_OFFSET;
    readings[thisReading] = potOffsetS2S;
  }


  // Setup Wifi and OTA updates
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }

  ArduinoOTA
    .onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else // U_SPIFFS
        type = "filesystem";

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      Serial.println("Start updating " + type);
    })
    .onEnd([]() {
      Serial.println("\nEnd");
    })
    .onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    })
    .onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });

  ArduinoOTA.begin();
  
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println(WiFi.macAddress());
  
  // Sound
  #ifdef AUDIO_ENABLED
    Serial1.begin(9600, SERIAL_8N1, 4, 5 );
    myDFPlayer.begin(Serial1);
    myDFPlayer.volume(20);
    myDFPlayer.play(1);
  #endif

  servo1.setPeriodHertz(50);
  servo1.attach(DOME_NECK_1, 1000, 2000);
  servo2.setPeriodHertz(50);
  servo1.attach(DOME_NECK_2, 1000, 2000);

  hub.registerSensor(tel_roll);       //Add sensor to the hub
  hub.registerSensor(tel_pitch);
  hub.begin();   

  if (esp_now_init() != 0) {
    Serial.println("Problem during ESP-NOW init");
    return;
  }
  
  // Register peer
  memcpy(peerDome.peer_addr, dome_mac, 6);
  peerDome.channel = 0;  
  peerDome.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerDome) != ESP_OK){
    Serial.println("Failed to add Dome");
    return;
  }

  currentDomeMessage.psi = false;
  currentDomeMessage.effect = 0;
  
  Serial.println("Startup complete!");
}

/**************************
 * Main Loop
 */

void loop()
{
  readIMU();
  tel_roll.value = roll;
  tel_pitch.value = pitch;
  #ifdef IMU_DEBUG
    Serial.print(roll);
    Serial.print(" ");
    Serial.print(pitch);
  #endif

  if (sbus_rx.Read())
  {

    driveMode = get_drive_mode();
    Serial.print(driveMode);

    if (driveMode == DriveMode::Enabled)
    {
      enable_drive();
      main_drive();
      flywheel();
      side_to_side();
      //dome_spin();
      dome_servos();
      #ifdef AUDIO_ENBLED
        sound_trigger();
      #endif
      #ifdef GLOBAL_DEBUG
        Serial.println();
      #endif

    }
    else if (driveMode == DriveMode::Static)
    {
      disable_drive();
      //dome_spin();
      dome_servos();
      #ifdef AUDIO_ENBLED
        sound_trigger();
      #endif
      #ifdef GLOBAL_DEBUG
        Serial.println();
      #endif
    }
    else
    {
      // Disabled
      disable_drive();
      calibrationMode = get_calibration_mode();
      #ifdef GLOBAL_DEBUG
        Serial.println();
      #endif
    }

  }
  if ((millis() - lastMillis) > 50)
  {
    panel1();
    panel2();
    panel3();
    panel4();
    panel5();
    panel6();

    body.show();

    lastMillis = millis();
  }
  hub.handle();
  esp_err_t result = esp_now_send(dome_mac, (uint8_t *) &currentDomeMessage, sizeof(currentDomeMessage));
  if (result == ESP_OK) {
    Serial.println("Sent with success");
  }
  else {
    Serial.println("Error sending the data");
  }
  ArduinoOTA.handle();


}
