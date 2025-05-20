#include <Adafruit_NeoPixel.h>
#define PIN 6
#define NUMPIXELS 50
#define TOP 22
#define DELAYVAL 100
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_RGB + NEO_KHZ800);
#define GREEN  pixels.Color(52, 173, 0)
#define RED    pixels.Color(173, 0, 11)
#define GREY   pixels.Color(211, 211, 211)
#define BLUE   pixels.Color(0, 69, 142)
#define YELLOW pixels.Color(228, 194, 0)
#define DARK   pixels.Color(0,0,0)


void setup() {
	pixels.begin();
}

void loop() {

	pixels.clear();
	pixels.show();
	delay(4*DELAYVAL);

	makeTree(DELAYVAL, GREEN);
	delay(20*DELAYVAL);
	makeTree(DELAYVAL, RED);
	delay(20*DELAYVAL);
	makeTree(DELAYVAL, GREY);
	delay(20*DELAYVAL);
	makeTree(DELAYVAL, DARK);
	delay(4*DELAYVAL);

	for (int i = 0; i < 8*6; i++) {
		glimmer(i);
		delay(DELAYVAL*2);
	}

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

bool emplace(int index, uint32_t color) {
	if (0 <= index && index < 50) pixels.setPixelColor(index, color);
	return index < 50;
}

void glimmer(int i) {
	pixels.clear();
	int stelle = ((i%8) * 3) % 8 - 8;
	while (true) {
		if (!emplace(stelle, BLUE)) break;
		stelle += 2;
		if (!emplace(stelle, YELLOW)) break;
		stelle += 2;
		if (!emplace(stelle, RED)) break;
		stelle += 2;
		if (!emplace(stelle, GREEN)) break;
		stelle += 2;
	}
	pixels.show();
}
