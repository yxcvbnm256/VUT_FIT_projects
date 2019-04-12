 /**
  * Projekt: Implementace interpretu imperativního jazyka IFJ14
  */
 /**
  * @file ial.c
  * @author Antonin Cala(xcalaa00), Jan Pawlus(xpawlu00)
  * @brief quicksort, knuth-morris, bst funkce
  */
#include "ial.h"
#include "memoryallocation2.h"
#include <string.h>
  
/*-----------------------QUICKSORT------------------------ */

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

/*-----------------------KNUTH-MORRIS-PRATH------------------------ */

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

/*-----------------------Binární stromy------------------------ */

treeNodeT * findBST(const char *name, treeNodeT *tree)
{
	int comp;

	while (tree != NULL)
	{
		comp = strcmp(tree -> name, name);
		if (comp == 0)
			return tree;
		if (comp < 0)
			tree = tree -> lChild;
		else
			tree = tree -> rChild;
	}
	return NULL;
}

treeNodeT * insertBST(const char *name, treeNodeT **tree)
{
  	int comp;
	
	while(*tree != NULL)
	{
		comp = strcmp((*tree) -> name, name);
		if(comp == 0)
		{
			return NULL;
		}
		if(comp < 0)
			tree = &((*tree)->lChild);
		else
			tree = &((*tree)->rChild);
	}

	*tree = malloc(sizeof(treeNodeT));

	size_t len = strlen(name) + 1;
	(*tree)->name = memcpy(getDString(len), name, len);
	(*tree) -> lChild = NULL;
	(*tree) -> rChild = NULL;
	return *tree;
}
