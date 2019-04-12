#include "CSV.h"

CSVParser::~CSVParser(){
	(this->bts).clear();
}

/* Method that parses the CSV files and makes the BTS list structure. Parameter filename - input csv, parameter argument - flag that says whether the input csv or bts.csv is being parsed */
bool CSVParser::ParseCSV(char* filename, bool argument) {
	using namespace boost;
	std::string data((argument) ? filename : "bts.csv");
	std::ifstream in(data.c_str());	
	if (in.fail() || !in.is_open() || !in.good()) {
		return false;
	}	
	typedef tokenizer< escaped_list_separator<char> > Tokenizer;
	escaped_list_separator<char> sep('\\', ';', '\"');
	std::vector<std::string> vec;
	std::string line;
	std::string buffer;
	bool inside_quotes(false);
	size_t last_quote(0);
	while (getline(in,buffer))
	{
		//line breaks in quoted string
		last_quote = buffer.find_first_of('"');
		while (last_quote != std::string::npos)
		{
			inside_quotes = !inside_quotes;
			last_quote = buffer.find_first_of('"',last_quote+1);
		}
		line.append(buffer);
		if (inside_quotes)
		{
			line.append("\n");
			continue;
		}
		Tokenizer tok(line, sep);
		vec.assign(tok.begin(),tok.end());
		line.clear();
		//skip the header row of the file
		if (vec[0] == "LAC" || vec[0] == "CID") {
			continue;
		}
		//if input file is being parsed, add row to the BTS vector
		if (argument) {
			std::unique_ptr<BTS> bts = std::make_unique<BTS>(std::stoi(vec[1], nullptr), std::stoi(vec[2], nullptr), std::stoi(vec[3], nullptr), std::stoi(vec[4], nullptr), std::stoi(vec[5], nullptr));
			(this->bts).push_back(move(bts));
		}
		//if bts.csv is being parsed, search for the row in the BTS vector from the input file (according to cidr) and add the GPS coordinates
		else {
			int cidr = stoi(vec[0],nullptr);
			auto it = find_if((this->bts).begin(), (this->bts).end(), [&cidr](const std::unique_ptr<BTS>& obj) {return obj->cidr == cidr;});
			if (it != (this->bts).end()) {				
				(*it)->gps = vec[4];
				//parse the coordinates to double
				(*it)->ParseCoordinates();
			}
		}		
	}
	in.close();
	return true;
}

/* Method that gets rid of useless BTS in the vector. If there are more rows with the same location, keep just the one with the strongest signal. Then sort it via signal */
bool CSVParser::ReduceBTS() {
	if (this->bts.size() < 3) {
		return false;
	}
	sort(this->bts.begin(), this->bts.end(), [](const std::unique_ptr<BTS>& bts1, const std::unique_ptr<BTS>& bts2) {
		return (bts1->gps == bts2->gps) ? bts1->signal > bts2->signal : bts1->gps < bts2->gps;
	});
	std::unique_ptr<BTS>& current = this->bts[0];
	this->utmZone = current->utmZone;
	int max = current->cidr;
	for(auto it = (this->bts).begin(); it != (this->bts).end(); it) {
		if (it == (this->bts).begin()) {
			it++;
			continue;
		}
		if ((*(it - 1))->gps.compare((*it)->gps) == 0) {			
			this->bts.erase(it);
			if (it == (this->bts).end()) {
				break;
			}
			else {
				continue;
			}
		}
		it++;
	}
	sort(this->bts.begin(), this->bts.end(), [](const std::unique_ptr<BTS>& bts1, const std::unique_ptr<BTS>& bts2) {
		return bts1->signal > bts2->signal;
	});
	if (this->bts.size() < 3) {
		return false;
	}
	return true;
}

/* Trilateration algorithm for getting an intersection of three circles (three BTSs).*/
void CSVParser::Trilateration(double& finalX, double& finalY) {
	double exx, exy, eyy, eyx, ey, i, j, d, x, y, g;
	std::unique_ptr<BTS>& p1 = this->bts[0];
	std::unique_ptr<BTS>& p2 = this->bts[1];
	std::unique_ptr<BTS>& p3 = this->bts[2];
	d = sqrt(pow(p2->utmX - p1->utmX, 2) + pow(p2->utmY - p1->utmY, 2));
	g = sqrt(pow(p3->utmX - p1->utmX - i*exx, 2) + pow(p3->utmY - p1->utmY - i*exx, 2));
	exx = (p2->utmX - p1->utmX)/d;
	exy = (p2->utmY - p1->utmY)/d;
	i = exx*(p3->utmX - p1->utmX) + exy*(p3->utmY - p1->utmY);
	eyx = (p3->utmX - p1->utmX - i*exx)/g;
	eyy = (p3->utmY - p1->utmY - i*exy)/g;	
	j = eyx*(p3->utmX - p1->utmX) + eyy*(p3->utmY - p1->utmY);
	x = (pow(p1->distance, 2) - pow(p2->distance, 2) + pow(d, 2))/(2*d);
	y = (pow(p1->distance, 2) - pow(p3->distance, 2) + pow(i,2) + pow(j, 2))/((2*j) - (i*x)/j);
	finalX = p1->utmX + (x*exx) + (y*eyx);
	finalY = p1->utmY + (x*exy) + (y*eyy);
}

/* Makes an output to the file */
void CSVParser::MakeOutput(double x, double y) {
	std::ofstream outFile;
	outFile.open("out.txt");
	outFile << "maps.google.com/maps?q=" << std::setprecision(8) << x << "," << y << std::endl;
	outFile.close();
}
