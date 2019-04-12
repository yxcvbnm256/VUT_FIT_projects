	
/* c206.c **********************************************************}
{* Téma: Dvousmìrnì vázaný lineární seznam
**
**                   Návrh a referenèní implementace: Bohuslav Køena, øíjen 2001
**                            Pøepracované do jazyka C: Martin Tuèek, øíjen 2004
**                                            Úpravy: Bohuslav Køena, øíjen 2014
**
** Implementujte abstraktní datový typ dvousmìrnì vázaný lineární seznam.
** U¾iteèným obsahem prvku seznamu je hodnota typu int.
** Seznam bude jako datová abstrakce reprezentován promìnnou
** typu tDLList (DL znamená Double-Linked a slou¾í pro odli¹ení
** jmen konstant, typù a funkcí od jmen u jednosmìrnì vázaného lineárního
** seznamu). Definici konstant a typù naleznete v hlavièkovém souboru c206.h.
**
** Va¹ím úkolem je implementovat následující operace, které spolu
** s vý¹e uvedenou datovou èástí abstrakce tvoøí abstraktní datový typ
** obousmìrnì vázaný lineární seznam:
**
**      DLInitList ...... inicializace seznamu pøed prvním pou¾itím,
**      DLDisposeList ... zru¹ení v¹ech prvkù seznamu,
**      DLInsertFirst ... vlo¾ení prvku na zaèátek seznamu,
**      DLInsertLast .... vlo¾ení prvku na konec seznamu, 
**      DLFirst ......... nastavení aktivity na první prvek,
**      DLLast .......... nastavení aktivity na poslední prvek, 
**      DLCopyFirst ..... vrací hodnotu prvního prvku,
**      DLCopyLast ...... vrací hodnotu posledního prvku, 
**      DLDeleteFirst ... zru¹í první prvek seznamu,
**      DLDeleteLast .... zru¹í poslední prvek seznamu, 
**      DLPostDelete .... ru¹í prvek za aktivním prvkem,
**      DLPreDelete ..... ru¹í prvek pøed aktivním prvkem, 
**      DLPostInsert .... vlo¾í nový prvek za aktivní prvek seznamu,
**      DLPreInsert ..... vlo¾í nový prvek pøed aktivní prvek seznamu,
**      DLCopy .......... vrací hodnotu aktivního prvku,
**      DLActualize ..... pøepí¹e obsah aktivního prvku novou hodnotou,
**      DLSucc .......... posune aktivitu na dal¹í prvek seznamu,
**      DLPred .......... posune aktivitu na pøedchozí prvek seznamu, 
**      DLActive ........ zji¹»uje aktivitu seznamu.
**
** Pøi implementaci jednotlivých funkcí nevolejte ¾ádnou z funkcí
** implementovaných v rámci tohoto pøíkladu, není-li u funkce
** explicitnì uvedeno nìco jiného.
**
** Nemusíte o¹etøovat situaci, kdy místo legálního ukazatele na seznam 
** pøedá nìkdo jako parametr hodnotu NULL.
**
** Svou implementaci vhodnì komentujte!
**
** Terminologická poznámka: Jazyk C nepou¾ívá pojem procedura.
** Proto zde pou¾íváme pojem funkce i pro operace, které by byly
** v algoritmickém jazyce Pascalovského typu implemenovány jako
** procedury (v jazyce C procedurám odpovídají funkce vracející typ void).
**/

#include "c206.h"

int solved;
int errflg;

void DLError() {
/*
** Vytiskne upozornìní na to, ¾e do¹lo k chybì.
** Tato funkce bude volána z nìkterých dále implementovaných operací.
**/	
    printf ("*ERROR* The program has performed an illegal operation.\n");
    errflg = TRUE;             /* globální promìnná -- pøíznak o¹etøení chyby */
    return;
}

void DLInitList (tDLList *L) {
/*
** Provede inicializaci seznamu L pøed jeho prvním pou¾itím (tzn. ¾ádná
** z následujících funkcí nebude volána nad neinicializovaným seznamem).
** Tato inicializace se nikdy nebude provádìt nad ji¾ inicializovaným
** seznamem, a proto tuto mo¾nost neo¹etøujte. V¾dy pøedpokládejte,
** ¾e neinicializované promìnné mají nedefinovanou hodnotu.
**/
	L->First = NULL;
	L->Act = NULL;
	L->Last = NULL;    
	return;
}

