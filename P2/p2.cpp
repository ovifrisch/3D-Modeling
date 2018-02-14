#ifdef WIN32
#include <windows.h>
#endif

#if defined (__APPLE__) || defined(MACOSX)
#include <OpenGL/gl.h>
#include <GLUT/glut.h>

#else
#include <GL/gl.h>
#include <GL/glut.h>
#endif

#include "Object.h"
#include "Edge.h"
#include "Vertex.h"
#include <vector>
#include <iostream>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <fstream>
using namespace std;

//Functions
void drawProjectionObject(int i, int j);
void showVertices();



void readFile();
void transferSceneToFile();
void drawScene();
void xz_projection(int index);
void xy_projection(int index);
void yz_projection(int index);
void handleInput();
char getTransformation();
void handleRotationInput();
void handleTranslationInput();
void handleScalingInput();

//MATH
float toRadian(float degree);


//3D TRANSFORMATIONS
//HIGH LEVEL
void rotateObject(float x1, float y1, float z1, float x2, float y2, float z2, float angleDegrees);
void translateObject(float tx, float ty, float tz);
void scaleObject(float factor);
vector<float> translatePoint(float x, float y, float z, float tx, float ty, float tz);
vector<float> scalePoint(float x, float y, float z, float fixed_x, float fixed_y, float fixed_z, float factor);
vector<float> rotatePoint(float x, float y, float z, float xo, float yo, float zo, float xf, float yf, float zf, float theta);

//LOW LEVEL
vector<float> rotatePointZ(float x, float y, float z, float theta);
vector<float> rotatePointX(float x, float y, float z, float theta);
vector<float> rotatePointY(float x, float y, float z, float theta);
vector<float> scalePointAtOrigin(float x, float y, float z, float factor);

//VECTOR FUNCTIONS
vector<float> unitVector(float xo, float yo, float zo, float xf, float yf, float zf);
float dotProduct(vector<float> v1, vector<float> v2);
float magnitudeVector(vector<float> v);


void init();
void display();
void idle();
void mouse(int button, int state, int x, int y);
void motion(int x, int y);
void check();
void reshape(int width, int height);
void key(GLubyte key, GLint xMouse, GLint yMouse);

void draw_line(float x1, float y1, float x2, float y2);
void drawRotLine(float x1, float y1, float x2, float y2);
void draw_black_line(float x1, float y1, float x2, float y2);

//LOCALS
char* filename;
vector<Object*> objects;
int numObjects;
int win_width;
int win_height;
int currentIndex;

float rotationAxisX1;
float rotationAxisX2;
float rotationAxisY1;
float rotationAxisY2;
float rotationAxisZ1;
float rotationAxisZ2;

int main(int argc, char** argv) {
	rotationAxisX1 = 0;
	rotationAxisX2 = 0;
	rotationAxisY1 = 0;
	rotationAxisY2 = 0;
	rotationAxisZ1 = 0;
	rotationAxisZ2 = 0;
	win_height = 500;
	win_width = 500;
	filename = argv[1];
	readFile();

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(win_width, win_height);
	glutCreateWindow("Project 2");


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

void init() {
	glClearColor(1.0, 1.0, 1.0, 1.0);
	check();
}

void display() {
	glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);
	glLoadIdentity();

	drawScene();
	glViewport(0, 0, win_width, win_height);
	draw_black_line(0.5, 0, 0.5, 1);
	draw_black_line(0, 0.5, 1, 0.5);

	glutSwapBuffers();
	check();
}

void idle()
{
    //redraw the scene over and over again
	glutPostRedisplay();	
}

void mouse(int button, int state, int x, int y) {
    //redraw the scene after mouse click
    glutPostRedisplay();
}

