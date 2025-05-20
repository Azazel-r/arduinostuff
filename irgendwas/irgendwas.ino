#include <Adafruit_NeoPixel.h>
#define PIN 6
#define NUMPIXELS 50
#define DELAYVAL 2000
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_RGB + NEO_KHZ800);
#define GREY pixels.Color(211, 211, 211)
#define RED pixels.Color(173, 0, 11)
#define GREEN pixels.Color(52, 173, 0)

void setup() {
	pixels.begin();
}

void loop() {
	fill_delay(GREY, 75);
	delay(DELAYVAL);
	fill_delay(RED, 75);
	delay(DELAYVAL);
	fill_delay(GREEN, 75);
	delay(DELAYVAL);
}

void fill_delay(uint32_t color, int dl) {
	for (int i = 0; i < NUMPIXELS; i++) {
		pixels.setPixelColor(i, color);
		pixels.show();
		delay(dl);
	}
}