/**
 * Projekt: Implementace interpretu imperativního jazyka IFJ14
 */
/**
 * @file interpret.h
 * @author Jan Pawlus (xpawlu00)
 * @brief Interpret
 */
#include <stdbool.h>
#include <string.h>
#include "interpret.h"
#include "3ak.h"
#include "symboltable.h"
#include "memoryallocation2.h"
#include "errors.h"
#include "globalvariables.h"
#include "substring.h"
#include "quicksort.h"
#include "scanner.h"
#include <ctype.h>

/* Makro, diky kteremu se overuje, zda je promenna, se kterou se pracuje, inicialozvana. Pokud neni, interpret je ukoncen s chybou. */
#define if_initialized(localOrGlobal, initArray, operand)                                                                                       \
	if (localOrGlobal->initArray[operand] == false){                                                                                            \
		errCode = UNINITIALIZED_VAR;                                                                                                            \
		fatalError("Nelze pracovat s neinicializovanou promennou.\n");} 

/* Makro, ktere priradi hodnotu do promenne na zaklade toho, jestli prirazujeme lokalni promennou, globalni promennou nebo konstantu. */
#define copy_switch(dest, array, initArray, val, dattype)                                                                                       \
	switch ((*globalThreeAddrC).op2.parameters->data->type)                                                                                     \
	{                                                                                                                                           \
		case dattype ## _VARIABLE:                                                                                                              \
			if_initialized(localVariableTable, initArray, (*globalThreeAddrC).op2.parameters->data->value.index);                               \
			dest = localVariableTable->array[(*globalThreeAddrC).op2.parameters->data->value.index];                                            \
			break;                                                                                                                              \
																																				\
		case dattype ## _GLOBAL:                                                                                                                \
			if_initialized(globalVariableTable, initArray, (*globalThreeAddrC).op2.parameters->data->value.index);                              \
			dest = globalVariableTable->array[(*globalThreeAddrC).op2.parameters->data->value.index];                                           \
			break;                                                                                                                              \
																																				\
		case dattype ## _CONSTANT:                                                                                                              \
			dest = (*globalThreeAddrC).op2.parameters->data->value.val;                                                                         \
			break;                                                                                                                              \
																																				\
		default:                                                                                                                                \
			break;                                                                                                                              \
	}

/* Makro, ktere obstarava instrukce readln. Pokud se scanf nezdari, interpret konci chybou. Pri zdaru nastavi promennou,
 do ktere se nacetla hodnota, jako inicializovanou. Nasledne se zbavime nezadoucich znaku ze STDIN. */
#define if_scanf(localOrGlobal, initArray, scanfType)                                                                                           \
		if (scanf(scanfType, localOrGlobal[(*globalThreeAddrC).op1.varInfo->value.index]) != 1){                                                \
			errCode = READLN_FAIL;                                                                                                              \
			fatalError("Nelze nacist hodnotu ze vstupu.\n");}                                                                                   \
		else                                                                                                                                    \
		{                                                                                                                                       \
			char l;                                                                                                                             \
			initArray[(*globalThreeAddrC).op1.varInfo->value.index] = true;                                                                     \
			if(!isspace(l=getchar()))                                                                                                           \
			{                                                                                                                                   \
				errCode = READLN_FAIL;                                                                                                          \
				fatalError("Hodnoty na vstupu nejsou oddeleny mezerou.\n");                                  				                    \
			}                                                                                                                                   \
			ungetc(l, stdin);                                                                                                                   \
			while ((l=getchar()) != '\n' && l != EOF);                                                                                          \
		}

/* Podminka, ktera overuje, zda jsou dvojice promennych urciteho typu - napr. INT a INT, REAL a INT apod. */
#define insane_if(first, second, third, fourth, fifth, sixth)                                                                                   \
	(((*globalThreeAddrC).op1.varInfo->type == first || (*globalThreeAddrC).op1.varInfo->type == second || (*globalThreeAddrC).op1.varInfo->type == third)  && ((*globalThreeAddrC).op2.varInfo->type == fourth || (*globalThreeAddrC).op2.varInfo->type == fifth || (*globalThreeAddrC).op2.varInfo->type == sixth))