void motion(int x, int y)
{
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

void reshape(int width, int height) {
	win_width = width;
	win_height = height;
	glViewport(0,0,width,height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, 1.0, 0, 1.0, 0, 10);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	check();
}

void key(unsigned char ch, int x, int y) {
	handleInput();
	glutPostRedisplay();
}

//i is width, j is height
void drawProjectionObject(int i, int j) {
	for (int k = 0; k < objects.size(); k++) {
		if (i == 1 && j == 1) {
			xz_projection(k);
		}
		else if (i == 0 && j == 0) {
			yz_projection(k);
		}
		else if (i == 0 && j == 1) {
			xy_projection(k);
		}
	}
}

//ignore z
//x is x and y is y!
void xy_projection(int index) {
	for (int i = 0; i < objects[index]->edges.size(); i++) {
		int vertexIndex1 = objects[index]->edges[i]->index1;
		int vertexIndex2 = objects[index]->edges[i]->index2;
		float x1 = objects[index]->vertices[vertexIndex1 - 1]->x;
		float y1 = objects[index]->vertices[vertexIndex1 - 1]->y;
		float x2 = objects[index]->vertices[vertexIndex2 - 1]->x;
		float y2 = objects[index]->vertices[vertexIndex2 - 1]->y;
		draw_line(x1, y1, x2, y2);
	}
}

//ignore x
//x is y and y is z
void yz_projection(int index) {
	for (int i = 0; i < objects[index]->edges.size(); i++) {
		int vertexIndex1 = objects[index]->edges[i]->index1;
		int vertexIndex2 = objects[index]->edges[i]->index2;
		float x1 = objects[index]->vertices[vertexIndex1 - 1]->y;
		float y1 = objects[index]->vertices[vertexIndex1 - 1]->z;
		float x2 = objects[index]->vertices[vertexIndex2 - 1]->y;
		float y2 = objects[index]->vertices[vertexIndex2 - 1]->z;
		draw_line(x1, y1, x2, y2);
	}
}

//Ignoring y-coordinate
//viewing from the XZ plane
void xz_projection(int index) {
	for (int i = 0; i < objects[index]->edges.size(); i++) {
		int vertexIndex1 = objects[index]->edges[i]->index1;
		int vertexIndex2 = objects[index]->edges[i]->index2;
		float x1 = objects[index]->vertices[vertexIndex1 - 1]->x; // x is x
		float y1 = objects[index]->vertices[vertexIndex1 - 1]->z; // y is z
		float x2 = objects[index]->vertices[vertexIndex2 - 1]->x;
		float y2 = objects[index]->vertices[vertexIndex2 - 1]->z;
		draw_line(x1, y1, x2, y2);
	}
}

void drawScene() {
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 2; j++) {
			glViewport(i * (win_width/2), j * (win_height/2), win_width/2, win_height/2);
			drawProjectionObject(i, j);
			if (i == 0 && j == 0) { //x is y and y is z
				drawRotLine(rotationAxisY1, rotationAxisZ1, rotationAxisY2, rotationAxisZ2);
			}
			else if (i == 1 && j == 1) { //x is x, y is z
				drawRotLine(rotationAxisX1, rotationAxisZ1, rotationAxisX2, rotationAxisZ2);
			}
			else if (i == 0 && j == 1) {
				drawRotLine(rotationAxisX1, rotationAxisY1, rotationAxisX2, rotationAxisY2);
			}
		}
	}
}




void draw_line(float x1, float y1, float x2, float y2) {
	glLineWidth(1.0);
	glBegin(GL_LINES);
	glColor3f(1.0,0,0);
	glVertex2f(x1, y1);
	glVertex2f(x2, y2);
	glEnd();
}

void draw_black_line(float x1, float y1, float x2, float y2) {
	glLineWidth(0.2);
	glBegin(GL_LINES);
	glColor3f(0,0,0);
	glVertex2f(x1, y1);
	glVertex2f(x2, y2);
	glEnd();
}

void drawRotLine(float x1, float y1, float x2, float y2) {
	glLineWidth(1.0);
	glBegin(GL_LINES);
	glColor3f(0,0,1.0);
	glVertex2f(x1, y1);
	glVertex2f(x2, y2);
	glEnd();
}

