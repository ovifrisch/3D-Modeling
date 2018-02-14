//GLUT PACKAGES
#if defined (__APPLE__) || defined(MACOSX)
#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#endif


//INCLUDES
#include "Object.h"
#include "Triangle.h"
#include "ObjectVertex.h"
#include "Material.h"
#include "FloatTriple.h"

#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
using namespace std;


float* buffer;
int win_width;
int win_height;
int mapType; //1->1 or 1->3

//LIGHT/MATERIAL CONSTANTS
vector<Material*> materials;
struct lightConstants {
	FloatTriple* lightSource;
	float ambientIntensity;
	float lightIntensity;
	float F; //distance of from point from the center of the screen
	float K; //average distance of scene from light source
} light;

struct point2D {
	int x;
	int y;
};

//OBJECTS
int numObjects;
vector<Object*> objects;

//TRIANGLES
vector<Triangle*> sortedTriangles_X;
vector<Triangle*> sortedTriangles_Y;
vector<Triangle*> sortedTriangles_Z;


//FILENAMES
char* modelFilename;



void init();
void display();
void idle();
void mouse(int button, int state, int x, int y);
void motion(int x, int y);
void check();
void reshape(int width, int height);
void key(unsigned char ch, int x, int y);
void drawScene(int mapType);
void drawSceneXY(int mapType);
void drawSceneYZ(int mapType);
void drawSceneXZ(int mapType);
void readModelFile();
void readLightFile();
void readMaterialFile();

void drawPixWithIntensity(int x, int y, float intensity);
void transferBufferToScreen();
void sortTriangles();
void mergeSort(vector<Triangle*>* a, int projection);
void mergeSort(vector<Triangle*>* a, vector<Triangle*>& tmpArray, int left, int right, int projection);
void merge(vector<Triangle*>* a, vector<Triangle*> tmpArray, int leftPos, int rightPos, int rightEnd, int projection);
void initializeTriangleArrays();
void setVertexNormals();
void setVertexIntensities();
void setPhongIntensity();
FloatTriple getFromPoint(int projection);
FloatTriple getNormalizedLightVector(ObjectVertex vertex);
FloatTriple getNormalizedReflectionVector(FloatTriple light, FloatTriple normal);
FloatTriple getViewingVector(FloatTriple fromPoint, ObjectVertex vertex);
FloatTriple normalize(FloatTriple vector);
float dotProduct(FloatTriple v1, FloatTriple v2);
void normalizePhongIntensity(FloatTriple maxIntensity);
FloatTriple maxIntensity();
void clampNegativeIntensities();
void loadVerticesXY(Triangle* triangle, int mapType);
void loadVerticesXZ(Triangle* triangle, int mapType);
void loadVerticesYZ(Triangle* triangle, int mapType);
void clearBuffer(int mapType);
void setPointsInBetween(float x1, float y1, float x2, float y2, float intensity1, float intensity2, int mapType);
vector<point2D> getPointsInBetween(int x1, int y1, int x2, int y2);
float getPointIntensity_y(int py, int yo, int yf, float intensity1, float intensity2);
float normalizePointIntensity(float intensity);
void draw_blue_line(float x1, float y1, float x2, float y2);
void transferBufferToMegaScreen();
void drawMegaPix1(int upLeftX, int upLeftY);
void drawMegaPix2(int upLeftX, int upLeftY);
void drawMegaPix3(int upLeftX, int upLeftY);
void drawMegaPix4(int upLeftX, int upLeftY);
void drawMegaPix5(int upLeftX, int upLeftY);
void drawMegaPix6(int upLeftX, int upLeftY);
void drawMegaPix7(int upLeftX, int upLeftY);
void drawMegaPix8(int upLeftX, int upLeftY);
void drawMegaPix9(int upLeftX, int upLeftY);
void drawPixBinary(int x, int y);
void handleInput();
void setKa();
void setKd();
void setKs();
void setN();
void setIa();
void setIl();
void setK();
void setS();

int main(int argc, char** argv) {
	mapType = 1; //map 1 to 1
	win_width = 600;
	win_height = 600;
	modelFilename = argv[1];
	readModelFile();
	readLightFile();
	readMaterialFile();
	sortTriangles();
	setVertexNormals();


	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(win_width, win_height);
	glutCreateWindow("Project 3");

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutKeyboardFunc(key);
	glutIdleFunc(idle);
	init();
	glutMainLoop();
	return 0;
}

