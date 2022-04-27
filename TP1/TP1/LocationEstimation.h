#pragma once

#include <atomic>

#include "opencv2/opencv.hpp"

#include "GeoChange.h"

class LocationEstimation
{
private:
	
protected:

public:
	std::atomic<int>(*pptz)[3];

	GeoChange geoChange;

	double offsetX[4];
	double offsetY[4];
	double offsetZ[4];
	double latitude[4];
	double longitude[4];
	double cctvTMNorth[4];
	double cctvTMEast[4];
	double cameraHeight[4];

private:

protected:

public:
	LocationEstimation();
	~LocationEstimation();

	void GetWorldPosition(int _thn, int u, int v, double fx, double fy, double cx, double cy, double* r, double xDegree, double* _lp, double* _geo);
	void ApplyOffSet(int _thn, double* _xAxisD, double* _yAxisD, double* _zAxisD);
	void GetRorationMatrix(double x, double y, double z, double* rm);
	void Mul3_3X3_3(double* f, double* b, double* result);
	void Mul3_3X3_1(double* f, double* b, double* result);
	void Mul3_1X3_3(double* f, double* b, double* result);
};

