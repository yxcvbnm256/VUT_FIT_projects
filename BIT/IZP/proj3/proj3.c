/*
* Soubor: proj3.c
* Jmeno: Jan Pawlus, xpawlu00@stud.fit.vutbr.cz
* Datum: 15. 12. 2013
* Projekt: Hledani obrazcu, projekt c. 3 pro predmet IZP
* Popis: Program bud testuje spravnost matice, hleda nejdelsi horizontalni a vertikalni primku a nejvetsi ctverec.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

void printhelp(){
    printf("Program Hledani obrazcu\n"
    "Autor: Jan Pawlus\n"
    "Program hleda nejvetsi ctverec a nejdelsi horizontalni nebo vertikalni usecku v matici.\n"
    "proj3 --help: Vypise tuto napovedu.\n"
    "proj3 --test jmenosouboru: Otestuje, zda je matice v textovem souboru validni.\n"
    "proj3 --vline jmenosouboru: Najde nejdelsi horizontalni usecku v matici.\n"
    "proj3 --hline jmenosouboru: Najde nejdelsi vertikalni usecku v matici.\n"
    "proj3 --square jmenosouboru: Najde nejvetsi ctverec v matici.\n");
}


typedef struct {
    int rows;
    int cols;
    char *cells;
} Bitmap;

//Vrati hodnotu hledaneho bitu, bud 1 nebo 0
char getcolor(Bitmap *bmp, int x, int y)
{
    if (x>bmp->rows || y>bmp->cols)
    {
        printf("Pokus o pristup za matici\n");
        return -1;
    }
    return bmp->cells[x*bmp->cols + y];
}


int detekceCisla(int *c)
{
    if (*c == 1 || *c == 0)
    {
        return 1;
    }

    else return 0;
}


int structInitialize(Bitmap *bmp, char* argv[])
{
    FILE *soubor;
    soubor = fopen(argv[2],"r");
    if (soubor == NULL)
    {
        printf("Spatna cesta k souboru.\n");
        return 0;
    }
    fscanf(soubor,"%d %d", &bmp->rows, &bmp->cols);
    if (bmp->rows==1 && bmp->cols)
    {
        printf("Spatne zadana matice.\n");
        return 0;
    }
    bmp->cells = malloc(bmp->rows*bmp->cols*sizeof(char));
    if (bmp->cells == NULL)
    {
        printf("Invalid\n");
        return 0;
    }
    int c = 0;
    int i = 0;
    while(i<=bmp->rows*bmp->cols)
    {
    	if (fscanf(soubor,"%d",&c)!=1)
        {
            break;
        }
    	if ((isspace(c)==0 && detekceCisla(&c)==0) || c==EOF)
        {
            break;
        }
        else if (detekceCisla(&c)==1)
        {
            bmp->cells[i] = c;
            i++;
        }
    }
    if (i!=bmp->cols*bmp->rows)
    {
        printf("Invalid\n");
        return 0;
    }
    else if (strcmp(argv[1],"--test")==0)
    {
        printf("Valid\n");
        return 0;
    }
        fclose(soubor);

    return 1;
}




void structFree(Bitmap *bmp)
{
    free(bmp->cells);
}

int find_square(Bitmap *bmp, int *x1, int *y1, int *x2, int *y2)
{
    int Length = 0;
    int maxLength = 0;
    int pomI = 0;
    int pomJ = 0;
    int realLength;
    for (int i = 0; i<bmp->rows; i++)
    {
        if (maxLength >= bmp->rows-i)
        {
            break;
        }
        for (int j = 0; j<bmp->cols; j++)
        {
            if (maxLength >= bmp->cols-j)
            {
                break;
            }
            pomI = i;
            pomJ = j;
            if (getcolor(bmp,i,j)==1)
            {
                Length = 1;
                while (((pomJ+1 < bmp->cols)&&(pomI+1 < bmp->rows))&&(getcolor(bmp,pomI+1,j)==1)&&(getcolor(bmp,i,pomJ+1)==1))
                {
                    pomI++;
                    pomJ++;
                    Length++;
                }
                realLength = Length;
                int m = 0;
                for (int k = 0; k<Length; k++)
                {
                    if ((pomJ < bmp->cols)&&(pomI < bmp->rows)&&(j+m+1 == pomJ) && (i+m+1 == pomI) && (getcolor(bmp,pomI,j+m+1)==1))
                    {
                        if (realLength > maxLength)
                        {
                            maxLength = realLength;
                            Length = 0;
                            realLength = 0;
                            *x1 = i;
                            *y1 = j;
                            *x2 = i+m+1;
                            *y2 = j+m+1;
                        }
                    }
                    if (getcolor(bmp,pomI,j+m+1)==1 && getcolor(bmp,i+m+1,pomJ)==1)
                    {
                        m++;
                    }
                    else if (getcolor(bmp,pomI,j+m+1)==0 || getcolor(bmp,i+m+1,pomJ)==0)
                    {
                        if (pomI < i || pomJ < j)
                        {
                            break;
                        }
                        pomI--;
                        pomJ--;
                        realLength--;
                        k--;
                        m=0;
                    }
                }
            }
        }
    }
    return maxLength;
}




int find_vline(Bitmap *bmp, int *x1, int *y1, int *x2, int *y2)
{
    int Length = 0;
    int maxLength = 0;
    int pom;
    for (int i = 0; i < bmp->rows; i++)
    {
        if (maxLength >= bmp->rows-i)
            {
                break;
            }
        for (int j = 0; j< bmp->cols; j++)
        {
            if ((i == 0 || getcolor(bmp,i-1,j)==0)&&(getcolor(bmp,i,j)==1))
            {
                Length = 1;
                pom = i+1;
                while (getcolor(bmp,pom,j)==1 && pom<=bmp->rows)
                {
                    pom++;
                    Length++;
                }
                if (Length > maxLength)
                {
                    maxLength = Length;
                    Length = 0;

                    *y1 = j;
                    *x1 = i;
                    *y2 = j;
                    *x2 = pom-1;
                    pom = 0;
                }
            }
        }
    }
    return maxLength;
}



int find_hline(Bitmap *bmp, int *x1, int *y1, int *x2, int *y2)
{
    int Length = 0;
    int maxLength = 0;
    int i = 0;
    int j = 0;
    for (i = 0; i < bmp->rows; i++)
    {
        Length = 0;
        for (j = 0; j < bmp->cols; j++)
        {
            if (getcolor(bmp,i,j)==1)
            {
                Length++;
            }
            else
            {
                if (Length>maxLength)
                {
                    maxLength = Length;
                    *x1 = i;
                    *y1 = j - maxLength;
                    *x2 = i;
                    *y2 = j-1;
                }
                Length = 0;
                if (maxLength >= bmp->cols-j)
                {
                    break;
                }
            }
        }
        if (Length>maxLength)
            {
                maxLength = Length;
                Length = 0;
                *x1 = i;
                *y1 = j - maxLength;
                *x2 = i;
                *y2 = j -1;
            }
    }
    return maxLength;
}




int main(int argc, char *argv[])
{
    if (strcmp(argv[1],"--help")==0)
    {
        printhelp();
        return 1;
    }
    if (argc <=2)
    {
        printf("Spatne zadane argumenty.\n");
        return 0;
    }
    Bitmap bmp;

    if (structInitialize(&bmp,argv)==0)
    {
        return 0;
    }
    int x1,x2,y1,y2;

    if (strcmp(argv[1],"--hline")==0)
    {
        if(find_hline(&bmp,&x1,&y1,&x2,&y2)!=0)
        {
            printf("%d %d %d %d\n",x1,y1,x2,y2);
        }
        else
        {
            printf("Nenalezen zadny obrazec.");
        }

    }
    else if (strcmp(argv[1],"--vline") == 0)
    {
        if(find_vline(&bmp,&x1,&y1,&x2,&y2)!=0)
        {
            printf("%d %d %d %d\n",x1,y1,x2,y2);
        }
        else
        {
            printf("Nenalezen zadny obrazec.");
        }

    }
     else if (strcmp(argv[1],"--square") == 0)
    {
        if(find_square(&bmp,&x1,&y1,&x2,&y2)!=0)
        {
            printf("%d %d %d %d\n",x1,y1,x2,y2);
        }
        else
        {
            printf("Nenalezen zadny obrazec.");
        }

    }

    structFree(&bmp);


    return 0;
}
