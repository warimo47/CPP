#include "pch.h"
#include "GeoChange.h"

#include <cmath>

#include "opencv2/opencv.hpp"

TM GeoChange::GeotoTM(double Lat, double Log, double alt)
{
    double Q4 = 1; // 원점 축적 계수

    double Q1 = 6378137; // 장반경
    double Q3 = 6356752.3142; // 단반경

    double Q5 = 600000.0; // 원점가산값 x(N)
    double Q6 = 200000.0; // 원점가산값 y(E)

    double R_Lat = 38;  // 원점 위도
    double R_Log = 127; // 원점 경도

    double R7 = R_Lat / 180.0 * CV_PI; // 0.663225115757845; // 원점 위도 라디안
    double R8 = R_Log / 180.0 * CV_PI; //  2.21656815003280; // 원점 경도 라디안

    double Q9 = (pow(Q1, 2) - pow(Q3, 2)) / pow(Q1, 2);
    double Q10 = (pow(Q1, 2) - pow(Q3, 2)) / pow(Q3, 2);
    double Q11 = Q1 * ((1 - Q9 / 4.0 - 3.0 * pow(Q9, 2) / 64.0 - 5.0 * pow(Q9, 3) / 256.0) * R7 - (3.0 * Q9 / 8.0 + 3.0 * pow(Q9, 2) / 32.0 + 45.0 * pow(Q9, 3) / 1024.0) * sin(2.0 * R7) + (15.0 * pow(Q9, 2) / 256.0 + 45.0 * pow(Q9, 3) / 1024.0) * sin(4.0 * R7) - (35.0 * pow(Q9, 3) / 3072.0) * sin(6.0 * R7));

    double Q12 = (1 - sqrt(1 - Q9)) / (1 + sqrt(1 - Q9));

    double A7 = Lat / 180.0 * CV_PI;
    double B7 = Log / 180.0 * CV_PI;
    double C7 = pow(tan(A7), 2);
    double D7 = (Q9 / (1 - Q9)) * pow(cos(A7), 2);
    double E7 = (B7 - R8) * cos(A7);
    double F7 = Q1 / sqrt(1 - Q9 * pow(sin(A7), 2));
    double G7 = Q1 * ((1 - Q9 / 4.0 - 3.0 * pow(Q9, 2) / 64.0 - 5.0 * pow(Q9, 3) / 256.0) * A7 - (3.0 * Q9 / 8.0 + 3.0 * pow(Q9, 2) / 32.0 + 45.0 * pow(Q9, 3) / 1024.0) * sin(2.0 * A7) + (15.0 * pow(Q9, 2) / 256.0 + 45.0 * pow(Q9, 3) / 1024.0) * sin(4 * A7) - 35 * pow(Q9, 3) / 3072.0 * sin(6.0 * A7));

    double N = Q5 + Q4 * (G7 - Q11 + F7 * tan(A7) * (pow(E7, 2) / 2.0 + pow(E7, 4) / 24.0 * (5 - C7 + 9.0 * D7 + 4.0 * pow(D7, 2)) + pow(E7, 6) / 720.0 * (61.0 - 58.0 * C7 + pow(C7, 2) + 600.0 * D7 - 330.0 * Q10)));

    double E = Q6 + Q4 * F7 * (E7 + pow(E7, 3) / 6.0 * (1 - C7 + D7) + pow(E7, 5) / 120.0 * (5.0 - 18.0 * C7 + pow(C7, 2) + 72.0 * D7 - 58.0 * Q10));

    TM tm(E, N, alt);

    return tm;
}

