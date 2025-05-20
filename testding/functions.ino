double constraind(double p, double a, double b) {
	return p < a ? a : p > b ? b : p;
}

double mapd(double p, double a1, double a2, double b1, double b2) {
	return (p - a1) * (b2 - b1) / (a2 - a1) + b1;
}

double absd(double p) {
	return p < 0 ? -p : p;
}

double period(double p) {
	return -0.5 * cos(TWO_PI * p) + 0.5;
}

uint32_t lerpColor(double p, uint32_t c1, uint32_t c2) {
	// extract color values (RGB) with shifting and mask it with 0xFF aka an unsigned 8bit integer
	// color value basically looks like this methinks: 0000 0000 RRRR RRRR GGGG GGGG BBBB BBBB
	// could also use Red(), Green(), Blue() but doing it this way teaches me how it works
	uint8_t r[2] = {(c1 >> 16) & 0xFF, (c2 >> 16) & 0xFF};
	uint8_t g[2] = {(c1 >> 8)  & 0xFF, (c2 >> 8)  & 0xFF};
	uint8_t b[2] = { c1        & 0xFF,  c2        & 0xFF};

	// return the mapped 
	return pixels.Color((uint8_t) (mapd(p, 0, 1, r[0], r[1])), 
						(uint8_t) (mapd(p, 0, 1, g[0], g[1])), 
						(uint8_t) (mapd(p, 0, 1, b[0], b[1])));
}

// hsv to rgb and rgb to hsv
// taken from
// https://stackoverflow.com/questions/3018313/algorithm-to-convert-rgb-to-hsv-and-hsv-to-rgb-in-range-0-255-for-both

hsv rgb2hsv(rgb in)
{
    hsv         out;
    double      min, max, delta;

    min = in.r < in.g ? in.r : in.g;
    min = min  < in.b ? min  : in.b;

    max = in.r > in.g ? in.r : in.g;
    max = max  > in.b ? max  : in.b;

    out.v = max;                                // v
    delta = max - min;
    if (delta < 0.00001)
    {
        out.s = 0;
        out.h = 0; // undefined, maybe nan?
        return out;
    }
    if( max > 0.0 ) { // NOTE: if Max is == 0, this divide would cause a crash
        out.s = (delta / max);                  // s
    } else {
        // if max is 0, then r = g = b = 0              
        // s = 0, h is undefined
        out.s = 0.0;
        out.h = NAN;                            // its now undefined
        return out;
    }
    if( in.r >= max )                           // > is bogus, just keeps compilor happy
        out.h = ( in.g - in.b ) / delta;        // between yellow & magenta
    else
    if( in.g >= max )
        out.h = 2.0 + ( in.b - in.r ) / delta;  // between cyan & yellow
    else
        out.h = 4.0 + ( in.r - in.g ) / delta;  // between magenta & cyan

    out.h *= 60.0;                              // degrees

    if( out.h < 0.0 )
        out.h += 360.0;

    return out;
}

rgb hsv2rgb(hsv in)
{
    double      hh, p, q, t, ff;
    long        i;
    rgb         out;

    if(in.s <= 0.0) {       // < is bogus, just shuts up warnings
        out.r = in.v;
        out.g = in.v;
        out.b = in.v;
        return out;
    }
    hh = in.h;
    if(hh >= 360.0) hh = 0.0;
    hh /= 60.0;
    i = (long)hh;
    ff = hh - i;
    p = in.v * (1.0 - in.s);
    q = in.v * (1.0 - (in.s * ff));
    t = in.v * (1.0 - (in.s * (1.0 - ff)));

    switch(i) {
    case 0:
        out.r = in.v;
        out.g = t;
        out.b = p;
        break;
    case 1:
        out.r = q;
        out.g = in.v;
        out.b = p;
        break;
    case 2:
        out.r = p;
        out.g = in.v;
        out.b = t;
        break;

    case 3:
        out.r = p;
        out.g = q;
        out.b = in.v;
        break;
    case 4:
        out.r = t;
        out.g = p;
        out.b = in.v;
        break;
    case 5:
    default:
        out.r = in.v;
        out.g = p;
        out.b = q;
        break;
    }
    return out;     
}

uint32_t rgbToColor(rgb col, int strength) {
	return pixels.Color(col.r*strength, col.g*strength, col.b*strength);
}