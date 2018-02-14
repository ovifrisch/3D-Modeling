#if defined (__APPLE__) || defined(MACOSX)
#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#endif

#include "Polygon.h"
#include "EdgeInfo.h"
#include <iostream>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <fstream>
#include <vector>
#include "Edge.h"
using namespace std;


//FILE INPUT
void readFile();
void transferSceneToFile();

//INITIALIZING AND CLEARING
void initializeLocalPolygonMatrices();
void initializeInternalMatrix();
void clearInternal();
void initializeClippingParameters();


//TRANSFERRING FRAMES
void transferInternalMatrixToMonitor();
void transferClippingDisplayToWorld(Polygon* p);


//POLYGON DRAWING HIGH LEVEL
void drawPolygon(Polygon* p, int lineAlgo);
void drawVerticalViewingEdges(Polygon* p);
void drawHorizontalViewingEdges(Polygon* p);
void setLocalVertices(Polygon* p);
void drawPolygonLines_DDA(Polygon* p);
void drawBresenhamLine(int x1, int y1, int x2, int y2, Polygon* p);
void drawPolygonLines_Bresenham(Polygon* p);


//POLYGON DRAWING LOW LEVEL
void drawDDALine(int xStart, int yStart, int xEnd, int yEnd, Polygon* p);
void drawVerticalLine(int x1, int y1, int y2, Polygon* pol);
void drawBresenhamLineSmallPositive(int x1, int y1, int x2, int y2, Polygon* pol);
void drawBresenhamLineLargePositive(int x1, int y1, int x2, int y2, Polygon* pol);
void drawBresenhamLineSmallNegative(int x1, int y1, int x2, int y2, Polygon* pol);
void drawBresenhamLineLargeNegative(int x1, int y1, int x2, int y2, Polygon* pol);


//RASTERIZATION
void rasterize(Polygon* p);
void rasterScanLine(Polygon* p, int row);


//BASIC MATH FUNCTIONS
double getYIntercept(int xStart, int yStart, double slope);
int min(int x, int y);
int max(int x, int y);
bool isOdd(int x);
double getSlope(double x1, double y1, double x2, double y2);
double toRadian(double degree);
void findCenter(int polyIndex, int& x_center, int& y_center);
bool isInsidePolygon(int x, int y, Polygon* p);
bool isOnEdge(int x, int y, Polygon* p, int &index);
int countIntersectionsRight(int x, int y, Polygon* p);


//CALLBACK FUNCTIONS
void init();
void display();
void draw_pix(int x, int y);
void idle();
void mouse(int button, int state, int x, int y);
void motion(int x, int y);
void check();
void reshape(int width, int height);
void menuFcn(GLint menuItemNumber);
void keyFcn(GLubyte key, GLint xMouse, GLint yMouse);



//INPUT/OUTPUT
void tryAgainSpaceBarMessage();
char getOperation();


//GEOMETRIC TRANSFORMATIONS
void translate(int tx, int ty, int polyIndex);
void rotate(double angle, int polyIndex);
void rotateAtOrigin(int polyIndex, double angle);
void scale(double scalingFactor, int polyIndex);
void scaleAtOrigin(double scalingFactor, int polyIndex);
void twoDimensionalClip(int xLL, int yLL, int x_ext, int y_ext);


//USER INTERFACING
void clipUserInterface();
void transformUserInterface();
void lineDrawUserInterface();


//GLOBALS
char* filename;
Polygon* polygons; //array of Polygon objects
int numPolygons;
int currentIndex; //of the polygons array
int lineDrawingAlgorithm; //1 for DDA, 0 for Bresenham
int** internalMatrix; //references are of the form [x][y]

float myPixelSize = 5;
int widthPixels = 100;
int heightPixels = 100;
int windowWidth = myPixelSize * widthPixels;
int windowHeight = myPixelSize * heightPixels;
int numCallBacks = 0;


/*
MAIN
*/

