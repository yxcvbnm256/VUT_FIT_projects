	
/* c206.c **********************************************************}
{* T�ma: Dvousm�rn� v�zan� line�rn� seznam
**
**                   N�vrh a referen�n� implementace: Bohuslav K�ena, ��jen 2001
**                            P�epracovan� do jazyka C: Martin Tu�ek, ��jen 2004
**                                            �pravy: Bohuslav K�ena, ��jen 2014
**
** Implementujte abstraktn� datov� typ dvousm�rn� v�zan� line�rn� seznam.
** U�ite�n�m obsahem prvku seznamu je hodnota typu int.
** Seznam bude jako datov� abstrakce reprezentov�n prom�nnou
** typu tDLList (DL znamen� Double-Linked a slou�� pro odli�en�
** jmen konstant, typ� a funkc� od jmen u jednosm�rn� v�zan�ho line�rn�ho
** seznamu). Definici konstant a typ� naleznete v hlavi�kov�m souboru c206.h.
**
** Va��m �kolem je implementovat n�sleduj�c� operace, kter� spolu
** s v��e uvedenou datovou ��st� abstrakce tvo�� abstraktn� datov� typ
** obousm�rn� v�zan� line�rn� seznam:
**
**      DLInitList ...... inicializace seznamu p�ed prvn�m pou�it�m,
**      DLDisposeList ... zru�en� v�ech prvk� seznamu,
**      DLInsertFirst ... vlo�en� prvku na za��tek seznamu,
**      DLInsertLast .... vlo�en� prvku na konec seznamu, 
**      DLFirst ......... nastaven� aktivity na prvn� prvek,
**      DLLast .......... nastaven� aktivity na posledn� prvek, 
**      DLCopyFirst ..... vrac� hodnotu prvn�ho prvku,
**      DLCopyLast ...... vrac� hodnotu posledn�ho prvku, 
**      DLDeleteFirst ... zru�� prvn� prvek seznamu,
**      DLDeleteLast .... zru�� posledn� prvek seznamu, 
**      DLPostDelete .... ru�� prvek za aktivn�m prvkem,
**      DLPreDelete ..... ru�� prvek p�ed aktivn�m prvkem, 
**      DLPostInsert .... vlo�� nov� prvek za aktivn� prvek seznamu,
**      DLPreInsert ..... vlo�� nov� prvek p�ed aktivn� prvek seznamu,
**      DLCopy .......... vrac� hodnotu aktivn�ho prvku,
**      DLActualize ..... p�ep�e obsah aktivn�ho prvku novou hodnotou,
**      DLSucc .......... posune aktivitu na dal�� prvek seznamu,
**      DLPred .......... posune aktivitu na p�edchoz� prvek seznamu, 
**      DLActive ........ zji��uje aktivitu seznamu.
**
** P�i implementaci jednotliv�ch funkc� nevolejte ��dnou z funkc�
** implementovan�ch v r�mci tohoto p��kladu, nen�-li u funkce
** explicitn� uvedeno n�co jin�ho.
**
** Nemus�te o�et�ovat situaci, kdy m�sto leg�ln�ho ukazatele na seznam 
** p�ed� n�kdo jako parametr hodnotu NULL.
**
** Svou implementaci vhodn� komentujte!
**
** Terminologick� pozn�mka: Jazyk C nepou��v� pojem procedura.
** Proto zde pou��v�me pojem funkce i pro operace, kter� by byly
** v algoritmick�m jazyce Pascalovsk�ho typu implemenov�ny jako
** procedury (v jazyce C procedur�m odpov�daj� funkce vracej�c� typ void).
**/

#include "c206.h"

int solved;
int errflg;

void DLError() {
/*
** Vytiskne upozorn�n� na to, �e do�lo k chyb�.
** Tato funkce bude vol�na z n�kter�ch d�le implementovan�ch operac�.
**/	
    printf ("*ERROR* The program has performed an illegal operation.\n");
    errflg = TRUE;             /* glob�ln� prom�nn� -- p��znak o�et�en� chyby */
    return;
}

void DLInitList (tDLList *L) {
/*
** Provede inicializaci seznamu L p�ed jeho prvn�m pou�it�m (tzn. ��dn�
** z n�sleduj�c�ch funkc� nebude vol�na nad neinicializovan�m seznamem).
** Tato inicializace se nikdy nebude prov�d�t nad ji� inicializovan�m
** seznamem, a proto tuto mo�nost neo�et�ujte. V�dy p�edpokl�dejte,
** �e neinicializovan� prom�nn� maj� nedefinovanou hodnotu.
**/
	L->First = NULL;
	L->Act = NULL;
	L->Last = NULL;    
	return;
}

