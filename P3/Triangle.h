#ifndef Triangle_H
#define Triangle_H

#include "ObjectVertex.h"
#include "FloatTriple.h"
#include <iostream>

class Triangle {
public:
	Triangle(ObjectVertex* v1, ObjectVertex* v2, ObjectVertex* v3) {
		this->v1 = v1;
		this->v2 = v2;
		this->v3 = v3;
		setNormal();
	}
	bool lessThan(Triangle rhs, int projection);
	float minX();
	float minY();
	float minZ();
	void setNormal();
	FloatTriple* crossProduct(FloatTriple a, FloatTriple b);
	FloatTriple* normalize(FloatTriple vector);
	FloatTriple depthOrderX();
	FloatTriple depthOrderY();
	FloatTriple depthOrderZ();


	ObjectVertex* v1;
	ObjectVertex* v2;
	ObjectVertex* v3;
	FloatTriple* normal;
};

FloatTriple Triangle::depthOrderX() {
	if (v1->x >= v2->x && v1->x >= v3->x) {
		if (v2->x >= v3->x) {
			return FloatTriple(1, 2, 3);
		}
		else {
			return FloatTriple(1, 3, 2);
		}
	}
	else if (v2->x >= v1->x && v2->x >= v3->x) {
		if (v1->x > v3->x) {
			return FloatTriple(2, 1, 3);
		}
		else {
			return FloatTriple(2, 3, 1);
		}
	}
	else {
		if (v2->x > v1->x) {
			return FloatTriple(3, 2, 1);
		}
		else {
			return FloatTriple(3, 1, 2);
		}
	}
}

FloatTriple Triangle::depthOrderY() {
	if (v1->y >= v2->y && v1->y >= v3->y) {
		if (v2->y >= v3->y) {
			return FloatTriple(1, 2, 3);
		}
		else {
			return FloatTriple(1, 3, 2);
		}
	}
	else if (v2->y >= v1->y && v2->y >= v3->y) {
		if (v1->y > v3->y) {
			return FloatTriple(2, 1, 3);
		}
		else {
			return FloatTriple(2, 3, 1);
		}
	}
	else {
		if (v2->y > v1->y) {
			return FloatTriple(3, 2, 1);
		}
		else {
			return FloatTriple(3, 1, 2);
		}
	}
}

FloatTriple Triangle::depthOrderZ() {
	if (v1->z >= v2->z && v1->z >= v3->z) {
		if (v2->z >= v3->z) {
			return FloatTriple(1, 2, 3);
		}
		else {
			return FloatTriple(1, 3, 2);
		}
	}
	else if (v2->z >= v1->z && v2->z >= v3->z) {
		if (v1->z > v3->z) {
			return FloatTriple(2, 1, 3);
		}
		else {
			return FloatTriple(2, 3, 1);
		}
	}
	else {
		if (v2->z > v1->z) {
			return FloatTriple(3, 2, 1);
		}
		else {
			return FloatTriple(3, 1, 2);
		}
	}
}

//the projection param indicates the type of ortho projection
bool Triangle::lessThan(Triangle rhs, int projection) {
	switch (projection) {
		case 0: //XY projection. compare minZ
			if (minZ() < rhs.minZ()) {
				return true;
			}
			return false;
		case 1: //XZ projection. compare minY
			if (minY() < rhs.minY()) {
				return true;
			}
			return false;
		case 2: //YZ projection. compare minX
			if (minX() < rhs.minX()) {
				return true;
			}
			return false;
		default:
			std::cout << "Error" << std::endl;
	}
	return 0;
}

float Triangle::minX() { 
	float minX = v1->x;
	if (v2->x < minX) minX = v2->x;
	if (v3->x < minX) minX = v3->x;
	return minX;
}

float Triangle::minY() {
	float minY = v2->y;
	if (v2->y < minY) minY = v2->y;
	if (v3->y < minY) minY = v3->y;
	return minY;
}

float Triangle::minZ() {
	float minZ = v1->z;
	if (v2->z < minZ) minZ = v2->z;
	if (v3->z < minZ) minZ = v3->z;
	return minZ;
}

void Triangle::setNormal() { //IMPLEMENT
	//compute cross product between v2 - v1 and v3 - v1
	normal = crossProduct(FloatTriple(v2->x - v1->x, v2->y - v1->y, v2->z - v1->z), FloatTriple(v3->x - v1->x, v3->y - v1->y, v3->z - v1->z));
}

FloatTriple* Triangle::crossProduct(FloatTriple a, FloatTriple b) {
	float x = (a.y * b.z) - (a.z * b.y);
	float y = (a.z * b.x) - (a.x * b.z);
	float z = (a.x * b.y) - (a.y * b.x);
	return normalize(FloatTriple(x, y, z));
}

FloatTriple* Triangle::normalize(FloatTriple vector) {
	float magnitude = sqrt(pow(vector.x, 2) + pow(vector.y, 2) + pow(vector.z, 2));
	float x = vector.x / magnitude;
	float y = vector.y / magnitude;
	float z = vector.z / magnitude;
	FloatTriple* result = new FloatTriple(x, y, z);
	return result;
}


#endif