/**
 * Projekt: Implementace interpretu imperativního jazyka IFJ14
 */
/**
 * @file parser.h
 * @author Michal Janousek(xjanou15)
 * @brief Hlavickovy soubor pro parser
 */


#ifndef _PARSER_H
#define _PARSER_H

#include <stdbool.h>
#include "parserexpression.h"
/**
 * @brief Vrátí, zda je na vstupu program bez chyby nebo ne a zároveň generuje kód.
 */
bool isProgram();

/**
 * @brief Vytvoří operátor z aktuálního tokenu.
 */
operListT * createOper();

/**
 * @brief Vytvoří operátor daného typu.
 */
operListT * createOperType(paramTypeT type);

operListT * createOperBoolConst(int value);

operListT * createOperIntConst(int value);

operListT * createOperRealConst(double value);

operListT * createOperStringConst(char* value);

operListT * createOperTypeEmpty(paramTypeT type);

/*
 * @brief Vypíše očekávaný token a aktuální token se zprávou.
 */
void tokenError(tokenTypeT expected, const char * comment) __attribute__((noreturn));

#endif // _PARSER_H