int main(int argc, char **argv) {
	filename = argv[1];
	readFile();
	initializeLocalPolygonMatrices();
	initializeClippingParameters();
	initializeInternalMatrix();

	lineDrawingAlgorithm = 1; //0 is Bresenham

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(windowWidth, windowHeight);
	glutCreateWindow("A Simple Two Dimensional Drawing System");

	glutCreateMenu(menuFcn);
	for (int i = 1; i <= numPolygons; i++) {
		char name[100];
		sprintf(name, "Polygon %d", i);
		glutAddMenuEntry(name, i);
	}
	glutAttachMenu(GLUT_RIGHT_BUTTON);
	glutDisplayFunc(display);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutIdleFunc(idle);
	glutReshapeFunc(reshape); //update GL on window size change
	glutKeyboardFunc(keyFcn);
	init();
	glutMainLoop();
	return 0;
}




void init() {
	glClearColor(1.0,1.0,1.0,1.0);
	check();
}

//this function should just draw polygons[currentIndex]
void display() {
	//clear internalMatrix
	clearInternal();

	//clears the screen
	glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);
    //clears the opengl Modelview transformation matrix
	glLoadIdentity();
	
	//For each polygon, draw it in its own frame buffer
	for (int i = 0; i < numPolygons; i++) {
		polygons[i].clearLocalDisplay();
		polygons[i].clearClippingDisplay();
		polygons[i].edgePoints.clear();
		polygons[i].initializeEdgePoints();
		//DEAL WITH OUT OF THE PICTURE VERTICES IN POLYGONS[i] 
		drawPolygon(&polygons[i], lineDrawingAlgorithm);
	}

	//Transfer each polygon's frame buffer to its clipping Buffer
	for (int i = 0; i < numPolygons; i++) {
		polygons[i].transferToClippingDisplay();
	}

	//Transfer each polygon's clipping display to the internalMatrix
	for (int i = 0; i < numPolygons; i++) {
		transferClippingDisplayToWorld(&polygons[i]);
	}

	//Transfer InternalMatrix to physical monitor
	transferInternalMatrixToMonitor();

    
    //blits the current opengl framebuffer on the screen
    glutSwapBuffers();
    check();
}

//gets called when a mouse button is pressed
void mouse(int button, int state, int x, int y) {
    glutPostRedisplay();
}

void draw_pix(int x, int y) {
	glBegin(GL_POINTS);
    glColor3f(1,0,0);
    glVertex3f(x+.5,y+.5,0);
    glEnd();
}

void idle() {
	glutPostRedisplay();
}

void motion(int x, int y)
{
    //redraw the scene after mouse movement
	glutPostRedisplay();
}

void reshape(int width, int height)
{
	/*set up projection matrix to define the view port*/
    //update the ne window width and height
	windowWidth = width;
	windowHeight = height;
    
    //creates a rendering area across the window
	glViewport(0,0,width,height);
    // up an orthogonal projection matrix so that
    // the pixel space is mapped to the grid space
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
	glOrtho(0,widthPixels,0,heightPixels,-10,10);
    
    //clear the modelview matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    //set pixel size based on width, if the aspect ratio
    //changes this hack won't work as well
    myPixelSize = width/(float)widthPixels;
    
    //set pixel size relative to the grid cell size
    glPointSize(myPixelSize);
    //check for opengl errors
	check();
}

void check()
{
	GLenum err = glGetError();
	if(err != GL_NO_ERROR)
	{
		printf("GLERROR: There was an error %s\n",gluErrorString(err) );
		exit(1);
	}
}

void menuFcn(GLint menuItemNumber) {
	currentIndex = menuItemNumber - 1;
	glutPostRedisplay();
}

void keyFcn(GLubyte key, GLint xMouse, GLint yMouse) {
	cout << endl << endl;
	char firstChoice;
	cout << "'p' to specify a Polygon to transform" << endl;
	cout << "'c' to clip" << endl;
	cout << "'a' to specify a different line drawing algorithm" << endl;
	cout << "'w' to write the scene to " << filename << endl;
	cout << "Your choice: ";
	cin >> firstChoice;
	if (firstChoice == 'p') {
		transformUserInterface();
	}
	else if (firstChoice == 'c') {
		clipUserInterface();
	}
	else if (firstChoice == 'a') {
		lineDrawUserInterface();
	}
	else if (firstChoice == 'w') {
		transferSceneToFile();
		cout << "Scene successfully updated!" << endl;
	}
	else {
		cout << "Error" << endl;
		return;
	}
}