void handleInput() {
	cout << endl << endl;
	int objID;
	cout << "Object ID [1, " << objects.size() << "]: " << endl;
	cout << "Your choice: ";
	if (!(cin >> objID)) {
		cout << "Invalid Input. Re-run program." << endl;
		return;
	}
	if (!(objID >= 1 && objID <= objects.size())) {
		cout << "Invalid Input. Re-run program." << endl;
		return;
	}
	currentIndex = objID - 1;
	char op = getTransformation();
	if (op != 'r' && op != 's' && op != 't' && op != 'v') {
		cout << "Invalid Input. Re-run program." << endl;
		return;
	}
	switch (op) {
		case 'r':
			handleRotationInput();
			break;
		case 's':
			handleScalingInput();
			break;
		case 't':
			handleTranslationInput();
			break;
		case 'v':
			showVertices();
	}
	transferSceneToFile();
}

void showVertices() {
	for (int i = 0; i < objects[currentIndex]->vertices.size(); i++) {
		cout << objects[currentIndex]->vertices[i]->x << ", " 
		<< objects[currentIndex]->vertices[i]->y << ", " << objects[currentIndex]->vertices[i]->z << endl;
	}
}

void handleRotationInput() {
	float angleDegrees;
	cout << "Rotation Axis Point 1" << endl;
	cout << "x: ";
	cin >> rotationAxisX1;
	cout << "y: ";
	cin >> rotationAxisY1;
	cout << "z: ";
	cin >> rotationAxisZ1;

	cout << "Rotation Axis Point 2" << endl;
	cout << "x: ";
	cin >> rotationAxisX2;
	cout << "y: ";
	cin >> rotationAxisY2;
	cout << "z: ";
	cin >> rotationAxisZ2;

	cout << "Rotation Angle (in degrees): ";
	cin >> angleDegrees;
	rotateObject(rotationAxisX1, rotationAxisY1, rotationAxisZ1, rotationAxisX2, rotationAxisY2, rotationAxisZ2, angleDegrees);
}

void handleTranslationInput() {
	float tx, ty, tz;
	cout << "Translation Vector" << endl;
	cout << "tx: ";
	cin >> tx;
	cout << "ty: ";
	cin >> ty;
	cout << "tz: ";
	cin >> tz;
	translateObject(tx, ty, tz);
}

void handleScalingInput() {
	float scaleFactor;
	cout << "Scaling Factor: ";
	cin >> scaleFactor;
	scaleObject(scaleFactor);
}

char getTransformation() {
	char op;
	cout << endl << endl;
	cout << "Press 'v' to display this object's vertices" << endl;
	cout << "Press 'r' to rotate" << endl;
	cout << "Press 's' to scale" << endl;
	cout << "Press 't' to translate" << endl;
	cout << "Your choice: ";
	cin >> op;
	return op;
}

void readFile() {
	ifstream inFile(filename);
	inFile >> numObjects;
	for (int i = 0; i < numObjects; i++) {
		Object* o = new Object();
		int numPoints;
		int numEdges;
		inFile >> numPoints;
		for (int i = 0; i < numPoints; i++) {
			float x;
			float y;
			float z;
			inFile >> x >> y >> z;
			o->addVertex(x, y, z);
		}
		inFile >> numEdges;
		for (int j = 0; j < numEdges; j++) {
			int i1, i2; //the indices of two points
			inFile >> i1 >> i2;
			o->addEdge(i1, i2);
		}
		objects.push_back(o);
	}
}

void transferSceneToFile() {
	ofstream outFile("temp.txt");
	outFile << objects.size() << endl;
	for (int i = 0; i < objects.size(); i++) {
		outFile << endl;
		outFile << objects[i]->vertices.size() << endl;
		for (int j = 0; j < objects[i]->vertices.size(); j++) {
			outFile << objects[i]->vertices[j]->x << " " << objects[i]->vertices[j]->y << " " << objects[i]->vertices[j]->z << endl;
		}
		outFile << objects[i]->edges.size() << endl;
		for (int k = 0; k < objects[i]->edges.size(); k++) {
			outFile << objects[i]->edges[k]->index1 << " " << objects[i]->edges[k]->index2 << endl;
		}
	}
	remove(filename);
	rename("temp.txt", filename);
}

