/**
 * Projekt: Implementace interpretu imperativního jazyka IFJ14
 */
/**
 * @file symboltable.h
 * @author Michal Janoušek(xjanou15)
 * @brief Hlavičkový soubor pro Tabulku symbolů
 */

#ifndef _SYMBOLTABLE_H
#define _SYMBOLTABLE_H

typedef struct STFunctionT STFunctionT;

typedef struct paramListItemT paramListItemT;

typedef struct STVariableT STVariableT;

#include "tokens.h"

typedef tokenTypeT paramTypeT;

#include <stdbool.h>
#include "3ak.h"

STVariableT * insertGlobalVariable(const char * name);
void insertFunctionInit(STFunctionT* function);
STFunctionT * insertFunctionTable(const char * name);
STVariableT * insertLocalVariable(const char * name);

void setVariable(STVariableT * variable, paramTypeT type);
paramListItemT * insertParam(char* name);
void setParam(paramListItemT * param, tokenTypeT tokenType);
void initLocalTable();
void initTable();

bool setFunction(const  char * name);


void checkDefinitions(STFunctionT * tree);

struct paramListItemT
{
	struct paramListItemT * next;
	paramTypeT type;
	char * name;
	unsigned index;
};

struct STFunctionT
{
	struct STFunctionT * lChild;
	struct STFunctionT * rChild;
	char * name;
	unsigned intVarCount; // počet proměných typu int a boolean
	unsigned intTmpCount; // počet proměných + pomocných proměnných typu int a boolean
	unsigned realVarCount; // počet proměných typu real
	unsigned realTmpCount; // počet proměných + pomocných proměnných typu real
	unsigned stringVarCount; // počet proměných typu string
	unsigned stringTmpCount; // počet proměných + pomocných proměnných typu string
	threeAddressCodeT * beginning; // začátek kódu funkce
	bool defined; // funkce byla definovaná
	bool declared; // funkce byla deklarovaná
	paramListItemT * params; // seznam parametrů (první je uložen typ fce)
};

struct STVariableT
{
	struct STVariableT * lChild;
	struct STVariableT * rChild;
	char * name;
	unsigned index;
	paramTypeT type;	
};

#endif // _SYMBOLTABLE_H
