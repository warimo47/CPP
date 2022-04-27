#include "pch.h"
#include "LocationEstimation.h"
#include "GeoChange.h"

LocationEstimation::LocationEstimation()
{
	pptz = nullptr;

	for (int thc = 0/* Thread count */; thc < 4; ++thc)
	{
		offsetX[thc] = 0.0;
		offsetY[thc] = 0.0;
		offsetZ[thc] = 0.0;
		cameraHeight[thc] = 0.0;
	}
}

LocationEstimation::~LocationEstimation()
{

}

void LocationEstimation::GetWorldPosition(
	int _thn, // Thread num
	int u, int v, // Viewport UV coordinate
	double fx, double fy, double cx, double cy, // Camera intrinsic parameters
	double* r,
	double xDegree,
	double* _lp,
	double* _geo)
{
	// 선택된 uv 좌표를 카메라 좌표계 벡터로 변환
	double cn[] = { (static_cast<double>(u) - cx) / fx, (static_cast<double>(v) - cy) / fy, 1.0 };

	double t[] = { 0.0, sin(xDegree * CV_PI / 180.0) * cameraHeight[_thn], -cos(xDegree * CV_PI / 180.0) * cameraHeight[_thn] };

	// 카메라 설치 높이(Z) ÷ 카메라 좌표계 벡터 높이(Z)
	double s = (r[2] * t[0] + r[5] * t[1] + r[8] * t[2]) / (r[2] * cn[0] + r[5] * cn[1] + r[8] * cn[2]);

	// 월드 좌표계 Ray 방향 벡터
	double ray[3];
	Mul3_1X3_3(cn, r, ray);

	// 월드 좌표계 카메라 좌표
	double zeroPoint[3];
	Mul3_1X3_3(t, r, zeroPoint);

	// 월드 좌표 값
	_lp[0] = ray[0] * s - zeroPoint[0];
	_lp[1] = ray[1] * s - zeroPoint[1];
	_lp[2] = ray[2] * s - zeroPoint[2];

	double panRadian = (static_cast<double>(*pptz[0]) / 100.0 + offsetY[_thn]) * CV_PI / 180.0;
	double localARX = cos(panRadian) * _lp[0] + sin(panRadian) * _lp[1];
	double localARZ = -sin(panRadian) * _lp[0] + cos(panRadian) * _lp[1];

	double tmEast = cctvTMEast[_thn] + localARX;
	double tmNorth = cctvTMNorth[_thn] + localARZ;

	GEO geo = geoChange.TMtoGeo(tmEast, tmNorth, _lp[2]);

	_geo[0] = geo.lat;
	_geo[1] = geo.log;
	_geo[2] = geo.alt;
}

void LocationEstimation::ApplyOffSet(
	int _thn, // Thread num
	double* _xAxisD, // X axis degree
	double* _yAxisD, // Y axis degree
	double* _zAxisD) // Z axis degree
{
	*_xAxisD = (18000.0 - static_cast<double>(pptz[_thn][1])) / 100.0 + offsetX[_thn];
	*_yAxisD = static_cast<double>(pptz[_thn][0]) / 100.0 + offsetY[_thn];
	*_zAxisD = offsetZ[_thn];
}

void LocationEstimation::GetRorationMatrix(double x, double y, double z, double* rm)
{
	double thetaX = x * CV_PI / 180.0;
	double thetaY = y * CV_PI / 180.0;
	double thetaZ = z * CV_PI / 180.0;

	double AxisXR[] = { 1.0, 0.0, 0.0,
						0.0, cos(thetaX), -sin(thetaX),
						0.0, sin(thetaX), cos(thetaX) };

	double AxisYR[] = { cos(thetaY), 0.0, -sin(thetaY),
						0.0, 1.0, 0.0,
						sin(thetaY), 0.0, cos(thetaY) };

	double AxisZR[] = { cos(thetaZ), -sin(thetaZ), 0.0,
						sin(thetaZ), cos(thetaZ), 0.0,
						0.0, 0.0, 1.0 };

	double AxisXYR[9];
	Mul3_3X3_3(AxisXR, AxisYR, AxisXYR);
	Mul3_3X3_3(AxisXYR, AxisZR, rm);
}

void LocationEstimation::Mul3_3X3_3(double* f, double* b, double* result)
{
	result[0] = f[0] * b[0] + f[1] * b[3] + f[2] * b[6];
	result[1] = f[0] * b[1] + f[1] * b[4] + f[2] * b[7];
	result[2] = f[0] * b[2] + f[1] * b[5] + f[2] * b[8];
	result[3] = f[3] * b[0] + f[4] * b[3] + f[5] * b[6];
	result[4] = f[3] * b[1] + f[4] * b[4] + f[5] * b[7];
	result[5] = f[3] * b[2] + f[4] * b[5] + f[5] * b[8];
	result[6] = f[6] * b[0] + f[7] * b[3] + f[8] * b[6];
	result[7] = f[6] * b[1] + f[7] * b[4] + f[8] * b[7];
	result[8] = f[6] * b[2] + f[7] * b[5] + f[8] * b[8];
}

void LocationEstimation::Mul3_3X3_1(double* f, double* b, double* result)
{
	result[0] = f[0] * b[0] + f[1] * b[1] + f[2] * b[2];
	result[1] = f[3] * b[0] + f[4] * b[1] + f[5] * b[2];
	result[2] = f[6] * b[0] + f[7] * b[1] + f[8] * b[2];
}

void LocationEstimation::Mul3_1X3_3(double* f, double* b, double* result)
{
	result[0] = f[0] * b[0] + f[1] * b[3] + f[2] * b[6];
	result[1] = f[0] * b[1] + f[1] * b[4] + f[2] * b[7];
	result[2] = f[0] * b[2] + f[1] * b[5] + f[2] * b[8];
}