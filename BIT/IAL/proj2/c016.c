
/* c016.c: **********************************************************}
{* Téma:  Tabulka s Rozptýlenými Polo¾kami
**                      První implementace: Petr Pøikryl, prosinec 1994
**                      Do jazyka C prepsal a upravil: Vaclav Topinka, 2005
**                      Úpravy: Karel Masaøík, øíjen 2014
**                      Úpravy: Radek Hranický, øíjen 2014
**
** Vytvoøete abstraktní datový typ
** TRP (Tabulka s Rozptýlenými Polo¾kami = Hash table)
** s explicitnì øetìzenými synonymy. Tabulka je implementována polem
** lineárních seznamù synonym.
**
** Implementujte následující procedury a funkce.
**
**  HTInit ....... inicializuje tabulku pøed prvním pou¾itím
**  HTInsert ..... vlo¾ení prvku
**  HTSearch ..... zji¹tìní pøítomnosti prvku v tabulce
**  HTDelete ..... zru¹ení prvku
**  HTRead ....... pøeètení hodnoty prvku
**  HTClearAll ... zru¹ení obsahu celé tabulky (inicializace tabulky
**                 poté, co ji¾ byla pou¾ita)
**
** Definici typù naleznete v souboru c016.h.
**
** Tabulka je reprezentována datovou strukturou typu tHTable,
** která se skládá z ukazatelù na polo¾ky, je¾ obsahují slo¾ky
** klíèe 'key', obsahu 'data' (pro jednoduchost typu float), a
** ukazatele na dal¹í synonymum 'ptrnext'. Pøi implementaci funkcí
** uva¾ujte maximální rozmìr pole HTSIZE.
**
** U v¹ech procedur vyu¾ívejte rozptylovou funkci hashCode.  Pov¹imnìte si
** zpùsobu pøedávání parametrù a zamyslete se nad tím, zda je mo¾né parametry
** pøedávat jiným zpùsobem (hodnotou/odkazem) a v pøípadì, ¾e jsou obì
** mo¾nosti funkènì pøípustné, jaké jsou výhody èi nevýhody toho èi onoho
** zpùsobu.
**
** V pøíkladech jsou pou¾ity polo¾ky, kde klíèem je øetìzec, ke kterému
** je pidán obsah - reálné èíslo.
*/

#include "c016.h"

int HTSIZE = MAX_HTSIZE;
int solved;

/*          ------- 
** Rozptylovací funkce - jejím úkolem je zpracovat zadaný klíè a pøidìlit
** mu index v rozmezí 0..HTSize-1.  V ideálním pøípadì by mìlo dojít
** k rovnomìrnému rozptýlení tìchto klíèù po celé tabulce.  V rámci
** pokusù se mù¾ete zamyslet nad kvalitou této funkce.  (Funkce nebyla
** volena s ohledem na maximální kvalitu výsledku). }
*/

int hashCode ( tKey key ) {
	int retval = 1;
	int keylen = strlen(key);
	for ( int i=0; i<keylen; i++ )
		retval += key[i];
	return ( retval % HTSIZE );
}

/*
** Inicializace tabulky s explicitnì zøetìzenými synonymy.  Tato procedura
** se volá pouze pøed prvním pou¾itím tabulky.
*/

void htInit ( tHTable* ptrht ) {
	for (int i=0; i<MAX_HTSIZE; i++)
	{
		(*ptrht)[i] = NULL;
		/* Vsechny prvky tabulky budou po inicializaci prazdne. */
	}
}

/* TRP s explicitnì zøetìzenými synonymy.
** Vyhledání prvku v TRP ptrht podle zadaného klíèe key.  Pokud je
** daný prvek nalezen, vrací se ukazatel na daný prvek. Pokud prvek nalezen není, 
** vrací se hodnota NULL.
**
*/

tHTItem* htSearch ( tHTable* ptrht, tKey key ) {
	
	int pom = hashCode(key);
	/* Cislo prvku v tabulce podle hashovaci funkce */
	tHTItem* tmp = (*ptrht)[pom];	
	if (tmp != NULL)
	{
		/* Pokud je prvek pole, kde se ma hledany zaznam nachazet, neprazdny */
		do {
			if (strcmp(tmp->key, key) == 0)
			{
				/* Pokud se shoduje zadany klic s klicem prvku v poli */				
				return tmp; 
			}
			else tmp = tmp->ptrnext;
			/* Pokud ne, prejdeme na dalsi prvek seznamu */
		} while (tmp != NULL);
		/* Toto se opakuje, dokud se nedojde na konec seznamu */
	} return NULL;
	
}

