#include <Adafruit_NeoPixel.h>
#define PIN 6
#define NUMPIXELS 50
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_RGB + NEO_KHZ800);

void setup() {
  pixels.begin();
  pixels.show();
}

void loop() {

}
