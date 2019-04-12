/*
* Soubor: proj1.c
* Jmeno: Jan Pawlus, xpawlu00@fit.vutbr.cz
* Datum: 9. 11. 2013
* Projekt: Pocitani slov, projekt c. 1 pro predmet IZP
* Popis: Program pocita znak, ktery zada uzivatel do argumentu, v retezci znaku.
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>


/* Funkce printhelp vypise napovedu k pouzivani programu */
void printhelp(){
    printf("Program pocitani slov\n"
    "Autor: Jan Pawlus\n"
    "Program pocita znak, ktery zada uzivatel do argumentu, v retezci znaku.\n"
    "proj1 --help: Vypise tuto napovedu.\n"
    "proj1 [X] N -d:\n"
    "   - [X] je hledany znak. Argument je povinny a musi byt na prvnim miste.\n"
    "   - [X] muze byt a-z, A-Z, 0-9, - nebo _. Muzou byt pouzity zastupne znaky:\n"
    "       . reprezentuje libovolny znak\n"
    "       : reprezentuje libovolnou cislici\n"
    "       ^ reprezentuje libovolne velke pismeno\n"
    "   - N je pozice, na ktere se ma hledany znak nachazet. Pozice je nepovinna.\n"
    "   - -d je mod vypisujici vsechny zadane znaky, nepovinny, max 80 znaku na slovo\n");
}

