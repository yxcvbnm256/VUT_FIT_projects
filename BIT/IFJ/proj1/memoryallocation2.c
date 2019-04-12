/**
 * Projekt: Implementace interpretu imperativního jazyka IFJ14
 */
/**
 * @file memoryallocation2.c
 * @author Antonin Cala(xcalaa00)
 * @brief soubor obsahujici funkce pro spravu pameti
 */

#include "memoryallocation2.h"
#include "bst.h"
#include <stdlib.h>
#include <stdio.h>
#include "errors.h"
#include "globalvariables.h"
#include "symboltable.h"

/**
 * @brief Makro pro volani inicializace jednotlivych struktur.
 */
#define initMemory(type, globalVariable, size, structType) \
\
	structType * tmp = malloc(sizeof(structType) + (size) * sizeof(type));\
	if (tmp == NULL)\
	{\
		errCode = ALLOCATION_ERROR;\
		fatalError("Nepodarila se alokovat pamet.\n");\
	}\
	tmp -> firstFree = 0;\
	tmp -> arraySize = size;\
	tmp -> nextArr = globalVariable;\
	globalVariable = tmp	


void initDPLI(unsigned size)
{
	initMemory(paramListItemT, dynamicPLIPtr, size, dynamicPLIT);
}

void initDCArr(unsigned size)
{
	initMemory(char*, dynamicCharPtr, size, dynamicCharT);
}


void initDIArr(unsigned size)
{
	initMemory(int, dynamicIntPtr, size, dynamicIntT);
}


void initDFArr(unsigned size)
{
	initMemory(float, dynamicFloatPtr, size, dynamicFloatT);
}

void initDTASArr(unsigned size)
{
	initMemory(threeAddressStackT, dynamicTASTPtr, size, dynamicTAST);
}

void initDVTArr(unsigned size)
{
	initMemory(variableTableT, dynamicVTPtr, size, dynamicVTT);
}

void initDEArr (unsigned size)
{
	initMemory(charList, dynamicElementPtr, size, dynamicElementT);
}

void initDElemArr(unsigned size)
{
	initMemory(tElemT, dynamicElemPtr, size, dynamicElemT);
}

/**
 * @brief Makro pro uvolneni pameti jednotlivych struktur.
 */
#define releaseMemory(globalVariable, structType) \
\
	structType* lilHelper = globalVariable;\
	while  (lilHelper != NULL)\
	{\
		globalVariable = globalVariable -> nextArr;\
		free(lilHelper);\
		lilHelper = globalVariable;\
	} 	


void releaseDPLI()
{
	releaseMemory(dynamicPLIPtr, dynamicPLIT);
}

void releaseDChar()
{
	releaseMemory(dynamicCharPtr, dynamicCharT);
}

void releaseDInt()
{
	releaseMemory(dynamicIntPtr, dynamicIntT);
}

void releaseDFloat()
{
	releaseMemory(dynamicFloatPtr, dynamicFloatT);
}

void releaseDTAS()
{
	releaseMemory(dynamicTASTPtr, dynamicTAST);
}

void releaseDVT()
{
	releaseMemory(dynamicVTPtr, dynamicVTT);
}

void releaseDElement()
{
	releaseMemory(dynamicElementPtr, dynamicElementT);
}

void releaseDElem()
{
	releaseMemory(dynamicElemPtr, dynamicElemT);
}

void releaseAll()
{
	releaseDString();
	releaseDTAC();
	releaseDSTF();
	releaseDSTV();
	releaseDPLI();
	releaseDChar();	
	releaseDFloat();
	releaseDInt();
	releaseDTAS();
	releaseDVT();
	releaseDElement();
	releaseDElem();
	releaseDOperList();
	releaseDVariableInfo();
	releaseDParameter();
	releaseDSElem();
	releaseDTElem();
	if(source != NULL)
		fclose(source);
}

/**
 * @brief Makro na pridani dalsiho pole v pripade nedostatecneho mista v prechozich.
 */
#define addDynamic(size, globalVariable, structType, type)\
{\
	structType *lilHelper = malloc(sizeof(structType) + size * sizeof(type));\
	if (lilHelper == NULL)\
	{\
		errCode = ALLOCATION_ERROR;\
		fatalError("Nepodarila se alokovat pamet.\n");\
	}\
	lilHelper -> nextArr = globalVariable;\
	globalVariable = lilHelper;\
	globalVariable -> arraySize = size;\
	globalVariable -> firstFree = 0u;\
}

/**
 * @brief makro na ziskani volneho prostoru daneho typu. Nahrazuje malloc.
 */
#define getDynamic(size, globalVariable, structType, Type)\
{\
	if(size == 0u) return NULL;\
	unsigned lilHelper = (globalVariable -> firstFree) + size - 1u;\
	if (lilHelper < (globalVariable -> arraySize))\
	{\
		globalVariable -> firstFree = ((globalVariable -> firstFree) + size);\
		return &(globalVariable -> Array[(globalVariable -> firstFree) - size]);\
	}\
	else\
	{\
		unsigned lilCounter = 2 * globalVariable -> arraySize;\
		while (lilCounter < size)\
			lilCounter = 2 * lilCounter;\
		addDynamic(lilCounter, globalVariable, structType, Type);\
		globalVariable -> firstFree = size;\
		return &(globalVariable -> Array[(globalVariable -> firstFree) - size]);\
	}\
}