/* Makro, ktere provadi operace s promennyma (+, -, *, /). V situaci, kdy je promenna, do ktere se prirazuje real, musi se overit vsechny mozne varianty typu
obou operandu. */
#define assignDest(action)                                                                                                                                  \
	switch ((*globalThreeAddrC).dest.varInfo->type){                                                                                                        \
		case INT_VARIABLE:                                                                                                                                  \
			localVariableTable->intArray[(*globalThreeAddrC).dest.varInfo->value.index] = (assignValueOp1.intVal action assignValueOp2.intVal);             \
			localVariableTable->intInitArray[(*globalThreeAddrC).dest.varInfo->value.index] = true;                                                         \
			break;                                                                                                                                          \
																																							\
		case INT_GLOBAL:                                                                                                                                    \
			globalVariableTable->intArray[(*globalThreeAddrC).dest.varInfo->value.index] = (assignValueOp1.intVal action assignValueOp2.intVal);            \
			globalVariableTable->intInitArray[(*globalThreeAddrC).dest.varInfo->value.index] = true;                                                        \
			break;                                                                                                                                          \
																																							\
		case REAL_VARIABLE:                                                                                                                                 \
			if (insane_if(INT_VARIABLE, INT_GLOBAL, INT_CONSTANT, REAL_VARIABLE, REAL_GLOBAL, REAL_CONSTANT))                                               \
			{                                                                                                                                               \
				localVariableTable->doubleArray[(*globalThreeAddrC).dest.varInfo->value.index] = (assignValueOp1.intVal action assignValueOp2.realVal);     \
				localVariableTable->doubleInitArray[(*globalThreeAddrC).dest.varInfo->value.index] = true;                                                  \
			}                                                                                                                                               \
			else if (insane_if(REAL_VARIABLE, REAL_GLOBAL, REAL_CONSTANT, REAL_VARIABLE, REAL_GLOBAL, REAL_CONSTANT))                                       \
			{                                                                                                                                               \
				localVariableTable->doubleArray[(*globalThreeAddrC).dest.varInfo->value.index] = (assignValueOp1.realVal action assignValueOp2.realVal);    \
				localVariableTable->doubleInitArray[(*globalThreeAddrC).dest.varInfo->value.index] = true;                                                  \
			}                                                                                                                                               \
			else if (insane_if(REAL_VARIABLE, REAL_GLOBAL, REAL_CONSTANT, INT_VARIABLE, INT_GLOBAL, INT_CONSTANT))                                          \
			{                                                                                                                                               \
				localVariableTable->doubleArray[(*globalThreeAddrC).dest.varInfo->value.index] = (assignValueOp1.realVal action assignValueOp2.intVal);     \
				localVariableTable->doubleInitArray[(*globalThreeAddrC).dest.varInfo->value.index] = true;                                                  \
			}                                                                                                                                               \
			else if (insane_if(INT_VARIABLE, INT_GLOBAL, INT_CONSTANT, INT_VARIABLE, INT_GLOBAL, INT_CONSTANT))                                             \
			{                                                                                                                                               \
				localVariableTable->doubleArray[(*globalThreeAddrC).dest.varInfo->value.index] = ((double)assignValueOp1.intVal action assignValueOp2.intVal);      \
				localVariableTable->doubleInitArray[(*globalThreeAddrC).dest.varInfo->value.index] = true;                                                  \
			}                                                                                                                                               \
			break;                                                                                                                                          \
																																							\
		case REAL_GLOBAL:                                                                                                                                   \
			if (insane_if(INT_VARIABLE, INT_GLOBAL, INT_CONSTANT, REAL_VARIABLE, REAL_GLOBAL, REAL_CONSTANT))                                               \
			{                                                                                                                                               \
				globalVariableTable->doubleArray[(*globalThreeAddrC).dest.varInfo->value.index] = (assignValueOp1.intVal action assignValueOp2.realVal);    \
				globalVariableTable->doubleInitArray[(*globalThreeAddrC).dest.varInfo->value.index] = true;                                                 \
			}                                                                                                                                               \
			else if (insane_if(REAL_VARIABLE, REAL_GLOBAL, REAL_CONSTANT, REAL_VARIABLE, REAL_GLOBAL, REAL_CONSTANT))                                       \
			{                                                                                                                                               \
				globalVariableTable->doubleArray[(*globalThreeAddrC).dest.varInfo->value.index] = (assignValueOp1.realVal action assignValueOp2.realVal);   \
				globalVariableTable->doubleInitArray[(*globalThreeAddrC).dest.varInfo->value.index] = true;                                                 \
			}                                                                                                                                               \
			else if (insane_if(REAL_VARIABLE, REAL_GLOBAL, REAL_CONSTANT, INT_VARIABLE, INT_GLOBAL, INT_CONSTANT))                                          \
			{                                                                                                                                               \
				globalVariableTable->doubleArray[(*globalThreeAddrC).dest.varInfo->value.index] = (assignValueOp1.realVal action assignValueOp2.intVal);    \
				globalVariableTable->doubleInitArray[(*globalThreeAddrC).dest.varInfo->value.index] = true;                                                 \
			}                                                                                                                                               \
			else if (insane_if(INT_VARIABLE, INT_GLOBAL, INT_CONSTANT, INT_VARIABLE, INT_GLOBAL, INT_CONSTANT))                                             \
			{                                                                                                                                               \
				globalVariableTable->doubleArray[(*globalThreeAddrC).dest.varInfo->value.index] = ((double)assignValueOp1.intVal action assignValueOp2.intVal);     \
				globalVariableTable->doubleInitArray[(*globalThreeAddrC).dest.varInfo->value.index] = true;                                                 \
			}                                                                                                                                               \
			break;                                                                                                                                          \
																																							\
		case STRING_VARIABLE:                                                                                                                               \
			{char * tmp;                                                                                                                                    \
			size_t len1 = strlen(assignValueOp1.stringVal);                                                                                                 \
			size_t len2 = strlen(assignValueOp2.stringVal);                                                                                                 \
			tmp = getDString(len1 + len2 + 1);                                                                                                              \
			memcpy(tmp, assignValueOp1.stringVal, len1);                                                                                                    \
			memcpy(&(tmp[len1]), assignValueOp2.stringVal, len2 + 1);                                                                                       \
			localVariableTable->charArray[(*globalThreeAddrC).dest.varInfo->value.index] = tmp;                                                             \
			localVariableTable->charInitArray[(*globalThreeAddrC).dest.varInfo->value.index] = true;                                                        \
			}break;                                                                                                                                         \
																																							\
		case STRING_GLOBAL:                                                                                                                                 \
			{char * tmp;                                                                                                                                    \
			size_t len1 = strlen(assignValueOp1.stringVal);                                                                                                 \
			size_t len2 = strlen(assignValueOp2.stringVal);                                                                                                 \
			tmp = getDString(len1 + len2 + 1);                                                                                                              \
			memcpy(tmp, assignValueOp1.stringVal, len1);                                                                                                    \
			memcpy(&(tmp[len1]), assignValueOp2.stringVal, len2 + 1);                                                                                       \
			globalVariableTable->charArray[(*globalThreeAddrC).dest.varInfo->value.index] = tmp;                                                            \
			globalVariableTable->charInitArray[(*globalThreeAddrC).dest.varInfo->value.index] = true;                                                       \
			}break;     default:                                                                                                                            \
			break;                                                                                                                                          \
}                                   