void display() {
	glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);
	glLoadIdentity();
	setVertexIntensities();
	drawScene(mapType);

	glViewport(0, 0, win_width, win_height);
	draw_blue_line(win_width / 2, 0, win_width/2, win_width);
	draw_blue_line(0, win_height/2, win_height, win_height / 2);
	glutSwapBuffers();
	//check();
}

void drawScene(int mapType) {
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 2; j++) {
			glViewport(i * (win_width/2), j * (win_height/2), win_width/2, win_height/2);
			if (i == 0 && j == 1) drawSceneXZ(mapType);
			else if (i == 0 && j == 0) drawSceneYZ(mapType);
			else if (i == 1 && j == 1) drawSceneXY(mapType);
		}
	}
}

void drawSceneXY(int mapType) {
	if (mapType == 3) { //map 1->3
		buffer = new float[(win_width / 3) * (win_height / 3)];
		memset(buffer,0,sizeof(float) * (win_width * win_height)/3);
	}
	else {
		buffer = new float[win_width * win_height];
		memset(buffer,0,sizeof(float) * win_width * win_height);
	}
	for (int i = sortedTriangles_Z.size() - 1; i >= 0; i--) {
		loadVerticesXY(sortedTriangles_Z[i], mapType);
		setPointsInBetween(sortedTriangles_Z[i]->v1->x, sortedTriangles_Z[i]->v1->y, sortedTriangles_Z[i]->v2->x, sortedTriangles_Z[i]->v2->y, 
			sortedTriangles_Z[i]->v1->intensityXY, sortedTriangles_Z[i]->v2->intensityXY, mapType);

		setPointsInBetween(sortedTriangles_Z[i]->v2->x, sortedTriangles_Z[i]->v2->y, sortedTriangles_Z[i]->v3->x, sortedTriangles_Z[i]->v3->y, 
			sortedTriangles_Z[i]->v2->intensityXY, sortedTriangles_Z[i]->v3->intensityXY, mapType);

		setPointsInBetween(sortedTriangles_Z[i]->v3->x, sortedTriangles_Z[i]->v3->y, sortedTriangles_Z[i]->v1->x, sortedTriangles_Z[i]->v1->y, 
			sortedTriangles_Z[i]->v3->intensityXY, sortedTriangles_Z[i]->v1->intensityXY, mapType);
	}

	if (mapType == 1) {
		transferBufferToScreen();
	}
	else {
		transferBufferToMegaScreen();
	}
	delete [] buffer;
}

void drawSceneYZ(int mapType) {
	if (mapType == 3) { //map 1->3
		buffer = new float[(win_width / 3) * (win_height / 3)];
		memset(buffer,0,sizeof(float) * (win_width * win_height)/3);
	}
	else {
		buffer = new float[win_width * win_height];
		memset(buffer,0,sizeof(float) * win_width * win_height);
	}
	for (int i = sortedTriangles_X.size() - 1; i >= 0; i--) {
		loadVerticesYZ(sortedTriangles_X[i], mapType);
		setPointsInBetween(sortedTriangles_X[i]->v1->y, sortedTriangles_X[i]->v1->z, sortedTriangles_X[i]->v2->y, sortedTriangles_X[i]->v2->z, 
			sortedTriangles_X[i]->v1->intensityYZ, sortedTriangles_X[i]->v2->intensityYZ, mapType);

		setPointsInBetween(sortedTriangles_X[i]->v2->y, sortedTriangles_X[i]->v2->z, sortedTriangles_X[i]->v3->y, sortedTriangles_X[i]->v3->z, 
			sortedTriangles_X[i]->v2->intensityYZ, sortedTriangles_X[i]->v3->intensityYZ, mapType);

		setPointsInBetween(sortedTriangles_X[i]->v3->y, sortedTriangles_X[i]->v3->z, sortedTriangles_X[i]->v1->y, sortedTriangles_X[i]->v1->z, 
			sortedTriangles_X[i]->v3->intensityYZ, sortedTriangles_X[i]->v1->intensityYZ, mapType);
	}
	if (mapType == 1) {
		transferBufferToScreen();
	}
	else {
		transferBufferToMegaScreen();
	}
	delete [] buffer;
}