/* 
** TRP s explicitnì zøetìzenými synonymy.
** Tato procedura vkládá do tabulky ptrht polo¾ku s klíèem key a s daty
** data.  Proto¾e jde o vyhledávací tabulku, nemù¾e být prvek se stejným
** klíèem ulo¾en v tabulce více ne¾ jedenkrát.  Pokud se vkládá prvek,
** jeho¾ klíè se ji¾ v tabulce nachází, aktualizujte jeho datovou èást.
**
** Vyu¾ijte døíve vytvoøenou funkci htSearch.  Pøi vkládání nového
** prvku do seznamu synonym pou¾ijte co nejefektivnìj¹í zpùsob,
** tedy proveïte.vlo¾ení prvku na zaèátek seznamu.
**/

void htInsert ( tHTable* ptrht, tKey key, tData data ) {	
	tHTItem *tmp = htSearch(ptrht, key);
	/* Vyhledani prvku, ktery chceme hledat, v tabulce */
	int pom = hashCode(key);
	if (tmp != NULL)
	{
		/* Pokud byl vkladany prvek vyhledany v tabulce, pouze se aktualizuji jeho data */
		tmp->data = data;
		return;
	}		
	if ((*ptrht)[pom] != NULL)
	{
		/* Pokud je na pozici urcene hashovaci funkci nejaky prvek, bude ho potreba napojit na novy prvek */
		tmp = (*ptrht)[pom];		
	}		
	(*ptrht)[pom] = malloc(sizeof(struct tHTItem));
	/* Alokuje se misto pro novy zaznam */
	(*ptrht)[pom]->key = malloc((strlen(key)*sizeof(char)) + 1);
	strcpy((*ptrht)[pom]->key, key);
	(*ptrht)[pom]->data = data;
	if (tmp != NULL)
	{
		(*ptrht)[pom]->ptrnext = tmp;
		/* Ukazatel noveho prvku ukazuje na prvek, ktery se nachazel na jeho miste */		
	}
	else (*ptrht)[pom]->ptrnext = NULL;
	/* Pokud vkladany prvek nema synonyma, ukazatel ukazuje na NULL */
}

/*
** TRP s explicitnì zøetìzenými synonymy.
** Tato funkce zji¹»uje hodnotu datové èásti polo¾ky zadané klíèem.
** Pokud je polo¾ka nalezena, vrací funkce ukazatel na polo¾ku
** Pokud polo¾ka nalezena nebyla, vrací se funkèní hodnota NULL
**
** Vyu¾ijte døíve vytvoøenou funkci HTSearch.
*/

tData* htRead ( tHTable* ptrht, tKey key ) {
	tHTItem *tmp = htSearch(ptrht, key);
	if (tmp != NULL)
	{
		/* Pokud se neco najde, vrati se adresa */
		return &(tmp->data);		
	}
	return NULL;
}

/*
** TRP s explicitnì zøetìzenými synonymy.
** Tato procedura vyjme polo¾ku s klíèem key z tabulky
** ptrht.  Uvolnìnou polo¾ku korektnì zru¹te.  Pokud polo¾ka s uvedeným
** klíèem neexistuje, dìlejte, jako kdyby se nic nestalo (tj. nedìlejte
** nic).
**
** V tomto pøípadì NEVYU®ÍVEJTE døíve vytvoøenou funkci HTSearch.
*/

void htDelete ( tHTable* ptrht, tKey key ) {	
	int pom = hashCode(key);
	if ((*ptrht)[pom] == NULL) return;
	/* Pokud je pozice, kde by se melo mazat, prazdna, rovnou se mazani ukonci */
	tHTItem* tmp = (*ptrht)[pom];
	if (strcmp(tmp->key, key) == 0)
	{
		/* Pokud se nalezne mazany prvek na prvni pozici v tabulce*/
		(*ptrht)[pom] = tmp->ptrnext;
		free(tmp->key);
		free(tmp);		
		tmp = NULL;
		return;
	}
	else
	{
		tHTItem* temp_next = tmp->ptrnext;
		while (temp_next != NULL)
		{		
			if (strcmp(temp_next->key, key) == 0)
			{
				tmp->ptrnext = temp_next->ptrnext;
				free(temp_next->key);
				free(temp_next);
				return;		
			}
			tmp = tmp->ptrnext;
			temp_next = temp_next->ptrnext;
		}
		
	}
}

/* TRP s explicitnì zøetìzenými synonymy.
** Tato procedura zru¹í v¹echny polo¾ky tabulky, korektnì uvolní prostor,
** který tyto polo¾ky zabíraly, a uvede tabulku do poèáteèního stavu.
*/

void htClearAll ( tHTable* ptrht ) {
	
	for (int i = 0; i < MAX_HTSIZE; i++)
	{	
		/* Zkontroluje se cele pole */		
		while ((*ptrht)[i] != NULL)
		{
			/* Pokud existuje prvni prvek na dane  pozici */
			tHTItem* tmp = (*ptrht)[i];								
			(*ptrht)[i] = tmp->ptrnext;
			/* Jako prvni prvek se nastavi dalsi prvek */
			free(tmp->key);
			free(tmp);
			/* Byvaly prvni prvek se uvolni */
			tmp = NULL;				
		}
	}
}

