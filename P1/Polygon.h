#ifndef PolygonH
#define PolygonH

#include <string.h>
#include <iostream>
#include "EdgeInfo.h"
#include "Edge.h"
#include <vector>

class Polygon {
public:
	Polygon() {} // default ctor
	Polygon(int numSides) {
		numVertices = numSides;

	}
	void initVertices(int** points);
	void initLocalDisplay(int width, int height);
	void initClippingParameters(int width, int height);
	void initializeEdgePoints();
	void clearLocalDisplay();
	void initClippingDisplay(int width, int height);
	void clearClippingDisplay();
	bool isMaximum(int x, int y);
	bool isMinimum(int x, int y);
	bool isPracticalMaxOrMin(int x, int y, int index);
	bool isVertex(int x, int y); //not yet used
	bool isEdge(int x0, int y0, int x1, int y1);
	bool isHorizontalEdge(int x0, int y0, int xf, int yf);
	bool doesEdgeChange(int e1x0, int e1y0, int e1x1, int e1y1, int e2x0, int e2y0, int e2x1, int e2x2);
	bool isEntirelyInsideClippingArea(int xLL, int yLL, int x_ext, int y_ext);
	bool isEntirelyOutsideClippingArea(int xLL, int yLL, int x_ext, int y_ext);
	int minXVertex();
	int minYVertex();
	int maxXVertex();
	int maxYVertex();
	void transferToClippingDisplay();
	void pushBackPt(int x, int y, int start_x, int start_y, int end_x, int end_y);
	std::vector<int> getIntersectionsToTheRight(int x, int row);
	std::vector<int> lineSegmentsOnRow(int x, int row);
	bool isStateChangingIntersection(int x, int row);
	int getNumConsecutive(int x, int row);
	bool allPointsOnSingleEdge(int x, int row, int numConsecPts);
	bool isSameEdge(Edge* edeg1, Edge* edge2);
	int getNumVertices(int x, int y, int numConsecPts);
	int getIthVertex(int x, int y, int numConsecPts, int vertex);


	bool definesMinimumOrMaximum(std::vector<Edge*> edges);
	void getCommonVertex(Edge* edge1, Edge* edge2, int &common_vert_x, int &common_vert_y);
	int findPoint(int x, int y);

	int numVertices;
	int** vertices; //v[i][0] is x, v[i][1] is y.
	std::vector<EdgeInfo> edgePoints;
	EdgeInfo** localDisplay;
	int** clippingDisplay;
	int localDisplayHeight;
	int localDisplayWidth;
	int isFullyContainedInClip; //0 if no, 1 if yes
	int lowerLeftXOfClippingWindow;
	int lowerLeftYOfClippingWindow;
	int x_extensionOfClippingWindow;
	int y_extensionOfClippingWindow;
};

void Polygon::initializeEdgePoints() {int xFirst;
	int yFirst;
	int xLast;
	int yLast;
	for (int i = 0; i < numVertices; i++) {
		if (i == 0) {
			xFirst = vertices[numVertices - 1][0];
			yFirst = vertices[numVertices - 1][1];
			xLast = vertices[0][0];
			yLast = vertices[0][1];
		}
		else if (i == numVertices - 1) {
			xFirst = vertices[i-1][0];
			yFirst = vertices[i-1][1];
			xLast = vertices[0][0];
			yLast = vertices[0][1];
		}
		else {
			xFirst = vertices[i-1][0];
			yFirst = vertices[i-1][1];
			xLast = vertices[i+1][0];
			yLast = vertices[i+1][1];
		}

		EdgeInfo* e = new EdgeInfo();
		e->x = vertices[i][0];
		e->y = vertices[i][1];
		e->edgeList.push_back(new Edge(xFirst, yFirst, vertices[i][0], vertices[i][1]));
		e->edgeList.push_back(new Edge(vertices[i][0], vertices[i][1], xLast, yLast));
		edgePoints.push_back(*e);
	}

}

void Polygon::initClippingParameters(int width, int height) {
	lowerLeftXOfClippingWindow = 0;
	lowerLeftYOfClippingWindow = 0;
	x_extensionOfClippingWindow = width;
	y_extensionOfClippingWindow = height;
}

void Polygon::initVertices(int** points) {
	//initialize
	vertices = new int*[numVertices];
	for (int i = 0; i < numVertices; i++) {
		vertices[i] = new int[2];
	}
	//copy
	memcpy(vertices, points, numVertices*sizeof(int*));
}

