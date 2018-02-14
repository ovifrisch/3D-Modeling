#ifndef OBJECT_H
#define OBJECT_H

#include <vector>
#include "Edge.h"
#include "Vertex.h"

class Object {
public:
	Object() {} //default constructor	
	std::vector<Vertex*> vertices;
	std::vector<Edge*> edges;


	void addEdge(int index1, int index2);	
	void addVertex(float x, float y, float z);
	std::vector<float> centroid();
};

std::vector<float> Object::centroid() {
	float x_sum = 0;
	float y_sum = 0;
	float z_sum = 0;
	for (int i = 0; i < vertices.size(); i++) {
		x_sum += vertices[i]->x;
		y_sum += vertices[i]->y;
		z_sum += vertices[i]->z;
	}

	std::vector<float> result;
	result.push_back(x_sum/vertices.size());
	result.push_back(y_sum/vertices.size());
	result.push_back(z_sum/vertices.size());
	return result;
}

void Object::addVertex(float x, float y, float z) {
	Vertex* v = new Vertex(x, y, z);
	vertices.push_back(v);
}

void Object::addEdge(int index1, int index2) {
	Edge* e = new Edge(index1, index2);
	edges.push_back(e);
}

#endif
