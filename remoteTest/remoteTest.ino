// -------------------------------------------------------------------------------------------------------------------------------------


#include <WiFi.h>
#include <WebServer.h>
#include <Adafruit_NeoPixel.h>
#define PIN 20
#define NUMPIXELS 50

// define pixels object whatever
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_RGB + NEO_KHZ800);

int frameCount = 1;
int startFramesForAnim = 0;
const int numFrames = 240;
const int width = 5;
unsigned long delayVal = 16;
unsigned long timePast = 0;
int activeFunction = -3;
int activeColValue = 0;
int rainbowarray[] =       {0, 10922, 21845, 32768, 43690, 54613};
int rainbowarraysorted[] = {21845, 10922, 0, 54613, 43690, 32768};

// Create a web server object
WebServer server(80);
int brightness = 200;

uint32_t lerpColor(float p, uint32_t c1, uint32_t c2) {
	// extract color values (RGB) with shifting and mask it with 0xFF aka an unsigned 8bit integer
	// color value basically looks like this methinks: 0000 0000 RRRR RRRR GGGG GGGG BBBB BBBB
	// could also use Red(), Green(), Blue() but doing it this way teaches me how it works
	uint8_t r[2] = {(c1 >> 16) & 0xFF, (c2 >> 16) & 0xFF};
	uint8_t g[2] = {(c1 >> 8)  & 0xFF, (c2 >> 8)  & 0xFF};
	uint8_t b[2] = { c1        & 0xFF,  c2        & 0xFF};

	// return the mapped 
	return pixels.Color((uint8_t) (mapf(p, 0, 1, r[0], r[1])), 
						(uint8_t) (mapf(p, 0, 1, g[0], g[1])), 
						(uint8_t) (mapf(p, 0, 1, b[0], b[1])));
}

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

float easeOut(float p) {
  return sin(0.5 * p * PI);
}

void solidColor(int n) {
  if      (n == 1) fill(pixels.Color(brightness, 0, 0)); // G
  else if (n == 2) fill(pixels.Color(brightness, brightness, 0)); // Y
  else if (n == 3) fill(pixels.Color(0, brightness, 0)); // R
  else if (n == 4) fill(pixels.Color(brightness, brightness, brightness)); // W
}

void solidLerp(float val) {
  fill(lerpColor(val, pixels.Color(brightness, 0, 0), pixels.Color(0,brightness,0)));
}

void fillFromMiddle(float p, uint32_t col, int add) {
  int upstart = NUMPIXELS / 2;
  int downstart = upstart;
  if (NUMPIXELS % 2 == 0) {
    downstart--;
  }
  int upToPixel = (int) floorf(mapf(p, 0, 1, upstart, NUMPIXELS));
  int downToPixel = (int) floorf(mapf(p, 0, 1, downstart, 0));
  for (int i = upstart;   i <= upToPixel;   ++i) pixels.setPixelColor((i+add) % NUMPIXELS, col);
  for (int i = downstart; i >= downToPixel; --i) pixels.setPixelColor((i+add) % NUMPIXELS, col);
}

void startupAnimation(int startframes, int count, int secs) {

  count = count - startframes;
  float t = mapf(count, 0, (secs * 60), 0, 1);
  float t1 = constrain(mapf(t, 0,    0.33, 0, 1), 0, 1);
  float t2 = constrain(mapf(t, 0.33, 0.67, 0, 1), 0, 1);
  float t3 = constrain(mapf(t, 0.67, 1,    0, 1), 0, 1);

  // erst 5 mal im kreis mit weiß

  // t1 = t; // zum test
  int steps = 5;
  int rounds = 3;

  if (t < 0.33) { // zu test if (t < 0.33) {

    int tPixel = (int) floorf(mapf(t1, 0, 1, 0, rounds * NUMPIXELS));
    int currentRound = tPixel / NUMPIXELS;
    brightness = (int) floorf(mapf(currentRound, 0, rounds-1, 0, 255));
    int currentPixel = (steps * (tPixel % NUMPIXELS)) % NUMPIXELS + (steps * (tPixel % NUMPIXELS)) / NUMPIXELS;
    pixels.setPixelColor(currentPixel, pixels.Color(brightness, brightness, brightness));
  } 

  // regenbogen segmente aufpoppen lassen maybe so wellenartig mit einem startpunkt und jede farbwelle ist kürzer als die davor

  else if (0.33 <= t && t < 0.67) {

    // float r_red[] = {0, 0.4};
    // float r_ora[] = {0.16, 0.52};
    // float r_ylw[] = {0.33, 0.64};
    // float r_grn[] = {0.5,  0.76};
    // float r_blu[] = {0.66, 0.88};
    // float r_pur[] = {0.83,    1};

    float r_all[][2] = {{0, 0.4}, {0.16, 0.52}, {0.33, 0.64}, {0.5,  0.76}, {0.66, 0.88}, {0.83,    1}};
    brightness = 255;

    for (int i = 0; i < 6; ++i) {
      float t_temp = constrain(mapf(t2, r_all[i][0], r_all[i][1], 0, constrain(1-(0.167 * i), 0, 1)), 0, constrain(1-(0.167 * i), 0, 1));
      fillFromMiddle(t_temp, pixels.ColorHSV(rainbowarraysorted[i], 255, brightness), 0);
    }

  }

  // das ganze im kreis drehen, (immer schneller?) -> fade into neutrale farbe

  else if (0.67 <= t && t < 1) {

    int spins = 3;
    for (int i = 0; i < 6; ++i) {
      float t_temp = constrain(1-(0.167*i), 0, 1);
      uint32_t col = lerpColor(t3*t3, pixels.ColorHSV(rainbowarraysorted[i], 255, brightness), pixels.Color(brightness, brightness, brightness));
      fillFromMiddle(t_temp, col, t3*t3*spins*NUMPIXELS);
    }

  }
  
  else activeFunction = 4;

}

void animate2(float t) { // rainbow effect
  fill(pixels.Color(0,0,0));
  for (int i = 0; i < NUMPIXELS; ++i) {
    float myT = (1.0 * i / NUMPIXELS + t);
    myT = myT - (int) myT;
    uint16_t mycolor = (int) (myT * 65536);
    pixels.setPixelColor(i, pixels.ColorHSV(mycolor, 255, brightness));
  }
}

void animate(float t) { // green yellow back and forth
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
  // setupServer();
  // fill();
  // pixels.show();
  timePast = millis();
}

void loop() {

  if ((millis() - timePast) > delayVal) {
    // Handle incoming client requests
    // server.handleClient();
    float t = mapf(frameCount % numFrames, 0, numFrames, 0, 1);
    if (activeFunction == -3) startupAnimation(startFramesForAnim, frameCount, 10);
    else if (activeFunction == -2) solidLerp(mapf(activeColValue, 0, 65536, 0, 1));
    else if (activeFunction == -1) animate(t);
    else if (activeFunction == 0) animate2(t);
    else solidColor(activeFunction);
    pixels.show();
    frameCount++;
    timePast = millis();
  }
}
