#ifndef Common_H
#define Common_H

#if defined (__APPLE__) || defined(MACOSX)
#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#endif

#include <iostream>
#include "PixelInfo.h"

class Common {
public:
	static int win_width;
	static int win_height;
	static PixelInfo* sceneBuffer;
	static void drawPix(int x, int y, int curveIndex) {
		glBegin(GL_POINTS);
		switch (curveIndex) {
			case 0:
				glColor3f(1,0,0);
				break;
			case 1:
				glColor3f(0,0,1);
				break;
			case 2:
				glColor3f(0,1,1);
				break;
			case 3:
				glColor3f(1,0,1);
				break;
			case 4:
				glColor3f(0,1,0);
				break;
			default:
				glColor3f(0,0,0); //if more than 5 curves then the over ones are all black
		}
    	glVertex2i(x,y);
    	glEnd();
	}
};

#endif