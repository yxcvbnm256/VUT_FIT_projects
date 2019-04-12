/** bwted.c - implementation of library bwted for encoding/decoding text file using BWT, MTE, RLE and Huffman.
* author - Jan Pawlus, xpawlu00
* created 4. 5. 2018
*/

#include "bwted.h"

using namespace std;

/**
Encodes the given text with Burrows-Wheeler transformation.
	@param input - input string that should be encoded
	@return output - the text transformed with BWT
*/
string BWTencode(string input) {
	string output;
	set<string> matrix;
	for (u_int64_t i = 1; i <= input.length(); i++) {

		rotate(input.begin(), input.end()-1, input.end());
		matrix.insert(input);
	}
	for(auto f : matrix) {
  		output.push_back(f.back());
	}   
	return output;
}

/**
Decodes text encoded in BTW to its original form.
	@param input - input string in BWT
	@param eof - a character that is used as a delimiter
	@return output - original text
*/
string BWTdecode(string input, char eof) {
	vector<tuple<char,int>> tempVec;
	for (u_int64_t i = 0; i < input.length(); i++) {
		tuple<char,int> tpl(input[i], i);
		tempVec.push_back(tpl);
	}
	vector<tuple<char, int>>sortedVec = tempVec;
	stable_sort(sortedVec.begin(), sortedVec.end(), 
    [](tuple<char, int> const &t1, tuple<char, int> const &t2) {
        return get<0>(t1) < get<0>(t2);
    });
    vector<u_int32_t> pomArray(input.length());
    for(u_int32_t i=0; i<input.size(); i++){
            pomArray[get<1>(sortedVec[i])]=i;
    }
    tuple<char, int>pointingTpl = tempVec[0];
    string output = "";
    for(u_int32_t i=0; i < input.size(); i++){
    	output.insert(0,1,get<0>(pointingTpl));
    	pointingTpl = tempVec[pomArray[get<1>(pointingTpl)]];
    }
    int i = 0;
    while(output.back() != eof) {
    	i++;
    	rotate(output.begin(), output.end()-1, output.end());
    }
    output.pop_back();
    return output;
}

/**
Function that initializes all the alphabet we can get from ASCII char.
	@return alphabet - alphabet represented as a string
*/
string initAlphabet() {
	string alphabet;
	for(int i=0; i<254; i++){
       	alphabet.push_back((char)i);
    }
    return alphabet;
	
}

/**
Gets an input from specified file or stdin
	@param inputFile - pointer to a file
	@return input - the input from the file or stdin
*/
string getInput(FILE *inputFile) {
    string input;
    int32_t c;
    if(inputFile != NULL) {
        fseek(inputFile, 0, SEEK_END);
        int32_t fileLen=ftell(inputFile);
        fseek(inputFile, 0, SEEK_SET);
        for(int32_t i=0; i<fileLen; i++){
            input.push_back(fgetc(inputFile));
        }
        fclose(inputFile);       
    }
    //if no file specified, read stdin
    else {
        while(true){
            c=getchar();
            if(c==EOF)
                break;
            input.push_back(c);
        };
    }
    return input;
}

/**
Function that initializes the numerical alphabet that is created after MTF and RLE encoding. It counts occurences of each number in the whole RLE encoded input.
	@param input - vector of integers representing the RLE encoding
	@return alphabet - vector of tuples - each representing occurence count of one symbol and the symbol.
*/
vector<tuple<int,int>> initNumAlphabet(vector<u_int32_t> input) {
	vector<tuple<int, int>> alphabet;
	alphabet.push_back(tuple<int,int>(-1, input.size()));
	vector<u_int32_t> output;
	for (u_int64_t i = 0; i < input.size(); i++) {
		int tmp = input[i];
		//check if a number already is in the alphabet
		auto it = find_if(begin(alphabet), end(alphabet), 
    		[&tmp](const tuple<int,int>& e) { return get<1>(e) == tmp; });
		//if not, push a new symbol
		if (it == alphabet.end()) {
			alphabet.push_back(tuple<int,int>(1,input[i]));
		}
		//else increase the occurence
		else {
			get<0>(*it)++;
		}
	}
	//sort via occurences
	sort(alphabet.begin(), alphabet.end(), 
    [](tuple<int, int> const &t1, tuple<int, int> const &t2) {
        return get<0>(t1) < get<0>(t2); // or use a custom compare function
    });
	return alphabet;
}

/**
Encodes the BWT text to MTF vector if integers. 
	@param input - input BWT string that should be encoded
	@param alphabet - ASCII alphabet
	@return output - MTF encoded vector of numbers representing the BWT text
*/
vector<u_int32_t> MTFCode(string input, string alphabet) {
	vector<u_int32_t> output;
	for (u_int64_t i = 0; i < input.length(); i++) {
		//search in alphabet
		for (u_int32_t j = 0; j < alphabet.length(); j++) {
			if (input[i] == alphabet[j]) {
				output.push_back(j);
				//move the found character to the beginning
				alphabet.erase(alphabet.begin() + j);
				alphabet.insert(0,1,input[i]);
				break;
			}
		}
	}
	return output;
}