/* Makro, ktere provadi porovnani (mensi, vetsi, rovno, mensi nebo rovno, vetsi nebo rovno) dvou hodnot. */
#define compare(action)\
	{assignFunction (&assignValueOp1, (*globalThreeAddrC).op1, localVariableTable, globalVariableTable);                                                    \
	assignFunction (&assignValueOp2, (*globalThreeAddrC).op2, localVariableTable, globalVariableTable);                                                     \
	int *dest, *init;                                                                                                                                       \
	if ((*globalThreeAddrC).dest.varInfo->type == INT_GLOBAL)                                                                                               \
	{                                                                                                                                                       \
		dest = &globalVariableTable->intArray[(*globalThreeAddrC).dest.varInfo->value.index];                                                               \
		init = &globalVariableTable->intInitArray[(*globalThreeAddrC).dest.varInfo->value.index];                                                           \
	}                                                                                                                                                       \
	else                                                                                                                                                    \
	{                                                                                                                                                       \
		dest = &localVariableTable->intArray[(*globalThreeAddrC).dest.varInfo->value.index];                                                                \
		init = &localVariableTable->intInitArray[(*globalThreeAddrC).dest.varInfo->value.index];                                                            \
	}                                                                                                                                                       \
	if (insane_if(INT_VARIABLE, INT_GLOBAL, INT_CONSTANT, INT_VARIABLE, INT_GLOBAL, INT_CONSTANT))                                                          \
	{                                                                                                                                                       \
		if (assignValueOp1.intVal action assignValueOp2.intVal){                                                                                            \
			*dest = true;                                                                                                                                   \
		}                                                                                                                                                   \
		else                                                                                                                                                \
		{                                                                                                                                                   \
			*dest = false;                                                                                                                                  \
		}                                                                                                                                                   \
	}                                                                                                                                                       \
	else if (insane_if(REAL_VARIABLE, REAL_GLOBAL, REAL_CONSTANT, REAL_VARIABLE, REAL_GLOBAL, REAL_CONSTANT))                                               \
	{                                                                                                                                                       \
		if (assignValueOp1.realVal action assignValueOp2.realVal)                                                                                           \
		{                                                                                                                                                   \
			*dest = true;                                                                                                                                   \
		}                                                                                                                                                   \
		else                                                                                                                                                \
		{                                                                                                                                                   \
			*dest = false;                                                                                                                                  \
		}                                                                                                                                                   \
	}                                                                                                                                                       \
	else if (insane_if(STRING_VARIABLE, STRING_GLOBAL, STRING_CONSTANT, STRING_VARIABLE, STRING_GLOBAL, STRING_CONSTANT))                                   \
		{                                                                                                                                                   \
		if (strcmp(assignValueOp1.stringVal, assignValueOp2.stringVal) action 0)                                                                            \
		{                                                                                                                                                   \
			*dest = true;                                                                                                                                   \
		}                                                                                                                                                   \
		else                                                                                                                                                \
		{                                                                                                                                                   \
			*dest = false;                                                                                                                                  \
		}                                                                                                                                                   \
	}                                                                                                                                                       \
	*init = true;}

