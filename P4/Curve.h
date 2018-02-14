#ifndef Curve_H
#define Curve_H

#if defined (__APPLE__) || defined(MACOSX)
#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#endif


#include "Common.h"
#include "Point.h"
#include <vector>
#include <math.h>

enum CurveType {Bezier, Spline, TBD};

class Curve {
public:
	Curve();
	Curve(Curve& copyMe);
	void addControlPoint(int x, int y, int curveIndex);
	void initRemoveControlPoint(int x, int y, int curveIndex);
	void removeControlPoint(int curveIndex);
	void initMoveControlPoint(int x, int y, int curveIndex);
	void endMoveControlPoint(int x, int y, int curveIndex);
	void relocatePoint(int x, int y, int curveIndex);
	void setCurveType(CurveType type);
	virtual void setBezier(int curveIndex, bool on) {};
	virtual void setBSpline(int curveIndex, bool on) {};
	std::vector<Point*> getControlPoints();

private:
	bool isLegalPtMove(int x, int y, int curveIndex, int& pointIndex);
	void updateAdjacentLineSegments(int old_x, int old_y, int new_x, int new_y, int curveIndex);

	int movingPointIndex;
	int removingPointIndex;
	int curveType;
	CurveType c;

protected:
	void setLine(int xStart, int yStart, int xEnd, int yEnd, int curveIndex, bool on);
	std::vector<Point*> controlPoints;
};

Curve::Curve() {
	movingPointIndex = removingPointIndex = -1; //no points being moved/removed upon curve initialization
	c = TBD;
}

Curve::Curve(Curve& copyMe) {
	for (int i = 0; i < copyMe.getControlPoints().size(); i++) {
		Point* p = new Point(copyMe.getControlPoints()[i]->getX(), copyMe.getControlPoints()[i]->getY());
		controlPoints.push_back(p);
	}
	movingPointIndex = removingPointIndex = -1;
}

void Curve::setCurveType(CurveType type) {
	c = type;
}

std::vector<Point*> Curve::getControlPoints() {
	return controlPoints;
}

void Curve::addControlPoint(int x, int y, int curveIndex) {
	if (c == Bezier) {
		setBezier(curveIndex, false);
	}
	else if (c == Spline) {
		setBSpline(curveIndex, false);
	}
	Point* p = new Point(x, y);
	controlPoints.push_back(p);
	p->drawPoint(curveIndex, controlPoints.size() - 1);
	if (controlPoints.size() > 1) {
		setLine(controlPoints[controlPoints.size() - 2]->getX(),
		controlPoints[controlPoints.size() - 2]->getY(), 
		controlPoints[controlPoints.size() - 1]->getX(),
		controlPoints[controlPoints.size() - 1]->getY(),
		curveIndex, true);
	}
	if (c == Bezier) {
		setBezier(curveIndex, true);
	}
	else if (c == Spline) {
		setBSpline(curveIndex, true);
	}
}

void Curve::removeControlPoint(int curveIndex) {
	if (removingPointIndex == -1) {
		return;
	}

	if (c == Bezier) {
		setBezier(curveIndex, false);
	}
	else if (c == Spline) {
		setBSpline(curveIndex, false);
	}
	//THE POINT INDEX FIELD OF OTHER CURVE CONTROL POINTS CHANGE WHEN A CONTROL POINT IS REMOVED

	if (removingPointIndex == 0 && controlPoints.size() == 1) { //1 control Point
		controlPoints[removingPointIndex]->clearPoint();
		controlPoints.erase(controlPoints.begin() + removingPointIndex);
	}
	else if (removingPointIndex == 0) { // >1 control point and remove point is first one
		controlPoints[removingPointIndex]->clearPoint();
		setLine(controlPoints[0]->getX(), controlPoints[0]->getY(), controlPoints[1]->getX(), controlPoints[1]->getY(), curveIndex, false);
		controlPoints.erase(controlPoints.begin() + removingPointIndex);
	}
	else if (removingPointIndex == controlPoints.size() - 1) { //>1 control point and remove point is last one
		controlPoints[removingPointIndex]->clearPoint();
		setLine(controlPoints[removingPointIndex - 1]->getX(), controlPoints[removingPointIndex - 1]->getY(), controlPoints[removingPointIndex]->getX(), controlPoints[removingPointIndex]->getY(),
			curveIndex, false);
		controlPoints.erase(controlPoints.begin() + removingPointIndex);
	}
	else if (controlPoints.size() > 2) { //remove point is embedded between two points
		controlPoints[removingPointIndex]->clearPoint();
		setLine(controlPoints[removingPointIndex - 1]->getX(), controlPoints[removingPointIndex - 1]->getY(), controlPoints[removingPointIndex]->getX(), controlPoints[removingPointIndex]->getY(),
			curveIndex, false);
		setLine(controlPoints[removingPointIndex]->getX(), controlPoints[removingPointIndex]->getY(), controlPoints[removingPointIndex + 1]->getX(), controlPoints[removingPointIndex + 1]->getY(),
			curveIndex, false);
		setLine(controlPoints[removingPointIndex - 1]->getX(), controlPoints[removingPointIndex - 1]->getY(), controlPoints[removingPointIndex + 1]->getX(), controlPoints[removingPointIndex + 1]->getY(),
			curveIndex, true);
		controlPoints.erase(controlPoints.begin() + removingPointIndex);
	}

	//modify affected point indices on screenBuffer
	for (int i = removingPointIndex; i < controlPoints.size(); i++) {
		controlPoints[i]->decrementPointIndex();
	}
	removingPointIndex = -1;


	if (c == Bezier) {
		setBezier(curveIndex, true);
	}
	else if (c == Spline) {
		setBSpline(curveIndex, true);
	}
}