/**
Decodes the given MTF numbers to BWT text
	@param mtfEncoded - MTF encoded vector of numbers
	@param alphabet - ASCII alphabet
	@return output - the BWT text
*/
string MTFDecode(vector<u_int32_t> mtfEncoded, string alphabet) {
	string output;
	for (u_int64_t i = 0; i < mtfEncoded.size(); i++) {
		output.push_back(alphabet[mtfEncoded[i]]);
		alphabet.erase(alphabet.begin() + mtfEncoded[i]);
		alphabet.insert(0,1,output.back());
	}
	return output;
}

/**
Encodes a given MTF encoded numbers to RLE - before each number sequence adds the sequence's count
	@param input - MTF encoded vector of numbers
	@return output - RLE encoded vector of numbers
*/
vector<u_int32_t> RLEcode(vector<u_int32_t> input) {
	vector<u_int32_t> output;
	u_int32_t prev = input.front();
	u_int32_t counter = 1;
	for (u_int32_t i = 1; i < input.size(); i++) {
		if (prev == input[i])
			counter++;
		else {
			output.push_back(counter);
			output.push_back(prev);
			counter = 1;			
		}
		prev = input[i];
	}
	output.push_back(counter);
	output.push_back(prev);
	return output;
}

/**
Decodes a given RLE encoded numbers to MTF - every odd number is a following number's count
	@param input - RLE encoded vector of numbers
	@return output - MTF encoded vector of numbers
*/
vector<u_int32_t> RLEdecode(vector<u_int32_t> input) {
	vector<u_int32_t> output;
	for (u_int64_t i = 0; i < input.size()-1; i++) {
		for (u_int64_t j = 0; j < input[i]; j++) {
			output.push_back(input[i+1]);
		}
		i++;
	}
	return output;
}

/**
recursive function that distributes a binary code of each node of huffman's encoding tree
	@param node - tree node that is going to be examined alongside with its two children
*/
void recursive_huffman(huffmanTree* node) {
	if (node == NULL)
		return;
	if (node->left != NULL) {
		(node->left)->code.append(node->code + string("0"));
		recursive_huffman(node->left);
	}
	if (node->right != NULL) {
		(node->right)->code.append(node->code + string("1"));
		recursive_huffman(node->right);
	}
	return;
}

/**
function that starts to distrubute the code values in the huffman tree
	@param tree - vector of all nodes in the tree
*/
void assign_huffman_codes(vector<huffmanTree*>* tree) {
	size_t size = tree->size();
	(*tree)[size-1]->code = "1";
	(*tree)[size-2]->code = "0";
	recursive_huffman((*tree)[size-2]);
	recursive_huffman((*tree)[size-1]);
	return;
}

/**
Function that deserializes a string of huffman codes to map
	@param mapString - serialized map
	@return huffmanMap - map of huffman codes
*/
map<string,int> stringToMap(string mapString) {
	map<string,int>huffmanMap;
	auto begin = mapString.begin();
	auto end = mapString.find(';');
	while (end != string::npos) {
		string tmp = string(mapString,0, end);
		auto innerEnd = tmp.find('=');
		huffmanMap[string(tmp, 0, innerEnd)] = stoi(string(tmp, innerEnd + 1, tmp.length()-1));
		mapString.erase(begin, begin + (end+1));
		end = mapString.find(';');
	}
	return huffmanMap;	
}

/**
function that serializes a map of huffman codes to a string
	@param huffmanMap - a map of huffman codes
	@return output - serialized map
*/
string mapToString(map<int,string> huffmanMap) {
	string output = "";
	for(auto const &map1 : huffmanMap) {
		output.append(map1.second + "=" + to_string(map1.first) + ";");
  	}
  	stringToMap(output);
  	return output;
}

/**
function that creates bytes from string binary code
	@param vec - string of binary code
	@return output - vector of bytes representing the binary coded huffman encoding
*/
string bitsToString(string vec) {
	string output;
	while(vec.length() > 8) {
		//take 8 'bits', create a bitset and from that, an actual byte
		string tmp = vec.substr(0,8);
		vec.erase(0,8);
		bitset<8> bset(tmp);
		output.push_back((char)bset.to_ulong());
	}
	bitset<8> bset;
	//remaining 'bits' that didnt create a whole byte
	for (u_int64_t i = 0; i < vec.length(); i++) {
		bset[7-i] = vec[i] == '1' ? 1 : 0;
	}
	output.push_back((char)bset.to_ulong());
	return output;

}

