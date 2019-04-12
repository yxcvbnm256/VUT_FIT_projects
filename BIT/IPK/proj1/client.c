#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <netdb.h>
#include <getopt.h>
#include <stdbool.h>

typedef struct charList charList;

/* Struktura uchovavajici zadane argumenty */
typedef struct arguments 
{
	charList *list;
	int listLength;
	char *hostname;
	int uid;
	int port;
	bool L;
	bool G;
	bool U;
	bool N;
	bool H;
	bool S;	 
} Arguments;

/* Linearni seznam, do ktereho se uchovavaji informace ve forme stringu */
struct charList
{
	char* c;
	charList * next;
	charList * previous;
};

/* Vlozeni prvku do linearniho seznamu */
charList* charListInsert(charList *list, char *c)
{
charList *tmp = list;
	if (list != NULL)
	{
		while (tmp->next != NULL)
		{
			tmp = tmp->next;
		}
	}
	charList *newList = NULL;
	newList = malloc(sizeof(charList));
	newList->c = c;
	newList->next = NULL;
	if (list == NULL) return newList;
	if (tmp != NULL)tmp->next = newList;
	return list;}

/* Zpracovani argumentu */
Arguments argumentCheck(int argc, char *argv[])
{
	Arguments args;
	args.list = NULL;
	int c;
	int index;
	char* next;
	args.listLength = 0;
	char *arguments;
	
	while((c = getopt(argc, argv, "h:p:l:u:LUGNHS")) != -1)
	{
		switch (c)
		{
			case 'h':
				args.hostname = optarg;
				break;
			case 'p':
				args.port = atoi(optarg);
				break;
			case 'l':
				index = optind - 1;
				while(index < argc)
				{

	                if ((next = malloc(3*sizeof(char) + strlen(argv[index])*sizeof(char))) == NULL) /* get login */
	                next[0]='\0';
	                strcat(next,"-l=");
	                strcat(next, argv[index]);
	                index++;	 

	                if(argv[index-1][0] != '-')
	                {         /* check if optarg is next switch */	                    
	                    args.list = charListInsert(args.list, next);	
	                    args.listLength++; 	                                 	
	                }
	                else
	                {
	                	free(next);
	                	next = NULL;
	                	break;	                	
	                }
            	}     	
				optind = index - 1;	
				break;	
			case 'u':
				index = optind - 1;
				while(index < argc)
				{
	                if ((next = malloc(3*sizeof(char) + strlen(argv[index])*sizeof(char))) == NULL) /* get login */
	                	{printf("chyba\n");exit(-1);}
	                next[0]='\0';
	                strcat(next,"-u=");
	                strcat(next, argv[index]);
	                index++;	 

	                if(argv[index-1][0] != '-')
	                {         /* check if optarg is next switch */	                    
	                    args.list = charListInsert(args.list, next);	
	                    args.listLength++;                	
	                }
	                else
	                {
	                	free(next);
	                	next = NULL;
	                	break;	                	
	                }
	                
            	}            				
				optind = index - 1;	
				break;	
			case 'L':
				arguments = malloc(sizeof(char)*2);
				arguments[0]='\0';
				strcpy(arguments, "-L");
				args.list = charListInsert(args.list, arguments);
				args.listLength++;
				break;
			case 'U':
				arguments = malloc(sizeof(char)*2);
				arguments[0]='\0';				
				strcpy(arguments, "-U");
				args.list = charListInsert(args.list, arguments);
				args.listLength++;
				break;
			case 'G':
				arguments = malloc(sizeof(char)*2);
				arguments[0]='\0';
				strcpy(arguments, "-G");
				args.list = charListInsert(args.list, arguments);
				args.listLength++;
				break;
			case 'N':
				arguments = malloc(sizeof(char)*2);
				arguments[0]='\0';
				strcpy(arguments, "-N");
				args.list = charListInsert(args.list, arguments);
				args.listLength++;
				break;
			case 'H':
				arguments = malloc(sizeof(char)*2);
				arguments[0]='\0';
				strcpy(arguments, "-H");
				args.list = charListInsert(args.list, arguments);
				args.listLength++;
				break;
			case 'S':
				arguments = malloc(sizeof(char)*2);
				arguments[0]='\0';
				strcpy(arguments, "-S");
				args.list = charListInsert(args.list, arguments);
				args.listLength++;
				break;
		}
	}
	return args;	
}	