void lineDrawUserInterface() {
	char algo;
	cout << endl << endl;
	cout << "'d' for DDA" << endl;
	cout << "'b' for Bresenham" << endl;
	cout << "Your choice: ";
	cin >> algo;
	if (algo == 'd') {
		lineDrawingAlgorithm = 1;
		cout << "Scene successfully updated!" << endl;
	}
	else if (algo == 'b'){
		lineDrawingAlgorithm = 0;
		cout << "Scene successfully updated!" << endl;
	}
	else {
		cout << "Error" << endl;
		return;
	}
}

void transformUserInterface() {
	int polygonID;
	cout << endl << endl;
	cout << "Polygon ID ([1, " << numPolygons << "]: " << endl;
	cout << "Your choice: ";
	if (!(cin >> polygonID)) {
		cout << "Error" << endl;
		return;
	}
	if (!(polygonID >= 1 && polygonID <= numPolygons)) {
		cout << "Error" << endl;
		return;
	}
	currentIndex = polygonID - 1;
	char op = getOperation();
	if (op != 'r' && op != 's' && op != 't') {
		cout << "Error" << endl;
		return;
	}
	switch(op) {
		case 'r':
			double angleDegrees;
			double angleRadians;
			cout << "Angle (in degrees): ";
			cin >> angleDegrees;
			angleRadians = toRadian(angleDegrees);
			rotate(angleRadians, currentIndex);
			cout << "Scene successfully updated!" << endl;
			break;
		case 's':
			double scalingFactor;
			cout << "Scaling Factor: ";
			cin >> scalingFactor;
			scale(scalingFactor, currentIndex);
			cout << "Scene successfully updated!" << endl;
			break;
		case 't':
			int tx;
			int ty;
			cout << "tx: ";
			cin >> tx;
			cout << "ty: ";
			cin >> ty;
			translate(tx, ty, currentIndex);
			cout << "Scene successfully updated!" << endl;
			break;
	}
}

void clipUserInterface() {
	int lowerLeft_X;
	int lowerLeft_Y;
	int x_extension;
	int y_extension;
	cout << "Lower Left Corner x: ";
	cin >> lowerLeft_X;
	cout << "Lower Left Corner y: ";
	cin >> lowerLeft_Y;
	cout << "x Extension: ";
	cin >> x_extension;
	cout << "y Extension: ";
	cin >> y_extension;
	twoDimensionalClip(lowerLeft_X, lowerLeft_Y, x_extension, y_extension);
}

void scale(double scalingFactor, int polyIndex) {
	int x_center;
	int y_center;
	findCenter(polyIndex, x_center, y_center);
	translate(-x_center, -y_center, polyIndex);
	scaleAtOrigin(scalingFactor, polyIndex);
	translate(x_center, y_center, polyIndex);
}

void scaleAtOrigin(double scalingFactor, int polyIndex) {
	for (int i = 0; i < polygons[polyIndex].numVertices; i++) {
		polygons[polyIndex].vertices[i][0] *= scalingFactor;
		polygons[polyIndex].vertices[i][1] *= scalingFactor;
	}
}

void twoDimensionalClip(int xLL, int yLL, int x_ext, int y_ext) {
	for (int i = 0; i < numPolygons; i++) {
		polygons[i].lowerLeftXOfClippingWindow = xLL;
		polygons[i].lowerLeftYOfClippingWindow = yLL;
		polygons[i].x_extensionOfClippingWindow = x_ext;
		polygons[i].y_extensionOfClippingWindow = y_ext;
	}
}

void translate(int tx, int ty, int polyIndex) {
	for (int i = 0; i < polygons[polyIndex].numVertices; i++) {
		polygons[polyIndex].vertices[i][0] += tx;
		polygons[polyIndex].vertices[i][1] += ty;
	}
}

void rotate(double angle, int polyIndex) {
	int x_center;
	int y_center;
	findCenter(polyIndex, x_center, y_center);
	translate(-x_center, -y_center, polyIndex);
	rotateAtOrigin(polyIndex, angle);
	translate(x_center, y_center, polyIndex);
}

