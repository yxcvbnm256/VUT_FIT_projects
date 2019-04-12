/**
 * Projekt: Implementace interpretu imperativního jazyka IFJ14
 */
/**
 * @file interpret.h
 * @author Jan Pawlus
 * @brief Hlavičkový soubor pro Interpret
 */
#ifndef _INTERPRET_H
#define _INTERPRET_H



typedef struct variableTableT variableTableT;

typedef struct threeAddressStackT threeAddressStackT;

typedef struct charList charList;

#include "3ak.h"

void interpret();

void variableTableFill(variableTableT *localVariableTable, variableTableT *globalVariableTable);

threeAddressStackT * threeAddressStackPush(threeAddressStackT * Stack);

void threeAddressStackPop(threeAddressStackT ** Stack);

variableTableT* variableTableAlloc(variableTableT* previous);

void variableTableDestroy(variableTableT **localVariableTable);

void assignFunction (parameterValT* assignValue, operandT operand, variableTableT* localVariableTable, variableTableT* globalVariableTable);

charList *charListInsert(charList *list, char c);

struct threeAddressStackT
{
	threeAddressCodeT * code;
	threeAddressStackT * next;
};


struct variableTableT
{
	int (* intArray);
	double (* doubleArray);
	char ( * (*charArray));
	int (* intInitArray);
	int (* doubleInitArray);
	int (* charInitArray);
	variableTableT* previous;	
};

struct charList
{
	char c;
	charList * next;
};

#endif // _INTERPRET_H
