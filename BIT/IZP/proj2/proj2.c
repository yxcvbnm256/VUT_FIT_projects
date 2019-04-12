/*
* Soubor: proj2.c
* Jmeno: Jan Pawlus, xpawlu00@stud.fit.vutbr.cz
* Datum: 1. 12. 2013
* Projekt: Uhly trojuhelniku, projekt c. 2 pro predmet IZP
* Popis: Program pocita uhly trojuhelniku, odmocninu nebo arkus sinus zadanych hodnot.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PI 3.14159265358
#define PRESNOST 0.00000000001

void printhelp(){
    printf("Program iteracni vypocty\n"
    "Autor: Jan Pawlus\n"
    "Program pocita odmocninu, arkus sinus nebo uhly trojuhelniku dle argumentu.\n"
    "proj2 --help: Vypise tuto napovedu.\n"
    "proj2 --sqrt X: Spocita druhou odmocninu z X. X musi byt vetsi nebo rovno nule.\n"
    "proj2 --asin X: Spocita arkus sinus z X. X musi byt z intervalu <-1;1>.\n"
    "proj2 --triangle AX AY BX BY CX CY: Spocita uhly trojuhelniku dle zadanych bodu.\n");
}

double abshod(double x)
/* Pokud je x vetsi nebo rovno nule, vratime x. Pokud je ale mensi jak 0, vratime x*(-1) */
{
    if (x < 0)
    {
        return (x = x*(-1));
    }
    if (x >= 0)
    {
        return x;
    }
    return 0;
}

double mocnina(double j, double x)
/* Vyrobí mocninu */
{
    double vysledek = x;
    for (int i = 1; i < j; i++)
    {
        vysledek = vysledek*x ;
    }
    return vysledek;
}

double my_sqrt(double x)
/* odmocnina */
{
    if (x == 0)
    {
        return 0;
    }
    else
    {
        double staraH = 0;
        double novaH = 1;
        /* relativní přesnost */
        while (abshod(staraH-novaH)>=PRESNOST*abshod(novaH))
        {
            /* Newtonova metoda - vzoreček  */
            staraH = novaH;
            novaH = 0.5 * (x/staraH + staraH);
        }
    return novaH;
    }
}

double my_asin(double x)
{
    if(x==0)
    {
        return 0;
    }
    double y;
    int altprocess = 0;
    int i = 0;
    double a = 1;
    double vysledek = 0;
    double cisla = 1;
    double staraH = 1;
    /* vypocet pro krajni hodnoty - arcsin (x) se dá napsat jako arcsin(p+q) = π - ArcSin(P) - ArcSin(Q),
    kde P = √((1/2)(1 + p² - q² + √(-4p² + (-1 - p² + q²)²))). Pokud si x rozdelime napul, p a q se budou
    rovnat a tím se vzorec zjednodusi. Pouziti teto metody se ale vyplati az od urcite hodnoty (zde 0.866 a vys).
    */
    if (x >= 0.866 || x <= -0.866)
    {
        y = x/2;
        x = my_sqrt((my_sqrt(-4*mocnina(2,y)+1)+1)*0.5);
        altprocess = 1;
    }
    while (abshod(vysledek-staraH)>=abshod(PRESNOST*vysledek))
    {
        /* Taylorova řada - vzoreček */
        cisla = cisla *(a/(a+1));
        staraH = vysledek;
        vysledek = vysledek + cisla*(mocnina(a+2,x)/(a+2));
        a = a+2;
        i++;
    }
    vysledek +=x;
    if (altprocess == 1)
    {
				/* arcsin(p+q) = π - ArcSin(P) - ArcSin(Q) */
        vysledek = PI - vysledek - vysledek;
        if (y<0)
        {
            vysledek = vysledek*(-1);
        }
    }
    //printf("pocet iteraci je %d\n",i);
    return vysledek;
}

