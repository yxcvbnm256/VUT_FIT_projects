
/* c401.c: **********************************************************}
{* T�ma: Rekurzivn� implementace operac� nad BVS
**                                         Vytvo�il: Petr P�ikryl, listopad 1994
**                                         �pravy: Andrea N�mcov�, prosinec 1995
**                                                      Petr P�ikryl, duben 1996
**                                                   Petr P�ikryl, listopad 1997
**                                  P�evod do jazyka C: Martin Tu�ek, ��jen 2005
**                                         �pravy: Bohuslav K�ena, listopad 2009
**                                         �pravy: Karel Masa��k, ��jen 2013
**                                         �pravy: Radek Hranick�, ��jen 2014
**
** Implementujte rekurzivn�m zp�sobem operace nad bin�rn�m vyhled�vac�m
** stromem (BVS; v angli�tin� BST - Binary Search Tree).
**
** Kl��em uzlu stromu je jeden znak (obecn� j�m m��e b�t cokoliv, podle
** �eho se vyhled�v�). U�ite�n�m (vyhled�van�m) obsahem je zde integer.
** Uzly s men��m kl��em le�� vlevo, uzly s v�t��m kl��em le�� ve stromu
** vpravo. Vyu�ijte dynamick�ho p�id�lov�n� pam�ti.
** Rekurzivn�m zp�sobem implementujte n�sleduj�c� funkce:
**
**   BSTInit ...... inicializace vyhled�vac�ho stromu
**   BSTSearch .... vyhled�v�n� hodnoty uzlu zadan�ho kl��em
**   BSTInsert .... vkl�d�n� nov� hodnoty
**   BSTDelete .... zru�en� uzlu se zadan�m kl��em
**   BSTDispose ... zru�en� cel�ho stromu
**
** ADT BVS je reprezentov�n ko�enov�m ukazatelem stromu (typ tBSTNodePtr).
** Uzel stromu (struktura typu tBSTNode) obsahuje kl�� (typu char), podle
** kter�ho se ve stromu vyhled�v�, vlastn� obsah uzlu (pro jednoduchost
** typu int) a ukazatel na lev� a prav� podstrom (LPtr a RPtr). P�esnou definici typ� 
** naleznete v souboru c401.h.
**
** Pozor! Je t�eba spr�vn� rozli�ovat, kdy pou��t dereferen�n� oper�tor *
** (typicky p�i modifikaci) a kdy budeme pracovat pouze se samotn�m ukazatelem 
** (nap�. p�i vyhled�v�n�). V tomto p��kladu v�m napov� prototypy funkc�.
** Pokud pracujeme s ukazatelem na ukazatel, pou�ijeme dereferenci.
**/

#include "c401.h"
int solved;

void BSTInit (tBSTNodePtr *RootPtr) {
/*   -------
** Funkce provede po��te�n� inicializaci stromu p�ed jeho prvn�m pou�it�m.
**
** Ov��it, zda byl ji� strom p�edan� p�es RootPtr inicializov�n, nelze,
** proto�e p�ed prvn� inicializac� m� ukazatel nedefinovanou (tedy libovolnou)
** hodnotu. Program�tor vyu��vaj�c� ADT BVS tedy mus� zajistit, aby inicializace
** byla vol�na pouze jednou, a to p�ed vlastn� prac� s BVS. Proveden�
** inicializace nad nepr�zdn�m stromem by toti� mohlo v�st ke ztr�t� p��stupu
** k dynamicky alokovan� pam�ti (tzv. "memory leak").
**	
** V�imn�te si, �e se v hlavi�ce objevuje typ ukazatel na ukazatel.	
** Proto je t�eba p�i p�i�azen� p�es RootPtr pou��t dereferen�n� oper�tor *.
** Ten bude pou�it i ve funkc�ch BSTDelete, BSTInsert a BSTDispose.
**/
	*RootPtr = NULL;	
}	

