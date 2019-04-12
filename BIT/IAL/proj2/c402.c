
/* c402.c: ********************************************************************}
{* Téma: Nerekurzivní implementace operací nad BVS 
**                                     Implementace: Petr Pøikryl, prosinec 1994
**                                           Úpravy: Petr Pøikryl, listopad 1997
**                                                     Petr Pøikryl, kvìten 1998
**			  	                        Pøevod do jazyka C: Martin Tuèek, srpen 2005
**                                         Úpravy: Bohuslav Køena, listopad 2009
**                                         Úpravy: Karel Masaøík, øíjen 2013
**                                         Úpravy: Radek Hranický, øíjen 2014
**
** S vyu¾itím dynamického pøidìlování pamìti, implementujte NEREKURZIVNÌ
** následující operace nad binárním vyhledávacím stromem (pøedpona BT znamená
** Binary Tree a je u identifikátorù uvedena kvùli mo¾né kolizi s ostatními
** pøíklady):
**
**     BTInit .......... inicializace stromu
**     BTInsert ........ nerekurzivní vlo¾ení nového uzlu do stromu
**     BTPreorder ...... nerekurzivní prùchod typu pre-order
**     BTInorder ....... nerekurzivní prùchod typu in-order
**     BTPostorder ..... nerekurzivní prùchod typu post-order
**     BTDisposeTree ... zru¹ v¹echny uzly stromu
**
** U v¹ech funkcí, které vyu¾ívají nìkterý z prùchodù stromem, implementujte
** pomocnou funkci pro nalezení nejlevìj¹ího uzlu v podstromu.
**
** Pøesné definice typù naleznete v souboru c402.h. Uzel stromu je typu tBTNode,
** ukazatel na nìj je typu tBTNodePtr. Jeden uzel obsahuje polo¾ku int Cont,
** která souèasnì slou¾í jako u¾iteèný obsah i jako vyhledávací klíè 
** a ukazatele na levý a pravý podstrom (LPtr a RPtr).
**
** Pøíklad slou¾í zejména k procvièení nerekurzivních zápisù algoritmù
** nad stromy. Ne¾ zaènete tento pøíklad øe¹it, prostudujte si dùkladnì
** principy pøevodu rekurzivních algoritmù na nerekurzivní. Programování
** je pøedev¹ím in¾enýrská disciplína, kde opìtné objevování Ameriky nemá
** místo. Pokud se Vám zdá, ¾e by nìco ¹lo zapsat optimálnìji, promyslete
** si v¹echny detaily Va¹eho øe¹ení. Pov¹imnìte si typického umístìní akcí
** pro rùzné typy prùchodù. Zamyslete se nad modifikací øe¹ených algoritmù
** napøíklad pro výpoèet poètu uzlù stromu, poètu listù stromu, vý¹ky stromu
** nebo pro vytvoøení zrcadlového obrazu stromu (pouze popøehazování ukazatelù
** bez vytváøení nových uzlù a ru¹ení starých).
**
** Pøi prùchodech stromem pou¾ijte ke zpracování uzlu funkci BTWorkOut().
** Pro zjednodu¹ení práce máte pøedem pøipraveny zásobníky pro hodnoty typu
** bool a tBTNodePtr. Pomocnou funkci BTWorkOut ani funkce pro práci
** s pomocnými zásobníky neupravujte 
** Pozor! Je tøeba správnì rozli¹ovat, kdy pou¾ít dereferenèní operátor *
** (typicky pøi modifikaci) a kdy budeme pracovat pouze se samotným ukazatelem 
** (napø. pøi vyhledávání). V tomto pøíkladu vám napoví prototypy funkcí.
** Pokud pracujeme s ukazatelem na ukazatel, pou¾ijeme dereferenci.
**/

#include "c402.h"
int solved;

void BTWorkOut (tBTNodePtr Ptr)		{
/*   ---------
** Pomocná funkce, kterou budete volat pøi prùchodech stromem pro zpracování
** uzlu urèeného ukazatelem Ptr. Tuto funkci neupravujte.
**/
			
	if (Ptr==NULL) 
    printf("Chyba: Funkce BTWorkOut byla volána s NULL argumentem!\n");
  else 
    printf("Výpis hodnoty daného uzlu> %d\n",Ptr->Cont);
}
	
/* -------------------------------------------------------------------------- */
/*
** Funkce pro zásobník hotnot typu tBTNodePtr. Tyto funkce neupravujte.
**/

void SInitP (tStackP *S)  
/*   ------
** Inicializace zásobníku.
**/
{
	S->top = 0;  
}	

