#ifndef Point_H
#define Point_H

#if defined (__APPLE__) || defined(MACOSX)
#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#endif

#include "Common.h"
#include "PixelInfo.h"

class Point {
public:
	Point(int x, int y);
	void drawPoint(int curveIndex, int pointIndex);
	void clearPoint();
	void decrementPointIndex();
	int getX();
	void setX(int x);
	int getY();
	void setY(int y);
private:
	int x;
	int y;
};

Point::Point(int x, int y) {
	this->x = x;
	this->y = y;
}

void Point::drawPoint(int curveIndex, int pointIndex) {
	int size = 3;
	for (int i = x - size; i < x + size; i++) {
		for (int j = y - size; j < y + size; j++) {
			Common::sceneBuffer[Common::win_width * i + j].setStatus(1);
			Common::sceneBuffer[Common::win_width * i + j].setCurve(curveIndex);
			Common::sceneBuffer[Common::win_width * i + j].setPoint(pointIndex);
		}
	}
}

void Point::clearPoint() {
	int size = 3;
	for (int i = x - size; i < x + size; i++) {
		for (int j = y - size; j < y + size; j++) {
			Common::sceneBuffer[Common::win_width * i + j].setStatus(0);
			Common::sceneBuffer[Common::win_width * i + j].setCurve(-1);
			Common::sceneBuffer[Common::win_width * i + j].setPoint(-1);
		}
	}
}

void Point::decrementPointIndex() {
	int size = 3;
	for (int i = x - size; i < x + size; i++) {
		for (int j = y - size; j < y + size; j++) {
			Common::sceneBuffer[Common::win_width * i + j].setPoint(Common::sceneBuffer[Common::win_width * i + j].getPoint() - 1);
		}
	}
}


int Point::getX() {
	return x;
}
void Point::setX(int x) {
	this->x = x;
}
int Point::getY() {
	return y;
}
void Point::setY(int y) {
	this->y = y;
}

#endif