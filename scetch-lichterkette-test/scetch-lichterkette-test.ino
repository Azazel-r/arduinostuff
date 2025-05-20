#include <Adafruit_NeoPixel.h>
#define PIN 6
#define NUMPIXELS 50
#define DELAYVAL 25
NEW SKETCH

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_RGB + NEO_KHZ800);
#define LOVE1 pixels.Color(127, 40, 107)
#define LOVE2 pixels.Color(200,0,120)


void setup() {
	pixels.begin();
}

// Hellblaue und Gelbe Wellen von rechts nach links
void loop() {
	testding();
}

void testding() {
	for (int i = 0; i < NUMPIXELS; i++) {
		pixels.clear();
		for (int j = i; j < (i+10); j++) {
			pixels.setPixelColor(j%50, pixels.Color(200,0,0));
			pixels.setPixelColor((j+25)%50, pixels.Color(0,200,0));
		}
		pixels.show();
		delay(DELAYVAL);
	}
}

void loveding() {
	for (int i = 0; i < NUMPIXELS; i++) {
		fill(LOVE1);
	for (int j = i; j < (i+8); j++) {
		pixels.setPixelColor(j%50, LOVE2);
		pixels.setPixelColor((j+25)%50, LOVE2);
		}
		pixels.show();
		delay(DELAYVAL);
	}
}

void fill(uint32_t color) {
	for (int i = 0; i < NUMPIXELS; i++) {
		pixels.setPixelColor(i, color);
	}
}