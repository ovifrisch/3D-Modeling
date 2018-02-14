#ifndef ObjectVertex_H
#define ObjectVertex_H

#include "FloatTriple.h"
#include <math.h>
#include <iostream>

class ObjectVertex {
public:
	ObjectVertex(float x, float y, float z) {
		this->x = x;
		this->y = y;
		this->z = z;
		triangleIndex = -1;
	}
	ObjectVertex() {
		triangleIndex = -1;
	}
	void setIntensity(int n, float ka, float kd, float ks, float Ia, float Il, float K, FloatTriple f, FloatTriple l, FloatTriple normal, FloatTriple r, FloatTriple v, int projection);
	ObjectVertex operator-(const ObjectVertex rhs);
	float x;
	float y;
	float z;
	float intensityXY;
	float intensityXZ;
	float intensityYZ;
	int triangleIndex;
	FloatTriple* normal;
	static float dotProduct(FloatTriple v1, FloatTriple v2);

private:
	float magnitude(FloatTriple vector);
	float normalizeIntensity(float intensity);
};

void ObjectVertex::setIntensity(int n, float ka, float kd, float ks, float Ia, float Il, float K, FloatTriple f, FloatTriple l, FloatTriple normal, FloatTriple r, FloatTriple v, int projection) {
	float ambient = ka * Ia;
	float diffuse = (Il / (magnitude(f - FloatTriple(x, y, z)) + K)) * (dotProduct(l, normal) * kd);
	float specular = (Il / (magnitude(f - FloatTriple(x, y, z)) + K) * (pow(dotProduct(r, v), n) * ks));

	if (projection == 0) intensityXY = ambient + diffuse + specular;
	else if (projection == 1) intensityXZ = ambient + diffuse + specular;
	else if (projection == 2) intensityYZ = ambient + diffuse + specular;
}

float ObjectVertex::normalizeIntensity(float intensity) {
	float magnitude = sqrt(pow(intensity, 2) + pow(intensity, 2) + pow(intensity, 2));
	std::cout << "Intensity: " << intensity << std::endl;
	std::cout << "Magnitude: " << magnitude << std::endl;
	std::cout << "Normalized: " << (intensity / magnitude) << std::endl;
	return (intensity / magnitude);
}

ObjectVertex ObjectVertex::operator-(const ObjectVertex rhs) {
	return ObjectVertex(x - rhs.x, y - rhs.y, z - rhs.z);
}

float ObjectVertex::magnitude(FloatTriple vector) {
	return sqrt(pow(vector.x, 2) + pow(vector.y, 2) + pow(vector.z, 2));
}

float ObjectVertex::dotProduct(FloatTriple v1, FloatTriple v2) {
	return (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z);
}

#endif

