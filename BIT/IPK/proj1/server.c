#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <netdb.h>
#include <stdbool.h>
/* Struktura uchovavajici informace o tom, jako chce klient informace */
typedef struct {
  bool N;
  bool L;
  bool U;
  bool G;
  bool H;
  bool S;
} Flags;
/* Funkce, ktera prida kus stringu do jineho stringu, treti parametr je oddelujici znak */
char* addToString(char *newString, char *occurence, char character)
{
  char *temp;
  if (newString == NULL)  {if ((temp = malloc((strlen(occurence)*sizeof(char)) + 2*sizeof(char))) == NULL) printf("chyba v alokaci\n");}
  else {if ((temp = malloc((sizeof(char)*strlen(newString)) + (strlen(occurence)*sizeof(char)) + 2*sizeof(char))) == NULL) printf("chyba v alokaci\n");}
  temp[0] = '\0';
  if (newString != NULL) strcat(temp, newString);
  strcat(temp, occurence);
  if (newString != NULL)  temp[strlen(newString)+strlen(occurence)] = character;
  else temp[strlen(occurence)] = character;
  if (newString != NULL)  temp[strlen(newString)+strlen(occurence)+1] = '\0';
  else temp[strlen(occurence)+1] = '\0';
  if (newString != NULL) free(newString);
  return temp;
}
/* Funkce, ktera prochazi soubor passwd po radcich a hleda login/uid. buffer - obsah souboru, b - hledany uid/login, setUid - true znamena hledani
uid, false hledani loginu. Flags - informace, ktere chce klient. FinalErrorString - zde se uklada pripadne nenalezeny login/uid */
char* checkFile(char* buffer, char *b, bool setUid, Flags *flags, char** finalErrorString)
{
    char *newString = NULL;
    int counter;
    char *tmp = NULL;
    char *c;
    char *d;
    char *e;
    char *occurence;
    if (buffer)
    {
        c = strtok_r(buffer, "\n", &tmp);
        
        while (c != NULL)
        {
            newString = NULL;
            d = malloc(sizeof(char)*strlen(b) + 2*sizeof(char));
            d[0] = '\0';
            /* Pokud server hleda uid, musi se v radku posunout */
            if (setUid == true)
            {
                
                char* uid = malloc(strlen(c)*sizeof(char) + sizeof(char));
                char *tmp2;
                tmp2 = uid;
                strcpy(uid,c);
                uid[strlen(c)] = '\0';
                uid = strstr(uid, ":");  
                uid[0] = 'l';              
                uid = strstr(uid, ":");
                uid[0] = 'l';              
                uid = strstr(uid, ":");
                uid = strstr(uid, uid+1);                         
                strncpy(d,uid,strlen(b)+1);
                free(tmp2); 
            }
            else strncpy(d,c,strlen(b)+1);            
            d[strlen(b)+1] = '\0';
            if (d[strlen(b)] == ':')
            {             
                d[strlen(b)] = '\0';
                if (strcmp(d, b) == 0)
                {
                    occurence = strtok_r(c, ":", &e);
                    for (counter=0; counter <= 6; counter++)
                    {
                        switch(counter)
                        {
                            case 0:
                              if (flags->L == true) newString = addToString(newString, occurence, ' ');                             
                              break;
                            case 1:
                              break;
                            case 2:
                              if (flags->U == true) newString = addToString(newString, occurence, ' ');
                              break;
                            case 3:
                              if (flags->G == true) newString = addToString(newString, occurence, ' ');
                              break;
                            case 4:
                              if (flags->N == true) newString = addToString(newString, occurence, ' ');
                              break;
                            case 5:
                              if (flags->H == true) newString = addToString(newString, occurence, ' ');
                              break;
                            case 6:
                              if (flags->S == true) newString = addToString(newString, occurence, ' ');
                              break;
                            default:
                              break;
                        }
                        occurence = strtok_r(NULL, ":", &e);
                    }
                }              
            }
            free(d);
            d = NULL;
            if (newString != NULL) return newString; 
            c = strtok_r(NULL, "\n", &tmp);
        }          
    }
    /* Pokud neni hledany login/uid nalezen, prida se do ErrStringu zaznam */
    if (strcmp(b,"") != 0)
    {
        char *temptmp = NULL;
        char* errorString;
        if (setUid != true) errorString = malloc(22*sizeof(char) + strlen(b)*sizeof(char));
        else errorString = malloc(20*sizeof(char) + strlen(b)*sizeof(char));
        errorString[0]='\0';
        if (setUid != true) strcat(errorString, "chyba: neznamy login ");
        else strcat(errorString, "chyba: nezname uid ");
        strcat(errorString, b);
        if (setUid != true) errorString[21+strlen(b)] = '\0';
        else errorString[19+strlen(b)] = '\0';
        if (*finalErrorString != NULL)temptmp = strstr(*finalErrorString, errorString);
        if (temptmp == NULL)    *finalErrorString = addToString(*finalErrorString, errorString, '\n');
        free(errorString);
    }
    return NULL;
}

