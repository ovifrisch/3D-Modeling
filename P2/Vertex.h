#ifndef VERTEX_H
#define VERTEX_H

class Vertex {
public:
	Vertex(float x, float y, float z) {
		this->x = x;
		this->y = y;
		this->z = z;
	}
	Vertex() {}
	float x;
	float y;
	float z;
};

#endif
