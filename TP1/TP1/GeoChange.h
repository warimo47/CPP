#pragma once

struct TM
{
    double X; // East
    double Y; // North
    double Z;

    TM(double x, double y, double z)
    {
        X = x;
        Y = y;
        Z = z;
    }
};

struct UTMK
{
    double X; // East
    double Y; // North
    double Z;

    UTMK(double x, double y, double z)
    {
        X = x;
        Y = y;
        Z = z;
    }
};

struct GEO
{
    double lat; // 위도
    double log; // 경도
    double alt;

    GEO(double Lat, double Log, double Alt)
    {
        lat = Lat;
        log = Log;
        alt = Alt;
    }
};

class GeoChange
{
public:
    TM GeotoTM(double Lat, double Log, double alt);

    UTMK GeotoUTMK(double Lat, double Log, double Alt);

    GEO UTMKtoGeo(double E, double N, double Z);

    GEO TMtoGeo(double E, double N, double Z);
};