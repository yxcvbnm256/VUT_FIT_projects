
/* c201.c *********************************************************************}
{* Téma: Jednosmìrný lineární seznam
**
**                     Návrh a referenèní implementace: Petr Pøikryl, øíjen 1994
**                                          Úpravy: Andrea Nìmcová listopad 1996
**                                                   Petr Pøikryl, listopad 1997
**                                Pøepracované zadání: Petr Pøikryl, bøezen 1998
**                                  Pøepis do jazyka C: Martin Tuèek, øíjen 2004
**	                                      Úpravy: Bohuslav Køena, øíjen 2014
**
** Implementujte abstraktní datový typ jednosmìrný lineární seznam.
** U¾iteèným obsahem prvku seznamu je celé èíslo typu int.
** Seznam bude jako datová abstrakce reprezentován promìnnou typu tList.
** Definici konstant a typù naleznete v hlavièkovém souboru c201.h.
** 
** Va¹ím úkolem je implementovat následující operace, které spolu s vý¹e
** uvedenou datovou èástí abstrakce tvoøí abstraktní datový typ tList:
**
**      InitList ...... inicializace seznamu pøed prvním pou¾itím,
**      DisposeList ... zru¹ení v¹ech prvkù seznamu,
**      InsertFirst ... vlo¾ení prvku na zaèátek seznamu,
**      First ......... nastavení aktivity na první prvek,
**      CopyFirst ..... vrací hodnotu prvního prvku,
**      DeleteFirst ... zru¹í první prvek seznamu,
**      PostDelete .... ru¹í prvek za aktivním prvkem,
**      PostInsert .... vlo¾í nový prvek za aktivní prvek seznamu,
**      Copy .......... vrací hodnotu aktivního prvku,
**      Actualize ..... pøepí¹e obsah aktivního prvku novou hodnotou,
**      Succ .......... posune aktivitu na dal¹í prvek seznamu,
**      Active ........ zji¹»uje aktivitu seznamu.
**
** Pøi implementaci funkcí nevolejte ¾ádnou z funkcí implementovaných v rámci
** tohoto pøíkladu, není-li u dané funkce explicitnì uvedeno nìco jiného.
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

#include "c201.h"

int solved;
int errflg;

void Error() {
/*
** Vytiskne upozornìní na to, ¾e do¹lo k chybì.
** Tato funkce bude volána z nìkterých dále implementovaných operací.
**/	
    printf ("*ERROR* The program has performed an illegal operation.\n");
    errflg = TRUE;                      /* globální promìnná -- pøíznak chyby */
}

void InitList (tList *L) {
/*
** Provede inicializaci seznamu L pøed jeho prvním pou¾itím (tzn. ¾ádná
** z následujících funkcí nebude volána nad neinicializovaným seznamem).
** Tato inicializace se nikdy nebude provádìt nad ji¾ inicializovaným
** seznamem, a proto tuto mo¾nost neo¹etøujte. V¾dy pøedpokládejte,
** ¾e neinicializované promìnné mají nedefinovanou hodnotu.
**/
	L->First = NULL;
	L->Act = NULL;	
}

void DisposeList (tList *L) {
/*
** Zru¹í v¹echny prvky seznamu L a uvede seznam L do stavu, v jakém se nacházel
** po inicializaci. Ve¹kerá pamì» pou¾ívaná prvky seznamu L bude korektnì
** uvolnìna voláním operace free.
***/
	L->Act = NULL;
	while (L->First != NULL)
	{
		/* Pokud je jeste co mazat, mazeme */
		tElemPtr tmp;
		tmp = L->First;
		L->First=L->First->ptr;
		/* First ukazuje na nasledujici prvek, muzeme tedy uvolnit pamet mazaneho prvku */
		free(tmp);		
	}
	return;
}

void InsertFirst (tList *L, int val) {
/*
** Vlo¾í prvek s hodnotou val na zaèátek seznamu L.
** V pøípadì, ¾e není dostatek pamìti pro nový prvek pøi operaci malloc,
** volá funkci Error().
**/
	tElemPtr newItem = NULL;
	if ((newItem = (tElemPtr) malloc(sizeof(struct tElem))) == NULL)
	{
		Error();
		/* Pokud neprobehne alokace spravne, vypise se chyba */
		return;
	}    
	newItem->data = val;
	newItem->ptr = L->First;
	L->First = newItem;
	/* Ukazatel noveho prvku ukazuje na prvek, ktery je oznacen jako prvni. Nasledne muze ukazatel First ukazovat na novy prvek. */
	return;
}