void Polygon::initClippingDisplay(int width, int height) {
	localDisplayWidth = width;
	localDisplayHeight = height;
	clippingDisplay = new int*[width];
	for (int i = 0; i < width; i++) {
		clippingDisplay[i] = new int[height];
		for (int j = 0; j < height; j++) {
			clippingDisplay[i][j] = 0;
		}
	}
}

void Polygon::clearClippingDisplay() {
	for (int i = 0; i < localDisplayWidth; i++) {
		for (int j = 0; j < localDisplayHeight; j++) {
			clippingDisplay[i][j] = 0;
		}
	}
}

void Polygon::initLocalDisplay(int width, int height) {
	localDisplayWidth = width;
	localDisplayHeight = height;
	localDisplay = new EdgeInfo*[width];
	for (int i = 0; i < width; i++) {
		localDisplay[i] = new EdgeInfo[height];
		for (int j = 0; j < height; j++) {
			localDisplay[i][j].x = i;
			localDisplay[i][j].y = j;
		}
	}
}

void Polygon::clearLocalDisplay() {
	for (int i = 0; i < localDisplayWidth; i++) {
		for (int j = 0; j < localDisplayHeight; j++) {
			localDisplay[i][j].value = 0;
			localDisplay[i][j].x = 0;
			localDisplay[i][j].y = 0;
			localDisplay[i][j].edgeList.clear();
		}
	}
}

void Polygon::transferToClippingDisplay() {
	for (int x = lowerLeftXOfClippingWindow; x < lowerLeftXOfClippingWindow + x_extensionOfClippingWindow; x++) {
		for (int y = lowerLeftYOfClippingWindow; y < lowerLeftYOfClippingWindow + y_extensionOfClippingWindow; y++) {
			if (localDisplay[x][y].value == 1) {
				clippingDisplay[x][y] = 1;
			}
		}
	}
}

bool Polygon::isMaximum(int x, int y) {
	int left;
	int right;
	for (int i = 0; i < numVertices; i++) {
		if (x == vertices[i][0] && y == vertices[i][1]) {
			if (i == 0) {
				left = numVertices - 1;
				right = i + 1;
			}
			else if (i == numVertices - 1) {
				left = i - 1;
				right = 0;
			}
			else {
				left = i - 1;
				right = i + 1;
			}

			if (vertices[left][1] <= y && vertices[right][1] <= y) {
					return true;
			}
			else {

				return false;
			}
		}
	}
	return false;
}

bool Polygon::isMinimum(int x, int y) {
	int left;
	int right;
	for (int i = 0; i < numVertices; i++) {
		if (x == vertices[i][0] && y == vertices[i][1]) {
			if (i == 0) {
				left = numVertices - 1;
				right = i + 1;
			}
			else if (i == numVertices - 1) {
				left = i - 1;
				right = 0;
			}
			else {
				left = i - 1;
				right = i + 1;
			}

			if (vertices[left][1] >= y && vertices[right][1] >= y) {
					return true;
			}
			else {
				return false;
			}
		}
	}
	return false;
}

bool Polygon::isPracticalMaxOrMin(int x, int y, int index) {
	if (edgePoints[index].edgeList.size() == 2) {
		if (definesMinimumOrMaximum(edgePoints[index].edgeList)) {
			return true;
		}
		return false;
	}
	return false;
}

void Polygon::getCommonVertex(Edge* edge1, Edge* edge2, int &common_vert_x, int &common_vert_y) {
	if (edge1->xo == edge2->xo && edge1->yo == edge2->yo) {
		common_vert_x = edge1->xo;
		common_vert_y = edge1->yo;
	}
	else if (edge1->xo == edge2->xf && edge1->yo == edge2->yf) {
		common_vert_x = edge1->xo;
		common_vert_y = edge1->yo;
	}
	else {
		common_vert_x = edge1->xf;
		common_vert_y = edge1->yf;
	}
}

bool Polygon::definesMinimumOrMaximum(std::vector<Edge*> edges) {
	Edge* edge1 = edges[0];
	Edge* edge2 = edges[1];
	int common_vert_x;
	int common_vert_y;
	getCommonVertex(edge1, edge2, common_vert_x, common_vert_y);
	int y1;
	int y2;

	if (edges[0]->xo == common_vert_x && edges[0]->yo == common_vert_y) {
		y1 = edges[0]->yf;
	}
	else {
		y1 = edges[0]->yo;
	}

	if (edges[1]->xo == common_vert_x && edges[1]->yo == common_vert_y) {
		y2 = edges[1]->yf;
	}
	else {
		y2 = edges[1]->yo;
	}

	if ((y1 <= common_vert_y && y2 <= common_vert_y) || (y1 >= common_vert_y && y2 >= common_vert_y)) {
		return true;
	}
	else {
		return false;
	}
}