/**
Decodes a huffman encoded bytes
	@param bits - huffman encoded bytes (each byte = 8 huffman bits)
	@param huffmanMap - map of huffman binary codes and actual RLE numbers
	@return output - RLE encoded numbers
*/
vector<u_int32_t> huffmanDecode(string bits, map<string,int> huffmanMap, int size) {
	bitset<8>bset;
	string str = "";
	vector<u_int32_t> output;
	for (u_int64_t i = 0; i < bits.length(); i++) {
		//take a byte (of huffman bits) and append it as a string to a sequence of bits
		bset = bitset<8>(bits[i]);
		string temp = bset.to_string();
		str.append(temp);
	}
	string tmp = "";
	//take one bit, append it to a temp string of bits and look whether this code is in the map
	while(!str.empty()) {
		tmp.push_back(str.front());
		str.erase(0,1);
		if (huffmanMap.count(tmp) >= 1) {
			output.push_back(huffmanMap[tmp]);
			tmp = "";
		}
	}
	return vector<u_int32_t>(output.begin(), output.begin() + size);
}

/**
Encodes RLE encoded number vector with a huffman map of codes
	@param huffmanMap - map of symbols and codes
	@param input - RLE encoded symbols
	@return bits - huffman encoded string of bits
*/
string huffmanEncode(map<int,string> huffmanMap, vector<u_int32_t> input) {
	string bitVector;
	for (u_int32_t i = 0; i < input.size(); i++) {
		string tmp = huffmanMap[input[i]];
		bitVector.append(tmp);
	}
	string bits = bitsToString(bitVector);
	return bits;
}

/**
Creates a huffman encoding tree. 
	@param input - RLE encoded numbers to be encoded with huffman
	@param alphabet - vector of symbols that occur in the RLE encoded numbers and its occurence count
	@return output - huffman encoded stream of bytes and the huffman encoding map
*/
tuple<string, map<int,string>> makeHuffmanTree(vector<u_int32_t> input, vector<tuple<int,int>> alphabet) {
	int size = get<1>(alphabet[0]);
	vector<huffmanTree*> tree;
	//make leafs in the tree from the alphabet
	for (u_int32_t i = 1; i < alphabet.size(); i++) {
		huffmanTree* tmp = new huffmanTree;
		tmp->key = get<1>(alphabet[i]);
		tmp->value = get<0>(alphabet[i]);
		tmp->left = NULL;
		tmp->right = NULL;
		tmp->isCreated = false;
		tree.push_back(tmp);
	}
	int i = 0;
	//make the tree - count the occurences between symbols until a final sum is reached
	while (true) {
		huffmanTree* parent = new huffmanTree;
		parent->key = -1;
		parent->value = (tree[i])->value + (tree[i+1])->value;
		if (parent->value == size){
			break;
		}
		parent->left = (tree)[i];
		parent->right = (tree[i+1]);
		parent->isCreated = true;
		i += 2;
		tree.push_back(parent);
		sort(begin(tree)+i, end(tree), 
    	[](huffmanTree* const t1, huffmanTree* const t2) {
        	return t1->value < t2->value;
    	});
	}
	sort(begin(tree), end(tree), 
    	[](huffmanTree* const t1, huffmanTree* const t2) {
        	return t1->value < t2->value;
    	});
	//propagate the binary codes from the root of the tree
	assign_huffman_codes(&tree);
	u_int32_t j = 0;
	//create the map, free memory from tree
	map<int,string>huffmanMap;
	while (j < tree.size()) {
		if((tree[j])->key != -1) {
			huffmanMap[tree[j]->key] = tree[j]->code;	
		}	
		delete(tree[j]);
		tree.erase(tree.begin() + j);
	}
	return make_tuple(huffmanEncode(huffmanMap, input), huffmanMap);

}

/**
Finds an unique character that does not occur in the input file that will be used as a delimiter.
	@param input - string of the input file
	@return myEOF - the delimiter
*/
char findEOF(string input) {
	int64_t myEOF=-1;
	for(int64_t i=SIZE_OF_ALPHABET-1; i>=0; i--) {
        myEOF = (i+SIZE_OF_ALPHABET/2)%SIZE_OF_ALPHABET;
        if(input.find(myEOF) == string::npos) {
            break;
        }
    }
	return (char)myEOF;
}

/**
Puts the output to a file.
	@param outputFile - the file
	@param output - the string to be put into a file
*/
void putOutput(FILE *outputFile, string output) {
    if(outputFile != NULL){
        for(int64_t i=0; i < (int64_t)output.size(); i++)
            fputc(output[i], outputFile);
        fclose(outputFile);
    }
    else {
        cout << output << endl;
    }
}