UTMK GeoChange::GeotoUTMK(double Lat, double Log, double Alt)
{
    double Q4 = 0.9996;  // 원점축적계수 

    double Q5 = 2000000.0; // 원점가산값 x(N)
    double Q6 = 1000000.0;// 원점가산값 y(E)

    double Q1 = 6378137; // 장반경
    double Q3 = 6356752.3142;    // 단반경

    double R_Lat = 38;  // 원점 위도
    double R_Log = 127.5; // 원점 경도

    double R7 = R_Lat / 180.0 * CV_PI;    // 0.663225115757845;// 원점위도 라디안
    double R8 = R_Log / 180.0 * CV_PI;      //  2.21656815003280;    // 원점경도 라디안

    double Q9 = (pow(Q1, 2) - pow(Q3, 2)) / pow(Q1, 2);
    double Q10 = (pow(Q1, 2) - pow(Q3, 2)) / pow(Q3, 2);
    double Q11 = Q1 * ((1 - Q9 / 4.0 - 3.0 * pow(Q9, 2) / 64.0 - 5.0 * pow(Q9, 3) / 256.0) * R7 - (3.0 * Q9 / 8.0 + 3.0 * pow(Q9, 2) / 32.0 + 45.0 * pow(Q9, 3) / 1024.0) * sin(2.0 * R7) + (15.0 * pow(Q9, 2) / 256.0 + 45.0 * pow(Q9, 3) / 1024.0) * sin(4.0 * R7) - (35.0 * pow(Q9, 3) / 3072.0) * sin(6.0 * R7));

    double Q12 = (1 - sqrt(1 - Q9)) / (1 + sqrt(1 - Q9));

    double A7 = Lat / 180.0 * CV_PI;
    double B7 = Log / 180.0 * CV_PI;
    double C7 = pow(tan(A7), 2);
    double D7 = (Q9 / (1 - Q9)) * pow(cos(A7), 2);
    double E7 = (B7 - R8) * cos(A7);
    double F7 = Q1 / sqrt(1 - Q9 * pow(sin(A7), 2));
    double G7 = Q1 * ((1 - Q9 / 4.0 - 3.0 * pow(Q9, 2) / 64.0 - 5.0 * pow(Q9, 3) / 256.0) * A7 - (3.0 * Q9 / 8.0 + 3.0 * pow(Q9, 2) / 32.0 + 45.0 * pow(Q9, 3) / 1024.0) * sin(2.0 * A7) + (15.0 * pow(Q9, 2) / 256.0 + 45.0 * pow(Q9, 3) / 1024.0) * sin(4 * A7) - 35 * pow(Q9, 3) / 3072.0 * sin(6.0 * A7));

    double N = Q5 + Q4 * (G7 - Q11 + F7 * tan(A7) * (pow(E7, 2) / 2.0 + pow(E7, 4) / 24.0 * (5 - C7 + 9.0 * D7 + 4.0 * pow(D7, 2)) + pow(E7, 6) / 720.0 * (61 - 58.0 * C7 + pow(C7, 2) + 600.0 * D7 - 330.0 * Q10)));

    double E = Q6 + Q4 * F7 * (E7 + pow(E7, 3) / 6.0 * (1 - C7 + D7) + pow(E7, 5) / 120.0 * (5 - 18.0 * C7 + pow(C7, 2) + 72.0 * D7 - 58.0 * Q10));

    UTMK utmk(E, N, Alt);

    return utmk;
}

GEO GeoChange::UTMKtoGeo(double E, double N, double Z) // E = X , N = Y
{
    double Q4 = 0.9996;  // 원점축적계수 

    double Q5 = 2000000.0; // 원점가산값 x(N)
    double Q6 = 1000000.0;// 원점가산값 y(E)

    double Q1 = 6378137; // 장반경
    double Q3 = 6356752.3142;    // 단반경

    double R_Lat = 38;  // 원점 위도
    double R_Log = 127.5; // 원점 경도

    double R7 = R_Lat / 180.0 * CV_PI;    // 0.663225115757845;// 원점위도 라디안
    double R8 = R_Log / 180.0 * CV_PI;      //  2.21656815003280;    // 원점경도 라디안

    double Q9 = (pow(Q1, 2) - pow(Q3, 2)) / pow(Q1, 2);
    double Q10 = (pow(Q1, 2) - pow(Q3, 2)) / pow(Q3, 2);
    double Q11 = Q1 * ((1 - Q9 / 4.0 - 3.0 * pow(Q9, 2) / 64.0 - 5.0 * pow(Q9, 3) / 256.0) * R7 - (3.0 * Q9 / 8.0 + 3.0 * pow(Q9, 2) / 32.0 + 45.0 * pow(Q9, 3) / 1024.0) * sin(2.0 * R7) + (15.0 * pow(Q9, 2) / 256.0 + 45.0 * pow(Q9, 3) / 1024.0) * sin(4.0 * R7) - (35.0 * pow(Q9, 3) / 3072.0) * sin(6.0 * R7));

    double Q12 = (1 - sqrt(1 - Q9)) / (1 + sqrt(1 - Q9));

    double A7 = Q11 + ((N - Q5) / Q4);

    double B7 = A7 / (Q1 * (1 - Q9 / 4.0 - 3.0 * pow(Q9, 2) / 64.0 - 5.0 * pow(Q9, 3) / 256.0));

    double C7 = B7 + (3.0 * Q12 / 2.0 - 27.0 * pow(Q12, 3) / 32.0) * sin(2 * B7) + (21.0 * pow(Q12, 2) / 16.0 - 55.0 * pow(Q12, 4) / 32.0) * sin(4 * B7) + (151.0 * pow(Q12, 3) / 96.0) * sin(6.0 * B7) + (1097.0 * pow(Q12, 4) / 512.0) * sin(8.0 * B7);

    double D7 = (Q1 * (1 - Q9)) / (pow((1 - Q9 * pow(sin(C7), 2)), 3.0 / 2.0));

    double E7 = Q10 * pow(cos(C7), 2);

    double F7 = pow(tan(C7), 2);

    double G7 = Q1 / sqrt(1 - Q9 * pow(sin(C7), 2));
    double H7 = (E - Q6) / (G7 * Q4);

    double Lat = (C7 - (G7 * tan(C7) / D7) * (pow(H7, 2) / 2.0 - pow(H7, 4) / 24.0 * (5 + 3.0 * F7 + 10.0 * E7 - 4.0 * pow(E7, 2) - 9.0 * Q10) + pow(H7, 6) / 720.0 * (61 + 90.0 * F7 + 298.0 * E7 + 45.0 * pow(F7, 2) - 252 * Q10 - 3.0 * pow(E7, 2)))) * 180.0 / CV_PI;

    double Log = R_Log + ((1 / cos(C7)) * (H7 - (pow(H7, 3) / 6.0) * (1 + 2 * F7 + E7) + (pow(H7, 5) / 120.0) * (5 - 2.0 * E7 + 28.0 * F7 - 3.0 * pow(E7, 2) + 8.0 * Q10 + 24.0 * pow(F7, 2)))) * 180.0 / CV_PI;

    GEO geo(Lat, Log, Z);

    return geo;
}

