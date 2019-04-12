
/* c402.c: ********************************************************************}
{* T�ma: Nerekurzivn� implementace operac� nad BVS 
**                                     Implementace: Petr P�ikryl, prosinec 1994
**                                           �pravy: Petr P�ikryl, listopad 1997
**                                                     Petr P�ikryl, kv�ten 1998
**			  	                        P�evod do jazyka C: Martin Tu�ek, srpen 2005
**                                         �pravy: Bohuslav K�ena, listopad 2009
**                                         �pravy: Karel Masa��k, ��jen 2013
**                                         �pravy: Radek Hranick�, ��jen 2014
**
** S vyu�it�m dynamick�ho p�id�lov�n� pam�ti, implementujte NEREKURZIVN�
** n�sleduj�c� operace nad bin�rn�m vyhled�vac�m stromem (p�edpona BT znamen�
** Binary Tree a je u identifik�tor� uvedena kv�li mo�n� kolizi s ostatn�mi
** p��klady):
**
**     BTInit .......... inicializace stromu
**     BTInsert ........ nerekurzivn� vlo�en� nov�ho uzlu do stromu
**     BTPreorder ...... nerekurzivn� pr�chod typu pre-order
**     BTInorder ....... nerekurzivn� pr�chod typu in-order
**     BTPostorder ..... nerekurzivn� pr�chod typu post-order
**     BTDisposeTree ... zru� v�echny uzly stromu
**
** U v�ech funkc�, kter� vyu��vaj� n�kter� z pr�chod� stromem, implementujte
** pomocnou funkci pro nalezen� nejlev�j��ho uzlu v podstromu.
**
** P�esn� definice typ� naleznete v souboru c402.h. Uzel stromu je typu tBTNode,
** ukazatel na n�j je typu tBTNodePtr. Jeden uzel obsahuje polo�ku int Cont,
** kter� sou�asn� slou�� jako u�ite�n� obsah i jako vyhled�vac� kl�� 
** a ukazatele na lev� a prav� podstrom (LPtr a RPtr).
**
** P��klad slou�� zejm�na k procvi�en� nerekurzivn�ch z�pis� algoritm�
** nad stromy. Ne� za�nete tento p��klad �e�it, prostudujte si d�kladn�
** principy p�evodu rekurzivn�ch algoritm� na nerekurzivn�. Programov�n�
** je p�edev��m in�en�rsk� discipl�na, kde op�tn� objevov�n� Ameriky nem�
** m�sto. Pokud se V�m zd�, �e by n�co �lo zapsat optim�ln�ji, promyslete
** si v�echny detaily Va�eho �e�en�. Pov�imn�te si typick�ho um�st�n� akc�
** pro r�zn� typy pr�chod�. Zamyslete se nad modifikac� �e�en�ch algoritm�
** nap��klad pro v�po�et po�tu uzl� stromu, po�tu list� stromu, v��ky stromu
** nebo pro vytvo�en� zrcadlov�ho obrazu stromu (pouze pop�ehazov�n� ukazatel�
** bez vytv��en� nov�ch uzl� a ru�en� star�ch).
**
** P�i pr�chodech stromem pou�ijte ke zpracov�n� uzlu funkci BTWorkOut().
** Pro zjednodu�en� pr�ce m�te p�edem p�ipraveny z�sobn�ky pro hodnoty typu
** bool a tBTNodePtr. Pomocnou funkci BTWorkOut ani funkce pro pr�ci
** s pomocn�mi z�sobn�ky neupravujte 
** Pozor! Je t�eba spr�vn� rozli�ovat, kdy pou��t dereferen�n� oper�tor *
** (typicky p�i modifikaci) a kdy budeme pracovat pouze se samotn�m ukazatelem 
** (nap�. p�i vyhled�v�n�). V tomto p��kladu v�m napov� prototypy funkc�.
** Pokud pracujeme s ukazatelem na ukazatel, pou�ijeme dereferenci.
**/

#include "c402.h"
int solved;

void BTWorkOut (tBTNodePtr Ptr)		{
/*   ---------
** Pomocn� funkce, kterou budete volat p�i pr�chodech stromem pro zpracov�n�
** uzlu ur�en�ho ukazatelem Ptr. Tuto funkci neupravujte.
**/
			
	if (Ptr==NULL) 
    printf("Chyba: Funkce BTWorkOut byla vol�na s NULL argumentem!\n");
  else 
    printf("V�pis hodnoty dan�ho uzlu> %d\n",Ptr->Cont);
}
	
/* -------------------------------------------------------------------------- */
/*
** Funkce pro z�sobn�k hotnot typu tBTNodePtr. Tyto funkce neupravujte.
**/

void SInitP (tStackP *S)  
/*   ------
** Inicializace z�sobn�ku.
**/
{
	S->top = 0;  
}	

void SPushP (tStackP *S, tBTNodePtr ptr)
/*   ------
** Vlo�� hodnotu na vrchol z�sobn�ku.
**/
{ 
                 /* P�i implementaci v poli m��e doj�t k p�ete�en� z�sobn�ku. */
  if (S->top==MAXSTACK) 
    printf("Chyba: Do�lo k p�ete�en� z�sobn�ku s ukazateli!\n");
  else {  
		S->top++;  
		S->a[S->top]=ptr;
	}
}	

tBTNodePtr STopPopP (tStackP *S)
/*         --------
** Odstran� prvek z vrcholu z�sobn�ku a sou�asn� vr�t� jeho hodnotu.
**/
{
                            /* Operace nad pr�zdn�m z�sobn�kem zp�sob� chybu. */
	if (S->top==0)  {
		printf("Chyba: Do�lo k podte�en� z�sobn�ku s ukazateli!\n");
		return(NULL);	
	}	
	else {
		return (S->a[S->top--]);
	}	
}

