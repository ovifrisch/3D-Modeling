#ifndef EdgeInfoH
#define EdgeInfoH

#include <vector>
#include "Edge.h"

class EdgeInfo {
public:
	EdgeInfo() {
	}
	int value;
	int x;
	int y;
	std::vector<Edge*> edgeList;

};


#endif