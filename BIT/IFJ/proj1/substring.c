/**
 * Projekt: Implementace interpretu imperativního jazyka IFJ14
 */
/**
 * @file bst.h
 * @author Jan Pawlus (xpawlu00)
 * @brief soubor pro praci s funkci find (Knuth-Morris-Prath algoritmus)
 */

#include <string.h>
#include "memoryallocation2.h"

/* Funkce vracejici pole o delce podretezce. Na kazdem indexu bude cislo udavajici delku nejdelsiho prefixu, ktery je zaroven 
 suffixem useku podretezce o delce indexu. */
void kmp_prefix (int prefix[], char* substring, int substring_length)
{
	int k = -1;
	prefix[0] = k;
	for (int i = 1; i < substring_length; i++)
	{
		while ((k > -1) && (substring[k+1] != substring[i])) k = prefix[k];
		if (substring[i] == substring[k+1])	k++;
		prefix[i] = k;
	}	
}

/* Funkce vracejici index, od ktereho se vyskytuje zadany podretezec v retezci. Pokud se shoduje podretezec s retezcem jen v neuplne casti, 
 posouva se podretezec o hodnotu obsazene v poli prefix. */
int kmp_search (char* string, char* substring)
{
	int string_length = strlen(string);
	int substring_length = strlen(substring);
	int *prefix = getDInt(substring_length);
	kmp_prefix(prefix, substring, substring_length);
	int k = -1;
	for (int i = 0; i < string_length; i++) {
		while ((k > -1) && (substring[k+1] != string[i])) k = prefix[k];
		if (string[i] == substring[k+1])
		{
			k++;
		} 
		if (k == substring_length - 1)
		{
			removeDInt(substring_length);
			return i-k+1;		
		}
	}
	removeDInt(substring_length);	
	return 0;
}
