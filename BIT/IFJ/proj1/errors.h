/**
 * Projekt: Implementace interpretu imperativního jazyka IFJ14
 */
/**
 * @file errors.h
 * @author Michal Janousek(xjanou15)
 * @brief Hlavičkový soubor pro error codes
 */

#ifndef _ERRORS_H
#define _ERRORS_H

#define FILE_ERROR 99
#define ALLOCATION_ERROR 99
#define SYNTAX_ERROR 2
#define SEMANTIC_ERROR_PROGRAM 3
#define UNDEFINED_ID SEMANTIC_ERROR_PROGRAM
#define ID_REDEFINITION SEMANTIC_ERROR_PROGRAM
#define TYPE_ERROR 4
#define READLN_FAIL 6
#define UNINITIALIZED_VAR 7
#define DIVISION_BY_ZERO 8


#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

void warning(const char *fmt, ...); 

void fatalError(const char *fmt, ...) __attribute__((noreturn));



#endif // _ERRORS_H