paramListItemT* getDPLI(unsigned size)
{
	getDynamic(size, dynamicPLIPtr, dynamicPLIT, paramListItemT);
}

char** getDChar(unsigned size)
{
	getDynamic(size, dynamicCharPtr, dynamicCharT, char*);
}

int* getDInt(unsigned size)
{
	getDynamic(size, dynamicIntPtr, dynamicIntT, int);
}

double* getDFloat(unsigned size)
{
	getDynamic(size, dynamicFloatPtr, dynamicFloatT, double);
}

threeAddressStackT* getDTAS()
{
	getDynamic(1u, dynamicTASTPtr, dynamicTAST, threeAddressStackT);
}

variableTableT* getDVT()
{
	getDynamic(1u, dynamicVTPtr, dynamicVTT, variableTableT);
}

charList* getDElement()
{
	getDynamic(1u, dynamicElementPtr, dynamicElementT, charList);
}

/**
 * @brief Makro na uvolneni jednoho pole v pripade jeho prazdnosti.
 */
#define freeDynamic(globalVariable, structType)\
{\
	structType *lilHelper = globalVariable;\
	globalVariable = globalVariable -> nextArr;\
	free(lilHelper);\
}

/**
 * @brief Makro ktere uvolnuje misto v poli. Nahrazuje free.
 */
#define removeDynamic(size, globalVariable, structType)\
{\
	unsigned lilSize = size;\
	do{\
		if (lilSize < (globalVariable -> firstFree))\
		{\
			globalVariable -> firstFree = (globalVariable -> firstFree) - lilSize;\
			lilSize = 0u;\
		}\
		else\
		{\
			lilSize -= globalVariable -> firstFree;\
			if(globalVariable -> nextArr == NULL)\
			{				\
				globalVariable -> firstFree = 0u;\
				break;\
			}\
			else\
			{\
				freeDynamic(globalVariable, structType);\
				globalVariable -> firstFree = (globalVariable -> firstFree)- lilSize;\
			}\
		}\
	}while(lilSize != 0u);\
}

void removeDPLI(unsigned size)
{
	removeDynamic(size, dynamicPLIPtr, dynamicPLIT);
}

void removeDChar(unsigned size)
{
	removeDynamic(size, dynamicCharPtr, dynamicCharT);	
}

void removeDInt(unsigned size)
{
	removeDynamic(size, dynamicIntPtr, dynamicIntT);
}

void removeDFloat(unsigned size)
{
	removeDynamic(size, dynamicFloatPtr, dynamicFloatT);
}

void removeDTAS(unsigned size)
{
	removeDynamic(size, dynamicTASTPtr, dynamicTAST);
}

void removeDVT(unsigned size)
{
	removeDynamic(size, dynamicVTPtr, dynamicVTT);
}

void removeDElement(unsigned size)
{
	removeDynamic(size, dynamicElementPtr, dynamicElementT);
}

void removeDElem(unsigned size)
{
	removeDynamic(size, dynamicElemPtr, dynamicElemT);
}

/**
 * @brief Zjednoduseni jednotlivych volanic funkci.
 */
#define dynamicFunctions(type, globalVariable, structType, FName) \
 \
void init ## FName(unsigned size) \
{ \
	initMemory(type, globalVariable, size, structType); \
} \
 \
void release ## FName() \
{ \
	releaseMemory(globalVariable, structType); \
} \
 \
void add ## FName(unsigned size) \
{ \
	addDynamic(size, globalVariable, structType, type); \
} \
 \
type* get ## FName(unsigned size) \
{ \
	getDynamic(size, globalVariable, structType, type); \
} \
 \
void free ## FName() \
{ \
	freeDynamic(globalVariable, structType); \
} \
 \
void remove ## FName(unsigned size) \
{ \
	removeDynamic(size, globalVariable, structType); \
}

dynamicFunctions(operListT, dynamicOperListPtr, dynamicOperListT, DOperList)

dynamicFunctions(variableInfoT, dynamicVIPtr, dynamicVariableInfoT, DVariableInfo)

dynamicFunctions(parameterT, dynamicParameterPtr, dynamicParameterT, DParameter)

dynamicFunctions(sElemT, dynamicSElemPtr, dynamicSElemT, DSElem)

dynamicFunctions(tElemT, dynamicTElemPtr, dynamicTElemT, DTElem)

dynamicFunctions(threeAddressCodeT, staticMTACPtr, staticTACT, DTAC)

dynamicFunctions(STFunctionT, staticMSTFPtr, staticSTFT, DSTF)

dynamicFunctions(STVariableT, staticMSTVPtr, staticSTVT, DSTV)

dynamicFunctions(char, staticMSPtr, staticStringT, DString)