/* Hlavni funkce interpret. Pracuje hlavne s globalni promennou globalThreeAddrC, ktera obsahuje ukazatel na aktualni triadresny kod (instrukci).
Struktura triadresneho kodu je popsana v souboru 3ak.h.  */
void interpret ()
{
 	/* Prvotni inicializace pouzivane pameti (jak velke pole pro jednotlive typy se ze zacatku vytvori). */
	initDTASArr(8);
	initDVTArr(32);
	initDIArr(256);
	initDFArr(32);
	initDCArr(32);
	initDEArr(32);
	threeAddressStackT* Stack;
	variableTableT *localVariableTable = NULL;
	/* Vytvoreni tabubulky globalnich promennych */
	variableTableT *globalVariableTable = variableTableAlloc(NULL);
	localVariableTable = globalVariableTable;   
	int length = 0;
	parameterValT assignValue;
	parameterValT assignValueOp1;
	parameterValT assignValueOp2;
	/* Pokud existuje instrukce k vykonani */
	while (globalThreeAddrC != NULL)
	{  		
		/* Velky switch, na zaklade ktereho se vybira, ktera instrukce bude provadena */ 
		switch (globalThreeAddrC->instruction)
		{
			/* Pri volani funkce se ulozi aktualni instrukce do zasobniku, vytvori se tabulka lokalnich promennych navazujici na
			predchozi tabulku lokalnich promennych a jako dalsi instrukce se nastavi zacatek volane funkce. */
			case I_CALL:                
				Stack = threeAddressStackPush(Stack);               
				actualFunction = globalThreeAddrC->op1.function;
				localVariableTable = variableTableAlloc(localVariableTable);
				variableTableFill(localVariableTable, globalVariableTable);
				globalThreeAddrC = globalThreeAddrC->op1.function->beginning;
				break;

			case I_LABEL:
				break;

			/* Pri navratu z funkce se nacte instrukce z vrcholu zasobniku instrukci, ulozi se navratova hodnota funkce a nasledne se znici tabulka 
			lokalnich promennych. */
			case I_RETURN_I:
				{globalThreeAddrC = (*Stack).code;  
				actualFunction = globalThreeAddrC->op1.function;            
				threeAddressStackPop(&Stack);				
				if_initialized(localVariableTable, intInitArray, 0);
				int i = localVariableTable->intArray[0];
				variableTableDestroy(&localVariableTable);                  
				switch ((*globalThreeAddrC).dest.varInfo->type)
				{
					case INT_VARIABLE:
						localVariableTable->intArray[(*globalThreeAddrC).dest.varInfo->value.index] = i;
						localVariableTable->intInitArray[(*globalThreeAddrC).dest.varInfo->value.index] = true;
						break;

					case INT_GLOBAL:
						globalVariableTable->intArray[(*globalThreeAddrC).dest.varInfo->value.index] = i;
						globalVariableTable->intInitArray[(*globalThreeAddrC).dest.varInfo->value.index] = true;
						break;

					default:
						break;
				}                       
				break;}

			case I_RETURN_R:
				{globalThreeAddrC = (*Stack).code;
				actualFunction = globalThreeAddrC->op1.function;
				threeAddressStackPop(&Stack);
				if_initialized(localVariableTable, doubleInitArray, 0);
				float i = localVariableTable->doubleArray[0];
				variableTableDestroy(&localVariableTable);
				switch ((*globalThreeAddrC).dest.varInfo->type)
				{
					case REAL_VARIABLE:
						localVariableTable->doubleArray[(*globalThreeAddrC).dest.varInfo->value.index] = i;
						localVariableTable->doubleInitArray[(*globalThreeAddrC).dest.varInfo->value.index] = true;
						break;

					case REAL_GLOBAL:
						globalVariableTable->doubleArray[(*globalThreeAddrC).dest.varInfo->value.index] = i;
						globalVariableTable->doubleInitArray[(*globalThreeAddrC).dest.varInfo->value.index] = true;
						break;

					default:
						break;
				}           
				break;}                             

			case I_RETURN_S:
				{globalThreeAddrC = (*Stack).code;
				actualFunction = globalThreeAddrC->op1.function;
				threeAddressStackPop(&Stack);						
				if_initialized(localVariableTable, charInitArray, 0);
				char *i = localVariableTable->charArray[0];
				variableTableDestroy(&localVariableTable);  
				switch ((*globalThreeAddrC).dest.varInfo->type)
				{
					case STRING_VARIABLE:
						localVariableTable->charArray[(*globalThreeAddrC).dest.varInfo->value.index] = i;
						localVariableTable->charInitArray[(*globalThreeAddrC).dest.varInfo->value.index] = true;
						break;

					case STRING_GLOBAL:
						globalVariableTable->charArray[(*globalThreeAddrC).dest.varInfo->value.index] = i;
						globalVariableTable->charInitArray[(*globalThreeAddrC).dest.varInfo->value.index] = true;
						break;

					default:
						break;
				}           
				break;}                 
			
			/* Pri instrukci continue se overuje podminka. Pokud neplati, skace se v kodu na urcene misto, jinak se pokracuje. */	
			case I_CONTINUE:
				assignFunction(&assignValue, (*globalThreeAddrC).op1, localVariableTable, globalVariableTable);    
				if (!assignValue.intVal)
				{
					globalThreeAddrC = globalThreeAddrC->dest.code;
				}                       
				break;

			/* Pri instrukci skip se overuje podminka. Pokud plati, skace se v kodu na urcene misto, jinak se pokracuje. */
			case I_SKIP:                
				assignFunction(&assignValue, (*globalThreeAddrC).op1, localVariableTable, globalVariableTable);    
				if (assignValue.intVal)
				{
					globalThreeAddrC = globalThreeAddrC->dest.code;
				}                       
				break;

			/* Skace se na urcene misto. */
			case I_GOTO:
				globalThreeAddrC = globalThreeAddrC->dest.code;
				break;

			/* Logicka operace and. Pokud jsou oba operandy true, cil je take true (samozrejme se musi nastavit inicializace cile) */
			case I_AND: 
				{assignFunction(&assignValueOp1, (*globalThreeAddrC).op1, localVariableTable, globalVariableTable);    
   				assignFunction(&assignValueOp2, (*globalThreeAddrC).op2, localVariableTable, globalVariableTable);    
   				if ((*globalThreeAddrC).dest.varInfo->type == INT_GLOBAL)
   				{
					if ((assignValueOp1.intVal == true) && (assignValueOp2.intVal == true))
					{
						globalVariableTable->intArray[(*globalThreeAddrC).dest.varInfo->value.index] = true;         
					} else {globalVariableTable->intArray[(*globalThreeAddrC).dest.varInfo->value.index] = false;}
					globalVariableTable->intInitArray[(*globalThreeAddrC).dest.varInfo->value.index] = true;
				}
				else
				{
					if ((assignValueOp1.intVal == true) && (assignValueOp2.intVal == true))
					{
						localVariableTable->intArray[(*globalThreeAddrC).dest.varInfo->value.index] = true;         
					} else {localVariableTable->intArray[(*globalThreeAddrC).dest.varInfo->value.index] = false;}
					localVariableTable->intInitArray[(*globalThreeAddrC).dest.varInfo->value.index] = true;
				}
				break;}

			case I_OR:              
				{assignFunction(&assignValueOp1, (*globalThreeAddrC).op1, localVariableTable, globalVariableTable);    
   				assignFunction(&assignValueOp2, (*globalThreeAddrC).op2, localVariableTable, globalVariableTable);    
   				if ((*globalThreeAddrC).dest.varInfo->type == INT_GLOBAL)
   				{
					if ((assignValueOp1.intVal == true) || (assignValueOp2.intVal == true))
					{
						globalVariableTable->intArray[(*globalThreeAddrC).dest.varInfo->value.index] = true;         
					} else {globalVariableTable->intArray[(*globalThreeAddrC).dest.varInfo->value.index] = false;}
					globalVariableTable->intInitArray[(*globalThreeAddrC).dest.varInfo->value.index] = true;
				}
				else
				{
					if ((assignValueOp1.intVal == true) || (assignValueOp2.intVal == true))
					{
						localVariableTable->intArray[(*globalThreeAddrC).dest.varInfo->value.index] = true;         
					} else {localVariableTable->intArray[(*globalThreeAddrC).dest.varInfo->value.index] = false;}
					localVariableTable->intInitArray[(*globalThreeAddrC).dest.varInfo->value.index] = true;
				}
				break;}

			case I_XOR:    
				{assignFunction (&assignValueOp1, (*globalThreeAddrC).op1, localVariableTable, globalVariableTable);    
   				assignFunction (&assignValueOp2, (*globalThreeAddrC).op2, localVariableTable, globalVariableTable);    
   				if ((*globalThreeAddrC).dest.varInfo->type == INT_GLOBAL)
   				{
					if ((assignValueOp1.intVal == true) && (assignValueOp2.intVal == true))
					{
						globalVariableTable->intArray[(*globalThreeAddrC).dest.varInfo->value.index] = true;         
					} else {globalVariableTable->intArray[(*globalThreeAddrC).dest.varInfo->value.index] = false;}
					globalVariableTable->intInitArray[(*globalThreeAddrC).dest.varInfo->value.index] = true;
				}
				else
				{
					if ((assignValueOp1.intVal == true && assignValueOp2.intVal == false) || (assignValueOp1.intVal == false && assignValueOp2.intVal == true))
					{
						localVariableTable->intArray[(*globalThreeAddrC).dest.varInfo->value.index] = true;         
					} else {localVariableTable->intArray[(*globalThreeAddrC).dest.varInfo->value.index] = false;}
					localVariableTable->intInitArray[(*globalThreeAddrC).dest.varInfo->value.index] = true;
				}
				break;}

			/* Vypsani promenne (ktera musi byt inicializovana). */
			case I_WRITE_SV:
				if ((*globalThreeAddrC).op1.varInfo->type == STRING_GLOBAL)
				{
					if_initialized(globalVariableTable, charInitArray, (*globalThreeAddrC).op1.varInfo->value.index);
					printf("%s",globalVariableTable->charArray[(*globalThreeAddrC).op1.varInfo->value.index]);
				}
				else if ((*globalThreeAddrC).op1.varInfo->type == STRING_VARIABLE)
				{
					if_initialized(localVariableTable, charInitArray, (*globalThreeAddrC).op1.varInfo->value.index);
					printf("%s",localVariableTable->charArray[(*globalThreeAddrC).op1.varInfo->value.index]);
				}
				break;

			case I_WRITE_RV:
				if ((*globalThreeAddrC).op1.varInfo->type == REAL_GLOBAL)
				{
					if_initialized(globalVariableTable, doubleInitArray, (*globalThreeAddrC).op1.varInfo->value.index);
					printf("%lg",globalVariableTable->doubleArray[(*globalThreeAddrC).op1.varInfo->value.index]);
				}
				else if ((*globalThreeAddrC).op1.varInfo->type == REAL_VARIABLE)
				{
					if_initialized(localVariableTable, doubleInitArray, (*globalThreeAddrC).op1.varInfo->value.index);
					printf("%lg",localVariableTable->doubleArray[(*globalThreeAddrC).op1.varInfo->value.index]);
				}
				break;

			case I_WRITE_IV:
				if ((*globalThreeAddrC).op1.varInfo->type == INT_GLOBAL)
				{
					if_initialized(globalVariableTable, intInitArray, (*globalThreeAddrC).op1.varInfo->value.index);
					printf("%d",globalVariableTable->intArray[(*globalThreeAddrC).op1.varInfo->value.index]);
				}
				else if ((*globalThreeAddrC).op1.varInfo->type == INT_VARIABLE)
				{
					if_initialized(localVariableTable, intInitArray, (*globalThreeAddrC).op1.varInfo->value.index);
					printf("%d",localVariableTable->intArray[(*globalThreeAddrC).op1.varInfo->value.index]);
				}
				break;

			case I_WRITE_BV:
				if ((*globalThreeAddrC).op1.varInfo->type == INT_GLOBAL)
				{       
					if_initialized(globalVariableTable, intInitArray, (*globalThreeAddrC).op1.varInfo->value.index);            
					printf("%s", globalVariableTable->intArray[(*globalThreeAddrC).op1.varInfo->value.index] ? "TRUE" : "FALSE");                   
				}
				else if ((*globalThreeAddrC).op1.varInfo->type == INT_VARIABLE)
				{
					if_initialized(localVariableTable, intInitArray, (*globalThreeAddrC).op1.varInfo->value.index);
					printf("%s", localVariableTable->intArray[(*globalThreeAddrC).op1.varInfo->value.index] ? "TRUE" : "FALSE");                    
				}
				break;

			/* Vypsani konstanty. */
			case I_WRITE_SC:
				printf("%s", (*globalThreeAddrC).op1.varInfo->value.stringVal);
				break;

			case I_WRITE_RC:
				printf("%lg", (*globalThreeAddrC).op1.varInfo->value.realVal);
				break;

			case I_WRITE_IC:
				printf("%d", (*globalThreeAddrC).op1.varInfo->value.intVal);
				break;

			case I_WRITE_BC:
				printf("%s", (*globalThreeAddrC).op1.varInfo->value.intVal ? "TRUE" : "FALSE");
				break;

			/* Nacteni hodnoty ze standardniho vstupu. */
			case I_READLN_I:
				if ((*globalThreeAddrC).op1.varInfo->type == INT_VARIABLE)
				{
					if_scanf(&(localVariableTable)->intArray, localVariableTable->intInitArray, "%d");
				}
				else
				{               
					if_scanf(&(globalVariableTable)->intArray, globalVariableTable->intInitArray, "%d");
				}
				break;

			case I_READLN_R:
				if ((*globalThreeAddrC).op1.varInfo->type == REAL_VARIABLE)
				{
					if_scanf(&(localVariableTable)->doubleArray, localVariableTable->doubleInitArray, "%lf");
				}
				else
				{
					if_scanf(&(globalVariableTable)->doubleArray, globalVariableTable->doubleInitArray, "%lf");
				}               
				break;

			/* Nacteni stringu je ponekud komplikovane - prvni se musi ukladat jednotlive znaky do linearniho seznamu (pocita se delka).
			Nasledne spravce pameti prideli misto pro string o spocitane delce a muze se zrusit linerani seznam. */
			case I_READLN_S:
				{char c;				
				int delka = 0;
				charList *list = NULL;          
				
				while ((c = getchar()) != '\n' && c != EOF)
				{
					if (c != '\n') list = charListInsert(list, c);
					delka++;
				}               
				char *str = getDString(delka+1);
				str[delka] = '\0';
				for (;delka > 0; delka--)
				{
					str[delka-1] = list->c;
					list = list->next;                  
				}
				removeDElement(delka);
				if ((*globalThreeAddrC).op1.varInfo->type == STRING_VARIABLE)
				{
					localVariableTable->charArray[(*globalThreeAddrC).op1.varInfo->value.index] = str;
					localVariableTable->charInitArray[(*globalThreeAddrC).op1.varInfo->value.index] = true;
				}
				else
				{
					globalVariableTable->charArray[(*globalThreeAddrC).op1.varInfo->value.index] = str;
					globalVariableTable->charInitArray[(*globalThreeAddrC).op1.varInfo->value.index] = true;
				}               
				break;}

			/* Instrukce, ktera zmeri delku parametru. */
			case I_CALL_LENGTH:             
				switch ((*globalThreeAddrC).op1.varInfo->type)
				{
					case STRING_GLOBAL:
						if_initialized(globalVariableTable, charInitArray, (*globalThreeAddrC).op1.varInfo->value.index);
						length = strlen(globalVariableTable->charArray[(*globalThreeAddrC).op1.varInfo->value.index]);
						break;

					case STRING_VARIABLE:
						if_initialized(localVariableTable, charInitArray, (*globalThreeAddrC).op1.varInfo->value.index);
						length = strlen(localVariableTable->charArray[(*globalThreeAddrC).op1.varInfo->value.index]);
						break;

					case STRING_CONSTANT:
						length = strlen((*globalThreeAddrC).op1.varInfo->value.stringVal);
						break;

					default:
						break;
				}
				switch ((*globalThreeAddrC).dest.varInfo->type)
				{
					case INT_GLOBAL:
						globalVariableTable->intArray[(*globalThreeAddrC).dest.varInfo->value.index] = length;
						globalVariableTable->intInitArray[(*globalThreeAddrC).dest.varInfo->value.index] = true;
						break;

					case INT_VARIABLE:
						localVariableTable->intArray[(*globalThreeAddrC).dest.varInfo->value.index] = length;
						localVariableTable->intInitArray[(*globalThreeAddrC).dest.varInfo->value.index] = true;
						break;

					default:
						break;
				}
				break;

			/* Vestavena funkce copy prvni nacte hodnoty parametru (string, pocatecni index, delka kopirovaneho substringu) a pote se pomoci
			funkce strncpy zkopiruje pozadovany usek. Take se musi zkontrolovat spravnost parametru. */
			case I_CALL_COPY:
				{char *c = NULL;
				int from = 0;
				int length = 0;            
				for (int i = 0; i<3; i++)
				{
					switch (i)
					{
						case 0:
							copy_switch(c, charArray, charInitArray, stringVal, STRING);
							(*globalThreeAddrC).op2.parameters = (*globalThreeAddrC).op2.parameters->next;
							break;

						case 1:
							copy_switch(from, intArray, intInitArray, intVal, INT);
							(*globalThreeAddrC).op2.parameters = (*globalThreeAddrC).op2.parameters->next;
							break;

						case 2:
							copy_switch(length, intArray, intInitArray, intVal, INT);
							break;
					}					
				}  
				int strlength = strlen(c);  
				if (from < 1 ) from = 1;  
				if (from + length > strlength) length = strlength - from+1;      
				char *str;
				if (from <= strlength && length > 0)
				{
					str = getDString(length +1);
					strncpy(str, &c[from -1], length);
					str[length] = '\0';
				}
				else
				{
					str = getDString(1);
					str[0] = '\0';
				}	
				if ((*globalThreeAddrC).dest.varInfo->type == STRING_GLOBAL)
				{
					globalVariableTable->charArray[(*globalThreeAddrC).dest.varInfo->value.index] = str;
					globalVariableTable->charInitArray[(*globalThreeAddrC).dest.varInfo->value.index] = true;
				}
				else if ((*globalThreeAddrC).dest.varInfo->type == STRING_VARIABLE)
				{
					localVariableTable->charArray[(*globalThreeAddrC).dest.varInfo->value.index] = str;
					localVariableTable->charInitArray[(*globalThreeAddrC).dest.varInfo->value.index] = true;
				}				
				break;}

			/* Prvni se nactou a zkontrolujou parametry, nasledne je zavolana funkce pro vyhledani podretezce v retezci. */
			case I_CALL_FIND:
				{char *param1 = NULL;
				char *param2 = NULL;
				copy_switch(param1, charArray, charInitArray, stringVal, STRING);
				(*globalThreeAddrC).op2.parameters = (*globalThreeAddrC).op2.parameters->next;
				copy_switch(param2, charArray, charInitArray, stringVal, STRING);
				if ((*globalThreeAddrC).dest.varInfo->type == INT_GLOBAL)
				{
					globalVariableTable->intArray[(*globalThreeAddrC).dest.varInfo->value.index] = kmp_search(param1,param2);
					globalVariableTable->intInitArray[(*globalThreeAddrC).dest.varInfo->value.index] = true;
				}
				else if ((*globalThreeAddrC).dest.varInfo->type == INT_VARIABLE)
				{
					localVariableTable->intArray[(*globalThreeAddrC).dest.varInfo->value.index] = kmp_search(param1,param2);
					localVariableTable->intInitArray[(*globalThreeAddrC).dest.varInfo->value.index] = true;
				}
				break;}

			/* Prvni se nacte a zkontroluje parametr, nasledne je zavolana funkce pro razeni (quicksort). */
			case I_CALL_SORT:
				{char *param1 = NULL;
				copy_switch(param1, charArray, charInitArray, stringVal, STRING);
				if ((*globalThreeAddrC).dest.varInfo->type == STRING_GLOBAL)
				{
					globalVariableTable->charArray[(*globalThreeAddrC).dest.varInfo->value.index] = sort(param1);
					globalVariableTable->charInitArray[(*globalThreeAddrC).dest.varInfo->value.index] = true;
				}
				else if ((*globalThreeAddrC).dest.varInfo->type == STRING_VARIABLE)
				{
					localVariableTable->charArray[(*globalThreeAddrC).dest.varInfo->value.index] = sort(param1);
					localVariableTable->charInitArray[(*globalThreeAddrC).dest.varInfo->value.index] = true;
				}
				break;}

			/* Logicka operace and. Pokud jsou oba operandy true, cil je take true (samozrejme se musi nastavit inicializace cile) */
			case I_ASSIGN:
				assignFunction (&assignValue, (*globalThreeAddrC).op1, localVariableTable, globalVariableTable);
				switch ((*globalThreeAddrC).dest.varInfo->type)
				{
					case INT_VARIABLE:
						localVariableTable->intArray[(*globalThreeAddrC).dest.varInfo->value.index] = assignValue.intVal;
						localVariableTable->intInitArray[(*globalThreeAddrC).dest.varInfo->value.index] = true;
						break;

					case INT_GLOBAL:
						globalVariableTable->intArray[(*globalThreeAddrC).dest.varInfo->value.index] = assignValue.intVal;
						globalVariableTable->intInitArray[(*globalThreeAddrC).dest.varInfo->value.index] = true;
						break;
					case REAL_VARIABLE:
						localVariableTable->doubleArray[(*globalThreeAddrC).dest.varInfo->value.index] = assignValue.realVal;
						localVariableTable->doubleInitArray[(*globalThreeAddrC).dest.varInfo->value.index] = true;
						break;

					case REAL_GLOBAL:
						globalVariableTable->doubleArray[(*globalThreeAddrC).dest.varInfo->value.index] = assignValue.realVal;
						globalVariableTable->doubleInitArray[(*globalThreeAddrC).dest.varInfo->value.index] = true;
						break;                  

					case STRING_VARIABLE:
						localVariableTable->charArray[(*globalThreeAddrC).dest.varInfo->value.index] = assignValue.stringVal;
						localVariableTable->charInitArray[(*globalThreeAddrC).dest.varInfo->value.index] = true;
						break;

					case STRING_GLOBAL:
						globalVariableTable->charArray[(*globalThreeAddrC).dest.varInfo->value.index] = assignValue.stringVal;
						globalVariableTable->charInitArray[(*globalThreeAddrC).dest.varInfo->value.index] = true;
						break;
					default:
						break;
				}
				break;

			/* Scitani - nactou a zkontroluji se obe promenne hodnoty, nasledne se provede soucet. */
			case I_ADD:         
				assignFunction (&assignValueOp1, (*globalThreeAddrC).op1, localVariableTable, globalVariableTable);
				assignFunction (&assignValueOp2, (*globalThreeAddrC).op2, localVariableTable, globalVariableTable);                     
				assignDest(+);
				break;      

			case I_SUB:             
				assignFunction (&assignValueOp1, (*globalThreeAddrC).op1, localVariableTable, globalVariableTable);
				assignFunction (&assignValueOp2, (*globalThreeAddrC).op2, localVariableTable, globalVariableTable);
				assignDest (-);
				break;  

			case I_MULT:                
				assignFunction (&assignValueOp1, (*globalThreeAddrC).op1, localVariableTable, globalVariableTable);
				assignFunction (&assignValueOp2, (*globalThreeAddrC).op2, localVariableTable, globalVariableTable);
				assignDest (*);
				break;  

			/* Pri deleni se navic musi overit, zda nedelime nulou. */
			case I_DIV:             
				assignFunction (&assignValueOp1, (*globalThreeAddrC).op1, localVariableTable, globalVariableTable);
				assignFunction (&assignValueOp2, (*globalThreeAddrC).op2, localVariableTable, globalVariableTable);
				if ((globalThreeAddrC->op1.varInfo->type % 3 == REAL_CONSTANT && assignValueOp2.realVal == 0) || (globalThreeAddrC->op1.varInfo->type % 3 == INT_CONSTANT && assignValueOp2.intVal == 0))
				{
					errCode = DIVISION_BY_ZERO;
					fatalError("Nelze delit nulou!");
				}               
				assignDest(/);
				break;

			/* Logicka operace not. Do cilove promenne se nacte opacna hodnota. */
			case I_NOT:
				assignFunction (&assignValueOp1, (*globalThreeAddrC).op1, localVariableTable, globalVariableTable);
				if(globalThreeAddrC->dest.varInfo->type == INT_GLOBAL)
				{
					globalVariableTable->intInitArray[globalThreeAddrC->dest.varInfo->value.index] = true;
					globalVariableTable->intArray[globalThreeAddrC->dest.varInfo->value.index] = assignValueOp1.intVal ? false : true;
				}
				else //INT_VARIABLE
				{
					localVariableTable->intInitArray[globalThreeAddrC->dest.varInfo->value.index] = true;
					localVariableTable->intArray[globalThreeAddrC->dest.varInfo->value.index] = assignValueOp1.intVal ? false : true;
				}
				break;

			case I_MORE:                    
				compare(>); 
				break;

			case I_LESS:                
				compare(<); 
				break;

			case I_EQUAL:
				compare(==);
				break;

			case I_NOT_EQUAL:
				compare(!=);
				break;

			case I_LESS_OR_EQUAL:
				compare(<=);
				break;

			case I_MORE_OR_EQUAL:
				compare(>=);
				break;

			default:                
				break;
		}
		globalThreeAddrC = globalThreeAddrC->next;          
	}   
}



