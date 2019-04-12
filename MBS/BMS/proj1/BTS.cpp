#include "BTS.h"

BTS::BTS()
{	
}

/* Constructor that creates a base for one BTS */
BTS::BTS(int cidr, int rssi, int signal, int antenaH, int power)
{
	this->cidr = cidr;
	this->rssi = rssi;
	this->signal = signal;
	this->antenaH = antenaH;
	this->power = power;
}

/* Method that parses the coordinates of the BTS to degrees */
void BTS::ParseCoordinates() {
	std::string degrees;
	std::string minutes;
	std::string seconds;
	std::string latitude;
	std::string longitude;
	std::istringstream f(this->gps);
	bool sign = false;
    getline(f, latitude, ',');
    getline(f, longitude, '\0');    
	int i = 0;
	//parse latitude
	if (latitude.at(i) == '-') {
		sign = true;
		i++;
	}
	while (isdigit(latitude.at(i))) {
		degrees += latitude.at(i);		
		i++;
	}
	i++;
	while (isdigit(latitude.at(i))) {
		minutes += latitude.at(i);		
		i++;
	}
	i++;
	while (isdigit(latitude.at(i)) || latitude.at(i) == '.') {
		seconds += latitude.at(i);		
		i++;
	}
	if (latitude.at(i) == 'S') {
		sign = true;
	}
	double degreeF = std::stod(degrees, NULL);
	double minuteF = std::stod(minutes, NULL);
	double secondF = std::stod(seconds, NULL);
	this->gpsLat = degreeF + (minuteF/60) + (secondF/3600);
	//parse longitude
	if (sign) {
		this->gpsLat = this->gpsLat * -1;
	}
	degrees = minutes = seconds = std::string();
	i = 0;
	sign = false;
	if (longitude.at(i) == '-') {
		sign = true;
		i++;
	}
	while (isdigit(longitude.at(i))) {
		degrees += longitude.at(i);		
		i++;
	}
	i++;
	while (isdigit(longitude.at(i))) {
		minutes += longitude.at(i);		
		i++;
	}
	i++;
	while (isdigit(longitude.at(i)) || longitude.at(i) == '.') {
		seconds += longitude.at(i);		
		i++;
	}
	if (longitude.at(i) == 'W') {
		sign = true;
	}
	degreeF = std::stod(degrees, NULL);
	minuteF = std::stod(minutes, NULL);
	secondF = std::stod(seconds, NULL);
	this->gpsLong = degreeF + (minuteF/60) + (secondF/3600);
	if (sign) {
		this->gpsLong = this->gpsLong * -1;
	}
	//parse coordinates to UTM
	this->LatLongToUTM();
}

/* Method that converts latitude and longitude to UTM. Used reusable code from http://alephnull.net/software/gis/UTM_WGS84_C_plus_plus.shtml */
void BTS::LatLongToUTM() {
	double latRad = DegToRad(this->gpsLat);
	double longRad = DegToRad(this->gpsLong);
	this->utmZone = LatLonToUTMXY(this->gpsLat, this->gpsLong, 0, this->utmX, this->utmY);
	this->GetDistance();
}

/* Method that computes a distance from BTS according to Hata model */
double BTS::GetDistance() {
	double lu = (10*log10(this->power) + 30) - this->signal;
	int frequency = 900;
	double mobileHeight = 1.2;
	double ch = 0.8 + (1.1*log10(frequency) - 0.7) * mobileHeight - 1.56*log10(frequency);
	this->distance = (pow(10, ((lu - 69.55 - 26.16*log10(frequency) + 13.82*log10(this->antenaH) + ch)/(44.9 - 6.55*log10(this->antenaH)))))*1000;
}