void DLDisposeList (tDLList *L) {
/*
** Zru�� v�echny prvky seznamu L a uvede seznam do stavu, v jak�m
** se nach�zel po inicializaci. Ru�en� prvky seznamu budou korektn�
** uvoln�ny vol�n�m operace free. 
**/
	if (L->First != NULL)
	/* Jen pokud je seznam neprázdný */	
	{
		L->Act = NULL;
		/* Seznam se stane neaktivním */
		while (L->First != NULL)		
		{
			tDLElemPtr tmp;			
			tmp = L->First;
			L->First = tmp->rptr;
			if (L->Last == tmp)
			{
				L->Last = NULL;
			}
			/* Ukazatel na prvni prvek nyni ukazuje na nasledujici prvek, nebo NULL */
			free(tmp);
		}		 
	}
	return;	
}

void DLInsertFirst (tDLList *L, int val) {
/*
** Vlo�� nov� prvek na za��tek seznamu L.
** V p��pad�, �e nen� dostatek pam�ti pro nov� prvek p�i operaci malloc,
** vol� funkci DLError().
**/
	tDLElemPtr newItem = (tDLElemPtr) malloc(sizeof(struct tDLElem));
	if (newItem != NULL)
	/* Pokud se spravne alokuje pamet */
	{
		if (L->First != NULL)
		{
			L->First->lptr = newItem;
			/* Pokud vkladame minimalne druhy prvek, byvaly prvni prvek odkazuje zleva na novy prvek */
		}		
		newItem->data = val;
		newItem->lptr = NULL;
		newItem->rptr = L->First;
		L->First = newItem;
		/* Ukazatel First nyni ukazuje na novy prvek, novy prvek ukazuje zprava na byvaly prvni prvek */
		if (L->Last == NULL)
		{
			L->Last = newItem;
			/* Pokud je vkladany prvek prvni v seznamu, je zaroven posledni */
		}
	} 	
	else DLError();
	return;
}

void DLInsertLast(tDLList *L, int val) {
/*
** Vlo�� nov� prvek na konec seznamu L (symetrick� operace k DLInsertFirst).
** V p��pad�, �e nen� dostatek pam�ti pro nov� prvek p�i operaci malloc,
** vol� funkci DLError().
**/ 	
	tDLElemPtr newItem = (tDLElemPtr) malloc(sizeof(struct tDLElem));
	if (newItem != NULL)
	{
	/* Princip stejny, pouze invertovany, jako u funkce DLInsertFirst */	
		if (L->Last != NULL)
		{
			L->Last->rptr = newItem;
			/* Pokud vkladame minimalne druhy prvek, byvaly posledni prvek odkazuje zprava na novy prvek */
		}	
		newItem->data = val;
		newItem->lptr = L->Last;
		newItem->rptr = NULL;
		L->Last = newItem;
		/* Ukazatel Last nyni ukazuje  na novy prvek, novy prvek ukazuje zleva na byvaly prvni prvek */
		if (L->First == NULL)
		{
			L->First = newItem;
			/* Pokud je vkladany prvek prvni v seznamu, je zaroven posledni */
		}
	}	
	else DLError();
	return;
}

void DLFirst (tDLList *L) {
/*
** Nastav� aktivitu na prvn� prvek seznamu L.
** Funkci implementujte jako jedin� p��kaz (nepo��t�me-li return),
** ani� byste testovali, zda je seznam L pr�zdn�.
**/
	L->Act = L->First;
	return;
}

void DLLast (tDLList *L) {
/*
** Nastav� aktivitu na posledn� prvek seznamu L.
** Funkci implementujte jako jedin� p��kaz (nepo��t�me-li return),
** ani� byste testovali, zda je seznam L pr�zdn�.
**/
	L->Act = L->Last;
	return;
}

void DLCopyFirst (tDLList *L, int *val) {
/*
** Prost�ednictv�m parametru val vr�t� hodnotu prvn�ho prvku seznamu L.
** Pokud je seznam L pr�zdn�, vol� funkci DLError().
**/
	if (L->First != NULL)
	{
		*val = L->First->data;
	}
	else DLError();
	return;
}

void DLCopyLast (tDLList *L, int *val) {
/*
** Prost�ednictv�m parametru val vr�t� hodnotu posledn�ho prvku seznamu L.
** Pokud je seznam L pr�zdn�, vol� funkci DLError().
**/
	if (L->Last != NULL)
	{
		*val = L->Last->data;
	}
	else DLError();
	return;
}

