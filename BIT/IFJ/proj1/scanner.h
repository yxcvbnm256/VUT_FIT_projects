 /**
 * Projekt: Implementace interpretu imperativního jazyka IFJ14
 */
/**
 * @file scanner.h
 * @author Zdenek Studeny (xstude21)
 * @brief Implementace lexikalniho analyzatoru
 */
#ifndef _SCANNER_H
#define _SCANNER_H

#include <stdbool.h>
#include <stdio.h>

// Jednosmerny seznam
typedef struct tElem {
    struct tElem *ptr;
    char data;
} *tElemPtr, tElemT;

typedef struct { 
    tElemPtr Act;
    tElemPtr First;
} tList;

/**
 * @brief Struktura stavu pro KA
 *
 */
typedef enum stateT
{

	S_SPACE, // Defaultní stav a prázdné znaky
	S_ID, // ID nebo jeden z tokenu
	S_INTEGER, // číslo, zatí integer
	S_INTPOINT, // Je tecka (po číslu)
	S_INTEXP, // e (po integeru)
	S_INTEXPSIGN, // e a -/+ (po integeru)
	S_FLOAT, // FLOAT s . 
	S_FLOAT2, // FLOAT s e
	S_FLOATEXP, // FLOAT s . a e
	S_FLOATEXPSIGN, // FLOAT s . e a -/+
	S_STRING, // Uvnitr stringu mezi ' 
	S_STRINGHASH, // Dostal jsem ''
	S_ASCI, // mám #
	S_COLON, // mám :
	S_MORE,	// >
	S_LESS, // <
	S_DOT // .

}stateT; 

// ALOKACE PAMETI
 
void InitList (tList *L);
void InsertFirst (tList *L, char val);
void First (tList *L);
void DeleteFirst (tList *L);

/**
 * @brief Zjisti jestli je char cislo
 *
 * vraci true, pokud je char z 0-9.
 * V opacnem pripade vraci false.
 */
bool isNumber(int c);

/**
 * @brief Vlozi konec retezce
 *
 * Do tokenBufferu vlozi \O na pozici i
 */
void insertArrayEnd( int i );

/**
 * @brief Převede pole znaků na velká písmena
 *
 */
void arrayToUpper();

/**
 * @brief Převede pole znaků na malá písmena
 *
 */
void arrayTolower();

/**
 * @brief Zjisti jestli je char znak abecedy
 *
 * vraci true, pokud je char a-z nebo A-Z.
 * V opacnem pripade vraci false.
 */
bool isAlphabet(int c);

/**
 * @brief Nastaví file z ktere se bude cist
 */
void setSourceFile(FILE *f);

/**
 * @brief Ziska dalsi token
 *
 * vraci true, pokud se podarilo token ziskat a je platny
 * V opacnem pripade vraci false.
 */
void getToken();

#endif // _SCANNER_H
