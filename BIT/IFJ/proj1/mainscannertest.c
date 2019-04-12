/**
 * Projekt: Implementace interpretu imperativního jazyka IFJ14
 */
/**
 * @file mainscanertest.h
 * @author Michal Janousek(xjanou15)
 * @brief Testovací soubor pro scanner
 */

#include <stdio.h>
#include <stdlib.h>

#include "globalvariables.h"
#include "scanner.h"
#include "debug.h"
#include "errors.h"

#define getTokenErr()     \
   do                    \
   {                     \
      if (!getToken()){  \
         printf("-----------------Lexikální chyba--------------------\n"); \
         return errCode;   \
      }\
   } while (0)

/**
 * ------------ GLOBÁLNÍ PROMĚNNÉ -------------------
 */

int main(int argc, char const *argv[])
{
	FILE *f;

   if (argc == 1)
   {
      errCode = FILE_ERROR;
      fatalError("Neni zadan vstupni soubor\n");
   }
   if ((f = fopen(argv[1], "r")) == NULL)
   {
      errCode = FILE_ERROR;
      fatalError("Soubor se nepodarilo otevrit\n");
   }   
   setSourceFile(f);

   do{
      getTokenErr();

      switch(actToken.type){
         case ID:
            printf("%s : ", actToken.value.idVal);
            break;
         case STRING:
            printf("-->%s<-- : ", actToken.value.stringVal);
            break;
         case INTEGER:
            printf("%d : ", actToken.value.integerVal);
            break;
         case REAL:
            printf("%g : ", actToken.value.realVal);
            break;
         case BOOLEAN:
            printf("%s : ", actToken.value.booleanVal ? "true" : "false");
            break;
         default:
            break;
      }
      printf("%s",tokenRewrite[actToken.type] );
      printf("\n");

   }while(actToken.type != FILE_END);

	return 0;
}