parameterValT getIntConst(variableInfoT * var, variableTableT *local, variableTableT *global)
{
	(void)local;
	(void)global;
	parameterValT retVal;
	retVal.intVal = var->value.intVal;
	return retVal;
}
parameterValT getStringConst(variableInfoT * var, variableTableT *local, variableTableT *global)
{
	(void)local;
	(void)global;
	parameterValT retVal;
	retVal.stringVal = var->value.stringVal;
	return retVal;
}
parameterValT getRealConst(variableInfoT * var, variableTableT *local, variableTableT *global)
{
	(void)local;
	(void)global;
	parameterValT retVal;
	retVal.realVal = var->value.realVal;
	return retVal;
}

parameterValT getIntVar(variableInfoT * var, variableTableT *local, variableTableT *global)
{
	(void)global;
	if_initialized(local, intInitArray, var->value.index);  
	parameterValT retVal;
	retVal.intVal = local->intArray[var->value.index];
	return retVal;
}
parameterValT getStringVar(variableInfoT * var, variableTableT *local, variableTableT *global)
{
	(void)global;
	if_initialized(local, charInitArray, var->value.index);
	parameterValT retVal;
	retVal.stringVal = local->charArray[var->value.index];
	return retVal;
}
parameterValT getRealVar(variableInfoT * var, variableTableT *local, variableTableT *global)
{
	(void)global;
	if_initialized(local, doubleInitArray, var->value.index);
	parameterValT retVal;
	retVal.realVal = local->doubleArray[var->value.index];
	return retVal;
}

