#include <Adafruit_NeoPixel.h>
#define PIN 6
#define NUMPIXELS 50
#define DELAYVAL 25
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_RGB + NEO_KHZ800);
#define FARBE pixels.Color(100,0,0) // Farbe 1
#define FARBE2 pixels.Color(100,100,0) // Farbe 2
#define BLACK pixels.Color(0,0,0)
#define WIDTH 4 // Breite der Welle, muss am besten kleiner als 25 sein

// HP
void setup() {
  pixels.begin();
  pixels.show();
}

void loop() {
  ncToRight(FARBE, FARBE2, 5);
  pause(250);
  ncToLeft(FARBE, FARBE2, 5);
  pause(250);
  ncToMiddle(FARBE, FARBE2, 10);
  pause(250);
}

// Meine eignen dingsns
void pause(int ms) {
  pixels.clear();
  pixels.show();
  delay(ms);
}

void ncToMiddle(uint32_t f1, uint32_t f2, int times) {
  for (int wdh = 0; wdh < times; wdh++) {
    for (int i = 0; i < NUMPIXELS/2; i++) {
      pixels.clear();
      for (int j = i; j < (i+WIDTH); j++) {
        if (j%3 == 0) {
          pixels.setPixelColor(j%25, f2);
          pixels.setPixelColor(NUMPIXELS-1-(j%25), f2);
        } else {
          pixels.setPixelColor(j%25, f1);
          pixels.setPixelColor(NUMPIXELS-1-(j%25), f1);
        }
      }
      pixels.show();
      delay(DELAYVAL);
    }
  }
}

void ncToLeft(uint32_t f1, uint32_t f2, int times) {
  for (int wdh = 0; wdh < times; wdh++) {
    for (int i = 0; i < NUMPIXELS; i++) {
      pixels.clear();
      for (int j = i; j < (i+WIDTH); j++) {
        if (j < i+WIDTH/2) {
          pixels.setPixelColor(j%50, f1);
          pixels.setPixelColor((j+16)%50, f1);
          pixels.setPixelColor((j+32)%50, f1);
        } else {
          pixels.setPixelColor(j%50, f2);
          pixels.setPixelColor((j+16)%50, f2);
          pixels.setPixelColor((j+32)%50, f2);
        }
      }
      pixels.show();
      delay(DELAYVAL);
    }
  }
}

void ncToRight(uint32_t f1, uint32_t f2, int times) {
  for (int wdh = 0; wdh < times; wdh++) {
    for (int i = NUMPIXELS-1; i > -1; i--) {
      pixels.clear();
      for (int j = i; j < (i+WIDTH); j++) {
        if (j < i+WIDTH/2) {
          pixels.setPixelColor(j%50, f1);
          pixels.setPixelColor((j+16)%50, f1);
          pixels.setPixelColor((j+32)%50, f1);
        } else {
          pixels.setPixelColor(j%50, f2);
          pixels.setPixelColor((j+16)%50, f2);
          pixels.setPixelColor((j+32)%50, f2);
        }
      }
      pixels.show();
      delay(DELAYVAL);
    }
  }
}