void DLDeleteFirst (tDLList *L) {
/*
** Zru�� prvn� prvek seznamu L. Pokud byl prvn� prvek aktivn�, aktivita 
** se ztr�c�. Pokud byl seznam L pr�zdn�, nic se ned�je.
**/
	if (L->First != NULL)
	{
		if (L->Act == L->First)
		{
			L->Act = NULL;
			/* Pokud je mazany prvek aktivni, seznam prestane byt aktivni */
		}		
		tDLElemPtr tmp;
		tmp = L->First;
		if (L->First == L->Last)
		{
			/* Pokud je v seznamu jen jeden prvek, First a Last ukazuji na NULL */
			L->First = NULL;
			L->Last = NULL;
		}		
		else
		{
			L->First = tmp->rptr;
			tmp->rptr->lptr = NULL;
			/* Pokud je v seznamu vice nez jeden prvek, First ukazuje na prvek za mazanym. Prvek za mazanym ukazuje zleva na NULL */
		}
		free(tmp);
	}	
	return;
}	

void DLDeleteLast (tDLList *L) {
/*
** Zru�� posledn� prvek seznamu L. Pokud byl posledn� prvek aktivn�,
** aktivita seznamu se ztr�c�. Pokud byl seznam L pr�zdn�, nic se ned�je.
**/ 
	if (L->Last != NULL)
	{
		if (L->Act == L->Last)
		{
			L->Act = NULL;
			/* Pokud je mazany prvek aktivni, seznam prestane byt aktivni */
		}		
		tDLElemPtr tmp;
		tmp = L->Last;
		if (L->First == L->Last)
		{
			/* Pokud je v seznamu jen jeden prvek, First a Last ukazuji na NULL */
			L->First = NULL;
			L->Last = NULL;
		}
		else
		{
			L->Last = tmp->lptr;
			tmp->lptr->rptr = NULL;
			/* Pokud je v seznamu vice nez jeden prvek, Last ukazuje na prvek pred mazanym. Prvek pred mazanym ukazuje zprava na NULL */
		}
		free(tmp);
	}	
	return;
}

void DLPostDelete (tDLList *L) {
/*
** Zru�� prvek seznamu L za aktivn�m prvkem.
** Pokud je seznam L neaktivn� nebo pokud je aktivn� prvek
** posledn�m prvkem seznamu, nic se ned�je.
**/
	if ((L->Act != NULL) && (L->Act != L->Last))
	/* Pokud je seznam aktivni a aktivni prvek neni posledni */
	{
		tDLElemPtr tmp;
		tmp = L->Act->rptr;
		/* Docasny prvek = prvek zprava za aktivnim */
		if (L->Last == tmp)
		{
			L->Act->rptr = NULL;
			L->Last = L->Act;	
			/* Pokud je mazany prvek posledni, pravy ukazatel aktivniho prvku ma hodnotu NULL a ukazatel Last ukazuje na aktivni prvek. */	
		}
		else
		{
			L->Act->rptr = tmp->rptr;
			tmp->rptr->lptr = L->Act;
			/* Pokud mazany prvek neni posledni, pravy ukazatel aktivniho prvku ukazuje tam, kam ukazuje pravy ukazatel mazaneho prvku.
			Levy ukazatel prvku za mazanym ukazuje na aktivni prvek */
		}
		free(tmp);
	}
	return;
}

void DLPreDelete (tDLList *L) {
/*
** Zru�� prvek p�ed aktivn�m prvkem seznamu L .
** Pokud je seznam L neaktivn� nebo pokud je aktivn� prvek
** prvn�m prvkem seznamu, nic se ned�je.
**/
	if ((L->Act != NULL) && (L->Act != L->First))
	{
		/* Viz komentare k funkci DLPostDelete */
		tDLElemPtr tmp;
		tmp = L->Act->lptr;
		/* Docasny prvek = prvek zleva pred aktivnim */
		if (L->First == tmp)
		{
			L->Act->lptr = NULL;
			L->First = L->Act;
			/* Pokud je mazany prvek prvni, levy ukazatel aktivniho prvku ma hodnotu NULL a ukazatel First ukazuje na aktivni prvek. */	
		}
		else
		{
			L->Act->lptr = tmp->lptr;
			tmp->lptr->rptr = L->Act;
			/* Pokud mazany prvek neni prvni, levy ukazatel aktivniho prvku ukazuje tam, kam ukazuje levy ukazatel mazaneho prvku.
			Pravy ukazatel prvku pred mazanym ukazuje na aktivni prvek */
		}
	free(tmp);
	}
	return;	
}