parameterValT getIntGlob(variableInfoT * var, variableTableT *local, variableTableT *global)
{
	(void)local;
	if_initialized(global, intInitArray, var->value.index);
	parameterValT retVal;
	retVal.intVal = global->intArray[var->value.index];
	return retVal;
}
parameterValT getStringGlob(variableInfoT * var, variableTableT *local, variableTableT *global)
{
	(void)local;
	if_initialized(global, charInitArray, var->value.index);
	parameterValT retVal;
	retVal.stringVal = global->charArray[var->value.index];
	return retVal;
}
parameterValT getRealGlob(variableInfoT * var, variableTableT *local, variableTableT *global)
{
	(void)local;
	if_initialized(global, doubleInitArray, var->value.index);
	parameterValT retVal;
	retVal.realVal = global->doubleArray[var->value.index];
	return retVal;
}

typedef parameterValT (*getVarFunctionT)(variableInfoT * , variableTableT *, variableTableT *);

getVarFunctionT getVarFunction[] = {
	&getIntConst, &getStringConst, &getRealConst,
	&getIntVar, &getStringVar, &getRealVar,
	&getIntGlob, &getStringGlob, &getRealGlob
};

#define getVarValue(var, local, global) ((getVarFunction[var->type])(var, local, global))

// parameterValT getVarValue(variableInfoT * var, variableTableT *local, variableTableT *global) 
// {
//  getParamsT params;
//  params.var = var;
//  params.local = local;
//  params.global = global;
//  return ((getVarFunction[var->type])(params));
// }

