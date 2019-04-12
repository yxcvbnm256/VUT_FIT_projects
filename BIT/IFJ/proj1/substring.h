/**
 * Projekt: Implementace interpretu imperativního jazyka IFJ14
 */
/**
 * @file bst.h
 * @author Jan Pawlus (xpawlu00)
 * @brief hlavickovy soubor pro praci s funkci find (Knuth-Morris-Prath algoritmus)
 */

#ifndef _SUBSTRING_H
#define _SUBSTRING_H

#include <string.h>

void kmp_prefix (int prefix[], char* substring, int substring_length);

int kmp_search (char* string, char* substring);

#endif // _SUBSTRING_H
