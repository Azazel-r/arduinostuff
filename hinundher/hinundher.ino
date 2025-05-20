#include <Adafruit_NeoPixel.h>
#define PIN 6
#define NUMPIXELS 50
#define DELAYVAL 50
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_RGB + NEO_KHZ800);
#define FARBE pixels.Color(0,100,100) // Farbe
#define WIDTH 8 // Breite der Welle


void setup() {
	pixels.begin();
	pixels.show();
	}

	void loop() {
	hin();
	her();
}

void hin() {
	for (int i = 0; i < NUMPIXELS-WIDTH; i++) {
		pixels.clear();
		for (int j = i; j < (i+WIDTH); j++) {
		pixels.setPixelColor(j, FARBE);
		}
		pixels.show();
		delay(DELAYVAL);
	}
}

void her() {
	for (int i = NUMPIXELS-1; i > WIDTH-1; i--) {
		pixels.clear();
		for (int j = i; j > (i-WIDTH); j--) {
		pixels.setPixelColor(j, FARBE);
		}
		pixels.show();
		delay(DELAYVAL);
	}
}