#include "BTS.h"
#include "CSV.h"
#include <iostream>

int main(int argc, char* argv[])
{
	CSVParser parser;
	if (argc != 2) {
		std::cerr << "Spatny pocet argumentu.\n";
		return 1;
	}
	//parse the input file
	if (!parser.ParseCSV(argv[1], true)) {
		std::cerr << "Soubor se vstupem spatne zadan.\n";
		return 1;
	}
	//parse the bts.csv file
	if (!parser.ParseCSV(NULL, false)) {
		std::cerr << "Soubor bts.csv se nenachazi v aktualni slozce.\n";
		return 1;
	}
	//erase redundant BTS records
	if (!parser.ReduceBTS()) {
		std::cerr << "Nedostatecny pocet ruznych vysilacich stanic pro vypocet lokalizace (minimum: tri).\n";
		return 1;
	}
	double utmX, utmY, latitude, longitude;
	//get intersection of three BTSs
	parser.Trilateration(utmX, utmY);
	//convert computed UTM coordinates to GPS longitude and latitude. Used reusable code Used reusable code from http://alephnull.net/software/gis/UTM_WGS84_C_plus_plus.shtml
	UTMXYToLatLon (utmX, utmY, parser.utmZone, false, latitude, longitude);
	latitude = RadToDeg(latitude);
	longitude = RadToDeg(longitude);
	parser.MakeOutput(latitude, longitude);
	return 0;
}