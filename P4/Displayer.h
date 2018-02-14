#ifndef Displayer_H
#define Displayer_H

#if defined (__APPLE__) || defined(MACOSX)
#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#endif

#include "Curve.h"
#include "BezierCurve.h"
#include "BSplineCurve.h"
#include "Common.h"
#include <vector>

class Displayer {
public:
	Displayer();
	void display();
	void mouseEvent(int state, int x, int y);
	void motionEvent(int x, int y);
	void addCurve(); //automatically sets the current curve to this one
	void setPointAdder(bool state);
	void setPointMover(bool state);
	void setPointRemover(bool state);
	void setCurrent(int pos);
	void bezierRequest();
	void bSplineRequest();
	void changeCurrentRequest();
	int changeCurrentToCurveAtClick(int x, int y);

private:
	std::vector<Curve*> curves;
	int current; //the current curve being operated on
	bool pointAdder; // if true, a mouse click will add a control point at that position for the current curve. if false, not ~
	bool pointRemover;
	bool pointMover;
	bool changer;
};

Displayer::Displayer() {
	Curve* first = new Curve(); //automatically create a curve when program is fired up
	curves.push_back(first);
	current = 0;
	pointAdder = true; //initialize to true so when program opens user can directly start adding points
	pointRemover = false;
	pointMover = false; //changes to true when user specifies that they would like to move a point
	changer = false;
	Common::sceneBuffer = new PixelInfo[Common::win_width * Common::win_height];
}

void Displayer::display() {
	for (int i = 0; i < Common::win_width; i++) {
		for (int j = 0; j < Common::win_height; j++) {
			int index = i * Common::win_width + j;
			if (Common::sceneBuffer[index].getStatus() == 1) {
				Common::drawPix(i, j, Common::sceneBuffer[index].getCurve());
			}
		}
	}
}

void Displayer::addCurve() {
	Curve* curve = new Curve();
	curves.push_back(curve);
	current = curves.size() - 1;
}

//state is 0 on press, 1 on release
void Displayer::mouseEvent(int state, int x, int y) {
	if (changer && state == 0) {
		current = changeCurrentToCurveAtClick(x, y);
	}
	if (pointAdder && !pointMover && !pointRemover && state == 0) {
		curves[current]->addControlPoint(x, y, current);
	}
	else if (pointMover && !pointAdder && !pointRemover && state == 0) {
		curves[current]->initMoveControlPoint(x, y, current);
	}
	else if (pointMover && !pointAdder && !pointRemover && state == 1) {
		curves[current]->endMoveControlPoint(x, y, current);
	}
	else if (pointRemover && !pointAdder && !pointMover && state == 0) {
		curves[current]->initRemoveControlPoint(x, y, current);
	}
}

int Displayer::changeCurrentToCurveAtClick(int x, int y) {
	int curveIndex = Common::sceneBuffer[Common::win_width * x + y].getCurve();
	if (curveIndex == -1) {
		return current;
	}
	else {
		return curveIndex;
	}
}

void Displayer::motionEvent(int x, int y) {
	if (pointMover) {
		curves[current]->relocatePoint(x, y, current);
	}
}

void Displayer::setPointAdder(bool state) {
	pointAdder = state;
}

void Displayer::setPointRemover(bool state) {
	pointRemover = state;
}

void Displayer::setPointMover(bool state) {
	pointMover = state;
}

void Displayer::setCurrent(int pos) {
	current = pos - 1;
}

void Displayer::bezierRequest() {
	Curve thisCurve = *curves[current];
	curves[current] = new BezierCurve(thisCurve);
	curves[current]->setCurveType(Bezier);
	curves[current]->setBezier(current, true);
}

void Displayer::bSplineRequest() {
	Curve thisCurve = *curves[current];
	curves[current] = new BSplineCurve(thisCurve);
	curves[current]->setCurveType(Spline);

	//initially k is 2
	curves[current]->setBSpline(current, true);
}

void Displayer::changeCurrentRequest() {
	changer = true;
}

#endif