/* Funkce, ktera naplni nove vytvorenou tabulku lokalnich promennych parametry, ktere byly predany volanim funkce. */
void variableTableFill(variableTableT *localVariableTable, variableTableT *globalVariableTable)
{
	paramListItemT * paramNew = globalThreeAddrC->op1.function->params->next;
	parameterT * paramOld = globalThreeAddrC->op2.parameters;

	while (paramNew != NULL)
	{       
		switch (paramOld->data->type % 3)
		{
			case REAL_CONSTANT:
				localVariableTable->doubleArray[paramNew->index] = getVarValue(paramOld->data, localVariableTable->previous, globalVariableTable).realVal;
				localVariableTable->doubleInitArray[paramNew->index] = true;
				break;

			case STRING_CONSTANT:
				localVariableTable->charArray[paramNew->index] = getVarValue(paramOld->data, localVariableTable->previous, globalVariableTable).stringVal;
				localVariableTable->charInitArray[paramNew->index] = true;
				break;

			case INT_CONSTANT:
				localVariableTable->intArray[paramNew->index] = getVarValue(paramOld->data, localVariableTable->previous, globalVariableTable).intVal;
				localVariableTable->intInitArray[paramNew->index] = true;
				break;
		}
		paramNew = paramNew->next;
		paramOld = paramOld->next;
	}
}