double triangle(int argc, char *argv[])
{/*  */
    char *chyba;
    double strana1;
    double strana2;
    double strana3;
    double cosvet;
    for (int i=2; i<argc; i++)
    {
        strtod(argv[i],&chyba);
        if(strlen(chyba)!=0)
        {
            printf("Neplatne body trojuhelniku!\n");
            return 0;
        }
    }
    /* vzdalenost bodu AB = √((b1 - a1)² + (b2 - a2)²) */
    strana1 = my_sqrt(mocnina(2,(strtod(argv[4],NULL)) - (strtod(argv[2],NULL))) + (mocnina(2,(strtod(argv[5],NULL)) - (strtod(argv[3],NULL)))));
    strana2 = my_sqrt(mocnina(2,(strtod(argv[6],NULL)) - (strtod(argv[4],NULL))) + (mocnina(2,(strtod(argv[7],NULL)) - (strtod(argv[5],NULL)))));
    strana3 = my_sqrt(mocnina(2,(strtod(argv[2],NULL)) - (strtod(argv[6],NULL))) + (mocnina(2,(strtod(argv[3],NULL)) - (strtod(argv[7],NULL)))));
    /* Zjistime, zda je objekt trojuhelnik - soucet kterychkoliv dvou stran musi byt vetsi nez strana treti */
    //if (strana1 + strana2 > strana3 && strana1 + strana3 > strana2 && strana2 + strana3 > strana1)
    if (abshod(strana1+strana2-strana3) > PRESNOST && abshod(strana1+strana3-strana2) > PRESNOST && abshod(strana2+strana3-strana1) > PRESNOST)
    {
		/* Vzorecek pro vypocet kosinovy vety */
		cosvet = (mocnina(2,strana3) + mocnina(2,strana1) - mocnina(2,strana2))/(2*strana1*strana3);
		/* arccos = π/2 - arcsin(x) */
        printf("%.10e\n",PI/2 - my_asin(cosvet));
        cosvet = (mocnina(2,strana1) + mocnina(2,strana2) - mocnina(2,strana3))/(2*strana1*strana2);
        printf("%.10e\n",PI/2 - my_asin(cosvet));
        cosvet = (mocnina(2,strana2) + mocnina(2,strana3) - mocnina(2,strana1))/(2*strana2*strana3);
        printf("%.10e\n",PI/2 - my_asin(cosvet));
        return 1;
    }
    else
    {
        printf("nan\n");
        printf("nan\n");
        printf("nan\n");
        return 0;
    }
    return 0;
}

int main(int argc, char *argv[])
{
    char* chyba;
    if (argc<=1)
    {
        printf("Nezadan zadny argument! Pro napovedu zadejte --help\n");
        return 0;
    }
    if (strcmp(argv[1],"--help") == 0)
    {
        printhelp();
        return 1;
    }
    else if (strcmp(argv[1],"--sqrt") == 0 && argc == 3)
    {
        /* Funkce strtod převede string na double, pokud je v převodu chyba, naplní pole chyba */
        double x = strtod(argv[2],&chyba);
        /* Pokud je argument k odmocnině inf nebo -inf, vrátí inf nebo -inf */
        if (strcmp(argv[2],"inf")==0 || strcmp(argv[2],"-inf")==0)
        {
            printf("%s\n",argv[2]);
            return 1;
        }
        /* Pokud je x menší než nula, pole s chybou je nenulové nebo je zadán argument nan, funkce vrátí nan */
        else if (x<0 || strlen(chyba)!=0 || strcmp(argv[2],"nan")==0)
        {
            printf("nan\n");
            return 0;
        }
        /* Pokud je vše v pořádku, odmocníme */
        else
        {
            printf("%.10e\n",my_sqrt(x));
            return 1;
        }
    }
    else if (strcmp(argv[1],"--asin") == 0 && argc == 3)
    {
        double x = strtod(argv[2],&chyba);
        if (x<-1 || x > 1 || strlen(chyba)!=0 || strcmp(argv[2],"nan") == 0)
        {
            printf("nan\n");
        }
        else if (x == 0)
        {
            printf("%.10e\n",x);
            return 1;
        }
        else
        {
            printf("%.10e\n",my_asin(x));
            return 1;
        }
    }
    else if (strcmp(argv[1],"--triangle") == 0 && argc==8)
    {
        triangle(argc,argv);
        return 1;
    }
    else
    {
        printf("Spatne zadane argumenty! Pro napovedu zadejte --help.\n");
        return 0;
    }
    return 0;
}