void SPushP (tStackP *S, tBTNodePtr ptr)
/*   ------
** Vlo¾í hodnotu na vrchol zásobníku.
**/
{ 
                 /* Pøi implementaci v poli mù¾e dojít k pøeteèení zásobníku. */
  if (S->top==MAXSTACK) 
    printf("Chyba: Do¹lo k pøeteèení zásobníku s ukazateli!\n");
  else {  
		S->top++;  
		S->a[S->top]=ptr;
	}
}	

tBTNodePtr STopPopP (tStackP *S)
/*         --------
** Odstraní prvek z vrcholu zásobníku a souèasnì vrátí jeho hodnotu.
**/
{
                            /* Operace nad prázdným zásobníkem zpùsobí chybu. */
	if (S->top==0)  {
		printf("Chyba: Do¹lo k podteèení zásobníku s ukazateli!\n");
		return(NULL);	
	}	
	else {
		return (S->a[S->top--]);
	}	
}

bool SEmptyP (tStackP *S)
/*   -------
** Je-li zásobník prázdný, vrátí hodnotu true.
**/
{
  return(S->top==0);
}	

/* -------------------------------------------------------------------------- */
/*
** Funkce pro zásobník hotnot typu bool. Tyto funkce neupravujte.
*/

void SInitB (tStackB *S) {
/*   ------
** Inicializace zásobníku.
**/

	S->top = 0;  
}	

void SPushB (tStackB *S,bool val) {
/*   ------
** Vlo¾í hodnotu na vrchol zásobníku.
**/
                 /* Pøi implementaci v poli mù¾e dojít k pøeteèení zásobníku. */
	if (S->top==MAXSTACK) 
		printf("Chyba: Do¹lo k pøeteèení zásobníku pro boolean!\n");
	else {
		S->top++;  
		S->a[S->top]=val;
	}	
}

bool STopPopB (tStackB *S) {
/*   --------
** Odstraní prvek z vrcholu zásobníku a souèasnì vrátí jeho hodnotu.
**/
                            /* Operace nad prázdným zásobníkem zpùsobí chybu. */
	if (S->top==0) {
		printf("Chyba: Do¹lo k podteèení zásobníku pro boolean!\n");
		return(NULL);	
	}	
	else {  
		return(S->a[S->top--]); 
	}	
}

bool SEmptyB (tStackB *S) {
/*   -------
** Je-li zásobník prázdný, vrátí hodnotu true.
**/
  return(S->top==0);
}

/* -------------------------------------------------------------------------- */
/*
** Následuje jádro domácí úlohy - funkce, které máte implementovat. 
*/

void BTInit (tBTNodePtr *RootPtr)	{
/*   ------
** Provede inicializaci binárního vyhledávacího stromu.
**
** Inicializaci smí programátor volat pouze pøed prvním pou¾itím binárního
** stromu, proto¾e neuvolòuje uzly neprázdného stromu (a ani to dìlat nemù¾e,
** proto¾e pøed inicializací jsou hodnoty nedefinované, tedy libovolné).
** Ke zru¹ení binárního stromu slou¾í procedura BTDisposeTree.
**	
** V¹imnìte si, ¾e zde se poprvé v hlavièce objevuje typ ukazatel na ukazatel,	
** proto je tøeba pøi práci s RootPtr pou¾ít dereferenèní operátor *.
**/
	*RootPtr = NULL;	
	
}

void BTInsert (tBTNodePtr *RootPtr, int Content) {
/*   --------
** Vlo¾í do stromu nový uzel s hodnotou Content.
**
** Z pohledu vkládání chápejte vytváøený strom jako binární vyhledávací strom,
** kde uzly s hodnotou men¹í ne¾ má otec le¾í v levém podstromu a uzly vìt¹í
** le¾í vpravo. Pokud vkládaný uzel ji¾ existuje, neprovádí se nic (daná hodnota
** se ve stromu mù¾e vyskytnout nejvý¹e jednou). Pokud se vytváøí nový uzel,
** vzniká v¾dy jako list stromu. Funkci implementujte nerekurzivnì.
**/	
	
		if (*RootPtr == NULL)
		{
			if ((*RootPtr = malloc(sizeof(struct tBTNode))) != NULL)
			{
				/* Pokud je strom prazdny a povede se alokace */
				(*RootPtr)->Cont = Content;
				(*RootPtr)->LPtr = NULL;
				(*RootPtr)->RPtr = NULL;
			}
		}
		else
		{
			/* Pokud strom neni prazdny */
			tBTNodePtr tmp = *RootPtr;
			do
			{
				if (tmp->Cont > Content)
				{
					if (tmp->LPtr == NULL)
					{
						/* Pokud je obsah vkladaneho prvku mensi nez obsah aktualniho prvku, a pokud neexistuje levy podstrom,
						vlozi se prvek jako levy podstrom a cyklus skonci. */
						if ((tmp->LPtr = malloc(sizeof(struct tBTNode))) != NULL)
						{
							tmp->LPtr->Cont = Content;
							tmp->LPtr->LPtr = NULL;
							tmp->LPtr->RPtr = NULL;
						}
						break;
					}
					else
					/* Pokud existuje levy podstrom, jede cyklus odznova s levym podstromem jako aktualnim prvkem. */
					tmp = tmp->LPtr;					
				}
				else if (tmp->Cont < Content)
				{
					if (tmp->RPtr == NULL)
					{
						/* Pokud je obsah vkladaneho prvku vetsi nez obsah aktualniho prvku, a pokud neexistuje pravy podstrom,
						vlozi se prvek jako pravy podstrom a cyklus skonci. */
						if ((tmp->RPtr = malloc(sizeof(struct tBTNode))) != NULL)
						{
							tmp->RPtr->Cont = Content;
							tmp->RPtr->LPtr = NULL;
							tmp->RPtr->RPtr = NULL;
						}
						break;
					}
					else
					/* Pokud existuje pravy podstrom, jede cyklus odznova s pravym podstromem jako aktualnim prvkem. */
					tmp = tmp->RPtr;
				}
				else break;
			} while (true);			
		}	
}
	

