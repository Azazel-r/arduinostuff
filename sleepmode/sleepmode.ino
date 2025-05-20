#include <Adafruit_NeoPixel.h>
#define PIN 6
#define NUMPIXELS 50
#define TOP 22
#define DELAYVAL 1000
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_RGB + NEO_KHZ800);
#define GREEN  pixels.Color(5, 17, 0)
#define RED    pixels.Color(17, 0, 1)
#define GREY   pixels.Color(21, 21, 21)
#define DARK   pixels.Color(0,0,0)

void setup() {
	pixels.begin();
}

void loop() {
	makeTree(DELAYVAL, GREEN);
	delay(5*DELAYVAL);
	makeTree(DELAYVAL, RED);
	delay(5*DELAYVAL);
	makeTree(DELAYVAL, GREY);
	delay(5*DELAYVAL);
	makeTree(DELAYVAL, DARK);
	delay(5*DELAYVAL);
}

void makeTree(int dl, uint32_t color) {
	int steps = TOP < 25 ? 50-TOP : TOP;
	for (int i = 0; i < steps; i++) {
		if (TOP+i < 50) pixels.setPixelColor(TOP+i, color);
		if (TOP-i >= 0) pixels.setPixelColor(TOP-i, color);
		pixels.show();
		delay(dl);
	}
}