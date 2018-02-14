#ifndef EdgeH
#define EdgeH

class Edge {
public:
	Edge() {
		
	}
	Edge(int xo, int yo, int xf, int yf) {
		this->xo = xo;
		this->yo = yo;
		this->xf = xf;
		this->yf = yf;
	}
	int xo;
	int yo;
	int xf;
	int yf;
};


#endif