void DLDisposeList (tDLList *L) {
/*
** Zru¹í v¹echny prvky seznamu L a uvede seznam do stavu, v jakém
** se nacházel po inicializaci. Ru¹ené prvky seznamu budou korektnì
** uvolnìny voláním operace free. 
**/
	if (L->First != NULL)
	/* Jen pokud je seznam neprÃ¡zdnÃ½ */	
	{
		L->Act = NULL;
		/* Seznam se stane neaktivnÃ­m */
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
** Vlo¾í nový prvek na zaèátek seznamu L.
** V pøípadì, ¾e není dostatek pamìti pro nový prvek pøi operaci malloc,
** volá funkci DLError().
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
** Vlo¾í nový prvek na konec seznamu L (symetrická operace k DLInsertFirst).
** V pøípadì, ¾e není dostatek pamìti pro nový prvek pøi operaci malloc,
** volá funkci DLError().
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
** Nastaví aktivitu na první prvek seznamu L.
** Funkci implementujte jako jediný pøíkaz (nepoèítáme-li return),
** ani¾ byste testovali, zda je seznam L prázdný.
**/
	L->Act = L->First;
	return;
}

void DLLast (tDLList *L) {
/*
** Nastaví aktivitu na poslední prvek seznamu L.
** Funkci implementujte jako jediný pøíkaz (nepoèítáme-li return),
** ani¾ byste testovali, zda je seznam L prázdný.
**/
	L->Act = L->Last;
	return;
}

void DLCopyFirst (tDLList *L, int *val) {
/*
** Prostøednictvím parametru val vrátí hodnotu prvního prvku seznamu L.
** Pokud je seznam L prázdný, volá funkci DLError().
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
** Prostøednictvím parametru val vrátí hodnotu posledního prvku seznamu L.
** Pokud je seznam L prázdný, volá funkci DLError().
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
** Zru¹í první prvek seznamu L. Pokud byl první prvek aktivní, aktivita 
** se ztrácí. Pokud byl seznam L prázdný, nic se nedìje.
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
** Zru¹í poslední prvek seznamu L. Pokud byl poslední prvek aktivní,
** aktivita seznamu se ztrácí. Pokud byl seznam L prázdný, nic se nedìje.
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
** Zru¹í prvek seznamu L za aktivním prvkem.
** Pokud je seznam L neaktivní nebo pokud je aktivní prvek
** posledním prvkem seznamu, nic se nedìje.
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
** Zru¹í prvek pøed aktivním prvkem seznamu L .
** Pokud je seznam L neaktivní nebo pokud je aktivní prvek
** prvním prvkem seznamu, nic se nedìje.
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
** Vlo¾í prvek za aktivní prvek seznamu L.
** Pokud nebyl seznam L aktivní, nic se nedìje.
** V pøípadì, ¾e není dostatek pamìti pro nový prvek pøi operaci malloc,
** volá funkci DLError().
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
** Vlo¾í prvek pøed aktivní prvek seznamu L.
** Pokud nebyl seznam L aktivní, nic se nedìje.
** V pøípadì, ¾e není dostatek pamìti pro nový prvek pøi operaci malloc,
** volá funkci DLError().
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
** Prostøednictvím parametru val vrátí hodnotu aktivního prvku seznamu L.
** Pokud seznam L není aktivní, volá funkci DLError ().
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
** Pøepí¹e obsah aktivního prvku seznamu L.
** Pokud seznam L není aktivní, nedìlá nic.
**/
	if (L->Act != NULL)
	{
		L->Act->data = val;
	}
	return;	
}

void DLSucc (tDLList *L) {
/*
** Posune aktivitu na následující prvek seznamu L.
** Není-li seznam aktivní, nedìlá nic.
** V¹imnìte si, ¾e pøi aktivitì na posledním prvku se seznam stane neaktivním.
**/
	if (L->Act != NULL)
	{
		L->Act = L->Act->rptr;
	}	
	return;
}


void DLPred (tDLList *L) {
/*
** Posune aktivitu na pøedchozí prvek seznamu L.
** Není-li seznam aktivní, nedìlá nic.
** V¹imnìte si, ¾e pøi aktivitì na prvním prvku se seznam stane neaktivním.
**/
	if (L->Act != NULL)
	{
		L->Act = L->Act->lptr;
	}	
	return;
}

int DLActive (tDLList *L) {
/*
** Je-li seznam aktivní, vrací true. V opaèném pøípadì vrací false.
** Funkci implementujte jako jediný pøíkaz.
**/
	return ((L->Act != NULL) ? 1 : 0);
}

/* Konec c206.c*/