bool Polygon::isVertex(int x, int y) {
	for (int i = 0; i < numVertices; i++) {
		if (vertices[i][0] == x && vertices[i][1] == y) {
			return true;
		}
	}
	return false;
}

bool Polygon::isHorizontalEdge(int x0, int y0, int xf, int yf) {
	if (xf == localDisplayWidth - 1 && isVertex(x0, y0)) {
		return true;
	}
	if (isVertex(x0, y0) && isVertex(xf, yf)) {
		return true;
	}
	return false;
}

bool Polygon::isEdge(int x0, int y0, int x1, int y1) {
	for (int i = 0; i < numVertices - 1; i++) {
		if ((x0 == vertices[i][0] && y0 == vertices[i][1] && x1 == vertices[i+1][0] && y1 == vertices[i+1][1]) ||
			(x1 == vertices[i][0] && y1 == vertices[i][1] && x0 == vertices[i+1][0] && y0 == vertices[i+1][1])) {
			return true;
		}
	}
	if ((x0 == vertices[numVertices - 1][0] && y0 == vertices[numVertices - 1][1] && x1 == vertices[0][0] && y1 == vertices[0][1]) ||
		(x1 == vertices[numVertices - 1][0] && y1 == vertices[numVertices - 1][1] && x0 == vertices[0][0] && y0 == vertices[0][1])) {
		return true;
	}
	return false;
}

std::vector<int> Polygon::lineSegmentsOnRow(int x, int row) {
	std::vector<int> startPositionXValues;
	for (int i = x; i < localDisplayWidth; i++) {
		if (localDisplay[i][row].value == 1) {
			startPositionXValues.push_back(i);
			while ((++i < localDisplayWidth) && localDisplay[i][row].value == 1);
		}
		else {
			continue;
		}
	}
	return startPositionXValues;
}
	

std::vector<int> Polygon::getIntersectionsToTheRight(int x, int row) {
	int numIntersectionsToTheRight = 0;
	std::vector<int> startPosXValues = lineSegmentsOnRow(x, row);
	std::vector<int> stateChgItersct;

	for (int i = 0; i < startPosXValues.size(); i++) {
		if (isStateChangingIntersection(startPosXValues[i], row)) {
			stateChgItersct.push_back(startPosXValues[i]);
		}
	}
	return stateChgItersct;
}


//Returns true if is state changing intersection, false otherwise
bool Polygon::isStateChangingIntersection(int x, int row) {
	int numConsecPts = getNumConsecutive(x, row);
	if (numConsecPts == 1) {
		if (x == 0 || x == localDisplayWidth - 1) {
			return true;
		}
		if (isVertex(x, row)) {
			if (isMinimum(x, row) || isMaximum(x, row)) {
				return false;
			}
			else { //equal to 1 && a vertex && ~ a min or max
				return true;
			}
		}
		else { //equal to 1 && not a vertex
			if (localDisplay[x][row].edgeList.size() == 1) {
				return true;
			}
			else if (localDisplay[x][row].edgeList.size() > 1) {
				return false;
			}
			else {
				return false;
			}
		}
	}
	else { //not equal to  1
		if (allPointsOnSingleEdge(x, row, numConsecPts)) {
			return true;
		}
		else { //not equal to 1 AND not all points on single line
			if (isHorizontalEdge(x, row, x + numConsecPts - 1, row)) {
				if (x + numConsecPts - 1 == localDisplayWidth) {
					return false;
				}
				else if ((isMinimum(x, row) && isMinimum(x + numConsecPts - 1, row)) || (isMaximum(x, row) && isMaximum(x + numConsecPts - 1, row))) {
					return false;
				}
				else { //not equal to 1 AND not all points on single line AND not a min or max
					return true;
				}
			}
			else { //not equal to 1 AND not all points on single line AND not an edge
				if (getNumVertices(x, row, numConsecPts) == 2) {
					int x_v1 = getIthVertex(x, row, numConsecPts, 1);
					int x_v2 = getIthVertex(x, row, numConsecPts, 2);
					if ((isMinimum(x_v1, row) && isMinimum(x_v2, row)) || (isMaximum(x_v1, row) && isMaximum(x_v2, row))) {
						return false;
					}
					else {
						return true;
					}
				}
				else if (getNumVertices(x, row, numConsecPts) == 1) {
					int x_pos = getIthVertex(x, row, numConsecPts, 1);
					if (isMinimum(x_pos, row) || isMaximum(x_pos, row)) {
						return false;
					}
					else {
						return true;
					}
				}
				else if (getNumVertices(x, row, numConsecPts) == 0) {
					//std::cout << x << ", " << row << std::endl;
					return false;
				}
				else {
					std::cout << "My program does not account for scan line edge segments that contain more than 2 vertices " << std::endl;
					return false;
				}
			}
		}
	}
}


