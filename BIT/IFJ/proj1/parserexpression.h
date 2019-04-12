/**
 * Projekt: Implementace interpretu imperativního jazyka IFJ14
 */
/**
 * @file parserexpression.h
 * @author Michal Janousek(xjanou15)
 * @brief Hlavickovy soubor pro parsovaní výrazu
 */


#ifndef _PARSEREXPRESSION_H
#define _PARSEREXPRESSION_H

typedef struct operListT operListT;

//Stack tokenů
typedef struct sElem *sElemPtr, sElemT;
#include <stdbool.h>
#include "tokens.h"
#include "symboltable.h"
#include "3ak.h"
 
/**
 * @brief Výčet neterminálů
 */
typedef enum {
	DOLLAR = TOKENS_COUNT, E,
	LEFT, RIGHT, MIDDLE, BLANK, FUNC
}controlS;

//Stack tokenů
struct sElem {
    struct sElem *ptr;
    int data;
};

//List operandů
struct operListT{
    operListT *next;
    variableInfoT * data;
    paramTypeT type;
};



#define RULE_MAX_L 3

/**
 * @brief Vrátí, zda je na vstupu výraz nebo ne.
 */
bool isExpression(operListT * retOper);
/**
 * @brieg Generování kódu z operlistu
 */
void insertCode(instructionT instruction, operListT *op1, operListT *op2, operListT *dest);

#endif // _PARSEREXPRESSION_H
