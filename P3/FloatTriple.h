#ifndef FloatTriple_H
#define FloatTriple_H

#include <iostream>
#include <math.h>

class FloatTriple {
public:
	FloatTriple(float x, float y, float z) {
		this->x = x;
		this->y = y;
		this->z = z;
	}
	FloatTriple() {}
	FloatTriple operator-(const FloatTriple rhs);
	float magnitude();
	float x;
	float y;
	float z;
};

FloatTriple FloatTriple::operator-(const FloatTriple rhs) { //FIX THIS SHIT
	return FloatTriple(x - rhs.x, y - rhs.y, z - rhs.z);
}

float FloatTriple::magnitude() {
	return sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
}

#endif