int main (int argc, char *argv[])
{
	Arguments args = argumentCheck(argc, argv);	
	if (args.port == 0) 
	{
		return -1;	
	}
	if (args.hostname == NULL) 
	{
		return -1;	
	}
	int s, n;
	char *msg = malloc(sizeof(char));

	char *newStr;
	char *temp;
	msg[0] = '\0';
	charList *tmp;
	/* Zruseni linearniho seznamu a zaroven zkopirovani jehoh prvku do jednoho pole typu char */
	for (int i = 0; i < args.listLength; i++)
	{
		tmp = args.list;
		newStr = malloc(strlen(msg)*sizeof(char)+strlen(tmp->c)*sizeof(char) + sizeof(char));
		newStr[0] = '\0';
		strcat(newStr, msg);
		strcat(newStr, tmp->c);
		newStr[strlen(msg)+strlen(tmp->c)] = '\0';
		free(msg);		
		msg = newStr;
		args.list = args.list->next;		
		free(tmp->c);
		free(tmp);
		tmp = NULL;
	}	
  	struct sockaddr_in sin; struct hostent *hptr;
    if ( (s = socket(PF_INET, SOCK_STREAM, 0 ) ) < 0)
    { /* create socket*/
	    perror("chyba pri tvorbe socketu");  /* socket error */
	    return -1;
  	}
  	sin.sin_family = PF_INET;              /*set protocol family to Internet */
	sin.sin_port = htons(args.port);  /* set port no. */
	if ( (hptr =  gethostbyname(args.hostname) ) == NULL)
	{
		fprintf(stderr, "gethostname chyba: %s", argv[1]);
	    return -1;
	}
	memcpy( &sin.sin_addr, hptr->h_addr, hptr->h_length);
	if (connect (s, (struct sockaddr *)&sin, sizeof(sin) ) < 0 )
	{
		perror("chyba pri pripojeni"); return -1;   /* connect error */
	}
	char lengthMsg[15];
	sprintf(lengthMsg, "%zu", strlen(msg));
	/* Zaslani zpravy serveru s informaci o delce dalsi zpravy obsahujici informace o argumentech */
	if ( write(s, lengthMsg, strlen(lengthMsg)) < 0 )
	{
		perror("chyba pri zapisu");    return -1;
	}
	usleep(10);
	/* Zaslani zpravy obsahujici informace o argumentech */
	if ( write(s, msg, strlen(msg) +1) < 0 ) 
	{
		perror("chyba pri zapisu");    return -1;
	}
	/* Precteni zpravy o tom, jak dlouha bude zprava obsahujici vysledek */
	memset(lengthMsg, 0, sizeof lengthMsg);  
	if ( ( n = read(s, lengthMsg, sizeof(lengthMsg) ) ) <0)
	{
		perror("chyba pri cteni"); return -1;
	}
	/* Pokud je vysledna zprava neprazdna, prijme se a vypise se */
	if (atoi(lengthMsg) != 0)
	{
		usleep(1);
		free(msg);
		msg = NULL;	
		msg = malloc(sizeof(char)*atoi(lengthMsg) + sizeof(char));
		msg[0] = '\0';
		if ( ( n = read(s, msg, atoi(lengthMsg) ) ) <0)
		{  /* read message from server */
		    perror("chyba pri cteni"); return -1; /*  read error */
		}
		msg[atoi(lengthMsg)] = '\0';
		printf ("%s\n", msg);  /* print message to screen */
		usleep(100);
	}
	/* To same se opakuje pro zpravu s chybovymi zapisy */
	memset(lengthMsg, 0, sizeof lengthMsg);  
	if ( ( n = read(s, lengthMsg, sizeof(lengthMsg) ) ) <0)
	{ 
		perror("chyba pri cteni"); return -1;
	} 
	if (atoi(lengthMsg) != 0)
	{
		usleep(1);
		free(msg);
		msg = NULL;
		msg = malloc(sizeof(char)*atoi(lengthMsg) + sizeof(char));
		msg[0] = '\0';
		if ( ( n = read(s, msg, atoi(lengthMsg) ) ) <0)
		{
		    perror("error on read"); return -1;
		}
		msg[atoi(lengthMsg)] = '\0';
		fprintf(stderr,"%s",msg);
		free(msg);
	}
	if (close(s) < 0)
	{ 
	    perror("chyba pri uzavirani");
	    return -1;
	}	
  	return 0;
}
