/**
 * Projekt: Implementace interpretu imperativního jazyka IFJ14
 */
/**
 * @file memoryallocation2.h
 * @author Antonín Čala (xcalaa00)
 * @brief Hlavičkový soubor pro správu paměti
 */
#ifndef _MEMORY_ALLOCATION_H
#define _MEMORY_ALLOCATION_H

typedef struct dynamicCharT dynamicCharT;
typedef struct dynamicIntT dynamicIntT;
typedef struct dynamicFloatT dynamicFloatT;
typedef struct staticNodeT staticNodeT;
typedef struct staticStringT staticStringT;
typedef struct staticTACT staticTACT;
typedef struct staticSTFT staticSTFT;
typedef struct staticSTVT staticSTVT;
typedef struct dynamicTAST dynamicTAST;
typedef struct dynamicVTT dynamicVTT;
typedef struct dynamicElementT dynamicElementT;
typedef struct dynamicElemT dynamicElemT;
typedef struct dynamicPLIT dynamicPLIT;
typedef struct dynamicOperListT dynamicOperListT;
typedef struct dynamicVariableInfoT dynamicVariableInfoT;
typedef struct dynamicParameterT dynamicParameterT;
typedef struct dynamicSElemT dynamicSElemT;
typedef struct dynamicTElemT dynamicTElemT;

#include <stdlib.h>
#include <stdio.h>
#include "interpret.h"
#include "bst.h"
#include "errors.h"
#include "globalvariables.h"
#include "symboltable.h"
#include "3ak.h"
#include "scanner.h"

/**
 * Deklarace struktur poli
*/

struct dynamicOperListT {
	struct dynamicOperListT *nextArr;
	unsigned firstFree;
	unsigned arraySize;
	operListT Array[];
};

struct dynamicVariableInfoT {
	struct dynamicVariableInfoT *nextArr;
	unsigned firstFree;
	unsigned arraySize;
	variableInfoT Array[];
};

struct dynamicParameterT{
	struct dynamicParameterT *nextArr;
	unsigned firstFree;
	unsigned arraySize;
	parameterT Array[];	
};

struct dynamicSElemT{
	struct dynamicSElemT *nextArr;
	unsigned firstFree;
	unsigned arraySize;
	sElemT Array[];	
};

struct dynamicTElemT{
	struct dynamicTElemT *nextArr;
	unsigned firstFree;
	unsigned arraySize;
	tElemT Array[];	
};

struct dynamicPLIT {
	struct dynamicPLIT *nextArr;
	unsigned firstFree;
	unsigned arraySize;
	paramListItemT Array[];
};

struct dynamicElemT {
	struct dynamicElemT *nextArr;
	unsigned firstFree;
	unsigned arraySize;
	tElemT Array[];
};


struct dynamicCharT {
	struct dynamicCharT *nextArr;
	unsigned firstFree;
	unsigned arraySize;
	char* Array[];
};

struct dynamicIntT {
	struct dynamicIntT *nextArr;
	unsigned firstFree;
	unsigned arraySize;
	int Array[];
};

struct dynamicFloatT {
	struct dynamicFloatT *nextArr;
	unsigned firstFree;
	unsigned arraySize;
	double Array[];
};

struct staticStringT {
	struct staticStringT *nextArr;
	unsigned firstFree;
	unsigned arraySize;
	char Array[];
};

struct staticTACT {
	struct staticTACT *nextArr;
	unsigned firstFree;
	unsigned arraySize;
	struct threeAddressCodeT Array[];
};

struct staticSTFT {
	struct staticSTFT *nextArr;
	unsigned firstFree;
	unsigned arraySize;
	struct STFunctionT Array[];
};

struct staticSTVT {
	struct staticSTVT *nextArr;
	unsigned firstFree;
	unsigned arraySize;
	struct STVariableT Array[];
};


struct dynamicTAST {
	struct dynamicTAST *nextArr;
	unsigned firstFree;
	unsigned arraySize;
	struct threeAddressStackT Array[];
};

struct dynamicVTT {
	struct dynamicVTT *nextArr;
	unsigned firstFree;
	unsigned arraySize;
	struct variableTableT Array[];
};

struct dynamicElementT {
	struct dynamicElementT *nextArr;
	unsigned firstFree;
	unsigned arraySize;
	struct charList Array[];
};

/**
 * Funkce pro praci se strukturami
*/
void initSSArr(unsigned size);
char* getDString(unsigned size);
void releaseSSArr();


#define dynamicFunctionsDefinitions(type, FName) \
void init ## FName(unsigned size); \
void release ## FName(); \
void add ## FName(unsigned size); \
type* get ## FName(unsigned size); \
void free ## FName(); \
void remove ## FName(unsigned size)

dynamicFunctionsDefinitions(operListT, DOperList);

dynamicFunctionsDefinitions(variableInfoT, DVariableInfo);

dynamicFunctionsDefinitions(parameterT, DParameter);

dynamicFunctionsDefinitions(sElemT, DSElem);

dynamicFunctionsDefinitions(tElemT, DTElem);

dynamicFunctionsDefinitions(threeAddressCodeT, DTAC);

dynamicFunctionsDefinitions(STFunctionT, DSTF);

dynamicFunctionsDefinitions(STVariableT, DSTV);

dynamicFunctionsDefinitions(char, DString);

void initDPLI(unsigned size);
void releaseDPLI();
void addDPLI(unsigned size);
paramListItemT* getDPLI(unsigned size);
void freeDPLI();
void removeDPLI(unsigned size);

void initDCArr(unsigned size);
void releaseDChar();
void addDChar(unsigned size);
void removeDChar(unsigned size);
char** getDChar(unsigned size);
void freeDChar();

void initDIArr(unsigned size);
void releaseDInt();
void addDInt(unsigned size);
void removeDInt(unsigned size);
int* getDInt(unsigned size);
void freeDInt();

void initDFArr(unsigned size);
void releaseDFloat();
void addDFloat(unsigned size);
void removeDFloat(unsigned size);
double* getDFloat(unsigned size);
void freeDFloat();

void initDTASArr(unsigned size);
void releaseDTAS();
void addDTAS();
void removeDTAS(unsigned size);
threeAddressStackT* getDTAS();
void freeDTAS();

void initDVTArr(unsigned size);
void releaseDVT();
void addDVT();
void removeDVT(unsigned size);
variableTableT* getDVT();
void freeDVT();

void initDEArr(unsigned size);
void releaseDElement();
void addDElement();
void removeDElement(unsigned size);
charList* getDElement();
void freeDElement();

void initDElemArr(unsigned size);
void releaseDElem();
void addDElem();
void removeDElem(unsigned size);
tElemT* getDElem();
void freeDElem();

void releaseAll();

#endif
