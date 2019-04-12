#ifndef _CSV_H
#define _CSV_H
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <iterator>
#include <boost/tokenizer.hpp>
#include "BTS.h"
#include <memory>
#include <locale>

/* Class that represents a CSV parser operating over BTSs. */
class CSVParser {
private:
	
public:
	std::vector<std::unique_ptr<BTS>> bts;
	int utmZone;
	~CSVParser();	
    bool ParseCSV(char* filename, bool argument);
    bool ReduceBTS();
    void Trilateration(double& finalX, double& finalY);
    void MakeOutput(double x, double y);
};

#endif // _CSV_H