/*                                  PREORDER                                  */

void Leftmost_Preorder (tBTNodePtr ptr, tStackP *Stack)	{
/*   -----------------
** Jde po levì vìtvi podstromu, dokud nenarazí na jeho nejlevìj¹í uzel.
**
** Pøi prùchodu Preorder nav¹tívené uzly zpracujeme voláním funkce BTWorkOut()
** a ukazatele na nì is ulo¾íme do zásobníku.
**/
	if (ptr != NULL)
	{
		do
		{
			/* Do zasobniku se uklada ukazatel na prochazeny prvek. */
			SPushP(Stack, ptr);
			/* Vypis prvku */
			BTWorkOut(ptr);
			if (ptr->LPtr != NULL)
			{
				/* Pokud existuje pravy podstrom, opakuje se cyklus s nim. Pokud ne, cyklus skonci. */
				ptr = ptr->LPtr;
			}
			else break;
		} while (true);		
	}
}

void BTPreorder (tBTNodePtr RootPtr)	{
/*   ----------
** Prùchod stromem typu preorder implementovaný nerekurzivnì s vyu¾itím funkce
** Leftmost_Preorder a zásobníku ukazatelù. Zpracování jednoho uzlu stromu
** realizujte jako volání funkce BTWorkOut(). 
**/
	if (RootPtr != NULL)
	{	
		tStackP *Stack = malloc(sizeof(tStackP));
		SInitP(Stack);
		Leftmost_Preorder(RootPtr, Stack);
		/* Nalezne se nejlevejsi prvek */
		do
		{	
			/* Pracuje se s prvkem, ktery je na vrcholu zasobniku */		
			RootPtr = STopPopP(Stack);			
			while ((RootPtr->RPtr == NULL) && (SEmptyP(Stack) == FALSE))
			{
				/* Pokud prvek nema pravy podstrom a zasobnik neni prazdny, prochazi se smerem mahoru */
				RootPtr = STopPopP(Stack);
			}
			if (RootPtr->RPtr != NULL) Leftmost_Preorder(RootPtr->RPtr, Stack);	
			/* Pokud ma prvek pravy podstrom, bude se v tomto podstromu hledat opet nejlevejsi prvek. */						
		} while (SEmptyP(Stack) == FALSE);
		free(Stack);
	}
	
}



/*                                  INORDER                                   */ 

void Leftmost_Inorder(tBTNodePtr ptr, tStackP *Stack)		{
/*   ----------------
** Jde po levì vìtvi podstromu, dokud nenarazí na jeho nejlevìj¹í uzel.
**
** Pøi prùchodu Inorder ukládáme ukazatele na v¹echny nav¹tívené uzly do
** zásobníku. 
**/
	if (ptr != NULL)
	{
		do
		{
			SPushP(Stack, ptr);
			if (ptr->LPtr != NULL)
			{
				ptr = ptr->LPtr;
			}
			else break;
		} while (true);		
	}
}

void BTInorder (tBTNodePtr RootPtr)	{
/*   ---------
** Prùchod stromem typu inorder implementovaný nerekurzivnì s vyu¾itím funkce
** Leftmost_Inorder a zásobníku ukazatelù. Zpracování jednoho uzlu stromu
** realizujte jako volání funkce BTWorkOut(). 
**/
	if (RootPtr != NULL)
	{
		tStackP *Stack = malloc(sizeof(tStackP));
		SInitP(Stack);
		Leftmost_Inorder(RootPtr, Stack);
		/* Nalezeni nejlevejsiho prvku */
		do
		{
			/* Pracuje se s prvkem z vrcholu zasobniku */
			RootPtr = STopPopP(Stack);	
			BTWorkOut(RootPtr);
			while ((RootPtr->RPtr == NULL) && (SEmptyP(Stack) == FALSE))
			{
				/* Prochazi se stromem smerem nahoru a vypisuji se prvky, pokud neexistuje pravy podstrom prvku. */
				RootPtr = STopPopP(Stack);
				BTWorkOut(RootPtr);
			}
			if (RootPtr->RPtr != NULL) Leftmost_Inorder(RootPtr->RPtr, Stack);
			/* Pokud existuje pravy podstrom, najde se v nem nejlevejsi prvek */
		} while (SEmptyP(Stack) == FALSE);
		free(Stack);
	}	
}