void drawSceneXZ(int mapType) {
	if (mapType == 3) { //map 1->3
		buffer = new float[(win_width / 3) * (win_height / 3)];
		memset(buffer,0,sizeof(float) * (win_width * win_height)/3);
	}
	else {
		buffer = new float[win_width * win_height];
		memset(buffer,0,sizeof(float) * win_width * win_height);
	}
	for (int i = sortedTriangles_Y.size() - 1; i >= 0; i--) {
		loadVerticesXZ(sortedTriangles_Y[i], mapType);
		setPointsInBetween(sortedTriangles_Y[i]->v1->x, sortedTriangles_Y[i]->v1->z, sortedTriangles_Y[i]->v2->x, sortedTriangles_Y[i]->v2->z, 
			sortedTriangles_Y[i]->v1->intensityXZ, sortedTriangles_Y[i]->v2->intensityXZ, mapType);

		setPointsInBetween(sortedTriangles_Y[i]->v2->x, sortedTriangles_Y[i]->v2->z, sortedTriangles_Y[i]->v3->x, sortedTriangles_Y[i]->v3->z, 
			sortedTriangles_Y[i]->v2->intensityXZ, sortedTriangles_Y[i]->v3->intensityXZ, mapType);

		setPointsInBetween(sortedTriangles_Y[i]->v3->x, sortedTriangles_Y[i]->v3->z, sortedTriangles_Y[i]->v1->x, sortedTriangles_Y[i]->v1->z, 
			sortedTriangles_Y[i]->v3->intensityXZ, sortedTriangles_Y[i]->v1->intensityXZ, mapType);
	}
	if (mapType == 1) {
		transferBufferToScreen();
	}
	else {
		transferBufferToMegaScreen();
	}
	delete [] buffer;
}

void setPointsInBetween(float x1, float y1, float x2, float y2, float intensity1, float intensity2, int mapType) {
	int xo = (int)(x1 * win_width / mapType);
	int yo = (int)(y1 * win_height / mapType);
	int xf = (int)(x2 * win_width / mapType);
	int yf = (int)(y2 * win_height / mapType);
	vector<point2D> pointsInBetween = getPointsInBetween(xo, yo, xf, yf);

	for (int i = 0; i < pointsInBetween.size(); i++) {
		float pointIntensity = getPointIntensity_y(pointsInBetween[i].y, yo, yf, intensity1, intensity2);
		buffer[(win_width * pointsInBetween[i].x) + pointsInBetween[i].y] = pointIntensity;
	}
}

vector<point2D> getPointsInBetween(int x1, int y1, int x2, int y2) {
	vector<point2D> linePoints;
	int dx = x2 - x1;
	int dy = y2 - y1;
	int steps;
	float xIncrement, yIncrement;
	float x = x1;
	float y = y1;

	if (fabs(dx) > fabs(dy)) {
		steps = fabs(dx);
	}
	else {
		steps = fabs(dy);
	}

	xIncrement = float(dx) / float(steps);
	yIncrement = float(dy) / float(steps);
	for (int k = 0; k < steps - 1; k++) {
		x += xIncrement;
		y += yIncrement;
		point2D point;
		point.x = (int)round(x);
		point.y = (int)round(y);
		linePoints.push_back(point);
	}
	return linePoints;
}

float getPointIntensity_y(int py, int yo, int yf, float intensity1, float intensity2) {
	if (yo - yf == 0) {
		return 1.0;
	}
	return normalizePointIntensity((((float)(py - yf) / (float)(yo - yf))*intensity1) + (((float)(yo - py) / (float)(yo - yf))*intensity2));
}

float normalizePointIntensity(float intensity) {
	if (intensity > 1) {
		return 1.0;
	}
	else if (intensity < 0) {
		return 0.0;
	}
	else {
		return intensity;
	}
}

void loadVerticesXY(Triangle* triangle, int mapType) {
	int v1x = (int)(triangle->v1->x * win_width / mapType);
	int v1y = (int)(triangle->v1->y * win_height / mapType);
	int v2x = (int)(triangle->v2->x * win_width / mapType);
	int v2y = (int)(triangle->v2->y * win_height / mapType);
	int v3x = (int)(triangle->v3->x * win_width / mapType);
	int v3y = (int)(triangle->v3->y * win_height / mapType);
	buffer[(v1x * (win_width / mapType)) + v1y] = triangle->v1->intensityXY;
	buffer[(v2x * (win_width / mapType)) + v2y] = triangle->v2->intensityXY;
	buffer[(v3x * (win_width / mapType)) + v3y] = triangle->v3->intensityXY;
}

