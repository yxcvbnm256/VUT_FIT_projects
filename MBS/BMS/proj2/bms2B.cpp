/* 
 * File:   bms1B.cpp
 */

#include <cstdlib>
#include <math.h>
#include <cmath>
#include <iostream>
#include <algorithm>
#include <fstream>
#include "sndfile.hh"
#define AMPLITUDE (1.0 * 0x7F000000)
#define FREQ (1000.0)
using namespace std;

/* funkce ktera vytovri vystupni soubor. */
void makeTxt(std::string sequence, std::string filename) {
	std::ofstream outFile;
	outFile.open(filename.substr(0, filename.length() - 4) + ".txt");
	outFile << sequence << std::endl;
	outFile.close();	
}

/* funkce, ktera prida do demodulovane sekvence prislusne bity */
std::string addToSequence(int status, std::string seq) {
	switch (status) {
		case 0:
			seq.append("00");
			break;
		case 1:
			seq.append("01");
			break;
		case 2:
			seq.append("10");
			break;
		case 3: 
			seq.append("11");
			break;
	}
	return seq;
}

/* funkce, ktera dekoduje digitalni signal. */
bool decode(int *buffer, int sampleRate, int end, std::string filename) {
	std::string sequence("");
	//indikuje, ktery symbol se prave cte. 0 = 00, 1 = 01, 2 = 10, 3 = 11
	int status = 0;
	//vektor obsahujici informace ze synchronizacni frekvence - po kolika vzorcich zacal byt sinus posunuty.
	std::vector<int> shifts;
	//pocitana predpokladana hodnota vzorku
	int value = 0;
	//indikuje po kolika vzorcich zacina byt sinus posunty.
	int samplesPerChange = sampleRate;
	//promenna rikajici, zda je potreba otestovat dalsi vzorek, aby byla spravne detekovana urcita faze.
	bool needCheck = false;
	//promenna udavajici mod, kdy se cte synchronizacni sekvence.
	bool syncMode = true;
	//magicka konstanta, ktera ovlivnuje detekci dvou stejnych symbolu jdoucich po sobe - pri vzorkovani 2xxx b/s je nutne postupovat jinak
	int magicConstant = sampleRate/1000 == 2 ? 2 : 3;
	//promenna rikajici kolikrat se program snazil spocitat jinou fazi k urcitemu vzorku a detekovat tak zmenu faze
	int changes = 0;
	//cyklus, ve kterem se ke kazdemu vzorku program snazi dopocitat prepokladanou hodnotu, pokud se nezmenila faze.
	for (int i = 0; i < end; i++) {
		switch (status) {
			// 00
			case 0:
				value = AMPLITUDE * sin((FREQ/sampleRate) * 2 * M_PI * i + (3*M_PI/4));
				goto check;
			// 01
			case 1:
				value = AMPLITUDE * sin((FREQ/sampleRate) * 2 * M_PI * i + (M_PI/4));
				goto check;
			//10
			case 2:			
				value = AMPLITUDE * sin((FREQ/sampleRate) * 2 * M_PI * i + (5*M_PI/4));
				goto check;
			//11
			case 3:
				value = AMPLITUDE * sin((FREQ/sampleRate) * 2 * M_PI * i + (7*M_PI/4));
				goto check;
			//overeni, zda predpokladana vypocitana hodnota odpovida vzorku
			check:
			default:
				//pokud se faze zmenila vice nez 3x, program nenasel ke vzorku zadnou moznou fazi
				if (changes > 3) {
					std::cerr << "Nepodarilo se rozpoznat vzorky.\n";
					shifts.clear();
					return false;
				}
				//pokud vypocitana hodnota neodpovida vzorku, faze se posouva				
				if (abs(value - buffer[i]) > 1000) {
					//zaznamenani, po kolika vzorcich se zmenila faze
					if (changes == 0){
						if (shifts.size() == 0)
							shifts.push_back(i);
						else
							shifts.push_back(i - shifts.size()*shifts.back());
					}
					changes++;
					//opakovani vypoctu pro stejny vzorek, jen se pocita hodnota s jinou fazi				
					i--;
					status = (status + 1) % 4;
				}
				//pokud vypoctena hodnota odpovida vzorku
				else {
					//situace, kdy program jiz zmenil fazi a vzorek odpovida i v dalsich kroku - tim je zaruceno, ze byla vybrana spravna faze
					if (needCheck) {
						needCheck = false;
						sequence = addToSequence(status, sequence);
						if (syncMode == false) {
							i += samplesPerChange-magicConstant;
						}										
					}
					//situace, kdy se zmenila faze a program nasel fazi ke vzorku. je ale treba overit, zda odpovida i nasledujici hodnota s touto fazi a nasledujici vzorek
					else if (changes > 0) {
						needCheck = true;
						continue;
					}
					//pokud program cte teprve prvni symbol ze synchonizacni sekvence, nebo pokud jsou detekovany dva stejne symboly jdouci za sebou
					else if (i % samplesPerChange == magicConstant - 2){
						sequence = addToSequence(status, sequence);
						if (syncMode == false) {
							i += samplesPerChange-magicConstant;
						}
					}
					changes = 0;
					//konec synchonizacni faze					
					if (sequence == "00110011" && syncMode) {
						sequence.clear();
						//pokud se faze v synchonizacni sekvenci zmenila po stejnem poctu vzorku, muze program detekovat i nasledujici symboly
						if (shifts[0] == shifts[1] && shifts[1] == shifts[2]) {
							samplesPerChange = shifts[0];
							//je mozne se posunout k dalsi predpokladane zmene faze	
							i += samplesPerChange-magicConstant;				
						}
						else {
							std::cerr << "Synchronizace se nepodarila.\n";
							shifts.clear();
							return false;
						}
						syncMode = false;						
					}
				}
				break;
		}		
	}
	makeTxt(sequence, filename);
	shifts.clear();
	return true;
}

int main(int argc, char** argv) {
    if (argc != 2) {
    	std::cerr << "Nespravny pocet parametru.\n";
    	return EXIT_FAILURE;
    }
    SndfileHandle inputFile;
    inputFile = SndfileHandle(argv[1]);
    if (inputFile.error() != 0) {
    	std::cerr << "Spatny vstupni soubor.\n";
    	return EXIT_FAILURE;
    }
    int sampleRate;
    int *buffer;    
    sampleRate = inputFile.samplerate();  
    buffer = new int[inputFile.frames()];
    inputFile.read(buffer, inputFile.frames());
    int end;
    //cyklus ktery najde konec souboru v pripade, ze je doplnen nulovymi vzorky
    for (end = inputFile.frames()-1; end > 0; end--) {
    	if (buffer[end] != 0)
    		break;
    }
    if (!decode(buffer, sampleRate, end, argv[1])) {
    	delete [] buffer;
    	return EXIT_FAILURE;
    }    
    delete [] buffer;
    return EXIT_SUCCESS;
}