void rotateAtOrigin(int polyIndex, double angle) {
	int x;
	int y;
	for (int i = 0; i < polygons[polyIndex].numVertices; i++) {
		x = polygons[polyIndex].vertices[i][0];
		y = polygons[polyIndex].vertices[i][1];
		polygons[polyIndex].vertices[i][0] = (int)round(((double)x * cos(angle)) - ((double)y * sin(angle)));
		polygons[polyIndex].vertices[i][1] = (int)round(((double)x * sin(angle)) + ((double)y * cos(angle)));
	}
}

void findCenter(int polyIndex, int& x_center, int& y_center) {
	int x_sum = 0;
	int y_sum = 0;
	for (int i = 0; i < polygons[polyIndex].numVertices; i++) {
		x_sum += polygons[polyIndex].vertices[i][0];
		y_sum += polygons[polyIndex].vertices[i][1];
	}
	x_center = (x_sum / polygons[polyIndex].numVertices);
	y_center = (y_sum / polygons[polyIndex].numVertices);
}

void readFile() {
	ifstream inFile(filename);
	inFile >> numPolygons;
	polygons = new Polygon[numPolygons];
	inFile.ignore(1000, '\n');

	int vertexCount;
	int** points;
	double x;
	double y;
	for (int i = 0; i < numPolygons; i++) {
		inFile.ignore(1000, '\n');
		inFile >> vertexCount;
		polygons[i] = Polygon(vertexCount);
		points = new int*[vertexCount];
		inFile.ignore(1000, '\n');
		for (int j = 0; j < vertexCount; j++) {
			points[j] = new int[2];
			inFile >> x;
			points[j][0] = round(x);
			inFile >> y;
			points[j][1] = round(y);
			inFile.ignore(1000, '\n');
		}
		polygons[i].initVertices(points);
	}
}

void transferSceneToFile() {
	//ifstream inFile(filename);
	ofstream outFile("temp.txt");
	outFile << numPolygons << '\t' << "number of polygons" << endl;
	for (int i = 0; i < numPolygons; i++) {
		string c;
		switch (i) {
			case 0:
				c = "st";
				break;
			case 1:
				c = "nd";
				break;
			case 2:
				c = "rd";
				break;
			default: 
				c = "th";
		}
		outFile << "\t" << "definition of " << i + 1 << c << " polygon" << endl;
		outFile << polygons[i].numVertices << "\t" << "number of points of " << i + 1 << c << " polygon" << endl;
		for (int j = 0; j < polygons[i].numVertices; j++) {
			string d;
			switch (j) {
				case 0:
					d = "st";
					break;
				case 1:
					d = "nd";
					break;
				case 2:
					d = "rd";
					break;
				default: 
					d = "th";
			}
			outFile << polygons[i].vertices[j][0] << " " << polygons[i].vertices[j][1] << "\t" << "coordinates of " << j+1 << d << " point" << endl;
		}
	}
	remove(filename);
	rename("temp.txt", filename);
}

void clearInternal() {
	for (int i = 0; i < widthPixels; i++) {
		for (int j = 0; j < heightPixels; j++) {
			internalMatrix[i][j] = 0;
		}
	}
}

void initializeInternalMatrix() {
	internalMatrix = new int*[widthPixels];
	for (int i = 0; i < widthPixels; i++) {
		internalMatrix[i] = new int[heightPixels];
	}
}

void initializeLocalPolygonMatrices() {
	for (int i = 0; i < numPolygons; i++) {
		polygons[i].initLocalDisplay(widthPixels, heightPixels);
		polygons[i].initClippingDisplay(widthPixels, heightPixels);
	}
}

void initializeClippingParameters() {
	for (int i = 0; i < numPolygons; i++) {
		polygons[i].initClippingParameters(widthPixels, heightPixels);
	}

}

//draws the polygon on the internalMatrix
// @param lineAlgo is 1 if the DDA algo is to be used, 0 if the Bresenham algo is to be used
//						
void drawPolygon(Polygon* p, int lineAlgo) {
	setLocalVertices(p);
	if (lineAlgo == 1) {
	 	drawPolygonLines_DDA(p);
	}
	else {
	 	drawPolygonLines_Bresenham(p);
	}
	drawVerticalViewingEdges(p);
	drawHorizontalViewingEdges(p);
	rasterize(p);
}