void loadVerticesXZ(Triangle* triangle, int mapType) {
	int v1x = (int)(triangle->v1->x * win_width / mapType);
	int v1y = (int)(triangle->v1->z * win_height / mapType);
	int v2x = (int)(triangle->v2->x * win_width / mapType);
	int v2y = (int)(triangle->v2->z * win_height / mapType);
	int v3x = (int)(triangle->v3->x * win_width / mapType);
	int v3y = (int)(triangle->v3->z * win_height / mapType);
	buffer[(v1x * (win_width / mapType)) + v1y] = triangle->v1->intensityXZ;
	buffer[(v2x * (win_width / mapType)) + v2y] = triangle->v2->intensityXZ;
	buffer[(v3x * (win_width / mapType)) + v3y] = triangle->v3->intensityXZ;
}

void loadVerticesYZ(Triangle* triangle, int mapType) {
	int v1x = (int)(triangle->v1->y * win_width / mapType);
	int v1y = (int)(triangle->v1->z * win_height / mapType);
	int v2x = (int)(triangle->v2->y * win_width / mapType);
	int v2y = (int)(triangle->v2->z * win_height / mapType);
	int v3x = (int)(triangle->v3->y * win_width / mapType);
	int v3y = (int)(triangle->v3->z * win_height / mapType);
	buffer[(v1x * (win_width / mapType)) + v1y] = triangle->v1->intensityYZ;
	buffer[(v2x * (win_width / mapType)) + v2y] = triangle->v2->intensityYZ;
	buffer[(v3x * (win_width / mapType)) + v3y] = triangle->v3->intensityYZ;
}

void init() {
	glClearColor(0.0, 0.0, 0.0, 0.0);
	check();
}
void reshape(int width, int height) {
	win_width = width;
	win_height = height;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, win_width, 0, win_height, -10, 10);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void idle() {
    //redraw the scene over and over again
	glutPostRedisplay();	
}
void mouse(int button, int state, int x, int y) {
    //redraw the scene after mouse click
    glutPostRedisplay();
}
void motion(int x, int y) {
    //redraw the scene after mouse movement
	glutPostRedisplay();
}
void check() {
	GLenum err = glGetError();
	if (err != GL_NO_ERROR) {
		printf("GLERROR: There was an error %s\n", gluErrorString(err));
		exit(1);
	}
}
void key(unsigned char ch, int x, int y) {
	handleInput();
	glutPostRedisplay();
}

void drawPixWithIntensity(int x, int y, float intensity) {
	glBegin(GL_POINTS);
    glColor3f(intensity,intensity,intensity);
    glVertex3f(x,y,0);
    glEnd();
}

void transferBufferToScreen() {
	for (int i = 0; i < win_width; i++) {
		for (int j = 0; j < win_height; j++) {
			drawPixWithIntensity(i, j, buffer[i * win_width + j]);
		}
	}
}

void transferBufferToMegaScreen() {
	for (int i = 0; i < win_width / 3; i++) {
		for (int j = 0; j < win_height / 3; j++) {
			float in = buffer[(i * win_width/3) + j];
			if (in >= 0 && in < 0.1) {
				continue;
			}
			if (in >= 0.1 && in < 0.2) {
				drawMegaPix1(i * 3, j * 3);
			}
			if (in >= 0.2 && in < 0.3) {
				drawMegaPix2(i * 3, j * 3);
			}
			if (in >= 0.3 && in < 0.4) {
				drawMegaPix3(i * 3, j * 3);
			}
			if (in >= 0.4 && in < 0.5) {
				drawMegaPix4(i * 3, j * 3);
			}
			if (in >= 0.5 && in < 0.6) {
				drawMegaPix5(i * 3, j * 3);
			}
			if (in >= 0.6 && in < 0.7) {
				drawMegaPix6(i * 3, j * 3);
			}
			if (in >= 0.7 && in < 0.8) {
				drawMegaPix7(i * 3, j * 3);
			}
			if (in >= 0.8 && in < 0.9) {
				drawMegaPix8(i * 3, j * 3);
			}
			if (in >= 0.9 && in <= 1.0) {
				drawMegaPix9(i * 3, j * 3);
			}
		}
	}
}