/* Funkce detekceznaku vrátí hodnotu true, pokud je promenna c zadana uzivatelem znak a-z, A-Z, 0-9, - nebo _. */
bool detekceznaku(int c){
    if (((c>='0') && (c <= '9')) || ((c>='a') && (c <= 'z')) || ((c>='A')&&(c <= 'Z')) || (c == '_') || (c == '-')){return true;}
    else return false;
}
/* Funkce velkapismena vrati true, pokud je promenna c zadana uzivatelem velke pismeno. */
bool velkapismena(int c){
    if((c>='A')&&(c <= 'Z')){return true;}
    else return false;
}
/* Funkce detekcecisla vrati true, pokud je ordinalni hodnota promenne c zadane uzivatelem rovna ordinalnim hodnotam cislic 0-9. */
bool detekcecisla(int c){
    if ((c>='0') && (c <= '9')){return true;}
    else return false;
}
/* Funkce hledanipozice zkouma, pokud je v argumentech programu cislo, neboli pozice a prevede jej na datovy typ integer. */
int hledanipozice(int argc, char *argv[]){
    int i;
        int pozice = 0;
        /* Tento cyklus prohledava jednotlive argumenty - zacina na poziici 2, nebot na pozici 1 musi byt hl. znak. */
        for (i = 2; i<argc; i++){
            /* Tenro cyklus for je zanoøen do minulého cyklu - prohledává jednotlivé  znaky urcitého argumentu. */
            for (unsigned int j=0; j< strlen(argv[i]); j++){
                /* Pokud znak neni cislo, funkce skonci a vrati 0. */
                if(detekcecisla(argv[i][j]) == false){return 0;}
                else{
                    /* Vynasobeni 10ti - zvyseni radu */
                    if (pozice>=INT_MAX/10){
                        return INT_MAX;
                    }
                    pozice *= 10;
                    /* Prevod z ASCII tabulky - 0 ma ordinalni hodnotu 48, takze z 0 jako charu dostaneme 0 jako integer
                    odectenim 48) */
                    pozice += (argv[i][j]-'0');
                }
            }
            return pozice;
        }
    return 0;
}
/* Funkce hledanimodu zjistuje, zda uzivatel zadal argument --help nebo -d. */
int hledanimodu(int argc, char *argv[]){
    int i;
    for (i = 1; i<argc; i++){
        /* Funkce strcmp porovnava dva retezce znaku. Pokud jsou stejne, vrati 0. */
        if(strcmp(argv[i],"--help") == 0){
            /* Pokud je nejaky argument roven --help, funkce vrati ordinalni hodnotu znaku h jako help */
            return 'h';
        }
        else if(strcmp(argv[i],"-d") == 0){
            /* Pokud je nejaky argument roven -d, funkce vrati ordinalni hodnotu znaku d jako debug */
            return 'd';
        }
    }
    /* Pokud neni zadny argument roven --help ani -d, vrati funkce 0. */
    return 0;
}
/* Funkce hledaniznaku zjistuje, zda je na prvni pozici opravdu spravne zadany hledany znak. */
int hledaniznaku(int argc, char *argv[]){
    /* Pokud je pocet argumentu vetsi nez jedna, funkce probehne dale, jinak ne - na pozici argumentu 0 je totiz cesta k souboru */
    if(argc>1){
        /* Funkce vrátí ordinální hodnotu ^, . nebo :, pokud jsou obsaženy v argumentu hledany znak. pokud je argument a-z, A-Z, - nebo _, vrátí funkce tento znak. */
        if(strcmp(argv[1],"^") == 0){return '^';}
        else if(strcmp(argv[1],".") == 0){return '.';}
        else if(strcmp(argv[1],":") == 0){return ':';}
        else if(strcmp(argv[1],"-d") == 0){return 0;}
        else if(detekceznaku(argv[1][0])){
            /* Delka argumentu musi byt 1 */
            if (strlen(argv[1])==1){
                return argv[1][0];
            }else return 0;
        }
        else return 0;
    }else return 0;
}
/* Pokud uzivatel nezadal pozici, funkce vrati true. Pokud ji zadal a hledany znak je na zadane pozici, take vrati true */
bool pozicepodminka (int *pozice, int *delkaslova){
    if (*pozice == 0){return true;}
    if (*pozice == *delkaslova){return true;}
    else return false;
}
/* Funkce switchznak vrati true, pokud plati sada podminek (vetve switche se vykonavaji podle hledaneho znaku) */
bool switchznak(int *c,int *hledanyznak,int *pozice,int *delkaslova){
    switch (*hledanyznak){
        case '^':
            /* Pokud hledame velka pismena, potom funkce vrati true, pokud je zadany znak c velke pismeno a pokud
            je zadany znak na spravne pozici */
            if (velkapismena(*c) && pozicepodminka(&(*pozice), &(*delkaslova))){return true;}else return false;
            break;
        case '.':
            if (detekceznaku(*c) && pozicepodminka(&(*pozice), &(*delkaslova))){return true;}else return false;
            break;
        case ':':
            if (detekcecisla(*c) && pozicepodminka(&(*pozice), &(*delkaslova))){return true;}else return false;
            break;
        default:
            if (*c == *hledanyznak && pozicepodminka(&(*pozice), &(*delkaslova))){return true;}else return false;
            break;
        return false;
    }
}
/* Funkce nactiznak nacte uzivatelem zadany znak, pokud se zadany znak rovna argumentu pro hledany znak, pricte se k souctu 1. */
void nactiznak(int *hledanyznak, int *pozice, int *mode){
    int delkaslova = 0;
    int wordcount = 0;
    /* Promenna alreadycounted zajistuje, aby se do pocitani znaku nezapocetly znaky vickrat. Na zacatku je false. */
    bool alreadycounted = false;
    int c;
    /* Pokud se zadany znak nerovna End of file, program pokracuje v nacitani znaku */
    while ((c = getchar()) != EOF){
        /* Pokud je c a-z, A-Z, 0-9, - nebo _ */
        if (detekceznaku(c)){
            /* Pocet znaku v jednom slove */
            delkaslova = delkaslova + 1;
            /* Pokud je mod debug a zaroven delka slova neni vetsi nez 80, zadany znak se vypise */
            if((*mode=='d')&&(delkaslova <= 80)){printf("%c", c);}
            /* Pokud vyse popsana funkce switchznak vrati hodnotu true, pricte se k poctu slov 1. */
            if (alreadycounted == false){
                if ((switchznak(&c,&(*hledanyznak),&(*pozice),&delkaslova)) == true){
                    wordcount = wordcount + 1;
					/* Pokud se pricte 1 k poctu slov, toto slovo uz je zapocitane - tato podminka zaridi, ze dokud nebude zadany znak oddelovac,
					do funkce switchznak se program nedostane */
					alreadycounted = true;
                }
            }
        }
        /* Pokud bude zadany znak oddelovac, alreadycounted se nastavi na false a delkaslova na 0 - tim se ukonci aktualni slovo */
        else{
            /* Delka slova musi byt ruzna od nuly, aby se vypsal novy radek - diky se pri vice oddelovacich za sebou neudela vic novych radku. */
            if((*mode == 'd') && (delkaslova != 0)){printf("\n"); }
            alreadycounted = false;
            delkaslova = 0;
        }
    }
    /* Vypise konecny pocet slov */
    printf("%d\n",wordcount);
}

int main(int argc, char *argv[]){
    /* Vytvori promennou mode, do ktere ulozi vysledek funkce hledanimodu */
    int mode = hledanimodu(argc, argv);
    /* Pokud je mode 'h' (help), zavola se funkce printhelp, ktera vypise napovedu a ukonci program tim, ze vrati 1. */
    if (mode == 'h'){
        printhelp();
        return 1;
    }
    int pozice = hledanipozice(argc, argv);
    int hledanyznak = hledaniznaku(argc, argv);
    /* Pokud je hledanyznak 0, je spatne zadany povinny argument hledany znak - vypise se chyba a program se ukonci. */
    if (hledanyznak == 0){
        printf("Spatne zadany argument pro hledany znak! Pro napovedu zadejte argument --help\n");
        return 0;
    }
    /* Pokud hledanyznak ruzny od nuly, provede se funkce nactiznak. */
    else {nactiznak(&hledanyznak, &pozice, &mode);}
    return 1;
}
