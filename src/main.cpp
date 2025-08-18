#include <Arduino.h>
#include <FastAccelStepper.h>

// --- YAW MOTOR ---
#define STEP_PIN_Y 18   // ESP32 GPIO for Yaw Step
#define DIR_PIN_Y 19    // ESP32 GPIO for Yaw Dir

// --- PITCH MOTOR ---
#define STEP_PIN_P 21   // ESP32 GPIO for Pitch Step
#define DIR_PIN_P 22    // ESP32 GPIO for Pitch Dir

FastAccelStepperEngine engine = FastAccelStepperEngine();
FastAccelStepper *stepperYaw = NULL;
FastAccelStepper *stepperPitch = NULL;

// --- COMMON SETTINGS ---
const int MICROSTEPS = 8;
const float STEPS_PER_REV = 200 * MICROSTEPS;
const float STEPS_PER_DEGREE = STEPS_PER_REV / 360.0;
const int MAX_SPEED = 10000; // steps/sec
const int ACCELERATION = 5000; // steps/sec^2

long targetYawSteps = 0;
long targetPitchSteps = 0;

void setup() {
  Serial.begin(115200);

  engine.init();
  stepperYaw = engine.stepperConnectToPin(STEP_PIN_Y);
  if (stepperYaw) {
    stepperYaw->setDirectionPin(DIR_PIN_Y);
    stepperYaw->setAutoEnable(true);
    stepperYaw->setSpeedInHz(MAX_SPEED); // steps/sec
    stepperYaw->setAcceleration(ACCELERATION);
  }

  stepperPitch = engine.stepperConnectToPin(STEP_PIN_P);
  if (stepperPitch) {
    stepperPitch->setDirectionPin(DIR_PIN_P);
    stepperPitch->setAutoEnable(true);
    stepperPitch->setSpeedInHz(MAX_SPEED); // steps/sec
    stepperPitch->setAcceleration(ACCELERATION);
  }
}

void loop() {
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n'); 
    input.trim();

    Serial.println(input);

    int yIndex = input.indexOf('Y');
    int pIndex = input.indexOf('P');
    int commaIndex = input.indexOf(',');
    if (yIndex != -1 && pIndex != -1 && commaIndex != -1) {
      String yawStr = input.substring(yIndex + 1, commaIndex);
      String pitchStr = input.substring(pIndex + 1);
      float yawAngle = yawStr.toFloat();
      float pitchAngle = pitchStr.toFloat();

      targetYawSteps = (long)(yawAngle * STEPS_PER_DEGREE);
      targetPitchSteps = (long)(pitchAngle * STEPS_PER_DEGREE);

      if (stepperYaw && abs(targetYawSteps - stepperYaw->getCurrentPosition()) > 2) {
        stepperYaw->moveTo(targetYawSteps);
      }
      if (stepperPitch && abs(targetPitchSteps - stepperPitch->getCurrentPosition()) > 2) {
        stepperPitch->moveTo(targetPitchSteps);
      }
    }
  }
}