int BSTSearch (tBSTNodePtr RootPtr, char K, int *Content)	{
/*  ---------
** Funkce vyhled� uzel v BVS s kl��em K.
**
** Pokud je takov� nalezen, vrac� funkce hodnotu TRUE a v prom�nn� Content se
** vrac� obsah p��slu�n�ho uzlu.�Pokud p��slu�n� uzel nen� nalezen, vrac� funkce
** hodnotu FALSE a obsah prom�nn� Content nen� definov�n (nic do n� proto
** nep�i�azujte).
**
** P�i vyhled�v�n� v bin�rn�m stromu bychom typicky pou�ili cyklus ukon�en�
** testem dosa�en� listu nebo nalezen� uzlu s kl��em K. V tomto p��pad� ale
** probl�m �e�te rekurzivn�m vol�n� t�to funkce, p�i�em� nedeklarujte ��dnou
** pomocnou funkci.
**/
	if (RootPtr != NULL)
	{
		/* Pokud je strom neprazdny */
		tBSTNodePtr tmp = RootPtr;
		if (tmp->Key == K)
		{
			/* Pokud je nalezen hledany prvek */
			*Content = tmp->BSTNodeCont;
			return 1;
		}
		else if (tmp->Key > K)
		{
			/* Pokud je hledany klic mensi, nez klic aktualniho prvku ve stromu,
			vyhledavani pokracuje v levem podstromu. */
			return BSTSearch(RootPtr->LPtr, K, Content);
		}
		else if (tmp->Key < K)
		{
			/* Pokud je hledany klic vetsi, nez klic aktualniho prvku ve stromu,
			vyhledavani pokracuje v pravem podstromu. */
			return BSTSearch(RootPtr->RPtr, K, Content);
		}		
	}
	return 0;
								   
} 


void BSTInsert (tBSTNodePtr* RootPtr, char K, int Content)	{	
/*   ---------
** Vlo�� do stromu RootPtr hodnotu Content s kl��em K.
**
** Pokud ji� uzel se zadan�m kl��em ve stromu existuje, bude obsah uzlu
** s kl��em K nahrazen novou hodnotou. Pokud bude do stromu vlo�en nov�
** uzel, bude vlo�en v�dy jako list stromu.
**
** Funkci implementujte rekurzivn�. Nedeklarujte ��dnou pomocnou funkci.
**
** Rekurzivn� implementace je m�n� efektivn�, proto�e se p�i ka�d�m
** rekurzivn�m zano�en� ukl�d� na z�sobn�k obsah uzlu (zde integer).
** Nerekurzivn� varianta by v tomto p��pad� byla efektivn�j�� jak z hlediska
** rychlosti, tak z hlediska pam�ov�ch n�rok�. Zde jde ale o �koln�
** p��klad, na kter�m si chceme uk�zat eleganci rekurzivn�ho z�pisu.
**/
	if (*RootPtr == NULL)
	{
		/* Pokud je aktualni prvek prazdny, vklada se */		
		if ((*RootPtr = malloc(sizeof(struct tBSTNode))) != NULL)
		{
			/* Poud se povede alokace, naplnime prvek */
			(*RootPtr)->Key = K;
			(*RootPtr)->BSTNodeCont = Content;
			(*RootPtr)->LPtr = NULL;
			(*RootPtr)->RPtr = NULL;
			return;
		}
	}	
	else if ((*RootPtr)->Key == K)
	{
		/* Pokud jiz vkladany prvek existuje, prepise se jeho obsah. */
		(*RootPtr)->BSTNodeCont = Content;
	}
	else if ((*RootPtr)->Key > K)
	{
		/* Pokud je klic vkladaneho prvku mensi, nez aktualni, pokracuje se v levem podstrmu. */
		BSTInsert(&(*RootPtr)->LPtr, K, Content);
	}
	else
	{
		/* Pokud je klic vkladaneho prvku vetsi, nez aktualni, pokracuje se v pravem podstromu. */
		BSTInsert(&(*RootPtr)->RPtr, K, Content);
	}
	return;
}