void First (tList *L) {
/*
** Nastaví aktivitu seznamu L na jeho první prvek.
** Funkci implementujte jako jediný pøíkaz, ani¾ byste testovali,
** zda je seznam L prázdný.
**/
	L->Act = L->First;	
	return;
}

void CopyFirst (tList *L, int *val) {
/*
** Prostøednictvím parametru val vrátí hodnotu prvního prvku seznamu L.
** Pokud je seznam L prázdný, volá funkci Error().
**/
	if (L->First != NULL)
	{
		*val = L->First->data; 
	}
	else Error();
	return;
}

void DeleteFirst (tList *L) {
/*
** Zru¹í první prvek seznamu L a uvolní jím pou¾ívanou pamì».
** Pokud byl ru¹ený prvek aktivní, aktivita seznamu se ztrácí.
** Pokud byl seznam L prázdný, nic se nedìje.
**/
	if (L->First != NULL)
	{
		/* Pokud je seznam neprazdny */
		if (L->First == L->Act)
		{
			L->Act = NULL;
			/* Pokud je mazany prvek zaroven aktivni, aktivita se ztraci. */
		}
		tElemPtr tmp;
		tmp = L->First;		
		L->First = L->First->ptr;
		/* Ukazatel First ukazuje na dalsi prvek, muzeme tedy byvaly prvni prvek uvolnit. */
		free(tmp);
	}
	return;
}	

void PostDelete (tList *L) {
/* 
** Zru¹í prvek seznamu L za aktivním prvkem a uvolní jím pou¾ívanou pamì».
** Pokud není seznam L aktivní nebo pokud je aktivní poslední prvek seznamu L,
** nic se nedìje.
**/
	if (L->Act != NULL)
	{
		/* Pokud existuje aktivni prvek */
		if (L->Act->ptr != NULL)
		{
			/* Pokud existuje prvek za aktivnim prvkem */	
			tElemPtr tmp;
			tmp = L->Act->ptr;
			L->Act->ptr = tmp->ptr;
			/* Ukazatel aktivniho prvku ukazuje na prvek, na ktery ukazuje mazany prvek. Muzeme tedy mazany prvek uvolnit */
			free(tmp);		
		}
	}
	return;	
}

void PostInsert (tList *L, int val) {
/*
** Vlo¾í prvek s hodnotou val za aktivní prvek seznamu L.
** Pokud nebyl seznam L aktivní, nic se nedìje!
** V pøípadì, ¾e není dostatek pamìti pro nový prvek pøi operaci malloc,
** zavolá funkci Error().
**/
	if (L->Act != NULL)
	{
		/* Pokud existuje aktivni prvek */
		tElemPtr newItem = NULL;
		if ((newItem = (tElemPtr) malloc(sizeof(struct tElem))) != NULL)
		{			
			newItem->ptr = L->Act->ptr;
			newItem->data = val;
			L->Act->ptr = newItem;
			/* Ukazatel noveho prvku ukazuje na prvek, na ktery ukazuje i ukazatel pktivniho prvku. 
			Nasledne bude ukazatel aktivniho prvku ukazovat na novy prvek. */
		}
		else
		{
			Error();
		}
	}
	return;	
}

void Copy (tList *L, int *val) {
/*
** Prostøednictvím parametru val vrátí hodnotu aktivního prvku seznamu L.
** Pokud seznam není aktivní, zavolá funkci Error().
**/
	if (L->Act != NULL)
	{
		*val = L->Act->data;
	}
	else Error();
	return;
}

void Actualize (tList *L, int val) {
/*
** Pøepí¹e data aktivního prvku seznamu L hodnotou val.
** Pokud seznam L není aktivní, nedìlá nic!
**/
	if (L->Act != NULL)
	{
		L->Act->data = val;
	}	
	return;
}

void Succ (tList *L) {
/*
** Posune aktivitu na následující prvek seznamu L.
** V¹imnìte si, ¾e touto operací se mù¾e aktivní seznam stát neaktivním.
** Pokud seznam L není aktivní, nedìlá nic.
**/
	if (L->Act != NULL)
	{
		L->Act = L->Act->ptr;
	}	
	return;
}

int Active (tList *L) {		
/*
** Je-li seznam L aktivní, vrací TRUE. V opaèném pøípadì vrací FALSE.
** Tuto funkci implementujte jako jediný pøíkaz return. 
**/
	return ((L->Act != NULL) ? 1 : 0);
}

/* Konec c201.c */