void drawVerticalViewingEdges(Polygon* p) {
	for (int i = 0; i < heightPixels; i++) {
		if (isInsidePolygon(0, i, p)) {
			p->localDisplay[0][i].value = 1;
		}
		if (isInsidePolygon(widthPixels - 1, i, p)) {
			p->localDisplay[widthPixels - 1][i].value = 1;
		}
	}
}

void drawHorizontalViewingEdges(Polygon* p) {
	for (int i = 0; i < widthPixels; i++) {
		if (isInsidePolygon(i, 0, p)) {
			p->localDisplay[i][0].value = 1;
		}
		if (isInsidePolygon(i, heightPixels - 1, p)) {
			p->localDisplay[i][heightPixels - 1].value = 1;
		}
	}
}

bool isInsidePolygon(int x, int y, Polygon* p) {
	int i;
	if (isOnEdge(x, y, p, i)) {
		return true;
	}
	else {
		if (isOdd(countIntersectionsRight(x, y, p))) {
			return true;
		}
		else {
			return false;
		}
	}
}


int countIntersectionsRight(int x, int y, Polygon* p) {
	int count = 0;
	int maxOrMin;
	int start;
	int edgeChanges;
	int startEdge[4];
	for (int i = x + 1; i <= p->maxXVertex(); i++) {
		int index;
		if(isOnEdge(i, y, p, index)) {
			maxOrMin = 0;
			start = i;
			edgeChanges = 0;
			startEdge[0] = p->edgePoints[index].edgeList[0]->xo;
			startEdge[1] = p->edgePoints[index].edgeList[0]->yo;
			startEdge[2] = p->edgePoints[index].edgeList[0]->xf;
			startEdge[3] = p->edgePoints[index].edgeList[0]->yf;
			while (i <= p->maxXVertex() && isOnEdge(i, y, p, index)) {
				if (p->isMinimum(i, y) || p->isMaximum(i, y) || p->isPracticalMaxOrMin(i, y, index)) {
					maxOrMin = 1;
				}
				else if (p->edgePoints[index].edgeList.size() > 1) {
					startEdge[0] = 0;
					startEdge[1] = 0;
					startEdge[2] = 0;
					startEdge[3] = 0;
				}
				else if (p->edgePoints[index].edgeList.size() == 1) {
					if (startEdge[0] == 0 && startEdge[1] == 0 && startEdge[2] == 0 && startEdge[3] == 0) {

					}
					else if (p->doesEdgeChange(startEdge[0], startEdge[1], startEdge[2], 
					startEdge[3], p->edgePoints[index].edgeList[0]->xo, p->edgePoints[index].edgeList[0]->yo, 
					p->edgePoints[index].edgeList[0]->xf, p->edgePoints[index].edgeList[0]->yf)) {
						edgeChanges++;
					}
					startEdge[0] = p->edgePoints[index].edgeList[0]->xo;
					startEdge[1] = p->edgePoints[index].edgeList[0]->yo;
					startEdge[2] = p->edgePoints[index].edgeList[0]->xf;
					startEdge[3] = p->edgePoints[index].edgeList[0]->yf;
				}
				i++;
			}

			if (p->isEdge(start, y, i - 1, y)) {
				if (!((p->isMinimum(start, y) && p->isMinimum(i - 1, y)) || (p->isMaximum(start, y) && p->isMaximum(i - 1, y)))) {
					count++;
				}
				continue;
			}

			if (maxOrMin != 1 && !p->isEdge(start, y, i - 1, y) && (edgeChanges % 2 == 0)) {
				count++;
			}
		}
	}
	return count;
}

bool isOnEdge(int x, int y, Polygon* p, int &index) {
	for (int i = 0; i < p->edgePoints.size(); i++) {
		if (p->edgePoints[i].x == x && p->edgePoints[i].y == y) {
			index = i;
			return true;
		}
	}
	index = -1;
	return false;
}