void ReplaceByRightmost (tBSTNodePtr PtrReplaced, tBSTNodePtr *RootPtr) {
/*   ------------------
** Pomocn� funkce pro vyhled�n�, p�esun a uvoln�n� nejprav�j��ho uzlu.
**
** Ukazatel PtrReplaced ukazuje na uzel, do kter�ho bude p�esunuta hodnota
** nejprav�j��ho uzlu v podstromu, kter� je ur�en ukazatelem RootPtr.
** P�edpokl�d� se, �e hodnota ukazatele RootPtr nebude NULL (zajist�te to
** testov�n�m p�ed vol�n� t�to funkce). Tuto funkci implementujte rekurzivn�. 
**
** Tato pomocn� funkce bude pou�ita d�le. Ne� ji za�nete implementovat,
** p�e�t�te si koment�� k funkci BSTDelete(). 
**/
	tBSTNodePtr tmp = *RootPtr;
	if (tmp != NULL)
	{		
		if (tmp->RPtr == NULL)
		{
			/* Pokud je nalezen nejpravejsi prvek */
			PtrReplaced->BSTNodeCont = tmp->BSTNodeCont;
			PtrReplaced->Key = tmp->Key;
			*RootPtr = tmp->LPtr;
			free(tmp);
		}
		else
		{			
			ReplaceByRightmost(PtrReplaced, &(*RootPtr)->RPtr);		
		}
	}	
}

void BSTDelete (tBSTNodePtr *RootPtr, char K) 		{
/*   ---------
** Zru�� uzel stromu, kter� obsahuje kl�� K.
**
** Pokud uzel se zadan�m kl��em neexistuje, ned�l� funkce nic. 
** Pokud m� ru�en� uzel jen jeden podstrom, pak jej zd�d� otec ru�en�ho uzlu.
** Pokud m� ru�en� uzel oba podstromy, pak je ru�en� uzel nahrazen nejprav�j��m
** uzlem lev�ho podstromu. Pozor! Nejprav�j�� uzel nemus� b�t listem.
**
** Tuto funkci implementujte rekurzivn� s vyu�it�m d��ve deklarovan�
** pomocn� funkce ReplaceByRightmost.
**/
	if (*RootPtr != NULL)
	{
		tBSTNodePtr tmp = *RootPtr;
		if (tmp->Key == K)
		{	
			/* Pokud je nalezen mazany prvek */		
			if ((tmp->LPtr == NULL) && (tmp->RPtr == NULL))
			{
				/* Pokud nema prvek zadne podstromy */
				*RootPtr = NULL;
				free(tmp);				
			}
			else if ((tmp->LPtr == NULL) && (tmp->RPtr != NULL))
			{
				/* Pokud ma mazany prvek pravy podstrom, ukazatel na mazany prvek bude ukazovat na tento pravy podstrom. */
				*RootPtr = tmp->RPtr;
				free(tmp);
			}
			else if ((tmp->LPtr != NULL) && (tmp->RPtr == NULL))
			{
				/* Pokud ma mazany prvek levy podstrom, ukazatel na mazany prvek bude ukazovat na tento levy podstrom. */
				*RootPtr = tmp->LPtr;
				free(tmp);
			}
			else
			{	
				/* Pokud ma prvek oba podstromy, musi se najit nejpravejsi prvek leveho podstromu, ktery mazany prvek nahradi. */			
				ReplaceByRightmost(tmp, &(tmp->LPtr));
			} 
		}
		else if (tmp->Key > K)
		{
			BSTDelete(&(tmp->LPtr), K);
		}
		else
		{
			BSTDelete(&(tmp->RPtr), K);
		}		
	}							   
	return;	
} 

void BSTDispose (tBSTNodePtr *RootPtr) {	
/*   ----------
** Zru�� cel� bin�rn� vyhled�vac� strom a korektn� uvoln� pam�.
**
** Po zru�en� se bude BVS nach�zet ve stejn�m stavu, jako se nach�zel po
** inicializaci. Tuto funkci implementujte rekurzivn� bez deklarov�n� pomocn�
** funkce.
**/	
	if (*RootPtr != NULL)
	{
		tBSTNodePtr tmp = *RootPtr;
		if ((tmp)->LPtr != NULL)
		{
			/* Pokud existuje levy podstrom, zavola se znovu funkce pro mazani leveho podstromu. */
			BSTDispose(&(*RootPtr)->LPtr);
		}
		if ((tmp)->RPtr != NULL)
		{
			/* Pokud existuje pravy podstrom, zavola se znovu funkce pro mazani praveho podstromu. */
			BSTDispose(&(*RootPtr)->RPtr);
		}
		/* Pokud neexistuje levy ani pravy podstrom, nebo se vynorujeme z rekurze, prvek se uvolni.  */
		free(tmp);
	}
	*RootPtr = NULL;
}

/* konc c401.c */