void drawMegaPix1(int upLeftX, int upLeftY) {
	drawPixBinary(upLeftX + 1, upLeftY + 1);
}
void drawMegaPix2(int upLeftX, int upLeftY) {
	drawMegaPix1(upLeftX, upLeftY);
	drawPixBinary(upLeftX + 2, upLeftY + 1);
}
void drawMegaPix3(int upLeftX, int upLeftY) {
	drawMegaPix2(upLeftX, upLeftY);
	drawPixBinary(upLeftX + 1, upLeftY);
}
void drawMegaPix4(int upLeftX, int upLeftY) {
	drawMegaPix3(upLeftX, upLeftY);
	drawPixBinary(upLeftX, upLeftY + 2);
}
void drawMegaPix5(int upLeftX, int upLeftY) {
	drawMegaPix4(upLeftX, upLeftY);
	drawPixBinary(upLeftX, upLeftY + 1);
}
void drawMegaPix6(int upLeftX, int upLeftY) {
	drawMegaPix5(upLeftX, upLeftY);
	drawPixBinary(upLeftX + 2, upLeftY + 2);
}
void drawMegaPix7(int upLeftX, int upLeftY) {
	drawMegaPix6(upLeftX, upLeftY);
	drawPixBinary(upLeftX + 2, upLeftY);
}
void drawMegaPix8(int upLeftX, int upLeftY) {
	drawMegaPix7(upLeftX, upLeftY);
	drawPixBinary(upLeftX, upLeftY);
}
void drawMegaPix9(int upLeftX, int upLeftY) {
	drawMegaPix8(upLeftX, upLeftY);
	drawPixBinary(upLeftX + 1, upLeftY + 2);
}

