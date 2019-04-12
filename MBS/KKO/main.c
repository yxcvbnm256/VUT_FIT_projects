/** bwted - program for encoding and decoding text input. Uses the bwted.c library
* author - Jan Pawlus, xpawlu00
* created 4. 5. 2018
*/

#include "bwted.h"

using namespace std;

void printHelp() {
	cout << "bwted - program for encoding/decoding a text input via BWT, MTF, RLE and Huffman. Usage:" << endl;
    cout << "bwted -i [inputFile] -o [outputFile] -l [logFile] -c -x -h" << endl;
    cout << "   -i [inputFile] - specifies input file. If not given, program will read stdin\n";
    cout << "   -o [OutputFile - specifies output file. If not given, output will be put to stdout.\n";
    cout << "   -l [logFile] - specifies log file. If not given, log will be ignored.\n";
    cout << "   -c - program will be encoding.\n";
    cout << "   -x - program will be decoding.\n";
    cout << "   -h - program will print this help.\n";
}


int main(int argc, char* argv[]) {
	string inputFile = "";
	string outputFile = "";
	string logFile = "";
	int c;
	bool decode = false;
	bool encode = false;
	while ((c = getopt(argc, argv, "i:o:xcl:h")) != -1) {
        switch (c) {
            case 'i':
                inputFile = std::string(optarg);
                break;     
            case 'o':
                outputFile = std::string(optarg);
                break;  
            case 'c':
                encode = true;
                break;  
            case 'x':
                decode = true;
                break; 
            case 'l':
            	logFile = std::string(optarg);
            	break;
           	case 'h':
           		printHelp();
           		return 0;
           		break;
            default:
                std::cerr << "Unknown option\n";
                return -1;
        }
    }
    int64_t outVal=0;
    tBWTED logBWT;
    logBWT.codedSize=-1;
    logBWT.uncodedSize=-1;
    FILE *inpFile, *outpFile;
    if (decode == encode)
        return 0;

    //input file
    if(!inputFile.empty()){
        inpFile = fopen(inputFile.c_str(), "rb");
        if(inpFile == NULL){
            return -3;
        }
    }
    else
        inpFile=NULL;

    //output file
    if(!outputFile.empty()){
        outpFile = fopen(outputFile.c_str(), "wb");
        if(outpFile == NULL){
            return -4;
        }
    }
    else
        outpFile=NULL;
    if(encode)
        outVal = BWTEncoding(&logBWT, inpFile, outpFile);
    else
        outVal = BWTDecoding(&logBWT, inpFile, outpFile);

    if (!logFile.empty()){
    	string log = (string("login = xpawlu00") + '\n' + string("uncodedSize = ") + to_string(logBWT.uncodedSize) + '\n' + string("codedSize = ") + to_string(logBWT.codedSize) + '\n');
    	FILE *lFile = fopen(logFile.c_str(), "wb");
    	if (lFile == NULL) {
    		return -5;
    	}
    	putOutput(lFile, log);
    }
	return outVal;
}

