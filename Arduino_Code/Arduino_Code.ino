#include <Servo.h>

Servo baseServo;
Servo shoulderServo;
Servo elbowServo;

float currentBase = 90;
float currentShoulder = 90;
float currentElbow = 90;

unsigned long lastUpdate = 0;
const int updateInterval = 15; 

float speedMultiplier = 1.5;  
const float baseStepSize = 2.0;  

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
  float speedMultiplier = 1.0;  

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

    // Button logic for speed
    bool L1 = (int)values[10] == 1;
    bool R1 = (int)values[11] == 1;

    if (L1) speedMultiplier = 1.5;
    else if (R1) speedMultiplier = 0.25;
    else speedMultiplier = 1.0;

    // Base, shoulder, elbow control
    float baseInput = values[5] - values[4];
    targetBase = constrain(map(baseInput * 100, -100, 100, 0, 180), 0, 180);
    targetShoulder = constrain(map(-values[1] * 100, -100, 100, 0, 180), 0, 180);
    targetElbow = constrain(map(-values[3] * 100, -100, 100, 0, 180), 0, 180);
  }

  if (millis() - lastUpdate >= updateInterval) {
    float step = baseStepSize * speedMultiplier;

    currentBase = moveToward(currentBase, targetBase, step);
    currentShoulder = moveToward(currentShoulder, targetShoulder, step);
    currentElbow = moveToward(currentElbow, targetElbow, step);

    baseServo.write(currentBase);
    shoulderServo.write(currentShoulder);
    elbowServo.write(currentElbow);

    lastUpdate = millis();
  }
}


float moveToward(float current, float target, float step) {
  if (abs(target - current) <= step) return target;
  return current + (target > current ? step : -step);
}
