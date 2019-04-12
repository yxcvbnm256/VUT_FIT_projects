
/* c016.c: **********************************************************}
{* T�ma:  Tabulka s Rozpt�len�mi Polo�kami
**                      Prvn� implementace: Petr P�ikryl, prosinec 1994
**                      Do jazyka C prepsal a upravil: Vaclav Topinka, 2005
**                      �pravy: Karel Masa��k, ��jen 2014
**                      �pravy: Radek Hranick�, ��jen 2014
**
** Vytvo�ete abstraktn� datov� typ
** TRP (Tabulka s Rozpt�len�mi Polo�kami = Hash table)
** s explicitn� �et�zen�mi synonymy. Tabulka je implementov�na polem
** line�rn�ch seznam� synonym.
**
** Implementujte n�sleduj�c� procedury a funkce.
**
**  HTInit ....... inicializuje tabulku p�ed prvn�m pou�it�m
**  HTInsert ..... vlo�en� prvku
**  HTSearch ..... zji�t�n� p��tomnosti prvku v tabulce
**  HTDelete ..... zru�en� prvku
**  HTRead ....... p�e�ten� hodnoty prvku
**  HTClearAll ... zru�en� obsahu cel� tabulky (inicializace tabulky
**                 pot�, co ji� byla pou�ita)
**
** Definici typ� naleznete v souboru c016.h.
**
** Tabulka je reprezentov�na datovou strukturou typu tHTable,
** kter� se skl�d� z ukazatel� na polo�ky, je� obsahuj� slo�ky
** kl��e 'key', obsahu 'data' (pro jednoduchost typu float), a
** ukazatele na dal�� synonymum 'ptrnext'. P�i implementaci funkc�
** uva�ujte maxim�ln� rozm�r pole HTSIZE.
**
** U v�ech procedur vyu��vejte rozptylovou funkci hashCode.  Pov�imn�te si
** zp�sobu p�ed�v�n� parametr� a zamyslete se nad t�m, zda je mo�n� parametry
** p�ed�vat jin�m zp�sobem (hodnotou/odkazem) a v p��pad�, �e jsou ob�
** mo�nosti funk�n� p��pustn�, jak� jsou v�hody �i nev�hody toho �i onoho
** zp�sobu.
**
** V p��kladech jsou pou�ity polo�ky, kde kl��em je �et�zec, ke kter�mu
** je pid�n obsah - re�ln� ��slo.
*/

#include "c016.h"

int HTSIZE = MAX_HTSIZE;
int solved;

/*          ------- 
** Rozptylovac� funkce - jej�m �kolem je zpracovat zadan� kl�� a p�id�lit
** mu index v rozmez� 0..HTSize-1.  V ide�ln�m p��pad� by m�lo doj�t
** k rovnom�rn�mu rozpt�len� t�chto kl��� po cel� tabulce.  V r�mci
** pokus� se m��ete zamyslet nad kvalitou t�to funkce.  (Funkce nebyla
** volena s ohledem na maxim�ln� kvalitu v�sledku). }
*/

int hashCode ( tKey key ) {
	int retval = 1;
	int keylen = strlen(key);
	for ( int i=0; i<keylen; i++ )
		retval += key[i];
	return ( retval % HTSIZE );
}

/*
** Inicializace tabulky s explicitn� z�et�zen�mi synonymy.  Tato procedura
** se vol� pouze p�ed prvn�m pou�it�m tabulky.
*/

void htInit ( tHTable* ptrht ) {
	for (int i=0; i<MAX_HTSIZE; i++)
	{
		(*ptrht)[i] = NULL;
		/* Vsechny prvky tabulky budou po inicializaci prazdne. */
	}
}

/* TRP s explicitn� z�et�zen�mi synonymy.
** Vyhled�n� prvku v TRP ptrht podle zadan�ho kl��e key.  Pokud je
** dan� prvek nalezen, vrac� se ukazatel na dan� prvek. Pokud prvek nalezen nen�, 
** vrac� se hodnota NULL.
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
** TRP s explicitn� z�et�zen�mi synonymy.
** Tato procedura vkl�d� do tabulky ptrht polo�ku s kl��em key a s daty
** data.  Proto�e jde o vyhled�vac� tabulku, nem��e b�t prvek se stejn�m
** kl��em ulo�en v tabulce v�ce ne� jedenkr�t.  Pokud se vkl�d� prvek,
** jeho� kl�� se ji� v tabulce nach�z�, aktualizujte jeho datovou ��st.
**
** Vyu�ijte d��ve vytvo�enou funkci htSearch.  P�i vkl�d�n� nov�ho
** prvku do seznamu synonym pou�ijte co nejefektivn�j�� zp�sob,
** tedy prove�te.vlo�en� prvku na za��tek seznamu.
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
** TRP s explicitn� z�et�zen�mi synonymy.
** Tato funkce zji��uje hodnotu datov� ��sti polo�ky zadan� kl��em.
** Pokud je polo�ka nalezena, vrac� funkce ukazatel na polo�ku
** Pokud polo�ka nalezena nebyla, vrac� se funk�n� hodnota NULL
**
** Vyu�ijte d��ve vytvo�enou funkci HTSearch.
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
** TRP s explicitn� z�et�zen�mi synonymy.
** Tato procedura vyjme polo�ku s kl��em key z tabulky
** ptrht.  Uvoln�nou polo�ku korektn� zru�te.  Pokud polo�ka s uveden�m
** kl��em neexistuje, d�lejte, jako kdyby se nic nestalo (tj. ned�lejte
** nic).
**
** V tomto p��pad� NEVYU��VEJTE d��ve vytvo�enou funkci HTSearch.
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

/* TRP s explicitn� z�et�zen�mi synonymy.
** Tato procedura zru�� v�echny polo�ky tabulky, korektn� uvoln� prostor,
** kter� tyto polo�ky zab�raly, a uvede tabulku do po��te�n�ho stavu.
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