void setLocalVertices(Polygon* p) {
	int xFirst;
	int yFirst;
	int xLast;
	int yLast;
	for (int i = 0; i < p->numVertices; i++) {
		if (p->vertices[i][0] >= widthPixels || p->vertices[i][0] < 0 || p->vertices[i][1]
			>= heightPixels || p->vertices[i][1] < 0) {
			continue;
		}


		p->localDisplay[p->vertices[i][0]][p->vertices[i][1]].value = 1;
		if (i == 0) {
			xFirst = p->vertices[p->numVertices - 1][0];
			yFirst = p->vertices[p->numVertices - 1][1];
			xLast = p->vertices[0][0];
			yLast = p->vertices[0][1];
		}
		else if (i == p->numVertices - 1) {
			xFirst = p->vertices[i-1][0];
			yFirst = p->vertices[i-1][1];
			xLast = p->vertices[0][0];
			yLast = p->vertices[0][1];
		}
		else {
			xFirst = p->vertices[i-1][0];
			yFirst = p->vertices[i-1][1];
			xLast = p->vertices[i+1][0];
			yLast = p->vertices[i+1][1];
		}
		p->localDisplay[p->vertices[i][0]][p->vertices[i][1]].edgeList.push_back(new Edge(xFirst, yFirst, xLast, yLast));
	}
}


void drawPolygonLines_DDA(Polygon* p) {
	for (int i = 0; i < p->numVertices - 1; i++) {
		drawDDALine(p->vertices[i][0], p->vertices[i][1], p->vertices[i + 1][0], p->vertices[i + 1][1], p);
	}
	drawDDALine(p->vertices[p->numVertices - 1][0], p->vertices[p->numVertices - 1][1], p->vertices[0][0], p->vertices[0][1], p);
}

void drawDDALine(int xStart, int yStart, int xEnd, int yEnd, Polygon* p) {
	int dx = xEnd - xStart;
	int dy = yEnd - yStart;
	int steps;
	double xIncrement, yIncrement;
	double x = xStart;
	double y = yStart;

	if (fabs(dx) > fabs(dy)) {
		steps = fabs(dx);
	}
	else {
		steps = fabs(dy);
	}

	xIncrement = double(dx) / double(steps);
	yIncrement = double(dy) / double(steps);
	for (int k = 0; k < steps; k++) {
		x += xIncrement;
		y += yIncrement;

		int index = p->findPoint((int)round(x), (int)round(y));
		if (index >= 0) {
			p->edgePoints[index].edgeList.push_back(new Edge(xStart, yStart, xEnd, yEnd));
		}
		else { //first point at this location being drawn
			p->pushBackPt((int)round(x), (int)round(y), xStart, yStart, xEnd, yEnd);
		}

		if ((int)round(x) >= widthPixels || (int)round(x) < 0 || (int)round(y)
			>= heightPixels || (int)round(y) < 0) {
			continue;
		}

		else {
			if (((int)round(x) == xStart && (int)round(y) == yStart) || ((int)round(x) == xEnd && (int)round(y) == yEnd)) {
				continue;
			}
			p->localDisplay[(int)round(x)][(int)round(y)].value = 1;
			p->localDisplay[(int)round(x)][(int)round(y)].edgeList.push_back(new Edge(xStart, yStart, xEnd, yEnd));
		}
	}
}

void drawPolygonLines_Bresenham(Polygon* p) {
	for (int i = 0; i < p->numVertices - 1; i++) {
		drawBresenhamLine(p->vertices[i][0], p->vertices[i][1], p->vertices[i + 1][0], p->vertices[i + 1][1], p);
	}
	drawBresenhamLine(p->vertices[p->numVertices - 1][0], p->vertices[p->numVertices - 1][1], p->vertices[0][0], p->vertices[0][1], p);
}

void drawBresenhamLine(int x1, int y1, int x2, int y2, Polygon* p) {
	double slope = getSlope(x1, y1, x2, y2);
	if (slope == 9999) {
		drawVerticalLine(x1, y1, y2, p);
	}
	else if (slope >= 0 && slope <= 1) {
		drawBresenhamLineSmallPositive(x1, y1, x2, y2, p);
	}
	else if (slope >= -1 && slope < 0) {
		drawBresenhamLineSmallNegative(x1, y1, x2, y2, p);
	}
	else if (slope > 1 && slope < 9999) {
		drawBresenhamLineLargePositive(x1, y1, x2, y2, p);
	}
	else {
		drawBresenhamLineLargeNegative(x1, y1, x2, y2, p);
	}
}