/*                                 POSTORDER                                  */ 

void Leftmost_Postorder (tBTNodePtr ptr, tStackP *StackP, tStackB *StackB) {
/*           --------
** Jde po levì vìtvi podstromu, dokud nenarazí na jeho nejlevìj¹í uzel.
**
** Pøi prùchodu Postorder ukládáme ukazatele na nav¹tívené uzly do zásobníku
** a souèasnì do zásobníku bool hodnot ukládáme informaci, zda byl uzel
** nav¹tíven poprvé a ¾e se tedy je¹tì nemá zpracovávat. 
**/
	if (ptr != NULL)
	{
		do
		{
			/* Hodnota FALSE znaci, ze se prvek jeste nema vypsat. */
			SPushP(StackP, ptr);
			SPushB(StackB, FALSE);
			if (ptr->LPtr != NULL)
			{
				ptr = ptr->LPtr;
			}
			else break;			
		} while (StackP->top!=MAXSTACK);		
	}
}

void BTPostorder (tBTNodePtr RootPtr)	{
/*           -----------
** Prùchod stromem typu postorder implementovaný nerekurzivnì s vyu¾itím funkce
** Leftmost_Postorder, zásobníku ukazatelù a zásobníku hotdnot typu bool.
** Zpracování jednoho uzlu stromu realizujte jako volání funkce BTWorkOut(). 
**/
	if (RootPtr != NULL)
	{	
		tStackP *StackP = malloc(sizeof(tStackP));
		tStackB *StackB = malloc(sizeof(tStackB));
		SInitP(StackP);
		SInitB(StackB);
		Leftmost_Postorder(RootPtr, StackP, StackB);
		do
		{
			/* Pracuje se s prvkem na vrcholu zasobniku */
			RootPtr = STopPopP(StackP);
			if (STopPopB(StackB))
				{
					/* Pokud k prvku vypadne z boolovskeho zasobniku TRUE, prvek se vypise a cyklus jede odznova */
					BTWorkOut(RootPtr);
					continue;
				}
			if ((RootPtr->RPtr != NULL))
			{
				/* Pokud existuje pravy podstrom prvku, ulozi se ukazatel tohoto prvku do zasobniku spolu s hodnotou
				TRUE, ktera rika, ze pri pristim popu se tento prvek vypise. Nasledne se hleda nejlevejsi prvek.
				 */
				SPushB(StackB, TRUE);
				SPushP(StackP, RootPtr);
				Leftmost_Postorder(RootPtr->RPtr, StackP, StackB);				
			}
			else if ((RootPtr->RPtr == NULL)) BTWorkOut(RootPtr);
			/* Pokud nema prvek pravy podstrom, vypise se */
		} while (SEmptyP(StackP) == FALSE);
		free(StackP);
		free(StackB);
	}		
}

void BTDisposeTree (tBTNodePtr *RootPtr)	{
/*   -------------
** Zru¹í v¹echny uzly stromu a korektnì uvolní jimi zabranou pamì».
**
** Funkci implementujte nerekurzivnì s vyu¾itím zásobníku ukazatelù.
**/
	if (*RootPtr != NULL)
	{
		tStackP *Stack = malloc(sizeof(tStackP));
		Leftmost_Inorder(*RootPtr, Stack);
		/* Hleda se nejlevejsi prvek */
		while (SEmptyP(Stack) == FALSE)
		{
			/* Cyklus jede do te doby, nez je prazdny zasobnik */
			tBTNodePtr tmp = STopPopP(Stack);			
			if (tmp == NULL)
			{
				*RootPtr = NULL;
				return;
			} 			
			if (tmp->RPtr != NULL)
			{
				/* Pokud existuje pravy podstrom, nalezne se v nem nejlevejsi prvek */
				SPushP(Stack, tmp);				
				Leftmost_Inorder((tmp)->RPtr, Stack);
				tmp->RPtr = NULL;
			}
			else if (tmp != NULL)
			{				
				free(tmp);
				tmp = NULL;
			} 
		}		
		free(Stack);
	}
	*RootPtr = NULL;	
}
	
	
	
	


/* konc c402.c */