void readModelFile() {
	ifstream inFile(modelFilename);
	inFile >> numObjects;
	for (int i = 0; i < numObjects; i++) {
		Object* o = new Object();
		int numVertices;
		int numTriangles;
		inFile >> o->materialID;
		inFile >> numVertices;
		for (int i = 0; i < numVertices; i++) {
			float x;
			float y;
			float z;
			inFile >> x >> y >> z;
			o->addVertex(x, y, z);
		}
		inFile >> numTriangles;
		for (int j = 0; j < numTriangles; j++) {
			int i1, i2, i3; //the indices of two points
			inFile >> i1 >> i2 >> i3;

			//link vertex to triangle
			if (o->vertices[i1 - 1]->triangleIndex == -1) {
				o->vertices[i1 - 1]->triangleIndex = o->triangles.size();
			}
			if (o->vertices[i2 - 1]->triangleIndex == -1) {
				o->vertices[i2 - 1]->triangleIndex = o->triangles.size();
			}
			if (o->vertices[i3 - 1]->triangleIndex == -1) {
				o->vertices[i3 - 1]->triangleIndex = o->triangles.size();
			}

			Triangle* t = new Triangle(o->vertices[i1 - 1], o->vertices[i2 - 1], o->vertices[i3 - 1]);
			o->addTriangle(t);
		}
		objects.push_back(o);
	}
}
void readLightFile() {
	ifstream inFile("light.txt");
	light.lightSource = new FloatTriple();
	inFile >> light.lightSource->x;
	inFile >> light.lightSource->y;
	inFile >> light.lightSource->z;

	inFile >> light.ambientIntensity;
	inFile.ignore(100, '\n');

	inFile >> light.lightIntensity;
	inFile.ignore(100, '\n');

	inFile >> light.F;
	inFile >> light.K;
}
void readMaterialFile() {
	ifstream inFile("materials.txt");
	int numMaterials;
	inFile >> numMaterials;
	for (int i = 0; i < numMaterials; i++) {
		Material* mat = new Material();
		inFile >> mat->ka;
		inFile.ignore(100, '\n');
		inFile >> mat->kd;
		inFile.ignore(100, '\n');
		inFile >> mat->ks;
		inFile.ignore(100, '\n');
		inFile >> mat->n;
		materials.push_back(mat);
	}
}
void sortTriangles() {
	initializeTriangleArrays();
	mergeSort(&sortedTriangles_X, 2);
	mergeSort(&sortedTriangles_Y, 1);
	mergeSort(&sortedTriangles_Z, 0);
}
void initializeTriangleArrays() {
	for (int i = 0; i < objects.size(); i++) {
		for (int j = 0; j < objects[i]->triangles.size(); j++) {
			sortedTriangles_X.push_back(objects[i]->triangles[j]);
			sortedTriangles_Y.push_back(objects[i]->triangles[j]);
			sortedTriangles_Z.push_back(objects[i]->triangles[j]);
		}
	}
}
//the projection param indicates the type of ortho projection which affects the < operator for a Triangle
void mergeSort(vector<Triangle*>* a, int projection) {
	vector<Triangle*> tmpArray(a->size());
	mergeSort(a, tmpArray, 0, a->size() - 1, projection);
}
void mergeSort(vector<Triangle*>* a, vector<Triangle*>& tmpArray, int left, int right, int projection) {
	if (left < right) {
		int center = (left + right) / 2;
		mergeSort(a, tmpArray, left, center, projection);
		mergeSort(a, tmpArray, center + 1, right, projection);
		merge(a, tmpArray, left, center + 1, right, projection);
	}
}
void merge(vector<Triangle*>* a, vector<Triangle*> tmpArray, int leftPos, int rightPos, int rightEnd, int projection) {
	int leftEnd = rightPos - 1;
	int tmpPos = leftPos;
	int numElements = rightEnd - leftPos + 1;

	while (leftPos <= leftEnd && rightPos <= rightEnd) {

		if ((*((*a)[leftPos])).lessThan(*((*a)[rightPos]), projection)) {
			tmpArray[tmpPos++] = std::move((*a)[leftPos++]);
		}
		else {
			tmpArray[tmpPos++] = std::move((*a)[rightPos++]);
		}
	}

	while (leftPos <= leftEnd) {
		tmpArray[tmpPos++] = std::move((*a)[leftPos++]);
	}

	while (rightPos <= rightEnd) {
		tmpArray[tmpPos++] = std::move((*a)[rightPos++]);
	}

	//copy tmpArray back
	for (int i = 0; i < numElements; ++i, --rightEnd) {
		(*a)[rightEnd] = std::move(tmpArray[rightEnd]);
	}
}
void setVertexNormals() {
	for (int i = 0; i < objects.size(); i++) {
		for (int j = 0; j < objects[i]->vertices.size(); j++) {
			objects[i]->vertices[j]->normal = objects[i]->triangles[objects[i]->vertices[j]->triangleIndex]->normal;
		}
	}
}
void setVertexIntensities() {
	setPhongIntensity();
	normalizePhongIntensity(maxIntensity());
	clampNegativeIntensities();
}
void setPhongIntensity() {
	for (int i = 0; i < objects.size(); i++) {
		for (int j = 0; j < objects[i]->vertices.size(); j++) {
			int n = materials[objects[i]->materialID - 1]->n;
			float ka = materials[objects[i]->materialID - 1]->ka;
			float kd = materials[objects[i]->materialID - 1]->kd;
			float ks = materials[objects[i]->materialID - 1]->ks;
			float Ia = light.ambientIntensity;
			float Il = light.lightIntensity;
			float K = light.K;
			FloatTriple f_xy = getFromPoint(0);
			FloatTriple f_xz = getFromPoint(1);
			FloatTriple f_yz = getFromPoint(2);
			FloatTriple l = getNormalizedLightVector(*(objects[i]->vertices[j]));
			FloatTriple normal = *(objects[i]->vertices[j]->normal);
			FloatTriple r = getNormalizedReflectionVector(l, normal);
			FloatTriple v_xy = getViewingVector(f_xy, *(objects[i]->vertices[j]));
			FloatTriple v_xz = getViewingVector(f_xz, *(objects[i]->vertices[j]));
			FloatTriple v_yz = getViewingVector(f_yz, *(objects[i]->vertices[j]));

			//SET INTENSITIES FOR ALL THREE PROJECTIONS
			objects[i]->vertices[j]->setIntensity(n, ka, kd, ks, Ia, Il, K, f_xy, l, normal, r, v_xy, 0);
			objects[i]->vertices[j]->setIntensity(n, ka, kd, ks, Ia, Il, K, f_xz, l, normal, r, v_xz, 1);
			objects[i]->vertices[j]->setIntensity(n, ka, kd, ks, Ia, Il, K, f_yz, l, normal, r, v_yz, 2);
		}
	}
}
FloatTriple getFromPoint(int projection) {
	float x, y, z;
	if (projection == 0) { //xy 0,5
		x = -0.5;
		y = -0.5;
		z = light.F;
	}
	else if (projection == 1) { //xz
		x = -0.5;
		y = light.F;
		z = -0.5;
	}
	else if (projection == 2) { //yz
		x = light.F;
		y = -0.5;
		z = -0.5;
	}

	return FloatTriple(x, y, z);
}
FloatTriple getNormalizedLightVector(ObjectVertex vertex) {
	float x = (light.lightSource->x - vertex.x);
	float y = (light.lightSource->y - vertex.y);
	float z = (light.lightSource->z - vertex.z);
	return normalize(FloatTriple(x, y, z));
}
FloatTriple getNormalizedReflectionVector(FloatTriple light, FloatTriple normal) {
	float constant = 2 * dotProduct(light, normal);
	float x = (constant * normal.x) - light.x;
	float y = (constant * normal.y) - light.y;
	float z = (constant * normal.z) - light.z;
	return normalize(FloatTriple(x, y, z));
}
FloatTriple getViewingVector(FloatTriple fromPoint, ObjectVertex vertex) {
	float x = fromPoint.x - vertex.x;
	float y = fromPoint.y - vertex.y;
	float z = fromPoint.z - vertex.z;
	return normalize(FloatTriple(x, y, z));
}
FloatTriple normalize(FloatTriple vector) {
	float magnitude = sqrt(pow(vector.x, 2) + pow(vector.y, 2) + pow(vector.z, 2));
	float x = vector.x / magnitude;
	float y = vector.y / magnitude;
	float z = vector.z / magnitude;
	return FloatTriple(x, y, z);
}
float dotProduct(FloatTriple v1, FloatTriple v2) {

	return (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z);
}
void normalizePhongIntensity(FloatTriple maxIntensity) {
	float maxXY = maxIntensity.x;
	float maxXZ = maxIntensity.y;
	float maxYZ = maxIntensity.z;
	for (int i = 0; i < objects.size(); i++) {
		for (int j = 0; j < objects[i]->vertices.size(); j++) {
			objects[i]->vertices[j]->intensityXY = objects[i]->vertices[j]->intensityXY / maxXY;
			objects[i]->vertices[j]->intensityXZ = objects[i]->vertices[j]->intensityXZ / maxXZ;
			objects[i]->vertices[j]->intensityYZ = objects[i]->vertices[j]->intensityYZ / maxYZ;
		}
	}
}
FloatTriple maxIntensity() {

	FloatTriple maxIntensity(-100.0, -100.0, -100.0);
	for (int i = 0; i < objects.size(); i++) {
		for (int j = 0; j < objects[i]->vertices.size(); j++) {
			if (objects[i]->vertices[j]->intensityXY > maxIntensity.x) {
				maxIntensity.x = objects[i]->vertices[j]->intensityXY;
			}

			if (objects[i]->vertices[j]->intensityXZ > maxIntensity.y) {
				maxIntensity.y = objects[i]->vertices[j]->intensityXZ;
			}

			if (objects[i]->vertices[j]->intensityYZ > maxIntensity.z) {
				maxIntensity.z = objects[i]->vertices[j]->intensityYZ;
			}
		}
	}
	return maxIntensity;
}
void clampNegativeIntensities() {
	for (int i = 0; i < objects.size(); i++) {
		for (int j = 0; j < objects[i]->vertices.size(); j++) {
			if (objects[i]->vertices[j]->intensityXY < 0) {
				objects[i]->vertices[j]->intensityXY = 0;
			}
			if (objects[i]->vertices[j]->intensityXZ < 0) {
				objects[i]->vertices[j]->intensityXZ = 0;
			}
			if (objects[i]->vertices[j]->intensityYZ < 0) {
				objects[i]->vertices[j]->intensityYZ = 0;
			}
		}
	}
}
void clearBuffer(int mapType) {
	for (int i = 0; i < (win_width * win_height) / 3; i++) {
		buffer[i] = 0;
	}
}
void draw_blue_line(float x1, float y1, float x2, float y2) {
	glLineWidth(0.2);
	glBegin(GL_LINES);
	glColor3f(0,0,1);
	glVertex2f(x1, y1);
	glVertex2f(x2, y2);
	glEnd();
}

