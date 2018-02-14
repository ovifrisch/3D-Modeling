#ifndef EDGE_H
#define EDGE_H

class Edge {
public:
	Edge(int pointIndex1, int pointIndex2) {
		index1 = pointIndex1;
		index2 = pointIndex2;
	}
	int index1;
	int index2;
};

#endif