GEO GeoChange::TMtoGeo(double E, double N, double Z)
{
    double Q4 = 1;  // 원점축적계수

    double Q1 = 6378137; // 장반경
    double Q3 = 6356752.3142;    // 단반경

    double Q5 = 600000.0; // 원점가산값 x(N)
    double Q6 = 200000.0;// 원점가산값 y(E)

    double R_Lat = 38;  // 원점 위도
    double R_Log = 127; // 원점 경도

    double R7 = R_Lat / 180.0 * CV_PI;    // 0.663225115757845;// 원점위도 라디안
    double R8 = R_Log / 180.0 * CV_PI;      //  2.21656815003280;    // 원점경도 라디안

    double Q9 = (pow(Q1, 2) - pow(Q3, 2)) / pow(Q1, 2);
    double Q10 = (pow(Q1, 2) - pow(Q3, 2)) / pow(Q3, 2);
    double Q11 = Q1 * ((1 - Q9 / 4.0 - 3.0 * pow(Q9, 2) / 64.0 - 5.0 * pow(Q9, 3) / 256.0) * R7 - (3.0 * Q9 / 8.0 + 3.0 * pow(Q9, 2) / 32.0 + 45.0 * pow(Q9, 3) / 1024.0) * sin(2.0 * R7) + (15.0 * pow(Q9, 2) / 256.0 + 45.0 * pow(Q9, 3) / 1024.0) * sin(4.0 * R7) - (35.0 * pow(Q9, 3) / 3072.0) * sin(6.0 * R7));

    double Q12 = (1 - sqrt(1 - Q9)) / (1 + sqrt(1 - Q9));

    double A7 = Q11 + ((N - Q5) / Q4);

    double B7 = A7 / (Q1 * (1 - Q9 / 4.0 - 3.0 * pow(Q9, 2) / 64.0 - 5.0 * pow(Q9, 3) / 256.0));

    double C7 = B7 + (3.0 * Q12 / 2.0 - 27.0 * pow(Q12, 3) / 32.0) * sin(2 * B7) + (21.0 * pow(Q12, 2) / 16.0 - 55.0 * pow(Q12, 4) / 32.0) * sin(4 * B7) + (151.0 * pow(Q12, 3) / 96.0) * sin(6.0 * B7) + (1097.0 * pow(Q12, 4) / 512.0) * sin(8.0 * B7);

    double D7 = (Q1 * (1 - Q9)) / (pow((1 - Q9 * pow(sin(C7), 2)), 3.0 / 2.0));

    double E7 = Q10 * pow(cos(C7), 2);

    double F7 = pow(tan(C7), 2);

    double G7 = Q1 / sqrt(1 - Q9 * pow(sin(C7), 2));
    double H7 = (E - Q6) / (G7 * Q4);

    double Lat = (C7 - (G7 * tan(C7) / D7) * (pow(H7, 2) / 2.0 - pow(H7, 4) / 24.0 * (5 + 3.0 * F7 + 10.0 * E7 - 4.0 * pow(E7, 2) - 9.0 * Q10) + pow(H7, 6) / 720.0 * (61 + 90.0 * F7 + 298.0 * E7 + 45.0 * pow(F7, 2) - 252 * Q10 - 3.0 * pow(E7, 2)))) * 180.0 / CV_PI;

    double Log = R_Log + ((1 / cos(C7)) * (H7 - (pow(H7, 3) / 6.0) * (1 + 2 * F7 + E7) + (pow(H7, 5) / 120.0) * (5 - 2.0 * E7 + 28.0 * F7 - 3.0 * pow(E7, 2) + 8.0 * Q10 + 24.0 * pow(F7, 2)))) * 180.0 / CV_PI;

    GEO geo(Lat, Log, Z);

    return geo;
}