void drawPixBinary(int x, int y) {
	glBegin(GL_POINTS);
    glColor3f(1,1,1);
    glVertex3f(x,y,0);
    glEnd();
}

void handleInput() {
	cout << endl << endl;
	bool userExit = false;
	cout << "Press q if you are done changing parameters" << endl;
	cout << "To change the shading method, press S" << endl;
	cout << "Note that you must press quit before any changes to the scene are to be made" << endl;
	while (!userExit) {
		string choice = "";
		cout << "Parameter choice(S, ka, kd, ks, Ia, Il, K, n): ";
		getline(cin, choice);
		if (choice.compare("q") == 0) {
			userExit = true;
		}
		else {
			if (choice.compare("ka") == 0) setKa();
			else if (choice.compare("S") == 0) setS();
			else if (choice.compare("kd") == 0) setKd();
			else if (choice.compare("ks") == 0) setKs();
			else if (choice.compare("Ia") == 0) setIa();
			else if (choice.compare("Il") == 0) setIl();
			else if (choice.compare("K") == 0) setK();
			else if (choice.compare("n") == 0) setN();
			else cout << "Wrong usage" << endl;
		}
	}
}
void setS() {
	string input = "";
	cout << "H for half-toning, P for Phong: ";
	getline(cin , input);
	stringstream stream(input);
	char choice;
	if (stream >> choice) {
		if (choice == 'H') {
			mapType = 3;
		}
		if (choice == 'P') {
			mapType = 1;
		}
	}
	else {
		cout << "Wrong usage" << endl;
	}
}