void DLPostInsert (tDLList *L, int val) {
/*
** Vlo�� prvek za aktivn� prvek seznamu L.
** Pokud nebyl seznam L aktivn�, nic se ned�je.
** V p��pad�, �e nen� dostatek pam�ti pro nov� prvek p�i operaci malloc,
** vol� funkci DLError().
**/
	if (L->Act != NULL)
	{
		tDLElemPtr newItem = (tDLElemPtr) malloc(sizeof(struct tDLElem));
		if (newItem != NULL)
		{				
			newItem->data = val;
			newItem->lptr = L->Act;
			newItem->rptr = L->Act->rptr;
			/* Novy prvek ukazuje zleva na aktivni prvek a zprava na prvek, na ktery ukazuje zprava aktivni prvek */
			if (L->Act == L->Last)
			{
				L->Last = newItem;	
				/* Pokud je aktivni prvek poslednim prvkem, je nyni poslednim novy prvek */		
			}
			else
			{
				L->Act->rptr->lptr = newItem;
				/* Pokud aktivni neni posledni, levy ukazatel prvku za aktivnim prvkem ukazuje na novy prvek. */	
			}
			L->Act->rptr = newItem;
			/* Pravy ukazatel aktivniho prvku bude vzdy ukazovat na novy prvek. */
		}
		else
		{
			DLError();
		}
	}
	return;	
}

void DLPreInsert (tDLList *L, int val) {
/*
** Vlo�� prvek p�ed aktivn� prvek seznamu L.
** Pokud nebyl seznam L aktivn�, nic se ned�je.
** V p��pad�, �e nen� dostatek pam�ti pro nov� prvek p�i operaci malloc,
** vol� funkci DLError().
**/
	if (L->Act != NULL)
	{
		tDLElemPtr newItem = (tDLElemPtr) malloc(sizeof(struct tDLElem));
		if (newItem != NULL)
		{
			newItem->data = val;
			newItem->rptr = L->Act;
			newItem->lptr = L->Act->lptr;			
			/* Novy prvek ukazuje zprava na aktivni prvek a zleva na prvek, na ktery ukazuje zleva aktivni prvek */
			if (L->Act == L->First)
			{
				L->First = newItem;	
				/* Pokud je aktivni prvek zaroven prvnim prvkem, ukazatel Act bude ukazovat na novy prvek */
			}
			else
			{
				L->Act->lptr->rptr = newItem;
				/* Pokud neni aktivni prvek zaroven i prvni, prvek, na jenz ukazuje aktivni prvek zleva, bude ukazovat zprava na novy prvek. */				
			}
			
			L->Act->lptr = newItem;
			/* Levy ukazatel aktivniho prvku bude vzdy ukazovat na novy prvek. */
		}
		else
			DLError();
	}
	return;
}

void DLCopy (tDLList *L, int *val) {
/*
** Prost�ednictv�m parametru val vr�t� hodnotu aktivn�ho prvku seznamu L.
** Pokud seznam L nen� aktivn�, vol� funkci DLError ().
**/
	if (L->Act != NULL)
	{
		*val = L->Act->data;
	}	
	else DLError();
	return;
}

void DLActualize (tDLList *L, int val) {
/*
** P�ep�e obsah aktivn�ho prvku seznamu L.
** Pokud seznam L nen� aktivn�, ned�l� nic.
**/
	if (L->Act != NULL)
	{
		L->Act->data = val;
	}
	return;	
}

void DLSucc (tDLList *L) {
/*
** Posune aktivitu na n�sleduj�c� prvek seznamu L.
** Nen�-li seznam aktivn�, ned�l� nic.
** V�imn�te si, �e p�i aktivit� na posledn�m prvku se seznam stane neaktivn�m.
**/
	if (L->Act != NULL)
	{
		L->Act = L->Act->rptr;
	}	
	return;
}


void DLPred (tDLList *L) {
/*
** Posune aktivitu na p�edchoz� prvek seznamu L.
** Nen�-li seznam aktivn�, ned�l� nic.
** V�imn�te si, �e p�i aktivit� na prvn�m prvku se seznam stane neaktivn�m.
**/
	if (L->Act != NULL)
	{
		L->Act = L->Act->lptr;
	}	
	return;
}

int DLActive (tDLList *L) {
/*
** Je-li seznam aktivn�, vrac� true. V opa�n�m p��pad� vrac� false.
** Funkci implementujte jako jedin� p��kaz.
**/
	return ((L->Act != NULL) ? 1 : 0);
}

/* Konec c206.c*/
