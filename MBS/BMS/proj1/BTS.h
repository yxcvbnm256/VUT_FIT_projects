#ifndef _BTS_H
#define _BTS_H
#include <vector>
#include <stdlib.h>
#include <cctype>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <cmath>
#include "UTM.h"

/* Class that represents one BTS. */
class BTS {
private:	
public:
	int cidr;
	int rssi;
	int signal;
	int antenaH;
	int power;
	std::string gps;
	double utmX;
	double utmY;
	int utmZone;
	double gpsLat;
	double gpsLong;
	double distance;
	BTS();
	BTS(int, int, int, int, int);
	void ParseCoordinates();
	void LatLongToUTM();
	double GetDistance();
};

#endif // _BTS_H