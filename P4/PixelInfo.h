#ifndef PixelInfo_H
#define PixelInfo_H
#include <iostream>
class PixelInfo {
public:
	PixelInfo(int curveIndex, int status, int pointIndex) {
		this->curveIndex = curveIndex;
		this->status = status;
		this->pointIndex = pointIndex;
	}
	PixelInfo() {
		curveIndex = -1;
		status = 0;
		pointIndex = -1;
	}
	void setCurve(int index) {
		curveIndex = index;
	}
	int getCurve() {
		return curveIndex;
	}
	void setStatus(int status) {
		this->status = status;
	}
	int getStatus() {
		return status;
	}
	void setPoint(int pointIndex) {
		this->pointIndex = pointIndex;
	}
	int getPoint() {
		return pointIndex;
	}
private:
	int curveIndex; //denotes which curve this pixel is linked to
	int status; //0 or 1 meaning pixel off or on
	int pointIndex; //
};

#endif