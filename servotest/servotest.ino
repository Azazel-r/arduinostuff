#include <ESP32Servo.h>

Servo myservo;
int delayVal = 16;
int timePast = 0;
int frameCount = 0;
int numFrames = 240;

float mapf(float p, float a1, float a2, float b1, float b2) {
	return (p - a1) * (b2 - b1) / (a2 - a1) + b1;
}

float period(float p) {
  return -0.5 * cos(TWO_PI * p) + 0.5;
}

float easeOutBounce(float p) {
  float n1 = 7.5625;
  float d1 = 2.75;

  if (p < 1 / d1) {
     return n1 * p * p;
  } else if (p < 2 / d1) {
    return n1 * (p -= 1.5 / d1) * p + 0.75;
  } else if (p < 2.5 / d1) {
    return n1 * (p -= 2.25 / d1) * p + 0.9375;
  } else {
    return n1 * (p -= 2.625 / d1) * p + 0.984375;
  }

}

void setup() {
  myservo.attach(4, 1000, 2000);

}

void loop() {
  if ((millis() - timePast) > delayVal) {
    float t = mapf(frameCount % numFrames, 0, numFrames, 0, 1);

    if (t < 0.5) myservo.write((int) floorf(easeOutBounce(t*2) * 180));
    else myservo.write((int) floorf(mapf(easeOutBounce((t-0.5)*2), 0, 1, 180, 0)));

    frameCount++;
    timePast = millis();
  }
}