void setKa() {
	string input = "";
	cout << "Specify Object: (1-" << objects.size() << "): ";
	getline(cin, input);
	stringstream stream(input);
	int objectIndex;
	if (stream >> objectIndex) {
		cout << "ka: ";
		input = "";
		getline(cin, input);
		stringstream stream1(input);
		float ka;
		if (stream1 >> ka) {
			if (objects.size() > 0) {
				materials[objects[objectIndex - 1]->materialID]->ka = ka;
			}
		}
		else {
			cout << "Wrong usage" << endl;
		}
	}
	else {
		cout << "Wrong usage" << endl;
	}
}
void setKd() {
	string input = "";
	cout << "Specify Object: (1-" << objects.size() << "): ";
	getline(cin, input);
	stringstream stream(input);
	int objectIndex;
	if (stream >> objectIndex) {
		cout << "kd: ";
		input = "";
		getline(cin, input);
		stringstream stream1(input);
		float kd;
		if (stream1 >> kd) {
			if (objects.size() > 0) {
				materials[objects[objectIndex - 1]->materialID]->kd = kd;
			}
		}
		else {
			cout << "Wrong usage" << endl;
		}
	}
	else {
		cout << "Wrong usage" << endl;
	}
}
void setKs() {
	string input = "";
	cout << "Specify Object: (1-" << objects.size() << "): ";
	getline(cin, input);
	stringstream stream(input);
	int objectIndex;
	if (stream >> objectIndex) {
		cout << "ks: ";
		input = "";
		getline(cin, input);
		stringstream stream1(input);
		float ks;
		if (stream1 >> ks) {
			if (objects.size() > 0) {
				materials[objects[objectIndex - 1]->materialID]->ks = ks;
			}
		}
		else {
			cout << "Wrong usage" << endl;
		}
	}
	else {
		cout << "Wrong usage" << endl;
	}
}
void setN() {
	string input = "";
	cout << "Specify Object: (1-" << objects.size() << "): ";
	getline(cin, input);
	stringstream stream(input);
	int objectIndex;
	if (stream >> objectIndex) {
		cout << "n: ";
		input = "";
		getline(cin, input);
		stringstream stream1(input);
		int n;
		if (stream1 >> n) {
			if (objects.size() > 0) {
				materials[objects[objectIndex - 1]->materialID]->n = n;
			}
		}
		else {
			cout << "Wrong usage" << endl;
		}
	}
	else {
		cout << "Wrong usage" << endl;
	}
}
void setIa() {
	string input = "";
	cout << "Ia: ";
	getline(cin, input);
	stringstream stream(input);
	float Ia;
	if (stream >> Ia) {
		light.ambientIntensity = Ia;
	}
	else {
		cout << "Wrong usage";
	}
}
void setIl() {
	string input = "";
	cout << "Il: ";
	getline(cin, input);
	stringstream stream(input);
	float Il;
	if (stream >> Il) {
		light.lightIntensity = Il;
	}
	else {
		cout << "Wrong usage";
	}
}
void setK() {
	string input = "";
	cout << "K: ";
	getline(cin, input);
	stringstream stream(input);
	float K;
	if (stream >> K) {
		light.K = K;
	}
	else {
		cout << "Wrong usage";
	}
}