/* Funkce, ktera pouze zpracuje port */
int checkArguments(int argc, char *argv[])
{
    int c;
    while((c = getopt(argc, argv, "p:")) != -1)
    {
        if (c == 'p') return atoi(optarg);      
    }
    return -1;
}

int main (int argc, char *argv[])
{
    int s, t, sinlen;
    struct sockaddr_in sin;
    int i,counter,msgLength;
    char *msg;
    char *a;
    char *b;
    char *c;
    char *d;
    bool loginInsert = false;
    bool uidInsert = false;
    char *finalErrorString = NULL;
    char *occurence;
    char *finalMsg = NULL;
    char *newString = NULL;
    //newString[0] = '\0';
    char *tmp;
    char * buffer = 0;
    long length;
    struct hostent * hp;
    int j;
Flags *flags;
    flags = malloc(sizeof(Flags));
    FILE *fp;
    int port = checkArguments(argc, argv);
    if (port < 0)
    { 
        printf("%d port\n", port); /* input error: need port no! */
        return -1;
    }
    if ( (s = socket(PF_INET, SOCK_STREAM, 0 ) ) < 0)
    { /* tvorba socketu */
        printf("chyba pri tvorbe socketu\n"); 
        return -1;
    }
    sin.sin_family = PF_INET;              
    sin.sin_port = htons(port);  
    sin.sin_addr.s_addr  = INADDR_ANY;  
    if (bind(s, (struct sockaddr *)&sin, sizeof(sin) ) < 0 )
    {
        printf("chyba pri bindu\n"); return -1;
    }
    if (listen(s, 5))
    { 
        printf ("chyba pri listen\n");
        return -1;
    }
    sinlen = sizeof(sin);
    while (1)
    {
        /* Smycka, ve ktere jsou zpracovavany pozadavky klienta */
        if ( (t = accept(s, (struct sockaddr *) &sin, &sinlen) ) < 0 )
        {
            printf("chyba pri acceptu\n");
            return -1;
        }
        pid_t pid = fork();
        if (pid < 0)
        {
          perror("fork se nezdaril");
          exit(EXIT_FAILURE);
        }
        /* Server dokaze zpracovavat vice klientu zaroven - rodicovksy proces ceka ve smycce na dalsiho klienta a
        vytvori detsky proces, ktery klienta obslouzi */
        if (pid == 0)
        {
            hp=(struct hostent *)gethostbyaddr((char *)&sin.sin_addr,4,AF_INET);
            j=(int)(hp->h_length);
            char lengthMsg[10];
            /* Server dostava informaci, jak dlouha bude zprava od klienta */
            if ( read(t, lengthMsg, sizeof(lengthMsg) ) <0)
            {  
                printf("chyba pri cteni\n");
                return -1;
            }
            usleep(1);
            /* Server dostava zpravu od klienta obsahujici informace o tom, co ma server hledat */
            msgLength = atoi(lengthMsg);
            msg = malloc((msgLength+1)*sizeof(char));
            if ( read(t, msg, msgLength ) <0)
            {
                printf("error on read\n");
                return -1;
            }
            msg[msgLength] = '\0';
            //printf("zprava je %s\n",msg);
            if (strstr(msg, "-L") != NULL)  flags->L = true;
            if (strstr(msg, "-U") != NULL)  flags->U = true;
            if (strstr(msg, "-G") != NULL)  flags->G = true;
            if (strstr(msg, "-N") != NULL)  flags->N = true;
            if (strstr(msg, "-H") != NULL)  flags->H = true;
            if (strstr(msg, "-S") != NULL)  flags->S = true; 
            /* Zprava je rozkodovana po znaku '-' */           
            a = strtok_r(msg,"-", &c);
            while (a != NULL)
            {                
                fp = fopen("/etc/passwd", "r");
                if (fp)
                {
                    fseek (fp, 0, SEEK_END);
                    length = ftell (fp);
                    fseek (fp, 0, SEEK_SET);
                    buffer = malloc (length);
                    if (buffer)
                    {
                        fread (buffer, 1, length, fp);
                    }
                    fclose (fp);
                }
                else return -1;
                if ((b = strstr(a, "l=")) != NULL)
                {
                   if (uidInsert == true)
                    {
                        if (finalMsg != NULL)
                        {
                            free(finalMsg);
                            finalMsg = NULL; 
                        }
                        if (finalErrorString != NULL)
                        {
                            free(finalErrorString);
                            finalErrorString = NULL;
                        }
                        uidInsert = false;
                    }
                    b = b+2;  
                    newString = checkFile(buffer, b, false, flags, &finalErrorString);
                    loginInsert = true;
                }
                else if ((b = strstr(a, "u=")) != NULL)
                {  
                    if (loginInsert == true)
                    {
                        if (finalMsg != NULL)
                        {
                            free(finalMsg);
                            finalMsg = NULL; 
                        }
                        if (finalErrorString != NULL)
                        {
                            free(finalErrorString);
                            finalErrorString = NULL;
                        }
                        loginInsert = false;
                    }                  
                    b = b+2;   
                    newString = checkFile(buffer, b, true, flags, &finalErrorString); 
                    uidInsert = true;                  
                }
                free(buffer);
                buffer = NULL;
                /* Pokud je nalezen zaznam, prida se k finalnimu vysledku */
                if (newString != NULL)
                {
                    char *temptmp = NULL;
                    if (finalMsg != NULL) temptmp = strstr(finalMsg, newString);
                    newString[strlen(newString)] = '\0';
                    if (temptmp == NULL)    finalMsg = addToString(finalMsg, newString, '\n');
                    free(newString);
                    newString = NULL;
                    finalMsg[strlen(finalMsg)] = '\0';
                }   
                a = strtok_r(NULL, "-", &c);                
            }
            finalMsg[strlen(finalMsg)-1] = '\0';            
            memset(lengthMsg, 0, sizeof lengthMsg);
            if (finalMsg == NULL) lengthMsg[0] = '0';
            else sprintf(lengthMsg, "%zu", strlen(finalMsg));
            /* Zaslani zpravy klientovi o tom, jak dlouha je vylsedna zprava */
            if ( write(t, lengthMsg, strlen(lengthMsg) ) < 0 )
            { 
                printf("chyba pri zapisu\n");    return -1;
            }
            /* Pokud je vysledna zprava neprazdna, posle se klientovi */
            if (finalMsg != NULL)
            {
                usleep(1);
                if ( write(t, finalMsg, strlen(finalMsg) ) < 0 )
                {
                    printf("chyba pri zapisu\n");    return -1;
                }
                free(finalMsg);
                finalMsg = NULL;
            }
            free(msg);                        
            msg = NULL;
            usleep(100); 
            /* Cely proces s posilanim zprav se opakuje pro nalezene chyby */
            memset(lengthMsg, 0, sizeof lengthMsg); 
            if (finalErrorString == NULL)   lengthMsg[0] = '0';           
            else sprintf(lengthMsg, "%zu", strlen(finalErrorString));          
            if ( write(t, lengthMsg, strlen(lengthMsg) ) < 0 )
            {  
                printf("chyba pri zapisu\n");    return -1; 
            }
            if (finalErrorString != NULL)
            {
                usleep(1);
                if ( write(t, finalErrorString, strlen(finalErrorString) ) < 0 )
                {
                    printf("chyba pri zapisu\n");    return -1;
                } 
                free(finalErrorString);
                finalErrorString = NULL;
            }   
free(flags);         
            if (close(t) < 0) 
            {
                printf("chyba pri ukonceni"); return -1;
            } 
        
        } // zde se nikdy server nedostane!
    }
    if (close(s) < 0) { printf("close"); return -1;}
    return 0;
}
