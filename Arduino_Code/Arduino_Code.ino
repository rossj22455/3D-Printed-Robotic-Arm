#include <Servo.h>

Servo baseServo;
Servo shoulderServo;
Servo elbowServo;

float currentBase = 90;
float currentShoulder = 90;
float currentElbow = 90;

unsigned long lastUpdate = 0;
const int updateInterval = 15; 

float speedMultiplier = 1.0;  
const float baseStepSize = 2.0;  

const int triggerThreshold = 0.99; 

bool leftTriggerHeld = false;
bool rightTriggerHeld = false;


void setup() {
  Serial.begin(9600);
  baseServo.attach(3);
  shoulderServo.attach(5);
  elbowServo.attach(6);
}

void loop() {
  static float targetBase = 90;
  static float targetShoulder = 90;
  static float targetElbow = 90;

  if (Serial.available()) {
    String packet = Serial.readStringUntil('\n');
    float values[13];
    int start = 0;
    int commaIndex = 0;
    for (int i = 0; i < 13; i++) {
      commaIndex = packet.indexOf(',', start);
      if (commaIndex == -1) commaIndex = packet.length();
      values[i] = packet.substring(start, commaIndex).toFloat();
      start = commaIndex + 1;
    }

    // Read triggers from values[]
    float leftTriggerValue = values[5];
    float rightTriggerValue = values[4];

    leftTriggerHeld = (leftTriggerValue > triggerThreshold);
    rightTriggerHeld = (rightTriggerValue > triggerThreshold);
    bool baseHold = leftTriggerHeld && rightTriggerHeld;

    // Only update base target if not holding
    if (!baseHold) {
      float baseInput = values[5] - values[4];
      targetBase = constrain(map(baseInput * 100, -100, 100, 0, 180), 0, 180);
    }

    targetShoulder = constrain(map(-values[1] * 100, -100, 100, 0, 180), 0, 180);
    targetElbow = constrain(map(-values[3] * 100, -100, 100, 0, 180), 0, 180);
  }

  if (millis() - lastUpdate >= updateInterval) {
  float step = baseStepSize * speedMultiplier;

  if (!(leftTriggerHeld && rightTriggerHeld)) {
    currentBase = moveToward(currentBase, targetBase, step);
    baseServo.write(currentBase);
  }

  currentShoulder = moveToward(currentShoulder, targetShoulder, step);
  currentElbow = moveToward(currentElbow, targetElbow, step);

  shoulderServo.write(currentShoulder);
  elbowServo.write(currentElbow);

  lastUpdate = millis();
}

}


float moveToward(float current, float target, float step) {
  if (abs(target - current) <= step) return target;
  return current + (target > current ? step : -step);
}