/**
Encodes a text string with BWT, MTF, RLE and Huffman.
	@param bwted - structure that logs information
	@param inputFile - file which should be read
	@param outputFile - where the output goes
	@return status code
*/
int BWTEncoding(tBWTED *bwted, FILE *inputFile, FILE *outputFile) {
	string content=getInput(inputFile);
	if (content.empty())
		return -1;
	if (bwted != NULL)
        bwted->uncodedSize=content.size();
	string alphabet = initAlphabet();
	char eof = findEOF(content);
	vector<string> blocks;
	if (alphabet.find(eof) == string::npos)
		alphabet.insert(0,1,eof);
	//make the blocks
	while(content.length() > BLOCK_SIZE) {
		string tmp = content.substr(0, BLOCK_SIZE);
		tmp.push_back(eof);
		blocks.push_back(tmp);
		content.erase(0, BLOCK_SIZE);
	}
	string outputTotal;
	outputTotal.push_back(eof);
	//put the delimiter to the beginning so decoding function knows what the delimiter is
	content.push_back(eof);
	blocks.push_back(content);
	string sum = "";
	while(blocks.size() > 0) {	
		//encode block with BWT
		string encoded = BWTencode(blocks[0]);
		if (encoded.empty())
			return -1;
		//encode block with MTF		
		vector<u_int32_t> mtfEncoded = MTFCode(encoded, alphabet);
		if (mtfEncoded.empty())
			return -1;
		//encode block with RLE
		vector<u_int32_t> rlEncoded = RLEcode(mtfEncoded);
		if (rlEncoded.empty())
			return -1;
		int rleSize = rlEncoded.size();
		vector<tuple<int,int>> numAlphabet = initNumAlphabet(rlEncoded);
		//make the huffman tree and encode the block
		tuple<string, map<int, string>> huffmanOutput = makeHuffmanTree(rlEncoded, numAlphabet);
		map<string,int>reversed;
		string mapString = mapToString(get<1>(huffmanOutput));
		//in the output we need to save the huffman map and also a length of RLE encoded block, because huffman can put extra 0 bits to the last byte (has to be adjusted)
		outputTotal.append(to_string(rleSize) + eof + mapString + eof);
		//divide the block
		outputTotal.append(get<0>(huffmanOutput) + eof+eof+'|'+eof+eof);
		sum.append(outputTotal);
		outputTotal = "";
		blocks.erase(blocks.begin());
	}	
	if (bwted != NULL)
        bwted->codedSize=sum.size();
    putOutput(outputFile, sum);
    return 0;
}

/**
Decodes a huffman encoded bytes to original text string.
	@param ahed - structure that logs information
	@param inputFile - file which should be read
	@param outputFile - where the output goes
	@return status code
*/
int BWTDecoding(tBWTED *ahed, FILE *inputFile, FILE *outputFile) {
	string content=getInput(inputFile);
	if (ahed!=NULL)
        ahed->codedSize = content.size();
	string alphabet = initAlphabet();
	char eof = content[0];
	string sum = "";
	content.erase(0,1);
	//the blocks	
	while(content.length() > 0) {
		int rleSize = 0;
		auto begin = content.begin();
		auto end = content.find(eof);
		//get the RLE encoded symbols length so it can be properly decoded
		if (end != string::npos) {
			rleSize = stoi(string(content,0,end));
			content.erase(begin, begin + (end+1));
		}
		begin = content.begin();
		//get the huffman map and deserialize it
		end = content.find(eof);
		map<string,int> huffmanMap = stringToMap(string(content, 0, end));
		content.erase(begin, begin + (end+1));
		//get the huffman encoded bytes
		begin = content.begin();
		string delimiter;
		delimiter.push_back(eof);
		delimiter.push_back(eof);
		delimiter.push_back('|');
		delimiter.push_back(eof);
		delimiter.push_back(eof);
		end = content.find(delimiter);
		string huffmanToDecode = content.substr(0, end);
		content.erase(begin, begin + (end + 5));
		//decode huffman encoded bytes
		vector<u_int32_t> huffmanDecoded = huffmanDecode(huffmanToDecode, huffmanMap, rleSize);
		if (huffmanDecoded.empty())
			return -1;
		//decode RLE encoded numbers
		vector<u_int32_t>rlDecoded = RLEdecode(huffmanDecoded);
		if (rlDecoded.empty())
			return -1;
		//decode MTF encoded numbers
		string mtfDecoded = MTFDecode(rlDecoded, alphabet);
		if (mtfDecoded.empty())
			return -1;
		//decode BWT encoded string
		string decoded = BWTdecode(mtfDecoded, eof);
		if (decoded.empty())
			return -1;
		sum.append(decoded);
	}
	if (ahed != NULL)
        ahed->uncodedSize=sum.size();
    putOutput(outputFile, sum);
    return 0;
}