#include <iostream>
#include "Displayer.h"
#include "Common.h"
using namespace std;

void init();
void idle();
void display();
void reshape(int width, int height);
void key(unsigned char ch, int x, int y);
void mouse(int button, int state, int x, int y);
void motion(int x, int y);
void check();
int* sceneBuffer;
int Common::win_width = 500;
int Common::win_height = 500;
PixelInfo* Common::sceneBuffer;
Displayer* displayer;

int main(int argc, char** argv) {
	displayer = new Displayer();
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(Common::win_width, Common::win_height);
	glutCreateWindow("A SIMPLE BEZIER AND B-SPLINE CURVE EDITOR");
	glutDisplayFunc(display);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutIdleFunc(idle);
	glutReshapeFunc(reshape); //update GL on window size change
	glutKeyboardFunc(key);
	init();
	glutMainLoop();
}

void display() {
    //clears the screen
	glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);
    //clears the opengl Modelview transformation matrix
	glLoadIdentity();
	
    displayer->display();
    
    //blits the current opengl framebuffer on the screen
    glutSwapBuffers();
    //checks for opengl errors
	check();
}

void init() {
	glClearColor(1.0,1.0,1.0,1.0);
	check();
}


void idle() {
    //redraw the scene over and over again
	glutPostRedisplay();	
}

void reshape(int width, int height) {
	Common::win_width = width;
	Common::win_height = height;
	glViewport(0,0,width,height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
	glOrtho(0,Common::win_width,0.0,Common::win_height,-10,10);
    glMatrixMode(GL_MODELVIEW); //why does not including this make the viewport weird?
    glLoadIdentity();
    
    //check for opengl errors
    check();
}

//1 is 49
void key(unsigned char ch, int x, int y) {
	if (ch == 'd') {
		displayer->setPointAdder(false); //disable point adding
		displayer->setPointMover(true); //enable point moving
		displayer->setPointRemover(false);
	}
	else if (ch == 'a') {
		displayer->setPointMover(false); //disable point moving
		displayer->setPointRemover(false); //disable point removing
		displayer->setPointAdder(true); //enable point adding
	}
	else if (ch == 'c') {
		displayer->addCurve();
		displayer->setPointAdder(true); //enable point adding
		displayer->setPointRemover(false); //disable point removing
		displayer->setPointMover(false); //disable point moving

	}
	else if (ch == 'r') {
		displayer->setPointRemover(true); //enable point removing
		displayer->setPointAdder(false); //disable point adding
		displayer->setPointMover(false); //disable point moving
	}
	else if (ch == 'b') {
		displayer->bezierRequest();
	}
	else if (ch == 's') {
		displayer->bSplineRequest();
	}
	else if (ch == 'i') {
		displayer->changeCurrentRequest();
	}

	glutPostRedisplay();
}

void mouse(int button, int state, int x, int y) {
	displayer->mouseEvent(state, x, Common::win_height - y);
    glutPostRedisplay();
}

void motion(int x, int y) {
	displayer->motionEvent(x, Common::win_height - y);
	glutPostRedisplay();
}

void check() {
	GLenum err = glGetError();
	if(err != GL_NO_ERROR)
	{
		printf("GLERROR: There was an error %s\n",gluErrorString(err) );
		exit(1);
	}
}

