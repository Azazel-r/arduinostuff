#include <Adafruit_NeoPixel.h>
#define PIN 6
#define NUMPIXELS 50
#define NUMFRAMES 300
#define FPS 60
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_RGB + NEO_KHZ800);
int frameCount = 0;

void setup() {
	pixels.begin();
}

void loop() {
	float t = 1.0 * frameCount++ / NUMFRAMES % 1;

	pixels.show()
	delay(1.0 / FPS)
}

// TODO sine stuff lerp stuff map t value whatever