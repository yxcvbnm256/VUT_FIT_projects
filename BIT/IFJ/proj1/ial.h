 /**
  * Projekt: Implementace interpretu imperativního jazyka IFJ14
  */
 /**
  * @file ial.h
  * @author Antonin Cala(xcalaa00), Jan Pawlus(xpawlu00)
  * @brief hlavickovy soubor quicksort, knuth-morris
  */

#ifndef _IAL_H
#define _IAL_H

/**
 * @brief Struktura pro uzel binarniho vyhledavaciho stromu.
 */
typedef struct treeNodeT {
	struct treeNodeT *lChild;  /**< Ukazatel na leveho potomka. */
	struct treeNodeT *rChild; /**< Ukazatel na praveho potomka. */
	char *name;
	void *data;     /**< Ukazatel na strukturu, ktera bude drzet data. */
} treeNodeT;

void shift(char string[], int left, int right);

void quicksort(char string[], int left, int right);

char * sort(char s1[]);

void kmp_prefix (int prefix[], char* substring, int substring_length);

int kmp_search (char* string, char* substring);

#endif // _IAL_H
