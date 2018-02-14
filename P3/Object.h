#ifndef Object_H
#define Object_H

#include <vector>
#include "Triangle.h"
#include "ObjectVertex.h"

class Object {
public:
	Object() {} //default constructor


	void addVertex(float x, float y, float z);
	void addTriangle(Triangle* t);


	std::vector<ObjectVertex*> vertices;
	std::vector<Triangle*> triangles;
	int materialID;
};


void Object::addVertex(float x, float y, float z) {
	ObjectVertex* v = new ObjectVertex(x, y, z);
	vertices.push_back(v);
}

void Object::addTriangle(Triangle* t) {
	triangles.push_back(t);
}


#endif
