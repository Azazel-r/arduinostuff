#include <Adafruit_NeoPixel.h>
#define PIN 6
#define NUMPIXELS 50
#define DELAYVAL 50
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_RGB + NEO_KHZ800);
#define FARBE pixels.Color(0, 200, 200)
#define VARIATION 0.2

static int FARBE2[] = {70,70,70};

void setup() {
  pixels.begin();
  colorWipe(FARBE);
  delay(1000);
}

void loop() {
  randomSparkle(15, FARBE, FARBE2, 2);
  delay(1000);
}

void colorWipe(uint32_t color) {
  for (int i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i, color);
    pixels.show();
    delay(DELAYVAL);
  }
}

void randomSparkle(int amount, uint32_t bg, int * color, int times) {
  int rndPixel[amount];
  for (int i = 0; i < amount; i++) {
    rndPixel[i] = random(0,50);
  }
  uint32_t * colors = colorField(color);
  for (int h = 0; h < times; h++) {
    // All pixels get set from the dimmest to the brightest color
    for (int i = 0; i < 5; i++) {
      for (int j = 0; j < amount; j++) {
        pixels.setPixelColor(rndPixel[j], colors[i]);
      }
      pixels.show();
      delay(125);
    }
    delay(200);
    // All pixels get set from the brightest to the dimmest color
    for (int i = 4; i > -1; i--) {
      for (int j = 0; j < amount; j++) {
        pixels.setPixelColor(rndPixel[j], colors[i]);
      }
      pixels.show();
      delay(125);
    }
  }
  // Reset all pixels to original color
  for (int i = 0; i < amount; i++) {
    pixels.setPixelColor(rndPixel[i], bg);
  }
  pixels.show();
}

uint32_t* colorField(int* rgb) {
  static uint32_t array[5];
  int rgbCopy[] = { rgb[0], rgb[1], rgb[2] };
  for (int i = 0; i < 5; i++) {
    for (int j = 0; j < 3; j++) {
      rgbCopy[j] = rgb[j] * (1-2*VARIATION + VARIATION * i);
      if (rgbCopy[j] < 0) rgbCopy[j] = 0;
      else if (rgbCopy[j] > 255) rgbCopy[j] = 255;
    }
    array[i] = pixels.Color(rgbCopy[0], rgbCopy[1], rgbCopy[2]);
  }
  return array;
}