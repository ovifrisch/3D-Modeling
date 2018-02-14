#ifndef BezierCurve_H
#define BezierCurve_H

#if defined (__APPLE__) || defined(MACOSX)
#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#endif

#include "Curve.h"
#include "Common.h"
#include "Point.h"
#include <vector>

class BezierCurve : public Curve {
public:
	BezierCurve(Curve c);
	void setBezier(int curveIndex, bool on);
private:
	Point getPoint(float t, std::vector<Point> points);

};

BezierCurve::BezierCurve(Curve c) : Curve(c) {}

void BezierCurve::setBezier(int curveIndex, bool on) {
	std::vector<Point> cPoints;
	for (int i = 0; i < controlPoints.size(); i++) {
		cPoints.push_back(*controlPoints[i]);
	}

	std::vector<Point> curvePoints;
	for (float t = 0; t <= 1; t += 0.01) {
		curvePoints.push_back(getPoint(t, cPoints));
		if (t == 0) continue;
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

Point BezierCurve::getPoint(float t, std::vector<Point> points) {
	if (points.size() == 1) {
		return Point(points[0].getX(), points[0].getY());
	}

	std::vector<Point> newPoints;
	for (int i = 0; i < points.size() - 1; i++) {
		int x = (int)(((1 - t) * points[i].getX()) + (t * points[i + 1].getX()));
		int y = (int)(((1 - t) * points[i].getY()) + (t * points[i + 1].getY()));
		newPoints.push_back(Point(x, y));
	}
	return getPoint(t, newPoints);
}

#endif