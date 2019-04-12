/* 
 * File:   bms1A.cpp
 */
#include <fstream>
#include <cstdlib>
#include <math.h>
#include <iostream>
#include "sndfile.hh"

#define SAMPLE_RATE 18000
#define CHANELS 1
#define FORMAT (SF_FORMAT_WAV | SF_FORMAT_PCM_24)
#define AMPLITUDE (1.0 * 0x7F000000)
#define FREQ (1000.0 / SAMPLE_RATE)

/* funkce parsujici argumenty */
std::string argParse(int argc, char** argv) {
	std::string line;
	std::string data;
    if (argc != 2) {
    	std::cerr << "Nespravny pocet parametru.\n";
    	return "";
    }
    std::ifstream in(argv[1]);
    if (in.fail() || !in.is_open() || !in.good()) {
    	std::cerr << "Nespravny vstupni soubor.\n";
		return "";
	}
	while (getline(in, line)) {
		data.append(line);
	}
	if (data.length() % 2 != 0) {
		std::cerr << "Vstupni sekvence musi byt suda.\n";
		return "";
	}
	return data;
}


/* funkce, ktera vytvori vystupni wav soubor. */
void makeWav(int *buffer, unsigned size, std::string filename) {
	SndfileHandle outputFile;
	outputFile = SndfileHandle(filename.substr(0, filename.length() - 4) + ".wav", SFM_WRITE, FORMAT, CHANELS, SAMPLE_RATE);
	outputFile.write(buffer, size);
}

/* funkce, ktera provede modulaci. */
bool makeQPSK(std::string sequence, std::string filename) {
	unsigned i = 0;
	int g = 0;
	unsigned bit = 0;
	//ocekavany pocet vzorku
	unsigned size = (sequence.size()/2)*(SAMPLE_RATE/1000);
	int *buffer = new int[size];
	std::string symbol;
	do {
		symbol = sequence.substr(i * 2, 2);
		bit = std::stoi(symbol, NULL);
		//pro kazdy symbol jinak pocitana faze sinu. jedna perioda = jeden symbol
		switch(bit) {
			case 0:
				for (int j = 0; j < SAMPLE_RATE/1000; j++) {					
					buffer [i*(SAMPLE_RATE/1000) + j] = AMPLITUDE * sin(FREQ * 2 * M_PI * g + (3*M_PI/4));
					g++;
				}				
				break;
			case 1:
				for (int j = 0; j < SAMPLE_RATE/1000; j++) {					
					buffer [i*(SAMPLE_RATE/1000) + j] = AMPLITUDE * sin(FREQ * 2 * M_PI * g + (M_PI/4));
					g++;
				}
				break;
			case 10:
				for (int j = 0; j < SAMPLE_RATE/1000; j++) {					
					buffer [i*(SAMPLE_RATE/1000) + j] = AMPLITUDE * sin(FREQ * 2 * M_PI * g + (5*M_PI/4));
					g++;
				}
				break;
			case 11:
				for (int j = 0; j < SAMPLE_RATE/1000; j++) {					
					buffer [i*(SAMPLE_RATE/1000) + j] = AMPLITUDE * sin(FREQ * 2 * M_PI * g + (7*M_PI/4));
					g++;
				}
				break;
			default:
				break;
		}		
		i++;
	} while(i* 2 < sequence.size());
	makeWav(buffer, size, filename);
	delete [] buffer;
	return true;
}

int main(int argc, char** argv) {
	std::string sequence("00110011");
    SndfileHandle outputFile;    
    std::string data = argParse(argc, argv);
    if (data.empty()) {
    	return EXIT_FAILURE;
    }
    sequence.append(data);
    makeQPSK(sequence, std::string(argv[1]));    
    return EXIT_SUCCESS;
}