//Points 1 and 2 define the rotation axis
//Rotates the object defined by the currentIndex
void rotateObject(float x1, float y1, float z1, float x2, float y2, float z2, float angleDegrees) {
	float angleRadians = toRadian(angleDegrees);
	for (int i = 0; i < objects[currentIndex]->vertices.size(); i++) {
		vector<float> result;
		float x = objects[currentIndex]->vertices[i]->x;
		float y = objects[currentIndex]->vertices[i]->y;
		float z = objects[currentIndex]->vertices[i]->z;
		result = rotatePoint(x, y, z, x1, y1, z1, x2, y2, z2, angleRadians);
		objects[currentIndex]->vertices[i]->x = result[0];
		objects[currentIndex]->vertices[i]->y = result[1];
		objects[currentIndex]->vertices[i]->z = result[2];
	}
}

//Translates the object defined by the currentIndex
void translateObject(float tx, float ty, float tz) {
	for (int i = 0; i < objects[currentIndex]->vertices.size(); i++) {
		vector<float> result;
		float x = objects[currentIndex]->vertices[i]->x;
		float y = objects[currentIndex]->vertices[i]->y;
		float z = objects[currentIndex]->vertices[i]->z;
		result = translatePoint(x, y, z, tx, ty, tz);
		objects[currentIndex]->vertices[i]->x = result[0];
		objects[currentIndex]->vertices[i]->y = result[1];
		objects[currentIndex]->vertices[i]->z = result[2];
	}
}

//Scales the object defined by the currentIndex
void scaleObject(float factor) {
	vector<float> centroid = objects[currentIndex]->centroid();
	float x_centroid = centroid[0];
	float y_centroid = centroid[1];
	float z_centroid = centroid[2];
	for (int i = 0; i < objects[currentIndex]->vertices.size(); i++) {
		vector<float> result;
		float x = objects[currentIndex]->vertices[i]->x;
		float y = objects[currentIndex]->vertices[i]->y;
		float z = objects[currentIndex]->vertices[i]->z;
		result = scalePoint(x, y, z, x_centroid, y_centroid, z_centroid, factor);
		objects[currentIndex]->vertices[i]->x = result[0];
		objects[currentIndex]->vertices[i]->y = result[1];
		objects[currentIndex]->vertices[i]->z = result[2];
	}
}

//Translate a point in 3D space by dx, dy, dz
vector<float> translatePoint(float x, float y, float z, float tx, float ty, float tz) {
	vector<float> result;
	result.push_back(x + tx);
	result.push_back(y + ty);
	result.push_back(z + tz);
	return result;
}

//Rotates a point in 3D space
vector<float> rotatePoint(float x, float y, float z, float xo, float yo, float zo, float xf, float yf, float zf, float theta) {
	vector<float> result;

	//MOVE ROTATION AXIS ONTO Z-AXIS
	result = translatePoint(x, y, z, -xo, -yo, -zo); //TRANSFORM 1

	//TRANSFORM 2
	//ROTATE ABOUT X BY ANGLE1
	float angle1;
	vector<float> unitVect = unitVector(xo, yo, zo, xf, yf, zf);

	//Factor 1 for dot product
	vector<float> unitVectPrime; //the projection of the unit vector onto the yz plane
	unitVectPrime.push_back(0);
	unitVectPrime.push_back(unitVect[1]);
	unitVectPrime.push_back(unitVect[2]);

	//Factor 2 for dot product
	vector<float> unitVectZ; //the unit vector along the z-axis
	unitVectZ.push_back(0);
	unitVectZ.push_back(0);
	unitVectZ.push_back(1);

	//angle 1 = inverse cosine (dot Product between two vectors)
	angle1 = acos(dotProduct(unitVectPrime, unitVectZ));

	//Rotate point about X axis by angle1:
	result = rotatePointX(result[0], result[1], result[2], angle1);

	//ROTATE ABOUT Y AXIS BY ANGLE2
	float angle2;

	//the unit vector rotaed about x by angle1
	vector<float> unitVectDoublePrime = rotatePointX(unitVect[0], unitVect[1], unitVect[2], angle1);

	//angle 2 = inverse cosine (dot Product between two vectors)
	angle2 = acos(dotProduct(unitVectDoublePrime, unitVectZ));

	//Rotate point about Y axis by angle 2:
	result = rotatePointY(result[0], result[1], result[2], angle2);

	//TRANSFORM 3
	result = rotatePointZ(result[0], result[1], result[2], theta);

	//TRANSFORM 4: UNDO TRANSFORM 2
	result = rotatePointY(result[0], result[1], result[2], -angle2);
	result = rotatePointX(result[0], result[1], result[2], -angle1);

	//TRANSFORM 5: UNDO TRANSFORM 1
	result = translatePoint(result[0], result[1], result[2], xo, yo, zo); //step 5

	return result;
}

