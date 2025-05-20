#include <Adafruit_NeoPixel.h>
#include <math.h>
#include <TimeLib.h>
#define PIN 6
#define NUMPIXELS 50
#define NUMFRAMES 450
#define FPS 60
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_RGB + NEO_KHZ800);
long frameCount = 1;
#define BLACK pixels.Color(0,0,0)

time_t THETIME = 1735092968; // yeag need to change ever so often
double points = 1.0;

typedef struct {
    double r;       // a fraction between 0 and 1
    double g;       // a fraction between 0 and 1
    double b;       // a fraction between 0 and 1
} rgb;

typedef struct {
    double h;       // angle in degrees
    double s;       // a fraction between 0 and 1
    double v;       // a fraction between 0 and 1
} hsv;

void setup() {
	// Serial.begin(9600);
	pixels.begin();
	//Serial.begin(9600);
	setTime(THETIME); 
}

void loop() {
	pixels.clear();

	double t = fmod((1.0 * frameCount) / (1.0 * NUMFRAMES), 1.0);
	double p = mapd(period(t), 0, 1, 0, NUMPIXELS);
	int hr = hour();
	int strength = 16 <= hr && hr <= 23 ? 150 : 0 <= hr && hr <= 7 ? 15 : 0;

	for (int n = 0; n < NUMPIXELS; ++n) {
		// color shenanigans
		hsv myHSV;
		myHSV.h = fmod(mapd(n, 0, (1/points) * NUMPIXELS, 0, 360),360);
		myHSV.s = 1; myHSV.v = 1;
		rgb myRGB = hsv2rgb(myHSV);
		uint32_t myColor = rgbToColor(myRGB, strength);
		
		// distance whatever
		int size = 10;
		double dist = absd(n-p);
		double val = constraind(mapd(dist, 0, size/2, 1, 0),0,1);
		pixels.setPixelColor(n, lerpColor(val, BLACK, myColor));

	}
	
	pixels.show();
	if (frameCount % NUMFRAMES == 0) points = fmod(points+1, 5);
	frameCount++;
	// delay(1000.0 / FPS);

}