/** bwted.h - interface of library bwted for encoding/decoding text file using BWT, MTE, RLE and Huffman.
* author - Jan Pawlus, xpawlu00
* created 4. 5. 2018
*/

#ifndef BWTED_H
#define BWTED_H

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <unistd.h>
#include <sstream>
#include <algorithm>
#include <set>
#include <tuple>
#include <map>
#include <functional>
#include <bitset>
#include <sys/types.h>

//sets the block size
#define BLOCK_SIZE 15000
#define SIZE_OF_ALPHABET 256

using namespace std;

typedef struct {
    int64_t uncodedSize;
    int64_t codedSize;
} tBWTED;

/* structure for creating the huffman encoding tree */
typedef struct huffmanTreeS {
    int key;
    int value;
    huffmanTreeS* left;
    huffmanTreeS* right;
    bool isCreated;
    string code;
} huffmanTree;

/**
Encodes the given text with Burrows-Wheeler transformation.
    @param input - input string that should be encoded
    @return output - the text transformed with BWT
*/
string BWTencode(string input);

/**
Decodes text encoded in BTW to its original form.
    @param input - input string in BWT
    @param eof - a character that is used as a delimiter
    @return output - original text
*/
string BWTdecode(string input, char eof);

/**
Function that initializes all the alphabet we can get from ASCII char.
    @return alphabet - alphabet represented as a string
*/
string initAlphabet();

/**
Gets an input from specified file or stdin
    @param inputFile - pointer to a file
    @return input - the input from the file or stdin
*/
string getInput(FILE *inputFile);

/**
Function that initializes the numerical alphabet that is created after MTF and RLE encoding. It counts occurences of each number in the whole RLE encoded input.
    @param input - vector of integers representing the RLE encoding
    @return alphabet - vector of tuples - each representing occurence count of one symbol and the symbol.
*/
vector<tuple<int,int>> initNumAlphabet(vector<u_int32_t> input);

/**
Encodes the BWT text to MTF vector if integers. 
    @param input - input BWT string that should be encoded
    @param alphabet - ASCII alphabet
    @return output - MTF encoded vector of numbers representing the BWT text
*/
vector<u_int32_t> MTFCode(string input, string alphabet);

/**
Decodes the given MTF numbers to BWT text
    @param mtfEncoded - MTF encoded vector of numbers
    @param alphabet - ASCII alphabet
    @return output - the BWT text
*/
string MTFDecode(vector<u_int32_t> mtfEncoded, string alphabet);

/**
Encodes a given MTF encoded numbers to RLE - before each number sequence adds the sequence's count
    @param input - MTF encoded vector of numbers
    @return output - RLE encoded vector of numbers
*/
vector<u_int32_t> RLEcode(vector<u_int32_t> input);

/**
Decodes a given RLE encoded numbers to MTF - every odd number is a following number's count
    @param input - RLE encoded vector of numbers
    @return output - MTF encoded vector of numbers
*/
vector<u_int32_t> RLEdecode(vector<u_int32_t> input);

/**
recursive function that distributes a binary code of each node of huffman's encoding tree
    @param node - tree node that is going to be examined alongside with its two children
*/
void recursive_huffman(huffmanTree* node);

/**
function that starts to distrubute the code values in the huffman tree
    @param tree - vector of all nodes in the tree
*/
void assign_huffman_codes(vector<huffmanTree*>* tree);

/**
Function that deserializes a string of huffman codes to map
    @param mapString - serialized map
    @return huffmanMap - map of huffman codes
*/
map<string,int> stringToMap(string mapString);

/**
function that serializes a map of huffman codes to a string
    @param huffmanMap - a map of huffman codes
    @return output - serialized map
*/
string mapToString(map<int,string> huffmanMap);

/**
function that creates bytes from string binary code
    @param vec - string of binary code
    @return output - vector of bytes representing the binary coded huffman encoding
*/
string bitsToString(string vec);

/**
Decodes a huffman encoded bytes
    @param bits - huffman encoded bytes (each byte = 8 huffman bits)
    @param huffmanMap - map of huffman binary codes and actual RLE numbers
    @return output - RLE encoded numbers
*/
vector<u_int32_t> huffmanDecode(string bits, map<string,int> huffmanMap, int size);

/**
Encodes RLE encoded number vector with a huffman map of codes
    @param huffmanMap - map of symbols and codes
    @param input - RLE encoded symbols
    @return bits - huffman encoded string of bits
*/
string huffmanEncode(map<int,string> huffmanMap, vector<u_int32_t> input);

/**
Creates a huffman encoding tree. 
    @param input - RLE encoded numbers to be encoded with huffman
    @param alphabet - vector of symbols that occur in the RLE encoded numbers and its occurence count
    @return output - huffman encoded stream of bytes and the huffman encoding map
*/
tuple<string, map<int,string>> makeHuffmanTree(vector<u_int32_t> input, vector<tuple<int,int>> alphabet);

/**
Finds an unique character that does not occur in the input file that will be used as a delimiter.
    @param input - string of the input file
    @return myEOF - the delimiter
*/
char findEOF(string input);

/**
Puts the output to a file.
    @param outputFile - the file
    @param output - the string to be put into a file
*/
void putOutput(FILE *outputFile, string output);

/**
Encodes a text string with BWT, MTF, RLE and Huffman.
    @param bwted - structure that logs information
    @param inputFile - file which should be read
    @param outputFile - where the output goes
    @return status code
*/
int BWTEncoding(tBWTED *bwted, FILE *inputFile, FILE *outputFile);

/**
Decodes a huffman encoded bytes to original text string.
    @param ahed - structure that logs information
    @param inputFile - file which should be read
    @param outputFile - where the output goes
    @return status code
*/
int BWTDecoding(tBWTED *ahed, FILE *inputFile, FILE *outputFile);

#endif
