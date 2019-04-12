/**
 * Projekt: Implementace interpretu imperativního jazyka IFJ14
 */
/**
 * @file tokens.h
 * @author Michal Janousek(xjanou15)
 * @brief Hlavickovy soubor s tokenama
 */

#ifndef _TOKENS_H
#define _TOKENS_H

/**
 * @brief Tokeny pro lexikalni analyzator a parser.
 */
typedef enum tokenTypeT{
	NOT = 1,
	MULTIPLY, DIVIDE, AND,
	PLUS, MINUS, OR, XOR,
	EQUAL, LESS, MORE, NOT_EQUAL, LESS_OR_EQUAL, MORE_OR_EQUAL,
	LEFT_PARENTHESIS, RIGHT_PARENTHESIS,
	ID, INTEGER, STRING, REAL, BOOLEAN,
	INTEGER_TYPE, STRING_TYPE, REAL_TYPE, BOOLEAN_TYPE, 
	VAR, COLON, 
	ARRAY, OF, DOUBLEDOT, 
	SEMICOLON, 
	FUNCTION, LEFT_BRACKET, RIGHT_BRACKET, FORWARD,
	BEGIN, END, DOT,
	IF, THEN, ELSE,
	WHILE, DO,
	REPEAT, UNTIL,
	ASSIGNMENT,
	WRITE, READLN, COMMA,

	FILE_END

} tokenTypeT;

typedef union tokenValueT tokenValueT;

#include "bst.h"
#include <stdbool.h>
#include "3ak.h"

/**
 * @brief Počet tokenů
 */
#define TOKENS_COUNT FILE_END+1

/**
 * @brief Union obsahující hodnoty tokenu různého typu.
 */
union tokenValueT {
	char * idVal; /**< Ukazatel na položku v tabulce symbolů. */
	char * stringVal; /**< Hodnota string konstanty. */
	int integerVal; /**< Hodnota integer konstanty. */
	double realVal; /**< Hodnota real konstanty. */
	int booleanVal; /**< Hodnota boolean konstanty */
	parameterValT parameterVal; /**< hodnota pouzita pro predani do 3ak */
};


/**
 * @brief Struktura pro token obsahující typ a hodnotu.
 */
typedef struct tokenT {
	tokenTypeT type;  /**< Typ tokenu. */
	tokenValueT value; /**< Hodnota tokenu. */
} tokenT;



#endif // _TOKENS_H