//Rotates a specified point about the z axis
vector<float> rotatePointZ(float x, float y, float z, float theta) {
	vector<float> result;
	result.push_back((x*cos(theta)) - (y*sin(theta)));
	result.push_back((x*sin(theta)) + (y*cos(theta)));
	result.push_back(z);
	return result;
}

//Rotates a specified point about the x axis
vector<float> rotatePointX(float x, float y, float z, float theta) {
	vector<float> result;
	result.push_back(x);
	result.push_back((y*cos(theta)) - (z*sin(theta)));
	result.push_back((y*sin(theta)) + (z*cos(theta)));
	return result;
}

//Rotates a specified point about the y axis
vector<float> rotatePointY(float x, float y, float z, float theta) {
	vector<float> result;
	result.push_back((x*cos(theta)) + (z*sin(theta)));
	result.push_back(y);
	result.push_back((-x*sin(theta)) + (z*cos(theta)));
	return result;
}

//Scales a point relative to a fixed point
vector<float> scalePoint(float x, float y, float z, float fixed_x, float fixed_y, float fixed_z, float factor) {
	vector<float> result;
	result = translatePoint(x, y, z, -fixed_x, -fixed_y, -fixed_z);
	result = scalePointAtOrigin(result[0], result[1], result[2], factor);
	result = translatePoint(result[0], result[1], result[2], fixed_x, fixed_y, fixed_z);
	return result;
}

//Scales a point at the origin
vector<float> scalePointAtOrigin(float x, float y, float z, float factor) {
	vector<float> result;
	result.push_back(x * factor);
	result.push_back(y * factor);
	result.push_back(z * factor);
	return result;
}

//computes the unit vector defined by 2 points in the xyz coord system
vector<float> unitVector(float xo, float yo, float zo, float xf, float yf, float zf) {
	vector<float> unit;
	float magnitude = sqrt((pow(xf - xo, 2) + pow(yf - yo, 2) + pow(zf -zo, 2)));
	unit.push_back((xf - xo) / magnitude);
	unit.push_back((yf - yo) / magnitude);
	unit.push_back((zf - zo) / magnitude);
	return unit;
}

//computes the length of a vector
float magnitudeVector(vector<float> v) {
	float sum = 0;
	for (int i = 0; i < v.size(); i++) {
		sum += pow(v[i], 2);
	}
	return sqrt(sum);
}

//coimputes the dot product between 2 vectors
float dotProduct(vector<float> v1, vector<float> v2) {
	float sum = 0;
	float magnitude1 = magnitudeVector(v1);
	float magnitude2 = magnitudeVector(v2);
	for (int i = 0; i < v1.size(); i++) {
		sum += (v1[i] * v2[i]);
	}
	return sum / (magnitude1 * magnitude2);
}

//Ignoring z-coordinate
//viewing from the XY plane
//void drawObjectXYOrtho()

float toRadian(float degree) {
	return (degree * M_PI) / 180;
}






