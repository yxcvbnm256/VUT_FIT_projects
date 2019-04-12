/**
 * Projekt: Implementace interpretu imperativního jazyka IFJ14
 */
/**
 * @file bst.c
 * @author Antonin Cala(xcalaa00)
 * @brief soubor pro praci s binarnim stromem
 */

#include "bst.h"
#include <string.h>
#include "memoryallocation2.h"
#include "symboltable.h"

#define findBST() \
 \
	int comp; \
 \
	while (tree != NULL) \
	{ \
		comp = strcmp(tree -> name, name); \
		if (comp == 0) \
			return tree; \
		if (comp < 0) \
			tree = tree -> lChild; \
		else \
			tree = tree -> rChild; \
	} \
	return NULL

STVariableT * findVariable(const char *name, STVariableT *tree)
{
	findBST();
}

STFunctionT * findFunction(const char *name, STFunctionT *tree)
{
	findBST();
}


#define insertBST(allocationFunction, findAction, initAction) \
 \
  	int comp; \
	 \
	while(*tree != NULL) \
	{ \
		comp = strcmp((*tree) -> name, name); \
		if(comp == 0) \
		{ \
			findAction; \
		} \
		if(comp < 0) \
			tree = &((*tree)->lChild); \
		else \
			tree = &((*tree)->rChild); \
	} \
 \
	*tree = allocationFunction; \
 \
	size_t len = strlen(name) + 1; \
	(*tree)->name = memcpy(getDString(len), name, len); \
	(*tree) -> lChild = NULL; \
	(*tree) -> rChild = NULL; \
	initAction;\
	return *tree

STFunctionT * insertFunction(const char *name, STFunctionT **tree)
{
	insertBST(getDSTF(1), return *tree, insertFunctionInit(*tree));
}

STVariableT * insertVariable(const char *name, STVariableT **tree)
{
	insertBST(getDSTV(1), return NULL, );
}