void drawVerticalLine(int x1, int y1, int y2, Polygon* pol) {
	int y0 = min(y1, y2);
	int yf = max(y1, y2);
	for (int i = y0 + 1; i < yf; i++) {

		int index = pol->findPoint(x1, i);
		if (index >= 0) {
			pol->edgePoints[index].edgeList.push_back(new Edge(x1, y1, x1, y2));
		}
		else { //first point at this location being drawn
			pol->pushBackPt(x1, i, x1, y1, x1, y2);
		}

		if (x1 >= widthPixels || x1 < 0 || i >= heightPixels || i < 0) {
			continue;
		}
		else {
			pol->localDisplay[x1][i].value = 1;
			pol->localDisplay[x1][i].edgeList.push_back(new Edge(x1, y1, x1, y2));
		}
	}
}

void drawBresenhamLineSmallPositive(int x1, int y1, int x2, int y2, Polygon* pol) {
	int x0 = min(x1, x2);
	int y0 = min(y1, y2);
	int delx = max(x1, x2) - x0;
	int dely = max(y1, y2) - y0;
	int two_dy = 2 * dely;
	int two_dy_minus_dx = 2*(dely - delx);
	int p = (2 * dely) - delx;

	for (int i = x0 + 1; i < x0 + delx; i++) {
		if (p < 0) {
			p += two_dy;
		}
		else {
			y0++;
			p += two_dy_minus_dx;
		}

		int index = pol->findPoint(i, y0);
		if (index >= 0) {
			pol->edgePoints[index].edgeList.push_back(new Edge(x1, y1, x2, y2));
		}
		else { //first point at this location being drawn
			pol->pushBackPt(i, y0, x1, y1, x2, y2);
		}
		

		if (i >= widthPixels || i < 0 || y0 >= heightPixels || y0 < 0) {
			continue;
		}
		else {
			pol->localDisplay[i][y0].value = 1;
			pol->localDisplay[i][y0].edgeList.push_back(new Edge(x1, y1, x2, y2));
		}
	}
}


void drawBresenhamLineSmallNegative(int x1, int y1, int x2, int y2, Polygon* pol) {
	int x0 = max(x1, x2);
	int xf = min(x1, x2);
	int y0 = min(y1, y2);
	int yf = max(y1, y2);
	int delx = fabs(x1 - x2);
	int dely = fabs(y1 - y2);
	int two_dy = 2 * dely;
	int two_dy_minus_dx = 2*(dely - delx);
	int p = (2 * dely) - delx;

	for (int i = x0 - 1; i > xf; i--) {
		if (p < 0) {
			p += two_dy;
		}
		else {
			y0++;
			p += two_dy_minus_dx;
		}

		int index = pol->findPoint(i, y0);
		if (index >= 0) {
			pol->edgePoints[index].edgeList.push_back(new Edge(x1, y1, x2, y2));
		}
		else { //first point at this location being drawn
			pol->pushBackPt(i, y0, x1, y1, x2, y2);
		}

		if (i >= widthPixels || i < 0 || y0 >= heightPixels || y0 < 0) {
			continue;
		}
		else {
			pol->localDisplay[i][y0].value = 1;
			pol->localDisplay[i][y0].edgeList.push_back(new Edge(x1, y1, x2, y2));
		}
	}
}

