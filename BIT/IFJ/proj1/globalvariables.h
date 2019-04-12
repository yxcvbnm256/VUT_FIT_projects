/**
 * Projekt: Implementace interpretu imperativního jazyka IFJ14
 */
/**
 * @file globalvariables.h
 * @author Michal Janousek(xjanou15)
 * @brief Hlavickovy soubor pro globální proměnné
 */


#ifndef _GLOBALVARIABLES_H
#define _GLOBALVARIABLES_H

#include "tokens.h" 
#include "symboltable.h"
#include "3ak.h"
#include "parserexpression.h"
#include "memoryallocation2.h"

#define PRECEDENTIAL_TABLE_ROW_LEN 18

/**
 * @brief Proměnná pro návratovou hodnotu mainu.
 */
extern int errCode;

/**
 * @brief Proměnná pro aktuálně načtený token.
 */
extern tokenT actToken;

extern char tokenBuffer[ 128];

extern char *specialTokens[];

extern tokenTypeT specialTokensT[];

/**
 * @brief Precedenční tabulka
 */
extern int precedentialTable[][PRECEDENTIAL_TABLE_ROW_LEN];

/**
 * @brief Tabulka pravidel
 */
extern char *expressionRuleTable[];

/**
 * @brief Tabulka symbolů pro funkce.
 */
extern STFunctionT* functionTable;

/**
 * @brief Tabulky symbolů pro proměnné.
 */
extern STVariableT* localVariables;
extern STVariableT* globalVariables;

/**
 * @brief Aktuální funkce.
 */
extern STFunctionT* actualFunction;

/**
 * @brief ukazatel na poslední 3ak.
 */
extern threeAddressCodeT * last3ak;

extern threeAddressCodeT * globalThreeAddrC;
extern operListT* operList;

extern unsigned intCounter;
extern unsigned realCounter;
extern unsigned stringCounter;

extern char * tokenRewrite[];

/**
 * Global variables pro spravu pameti
*/
extern staticNodeT *staticMNPtr;
extern staticStringT *staticMSPtr;
extern staticTACT *staticMTACPtr;
extern staticSTFT *staticMSTFPtr;
extern staticSTVT *staticMSTVPtr;
extern dynamicOperListT * dynamicOperListPtr;
extern dynamicVariableInfoT* dynamicVIPtr;
extern dynamicParameterT * dynamicParameterPtr;
extern dynamicSElemT * dynamicSElemPtr;
extern dynamicTElemT * dynamicTElemPtr;
extern dynamicPLIT *dynamicPLIPtr;
extern dynamicCharT *dynamicCharPtr;
extern dynamicIntT *dynamicIntPtr;
extern dynamicFloatT *dynamicFloatPtr;
extern dynamicTAST *dynamicTASTPtr;
extern dynamicVTT *dynamicVTPtr;
extern dynamicElementT *dynamicElementPtr;
extern dynamicElemT *dynamicElemPtr;

extern FILE *source;


#endif // _GLOBALVARIABLES_H
