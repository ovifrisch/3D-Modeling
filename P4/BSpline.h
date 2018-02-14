#ifndef BSpline_H
#define BSpline_H

#if defined (__APPLE__) || defined(MACOSX)
#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#endif

#include "Curve.h"
#include "Common.h"
#include "Point.h"
#include <vector>

class BSpline : public Curve {
public:
	BezierCurve(Curve c);
	void drawBezier();
};


#endif