bool SEmptyP (tStackP *S)
/*   -------
** Je-li z�sobn�k pr�zdn�, vr�t� hodnotu true.
**/
{
  return(S->top==0);
}	

/* -------------------------------------------------------------------------- */
/*
** Funkce pro z�sobn�k hotnot typu bool. Tyto funkce neupravujte.
*/

void SInitB (tStackB *S) {
/*   ------
** Inicializace z�sobn�ku.
**/

	S->top = 0;  
}	

void SPushB (tStackB *S,bool val) {
/*   ------
** Vlo�� hodnotu na vrchol z�sobn�ku.
**/
                 /* P�i implementaci v poli m��e doj�t k p�ete�en� z�sobn�ku. */
	if (S->top==MAXSTACK) 
		printf("Chyba: Do�lo k p�ete�en� z�sobn�ku pro boolean!\n");
	else {
		S->top++;  
		S->a[S->top]=val;
	}	
}

bool STopPopB (tStackB *S) {
/*   --------
** Odstran� prvek z vrcholu z�sobn�ku a sou�asn� vr�t� jeho hodnotu.
**/
                            /* Operace nad pr�zdn�m z�sobn�kem zp�sob� chybu. */
	if (S->top==0) {
		printf("Chyba: Do�lo k podte�en� z�sobn�ku pro boolean!\n");
		return(NULL);	
	}	
	else {  
		return(S->a[S->top--]); 
	}	
}

bool SEmptyB (tStackB *S) {
/*   -------
** Je-li z�sobn�k pr�zdn�, vr�t� hodnotu true.
**/
  return(S->top==0);
}

/* -------------------------------------------------------------------------- */
/*
** N�sleduje j�dro dom�c� �lohy - funkce, kter� m�te implementovat. 
*/

void BTInit (tBTNodePtr *RootPtr)	{
/*   ------
** Provede inicializaci bin�rn�ho vyhled�vac�ho stromu.
**
** Inicializaci sm� program�tor volat pouze p�ed prvn�m pou�it�m bin�rn�ho
** stromu, proto�e neuvol�uje uzly nepr�zdn�ho stromu (a ani to d�lat nem��e,
** proto�e p�ed inicializac� jsou hodnoty nedefinovan�, tedy libovoln�).
** Ke zru�en� bin�rn�ho stromu slou�� procedura BTDisposeTree.
**	
** V�imn�te si, �e zde se poprv� v hlavi�ce objevuje typ ukazatel na ukazatel,	
** proto je t�eba p�i pr�ci s RootPtr pou��t dereferen�n� oper�tor *.
**/
	*RootPtr = NULL;	
	
}

void BTInsert (tBTNodePtr *RootPtr, int Content) {
/*   --------
** Vlo�� do stromu nov� uzel s hodnotou Content.
**
** Z pohledu vkl�d�n� ch�pejte vytv��en� strom jako bin�rn� vyhled�vac� strom,
** kde uzly s hodnotou men�� ne� m� otec le�� v lev�m podstromu a uzly v�t��
** le�� vpravo. Pokud vkl�dan� uzel ji� existuje, neprov�d� se nic (dan� hodnota
** se ve stromu m��e vyskytnout nejv��e jednou). Pokud se vytv��� nov� uzel,
** vznik� v�dy jako list stromu. Funkci implementujte nerekurzivn�.
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
** Jde po lev� v�tvi podstromu, dokud nenaraz� na jeho nejlev�j�� uzel.
**
** P�i pr�chodu Preorder nav�t�ven� uzly zpracujeme vol�n�m funkce BTWorkOut()
** a ukazatele na n� is ulo��me do z�sobn�ku.
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
** Pr�chod stromem typu preorder implementovan� nerekurzivn� s vyu�it�m funkce
** Leftmost_Preorder a z�sobn�ku ukazatel�. Zpracov�n� jednoho uzlu stromu
** realizujte jako vol�n� funkce BTWorkOut(). 
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
** Jde po lev� v�tvi podstromu, dokud nenaraz� na jeho nejlev�j�� uzel.
**
** P�i pr�chodu Inorder ukl�d�me ukazatele na v�echny nav�t�ven� uzly do
** z�sobn�ku. 
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
** Pr�chod stromem typu inorder implementovan� nerekurzivn� s vyu�it�m funkce
** Leftmost_Inorder a z�sobn�ku ukazatel�. Zpracov�n� jednoho uzlu stromu
** realizujte jako vol�n� funkce BTWorkOut(). 
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
** Jde po lev� v�tvi podstromu, dokud nenaraz� na jeho nejlev�j�� uzel.
**
** P�i pr�chodu Postorder ukl�d�me ukazatele na nav�t�ven� uzly do z�sobn�ku
** a sou�asn� do z�sobn�ku bool hodnot ukl�d�me informaci, zda byl uzel
** nav�t�ven poprv� a �e se tedy je�t� nem� zpracov�vat. 
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
** Pr�chod stromem typu postorder implementovan� nerekurzivn� s vyu�it�m funkce
** Leftmost_Postorder, z�sobn�ku ukazatel� a z�sobn�ku hotdnot typu bool.
** Zpracov�n� jednoho uzlu stromu realizujte jako vol�n� funkce BTWorkOut(). 
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
** Zru�� v�echny uzly stromu a korektn� uvoln� jimi zabranou pam�.
**
** Funkci implementujte nerekurzivn� s vyu�it�m z�sobn�ku ukazatel�.
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