void Curve::initMoveControlPoint(int x, int y, int curveIndex) {
	isLegalPtMove(x, y, curveIndex, movingPointIndex);
}

void Curve::initRemoveControlPoint(int x, int y, int curveIndex) {
	isLegalPtMove(x, y, curveIndex, removingPointIndex);
	removeControlPoint(curveIndex);
}

void Curve::endMoveControlPoint(int x, int y, int curveIndex) {
	movingPointIndex = -1; //point has finished being moved
}


bool Curve::isLegalPtMove(int x, int y, int curveIndex, int& pointIndex) {
	if (Common::sceneBuffer[Common::win_width * x + y].getPoint() >= 0
		&& Common::sceneBuffer[Common::win_width * x + y].getCurve() == curveIndex) {

		pointIndex = Common::sceneBuffer[Common::win_width * x + y].getPoint();
		return true;
	}

	return false;
}


void Curve::relocatePoint(int x, int y, int curveIndex) {
	if (movingPointIndex == -1) {
		return;
	}

	if (c == Bezier) {
		setBezier(curveIndex, false);
	}
	else if (c == Spline) {
		setBSpline(curveIndex, false);
	}

	controlPoints[movingPointIndex]->clearPoint();

	int old_x = controlPoints[movingPointIndex]->getX();
	int old_y = controlPoints[movingPointIndex]->getY();
	controlPoints[movingPointIndex]->setX(x);
	controlPoints[movingPointIndex]->setY(y);

	controlPoints[movingPointIndex]->drawPoint(curveIndex, movingPointIndex);
	updateAdjacentLineSegments(old_x, old_y, x, y, curveIndex);


	if (c == Bezier) {
		setBezier(curveIndex, true);
	}
	else if (c == Spline) {
		setBSpline(curveIndex, true);
	}
}

void Curve::updateAdjacentLineSegments(int old_x, int old_y, int new_x, int new_y, int curveIndex) {
	if (movingPointIndex == 0 && controlPoints.size() > 1) {
		setLine(old_x, old_y, controlPoints[1]->getX(), controlPoints[1]->getY(), curveIndex, false); //erase old line
		setLine(new_x, new_y, controlPoints[1]->getX(), controlPoints[1]->getY(), curveIndex, true); //draw new line.
	}
	else if (movingPointIndex == controlPoints.size() - 1 && controlPoints.size() > 1) {
		setLine(controlPoints[controlPoints.size() - 2]->getX(), controlPoints[controlPoints.size() - 2]->getY(), old_x, old_y, curveIndex, false); //erase old line
		setLine(controlPoints[controlPoints.size() - 2]->getX(), controlPoints[controlPoints.size() - 2]->getY(), new_x, new_y, curveIndex, true); //draw new line.
	}
	else if (controlPoints.size() > 2) {
		setLine(controlPoints[movingPointIndex - 1]->getX(), controlPoints[movingPointIndex - 1]->getY(), old_x, old_y, curveIndex, false); //erase old line on left
		setLine(old_x, old_y, controlPoints[movingPointIndex + 1]->getX(), controlPoints[movingPointIndex + 1]->getY(), curveIndex, false);	//erase old line on right
		setLine(controlPoints[movingPointIndex - 1]->getX(), controlPoints[movingPointIndex - 1]->getY(), new_x, new_y, curveIndex, true); //draw new line on left
		setLine(new_x, new_y, controlPoints[movingPointIndex + 1]->getX(), controlPoints[movingPointIndex + 1]->getY(), curveIndex, true); //draw new line on right
	}
}

void Curve::setLine(int xStart, int yStart, int xEnd, int yEnd, int curveIndex, bool on) {
	int dx = xEnd - xStart;
	int dy = yEnd - yStart;
	int steps;
	float xIncrement, yIncrement;
	float x = xStart;
	float y = yStart;

	if (fabs(dx) > fabs(dy)) {
		steps = fabs(dx);
	}
	else {
		steps = fabs(dy);
	}

	xIncrement = float(dx) / float(steps);
	yIncrement = float(dy) / float(steps);
	for (int k = 0; k < steps; k++) {
		x += xIncrement;
		y += yIncrement;

		int arrPos = Common::win_width * (int)round(x) + (int)round(y);

		if (Common::sceneBuffer[arrPos].getPoint() >= 0) {
			continue;
		}
		if (on) {
			Common::sceneBuffer[arrPos].setStatus(1);
			Common::sceneBuffer[arrPos].setCurve(curveIndex);
		}
		else {
			Common::sceneBuffer[arrPos].setStatus(0);
			Common::sceneBuffer[arrPos].setCurve(-1);
		}
		Common::sceneBuffer[arrPos].setPoint(-1);
	}
}

#endif