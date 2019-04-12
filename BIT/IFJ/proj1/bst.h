/**
 * Projekt: Implementace interpretu imperativního jazyka IFJ14
 */
/**
 * @file bst.h
 * @author Antonin Cala(xcalaa00)
 * @brief hlavickovy soubor pro praci s binarnim stromem
 */

#ifndef _BINARY_SEARCH_TREE_H
#define _BINARY_SEARCH_TREE_H

#include "symboltable.h"


STVariableT * findVariable(const char *name, STVariableT *tree);

STVariableT * insertVariable(const char *name, STVariableT **tree);

STFunctionT * findFunction(const char *name, STFunctionT *tree);

STFunctionT * insertFunction(const char *name, STFunctionT **tree);

#endif // _BINARY_SEARCH_TREE_H