//returns the x pos of the vertex'th vertex in this sequence defined by x and numConsecPts (scanLine)
int Polygon::getIthVertex(int x, int y, int numConsecPts, int vertex) {
	int count = 0;
	for (int i = x; i < x + numVertices; i++) {
		if (isVertex(i, y)) {
			count++;
			if (count == vertex) {
				return i;
			}
		}
	}
	return -1;
}

int Polygon::getNumVertices(int x, int y, int numConsecPts) {
	int count = 0;
	for (int i = x; i < x + numConsecPts; i++) {
		if (isVertex(i, y)) {
			count++;
		}
	}
	return count;
}

bool Polygon::allPointsOnSingleEdge(int x, int row, int numConsecPts) {
	for (int i = x; i < x + numConsecPts - 1; i++) {
		if (localDisplay[i][row].edgeList.size() != 1) {
			return false;
		}
	}
	//at this point we know that each point has a single unique edge

	Edge* startEdge = localDisplay[x][row].edgeList[0];
	for (int i = x + 1; i < x + numConsecPts; i++) {
		if (!isSameEdge(startEdge, localDisplay[i][row].edgeList[0])) {
			return false;
		}
	}
	return true;
}

bool Polygon::isSameEdge(Edge* edge1, Edge* edge2) {
	if ((edge1->xo == edge2->xo) && (edge1->yo == edge2->yo) && (edge1->xf == edge2->xf) && (edge1->yf == edge2->yf)) {
		return true;
	}
	return false;
}

int Polygon::getNumConsecutive(int x, int row) {
	int count = 1;
	while ((++x < localDisplayWidth) && localDisplay[x][row].value == 1) {
		count++;
	}
	return count;
}

bool Polygon::doesEdgeChange(int e1x0, int e1y0, int e1x1, int e1y1, int e2x0, int e2y0, int e2x1, int e2y1) {
	if ((e1x0 == e2x0) && (e1y0 == e2y0) && (e1x1 == e2x1) && (e1y1 == e2y1)) {
		return false;
	}
	return true;
}


bool Polygon::isEntirelyInsideClippingArea(int xLL, int yLL, int x_ext, int y_ext) {
	if((minXVertex() >= xLL) && (minYVertex() >= yLL) && (maxXVertex() <= xLL + x_ext) && (maxYVertex() <= yLL + y_ext)) {
		return true;
	}
	return false;
}

bool Polygon::isEntirelyOutsideClippingArea(int xLL, int yLL, int x_ext, int y_ext) {
	for (int x = xLL; x < xLL + x_ext; x++) {
		for (int y = yLL; y < yLL + y_ext; y++) {
			if (localDisplay[x][y].value == 1) {
				return false;
			}
		}
	}
	return true; 
}

int Polygon::minXVertex() {
	int min = vertices[0][0];
	for (int i = 1; i < numVertices; i++) {
		if (vertices[i][0] < min) {
			min = vertices[i][0];
		}
	}
	return min;
}

int Polygon::minYVertex() {
	int min = vertices[0][1];
	for (int i = 1; i < numVertices; i++) {
		if (vertices[i][1] < min) {
			min = vertices[i][1];
		}
	}
	return min;
}

int Polygon::maxXVertex() {
	int max = vertices[0][0];
	for (int i = 1; i < numVertices; i++) {
		if (vertices[i][0] > max) {
			max = vertices[i][0];
		}
	}
	return max;
}

int Polygon::maxYVertex() {
	int max = vertices[0][1];
	for (int i = 1; i < numVertices; i++) {
		if (vertices[i][1] > max) {
			max = vertices[i][1];
		}
	}
	return max;
}


void Polygon::pushBackPt(int x, int y, int start_x, int start_y, int end_x, int end_y) {
	EdgeInfo* e = new EdgeInfo();
	e->x = x;
	e->y = y;
	e->edgeList.push_back(new Edge(start_x, start_y, end_x, end_y));
	edgePoints.push_back(*e);
}

//scans through edgePoints to find Point
//returns the index or -1 if not found
int Polygon::findPoint(int x, int y) {
	for (int i = 0; i < edgePoints.size(); i++) {
		if (edgePoints[i].x == x && edgePoints[i].y == y) {
			return i;
		}
	}
	return -1;
}

#endif