void drawBresenhamLineLargePositive(int x1, int y1, int x2, int y2, Polygon* pol) {
	int y0 = min(y1, y2);
	int x0 = min(x1, x2);
	int delx = max(x1, x2) - x0;
	int dely = max(y1, y2) - y0;
	int two_dx = 2 * delx;
	int two_dx_minus_dy = 2*(delx - dely);
	int p = (2 * delx) - dely;

	for (int i = y0 + 1; i < y0 + dely; i++) {
		if (p < 0) {
			p += two_dx;
		}
		else {
			x0++;
			p += two_dx_minus_dy;
		}

		int index = pol->findPoint(x0, i);
		if (index >= 0) {
			pol->edgePoints[index].edgeList.push_back(new Edge(x1, y1, x2, y2));
		}
		else { //first point at this location being drawn
			pol->pushBackPt(x0, i, x1, y1, x2, y2);
		}

		if (x0 >= widthPixels || x0 < 0 || i >= heightPixels || i < 0) {
			continue;
		}
		else {
			pol->localDisplay[x0][i].value = 1;
			pol->localDisplay[x0][i].edgeList.push_back(new Edge(x1, y1, x2, y2));
		}
	}
}


void drawBresenhamLineLargeNegative(int x1, int y1, int x2, int y2, Polygon* pol) {
	int x0 = max(x1, x2);
	int xf = min(x1, x2);
	int y0 = min(y1, y2);
	int yf = max(y1, y2);
	int delx = fabs(x1 - x2);
	int dely = fabs(y1 - y2);
	int two_dx = 2 * delx;
	int two_dx_minus_dy = 2*(delx - dely);
	int p = (2 * delx) - dely;

	for (int i = y0 + 1; i < yf; i++) {
		if (p < 0) {
			p += two_dx;
		}
		else {
			x0--;
			p += two_dx_minus_dy;
		}

		int index = pol->findPoint(x0, i);
		if (index >= 0) {
			pol->edgePoints[index].edgeList.push_back(new Edge(x1, y1, x2, y2));
		}
		else { //first point at this location being drawn
			pol->pushBackPt(x0, i, x1, y1, x2, y2);
		}

		if (x0 >= widthPixels || x0 < 0 || i >= heightPixels || i < 0) {
			continue;
		}
		else {
			pol->localDisplay[x0][i].value = 1;
			pol->localDisplay[x0][i].edgeList.push_back(new Edge(x1, y1, x2, y2));
		}
	}
}


void rasterize(Polygon* p) {
	for (int y = 0; y < heightPixels; y++) {
		rasterScanLine(p, y);
	}
}



void rasterScanLine(Polygon* p, int row) {
	vector<int> intersections = p->getIntersectionsToTheRight(0, row);
	if (isOdd(intersections.size())) {
		return;
	}
	for (int i = 0; i < intersections.size(); i+=2) {
		for (int j = intersections[i]; j <= intersections[i+1]; j++) {
			p->localDisplay[j][row].value = 1;
		}
	}
}
	
void transferClippingDisplayToWorld(Polygon* p) {
	for (int i = 0; i < widthPixels; i++) {
		for (int j = 0; j < heightPixels; j++) {
			if (p->clippingDisplay[i][j] == 1) {
				internalMatrix[i][j] = 1;
			}
		}
	}
}


void transferInternalMatrixToMonitor() {
	for (int i = 0; i < widthPixels; i++) {
		for (int j = 0; j < heightPixels; j++) {
			if (internalMatrix[i][j] == 1) {
				draw_pix(i, j);
			}
		}
	}
}

double getSlope(double x1, double y1, double x2, double y2) {
	if (x1 == x2) {
		return 9999;
	}
	else {
		return (y2 - y1) / (x2 - x1);
	}
}

double getYIntercept(int xStart, int yStart, double slope) {
	return yStart - (double(xStart) * slope);
}

int min(int x, int y) {
	if (x < y) {
		return x;
	}
	else {
		return y;
	}
}

int max(int x, int y) {
	if (x > y) {
		return x;
	}
	else {
		return y;
	}
}

bool isOdd(int x) {
	if (x % 2 != 0) {
		return true;
	}
	return false;
}

double toRadian(double degree) {
	return (degree * M_PI) / 180;
}

void tryAgainSpaceBarMessage() {
	cout << "Invalid entry. Try again by pressing the space bar" << endl;
}

char getOperation() {
	char op;
	cout << endl << endl;
	cout << "Press 'r' to rotate" << endl;
	cout << "Press 's' to scale" << endl;
	cout << "Press 't' to translate" << endl;
	cout << "Your choice: ";
	cin >> op;
	return op;
}