/* Funkce, ktera ulozi aktualni instrukci na vrchol zasobniku. Spravce pameti vrati ukazatel na volnou pamet, kam se novy prvek ulozi. */
threeAddressStackT* threeAddressStackPush(threeAddressStackT * Stack)
{
	threeAddressStackT *tmp;
	tmp = getDTAS();        
	(*tmp).code = globalThreeAddrC;
	(*tmp).next = Stack;
	return tmp;
}

/* Funkce, ktera vlozi novy znak do seznamu znaku (pri dynamickem cteni stringu ze standardniho vstupu). */
charList* charListInsert(charList *list, char c)
{
	charList *tmp;
	tmp = getDElement();
	tmp->c = c;
	tmp->next = list;
	return tmp;
}

/* Funkce, ktera nastavi zasobnik na instrukci o jedno niz a pozada spravce pameti o uvolneni nejvyssi instrukce. */
void threeAddressStackPop(threeAddressStackT ** Stack)
{
	*Stack = (*Stack)->next;
	removeDTAS(1);  
}

/* Alokace nove tabulky promennych. V globalni promenne actualFunction je ulozeny pocet veskerych promennych, se kteryma se
ma v dane funkci pracovat. */
variableTableT* variableTableAlloc(variableTableT* previous)
{
	variableTableT *tmp = getDVT();
	tmp->intArray = getDInt(actualFunction->intTmpCount);
	tmp->doubleArray = getDFloat(actualFunction->realTmpCount);
	tmp->charArray = getDChar(actualFunction->stringTmpCount);	
	tmp->intInitArray = getDInt(actualFunction->intTmpCount);
	tmp->doubleInitArray = getDInt(actualFunction->realTmpCount);
	tmp->charInitArray = getDInt(actualFunction->stringTmpCount);
	memset(tmp->intInitArray, 0, sizeof((tmp->intInitArray)) * actualFunction->intTmpCount);
	memset(tmp->doubleInitArray, 0, sizeof((tmp->doubleInitArray)) * actualFunction->realTmpCount);
	memset(tmp->charInitArray, 0, sizeof((tmp->charInitArray)) * actualFunction->stringTmpCount);   
	(tmp)->previous =previous;
	return (tmp);
}

/* Pri returnu se tabulka lokalnich promennych rusi, je treba uvolnit misto vsech promennych pouzivanych ve funkci */
void variableTableDestroy(variableTableT **localVariableTable)
{	
	removeDInt(2*(actualFunction->intTmpCount) + actualFunction->realTmpCount + actualFunction->stringTmpCount);
	removeDFloat(actualFunction->realTmpCount);
	removeDChar(actualFunction->stringTmpCount);	
	*localVariableTable = (*localVariableTable)->previous;
	removeDVT(1);	
}

/* Funkce, ktera zkontroluje, zda je promenna, se kterou chceme pracovat, inicializovana. Pokud ano, je jeji hodnota ulozena do assignValue. */
void assignFunction (parameterValT* assignValue, operandT operand, variableTableT* localVariableTable, variableTableT* globalVariableTable)
{    
	switch (operand.varInfo->type)
	{
		case INT_VARIABLE:
			if_initialized(localVariableTable, intInitArray, operand.varInfo->value.index);
			(*assignValue).intVal = localVariableTable->intArray[operand.varInfo->value.index];         
			break;

		case INT_GLOBAL:
			if_initialized(globalVariableTable, intInitArray, operand.varInfo->value.index);
			(*assignValue).intVal = globalVariableTable->intArray[operand.varInfo->value.index];            
			break;

		case INT_CONSTANT:
			(*assignValue).intVal = operand.varInfo->value.intVal;
			break;

		case REAL_VARIABLE:
			if_initialized(localVariableTable, doubleInitArray, operand.varInfo->value.index);
			(*assignValue).realVal = localVariableTable->doubleArray[operand.varInfo->value.index];
			break;

		case REAL_GLOBAL:
			if_initialized(globalVariableTable, doubleInitArray, operand.varInfo->value.index);
			(*assignValue).realVal = globalVariableTable->doubleArray[operand.varInfo->value.index];    
			break;

		case REAL_CONSTANT:
			(*assignValue).realVal = operand.varInfo->value.realVal;        
			break;

		case STRING_VARIABLE:
			if_initialized(localVariableTable, charInitArray, operand.varInfo->value.index);
			(*assignValue).stringVal = localVariableTable->charArray[operand.varInfo->value.index];
			break;

		case STRING_GLOBAL:
			if_initialized(globalVariableTable, charInitArray, operand.varInfo->value.index);
			(*assignValue).stringVal = globalVariableTable->charArray[operand.varInfo->value.index];
			break;

		case STRING_CONSTANT:
			(*assignValue).stringVal = operand.varInfo->value.stringVal;                        
			break;
	}
}
