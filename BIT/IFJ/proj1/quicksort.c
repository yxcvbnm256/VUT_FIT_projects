/**
 * Projekt: Implementace interpretu imperativního jazyka IFJ14
 */
/**
 * @file bst.h
 * @author Antonin Cala(xcalaa00)
 * @brief soubor pro praci s funkci quicksort
 */

#include <string.h>
#include "memoryallocation2.h"
#include "quicksort.h"

void shift(char string[], int left, int right)
{
	int lilHelper = string[left];
	string[left] = string[right];
	string[right] = lilHelper;
}

void quicksort(char string[], int left, int right)
{
	if(left < right)
	{
		int frontier = left;
		for(int i = left + 1; i < right; i++)
		{
			if(string[i] < string[left])
				shift(string, i, ++frontier);
		}
		shift(string, left, frontier);
		quicksort(string, left, frontier);
		quicksort(string, frontier + 1, right);
	}    
}

char * sort (char s1[])
{
	char *s2 = getDString(strlen(s1)+1);
	strcpy(s2, s1);
	quicksort(s2, 0, strlen(s1));
	return s2;
}
