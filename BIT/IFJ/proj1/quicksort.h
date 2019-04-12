/**
 * Projekt: Implementace interpretu imperativního jazyka IFJ14
 */
/**
 * @file bst.h
 * @author Antonin Cala(xcalaa00)
 * @brief hlavickovy soubor pro praci s funkci quiksort
 */

#ifndef _QUICKSORT_H
#define _QUICKSORT_H

#include <string.h>

/**
 * @brief Pomocna funkce pro quicksort. Prohodi dva chary v ramci serazovaneho stringu.
 */
void shift(char string[], int left, int right);

/**
 * @brief Provede operaci quicksort nad parametrem string.
 */
void quicksort(char string[], int left, int right);

/**
 * @brief Vrati serazeny string podle operace quicksort na zaklade parametru s1.
 */
char * sort(char s1[]);

#endif // _QUICKSORT_H
