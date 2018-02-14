#ifndef BSplineCurve_H
#define BSplineCurve_H

#if defined (__APPLE__) || defined(MACOSX)
#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#endif

#include "Curve.h"
#include "Common.h"
#include "Point.h"
#include <vector>

class BSplineCurve : public Curve {
public:
	BSplineCurve(Curve c);
	void setBSpline(int curveIndex, bool on);
	Point getPoint(float t);
	float deBoor(float t, int k, int i);
	void setDegree(int degree);
	int getDegree();
private:
	int degree;
	std::vector<float> knots;
};

BSplineCurve::BSplineCurve(Curve c) : Curve(c) {
	degree = 2;
	for (int i = 0; i < controlPoints.size() + degree; i++) {
		knots.push_back(i);
	}
}

void BSplineCurve::setDegree(int degree) {
	this->degree = degree;
}

int BSplineCurve::getDegree() {
	return degree;
}

void BSplineCurve::setBSpline(int curveIndex, bool on) {
	std::vector<Point> curvePoints;
	for (float t = degree - 1; t <= controlPoints.size(); t += 0.1) {
		curvePoints.push_back(getPoint(t));
		if (t == degree - 1) continue;
		if (on) {
			setLine(curvePoints[curvePoints.size() - 2].getX(), curvePoints[curvePoints.size() - 2].getY(), 
			curvePoints[curvePoints.size() - 1].getX(), curvePoints[curvePoints.size() - 1].getY(), curveIndex, true);
		}
		else {
			setLine(curvePoints[curvePoints.size() - 2].getX(), curvePoints[curvePoints.size() - 2].getY(), 
			curvePoints[curvePoints.size() - 1].getX(), curvePoints[curvePoints.size() - 1].getY(), curveIndex, false);
		}
	}
}


Point BSplineCurve::getPoint(float t) {
	float x_sum = 0;
	float y_sum = 0;

	for (int i = 0; i < controlPoints.size(); i++) {
		x_sum += controlPoints[i]->getX() * deBoor(t, degree, i);
		y_sum += controlPoints[i]->getY() * deBoor(t, degree, i);
	}
	return Point(x_sum, y_sum);
}


float BSplineCurve::deBoor(float t, int k, int i) {
	if (k == 0) {
		if (t <= knots[i + 1] && t >= knots[i]) {
			return 1.0;
		}
		else {
			return 0.0;
		}
	}
	else {
		return (((t - knots[i]) / (knots[i + k] - knots[i])) * deBoor(t, k - 1, i)) + (((knots[i + k + 1] - t)/(knots[i + k + 1] - knots[i + 1])) * deBoor(t, k - 1, i + 1));
	}
}

#endif