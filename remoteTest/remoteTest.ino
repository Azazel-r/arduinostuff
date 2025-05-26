// -------------------------------------------------------------------------------------------------------------------------------------


#include <WiFi.h>
#include <WebServer.h>
#include <Adafruit_NeoPixel.h>
#define PIN 20
#define NUMPIXELS 150

// define pixels object whatever
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_RGB + NEO_KHZ800);

int frameCount = 1;
const int numFrames = 240;
const int width = 5;
unsigned long delayVal = 16;
unsigned long timePast = 0;

// Create a web server object
WebServer server(80);
int brightness = 127;

void fill(uint32_t c) {
  // uint32_t color = pixels.Color(brightness, brightness, brightness);
	for (int i = 0; i < NUMPIXELS; i++) {
		pixels.setPixelColor(i, c);
	}
}

float mapf(float p, float a1, float a2, float b1, float b2) {
	return (p - a1) * (b2 - b1) / (a2 - a1) + b1;
}

float period(float p) {
  return -0.5 * cos(TWO_PI * p) + 0.5;
}

void animate2(float t) {
  fill(pixels.Color(0,0,0));
  for (int i = 0; i < NUMPIXELS; ++i) {
    float myT = (1.0 * i / NUMPIXELS + t);
    myT = myT - (int) myT;
    uint16_t mycolor = (int) (myT * 65536);
    pixels.setPixelColor(i, pixels.ColorHSV(mycolor, 255, brightness));
  }
}

void animate(float t) {
  fill(pixels.Color(0,0,0));

  int w2 = width/2;

  float x1 = mapf(period(t), 0, 1, w2, NUMPIXELS - w2);
  float x1r = constrain(x1 - (int) x1, 0, 0.99);
  int x1LOW  = constrain((int) floorf(x1) - w2, 0, NUMPIXELS-1);
  int x1HIGH = constrain((int) floorf(x1) + w2, 0, NUMPIXELS-1);

  float x2 = mapf(1-period(t), 0, 1, w2, NUMPIXELS - w2);
  float x2r = constrain(x2 - (int) x2, 0, 0.99);
  int x2LOW  = constrain((int) floorf(x2) - w2, 0, NUMPIXELS-1);
  int x2HIGH = constrain((int) floorf(x2) + w2, 0, NUMPIXELS-1);

  if (t < 0.5) {

    for (int i = x1LOW; i <= x1HIGH; ++i) {
      if      (i == x1LOW)  pixels.setPixelColor(i, int(brightness * (1-x1r)), 0, 0);
      else if (i == x1HIGH) pixels.setPixelColor(i, int(brightness * (x1r)),   0, 0);
      else                  pixels.setPixelColor(i, brightness,                0, 0); 
    }
    for (int i = x2LOW; i <= x2HIGH; ++i) {
      if      (i == x2LOW)  pixels.setPixelColor(i, int(brightness * (1-x2r)), int(brightness * (1-x2r)), 0);
      else if (i == x2HIGH) pixels.setPixelColor(i, int(brightness * (x2r)),   int(brightness * (x2r)),   0);
      else                  pixels.setPixelColor(i, brightness,                brightness,                0); 
    }

  } else {

    for (int i = x2LOW; i <= x2HIGH; ++i) {
      if      (i == x2LOW)  pixels.setPixelColor(i, int(brightness * (1-x2r)), int(brightness * (1-x2r)), 0);
      else if (i == x2HIGH) pixels.setPixelColor(i, int(brightness * (x2r)),   int(brightness * (x2r)),   0);
      else                  pixels.setPixelColor(i, brightness,                brightness,                0); 
    }
    for (int i = x1LOW; i <= x1HIGH; ++i) {
      if      (i == x1LOW)  pixels.setPixelColor(i, int(brightness * (1-x1r)), 0, 0);
      else if (i == x1HIGH) pixels.setPixelColor(i, int(brightness * (x1r)),   0, 0);
      else                  pixels.setPixelColor(i, brightness,                0, 0); 
    }

  }
}

void setup() {
  Serial.begin(115200);

  pixels.begin();
  Serial.println("LED Strip is ready.");
  setupServer();
  // fill();
  // pixels.show();
  timePast = millis();
}

void loop() {

  if ((millis() - timePast) > delayVal) {
    // Handle incoming client requests
    server.handleClient();
    float t = mapf(frameCount % numFrames, 0, numFrames, 0, 1);
    animate2(t);
    pixels.show();
    frameCount++;
    timePast = millis();
  }
}
