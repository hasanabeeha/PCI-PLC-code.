/*
  NEMA23 Stepper Speed Control via Microstep Driver (PUL/DIR/ENA)
  ------------------------------------------------------------
  Wiring (as set up on the driver's Signal side):
    PUL+ / DIR+ / ENA+  -> Arduino 5V (tied together)
    PUL-                -> Arduino pin D3
    DIR-                -> Arduino pin D4
    ENA-                -> Arduino pin D5

  Type a speed in RPM into the Serial Monitor and press Enter.
    - Positive number  = spin one direction (e.g. 200)
    - Negative number  = spin the other direction (e.g. -200)
    - 0                = stop

  Set STEPS_PER_REV below to match the DIP switch microstep
  setting you chose on the driver (Pulse/rev column).
*/

#include <AccelStepper.h>

// ---- USER SETTINGS ----
const int STEPS_PER_REV = 1600;   // match this to your DIP switch setting (e.g. 1600 for 8 microstep)
const float MAX_RPM = 300.0;      // safety cap, adjust to your motor/driver limits
const float ACCEL_STEPS_PER_SEC2 = 2000.0; // acceleration, tune to taste

// ---- PIN DEFINITIONS ----
const int PUL_PIN = 3;
const int DIR_PIN = 4;
const int ENA_PIN = 5;

// AccelStepper in DRIVER mode uses (interface, stepPin, dirPin)
AccelStepper stepper(AccelStepper::DRIVER, PUL_PIN, DIR_PIN);

float currentRPM = 0;

void setup() {
  Serial.begin(9600);
  pinMode(ENA_PIN, OUTPUT);
  digitalWrite(ENA_PIN, LOW); // LOW = driver enabled (ENA- pulled low -> optocoupler on)

  float maxStepsPerSec = (MAX_RPM / 60.0) * STEPS_PER_REV;
  stepper.setMaxSpeed(maxStepsPerSec);
  stepper.setAcceleration(ACCEL_STEPS_PER_SEC2);

  Serial.println("Stepper ready.");
  Serial.println("Enter a speed in RPM (negative = reverse, 0 = stop):");
}

void loop() {
  // Check for new speed input
  if (Serial.available() > 0) {
    float inputRPM = Serial.parseFloat();
    // clear any trailing newline/garbage
    while (Serial.available() > 0) Serial.read();

    if (abs(inputRPM) > MAX_RPM) {
      Serial.print("Requested speed exceeds MAX_RPM (");
      Serial.print(MAX_RPM);
      Serial.println("). Clamping.");
      inputRPM = (inputRPM > 0) ? MAX_RPM : -MAX_RPM;
    }

    currentRPM = inputRPM;
    float stepsPerSec = (currentRPM / 60.0) * STEPS_PER_REV;
    stepper.setSpeed(stepsPerSec);

    Serial.print("Speed set to ");
    Serial.print(currentRPM);
    Serial.println(" RPM");
  }

  // Continuous rotation at constant speed (no acceleration curve on the fly)
  // Use runSpeed() for constant-speed spinning based on setSpeed()
  if (currentRPM != 0) {
    stepper.runSpeed();
  }
}
