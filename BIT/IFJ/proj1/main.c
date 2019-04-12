 /**
  * Projekt: Implementace interpretu imperativního jazyka IFJ14
  */
 /**
  * @file main.c
  * @author Michal Janousek(xjanou15), Antonin Cala(xcalaa00), Jan Pawlus(xpawlu00), Zdenek Studeny(xstude21), Martin Nosek(xnosek10)
  * @brief main
  */

#include <stdio.h>
#include <stdlib.h>

#include "globalvariables.h"
#include "scanner.h"
#include "debug.h"
#include "errors.h"
#include "parser.h"
#include "interpret.h"
#include "rewritecode.h"

#define INITDSTRING_SIZE 1024
#define INITDTAC_SIZE 64
#define INITDSELEM_SIZE 16
#define INITDTELEM_SIZE 256

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
   initDString(INITDSTRING_SIZE);
   initDTAC(INITDTAC_SIZE);
   initDSElem(INITDSELEM_SIZE);
   initDTElem(INITDTELEM_SIZE);

   if(!isProgram())
   {
      if(errCode == SYNTAX_ERROR)
         fatalError("V programu se objevila syntakticka chyba\n");
      else
         fatalError("V programu se objevila jina chyba\n");
   }

   actualFunction = findFunction("",functionTable);
   
   globalThreeAddrC = actualFunction->beginning->next;

   releaseDSTV();

   // writeCode(globalThreeAddrC);

   interpret();

   releaseAll();

	return 0;
}
