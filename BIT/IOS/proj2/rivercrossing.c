#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <fcntl.h>
#include <string.h>

//Struktura promennych ve sdilene pameti
typedef struct {
	FILE *file;
	int counter;
	int reallyBoarded;
	int hackerOnMolo;
	int serfOnMolo;	
	int hackerReady;
	int serfReady;	
	int randomNumber;
	int captainTime;
	int pocetPlaveb;	
	int anotherRandom;

} processinfo;

//Struktura semaforu
typedef struct {
	sem_t *molo;
	sem_t *vypis;
	sem_t *boarding;
	sem_t *onBoard;
	sem_t *mainstreamSemaphore;
	sem_t *mainstreamSemaphoreX;
	sem_t *waitForOthers;
	sem_t *waitCaptain;
	sem_t *waitFinished;
	sem_t *waitDecrement;	
	sem_t *waitReady;
} semafory;

//Vycisteni pameti a semaforu a uzavreni souboru
void unlinkSemShm(processinfo *info, semafory *sema, int shm_fd) {
	sem_close(sema->molo);
	sem_close(sema->vypis);
	sem_close(sema->waitCaptain);
	sem_close(sema->onBoard);
	sem_close(sema->boarding);
	sem_close(sema->waitForOthers);
	sem_close(sema->mainstreamSemaphore);	
	sem_close(sema->mainstreamSemaphoreX);	
	sem_close(sema->waitFinished);
	sem_close(sema->waitDecrement);
	sem_close(sema->waitReady);
		
	sem_unlink("/xpawlu00sema1");
	sem_unlink("/xpawlu00sema2");
	sem_unlink("/xpawlu00sema3");
	sem_unlink("/xpawlu00sema4");
	sem_unlink("/xpawlu00sema5");
	sem_unlink("/xpawlu00sema6");
	sem_unlink("/xpawlu00sema7");	
	sem_unlink("/xpawlu00sema9");
	sem_unlink("/xpawlu00sema10");
	sem_unlink("/xpawlu00sema11");
	sem_unlink("/xpawlu00sema12");
	//Zavreni souboru
	fclose(info->file);
	munmap(info, sizeof(processinfo));
	shm_unlink("/xpawlu00Memory");
	close(shm_fd);
}

//Aktivita kazdeho hackera
void hacker(processinfo *info, semafory *sema, int hackerID, int *p) {
	int isCaptain = 0;
	sem_wait(sema->vypis);		
	fprintf(info->file,"%d: hacker: %d: started\n", ++(info->counter), hackerID);
	fflush(info->file);
	//Do souboru muze v jednu chvili zapisovat pouze jeden proces -> dalsi musi cekat, az zapisujici proces uvolni semafor
	sem_post(sema->vypis);
	//Prvni proces jde automaticky na molo, ostatni musi cekat na povoleni
	sem_wait(sema->molo);	
	sem_wait(sema->vypis);
	fprintf(info->file,"%d: hacker: %d: waiting for boarding: %d: %d\n", ++(info->counter), hackerID, ++(info->hackerOnMolo), info->serfOnMolo);
	fflush(info->file);
	sem_post(sema->vypis);
	//Pokud prisel ctvrty hacker na molo, stava se kapitanem a skupina je uzavrena; Pocet hackeru na molu se nastavi na 0
	if (info->hackerOnMolo == 4)
	{
		isCaptain = 1;
		info->hackerOnMolo = 0;
		//Posledni hacker, resp. kapitan, povoli ostatnim procesum v dalsim postupu
		sem_post(sema->boarding);
        	sem_post(sema->boarding);
        	sem_post(sema->boarding);
	}
	//Pokud prisel druhy hacker a pocet serfu je dva nebo vice, stava se tento hacker kapitanem a uzavira se skupinka (2:2). Pripadny prebytecny serf zustava na molu
	else if (info->hackerOnMolo == 2 && info->serfOnMolo >= 2)
	{
		isCaptain = 1;
		info->serfOnMolo -= 2;
		info->hackerOnMolo = 0;
		sem_post(sema->boarding);
		sem_post(sema->boarding);
		sem_post(sema->boarding);
	}
	else
	//Pokud skupinka neni v potrebnem pomeru, tak tento hacker uvolni semafor, ktery posle dalsi proces na molo
	{
		sem_post(sema->molo);
		sem_wait(sema->boarding);
	}	
	sem_wait(sema->waitReady);
	info->hackerReady++;
	sem_post(sema->waitReady);
	//Pokud soucet hackeru a serfu neni 4, cekaji na ctvrteho
	if (info->hackerReady + info->serfReady != 4)
	{
		sem_wait(sema->mainstreamSemaphore);
	} 
	//Ctvrty hacker/serf odblokuje semafor, ktery povoluje vypis boarding
	else
	{
		info->hackerReady = 0;
		info->serfReady = 0;
		sem_post(sema->mainstreamSemaphore);
		sem_post(sema->mainstreamSemaphore);
		sem_post(sema->mainstreamSemaphore);		
	}
	sem_wait(sema->vypis);
	fprintf(info->file, "%d: hacker: %d: boarding: %d: %d\n", ++(info->counter), hackerID, info->hackerOnMolo, info->serfOnMolo);
	fflush(info->file);
	info->reallyBoarded++;
	sem_post(sema->vypis);	
	if (info->reallyBoarded != 4)
	{
		sem_wait(sema->mainstreamSemaphoreX);
	}
	else
	{
		info->reallyBoarded = 0;
		sem_post(sema->mainstreamSemaphoreX);
		sem_post(sema->mainstreamSemaphoreX);
		sem_post(sema->mainstreamSemaphoreX);		
	}	
	//Pokud jsou nalodeni vsichni clenove posadky, muze zacit vypis member/captain
	sem_wait(sema->vypis);
	if (isCaptain == 1)
	{
		fprintf(info->file, "%d: hacker: %d: captain\n", ++(info->counter), hackerID);
		info->randomNumber++;
	}
	else
	{
		fprintf(info->file, "%d: hacker: %d: member\n", ++(info->counter), hackerID);
		info->randomNumber++;
	}	
	fflush(info->file);	
	sem_post(sema->vypis);
	//Pokud nevypisou sve role vsechny procesy, ceka se na posledniho
	if (info->randomNumber != 4)
	{
		sem_wait(sema->onBoard);
	}
	else
	{
		info->randomNumber = 0;
		sem_post(sema->onBoard);
		sem_post(sema->onBoard);
		sem_post(sema->onBoard);
		
	}
	//Pokud je tento hacker kapitan, simuluje plavbu uspanim
	if (isCaptain == 1)
	{
		usleep(info->captainTime*1000);
		sem_post(sema->waitCaptain);
		sem_post(sema->waitCaptain);
		sem_post(sema->waitCaptain);
	}
	//Ostatni clenove cekaji, az se kapitan probudi a uvolni semafor
	else
	{
		sem_wait(sema->waitCaptain);
	}
	//Po probuzeni se lod vraci k molu
	sem_wait(sema->vypis);
	fprintf(info->file, "%d: hacker: %d: landing: %d: %d\n", ++(info->counter), hackerID, info->hackerOnMolo, info->serfOnMolo);
	info->anotherRandom++;	
	fflush(info->file);
	sem_post(sema->vypis);
	if (info->anotherRandom != 4)
	{		
		sem_wait(sema->waitDecrement);
	}
	else
	{
		info->anotherRandom = 0;
		sem_post(sema->waitDecrement);
		sem_post(sema->waitDecrement);
		sem_post(sema->waitDecrement);		
	}
	//Az jsou vsechny procesy mimo lod, kapitan odblokuje semafor blokujici pristup dalsich procesu na molo
	if (isCaptain == 1)
	{
		sem_post(sema->molo);			
		info->pocetPlaveb++;
	}
	//Pokud probehla posledni plavba, procesy se postupne ukoncuji
	if (info->pocetPlaveb == 0.5 * (*p))
	{
		sem_post(sema->waitFinished);
	}
	sem_wait(sema->waitFinished);
	sem_post(sema->waitFinished);
	sem_wait(sema->vypis);
	fprintf(info->file, "%d: hacker: %d: finished\n", ++(info->counter), hackerID);
	fflush(info->file);
	sem_post(sema->vypis);
}
//Popis velmi podobny, jako u funkce hacker()
void serf(processinfo *info, semafory *sema, int serfID, int *p) {
	int isCaptain = 0;
	sem_wait(sema->vypis);		
	fprintf(info->file,"%d: serf: %d: started\n", ++(info->counter), serfID);
	fflush(info->file);
	sem_post(sema->vypis);
	sem_wait(sema->molo);	
	sem_wait(sema->vypis);
	fprintf(info->file,"%d: serf: %d: waiting for boarding: %d: %d\n", ++(info->counter), serfID, info->hackerOnMolo, ++(info->serfOnMolo));
	fflush(info->file);
	sem_post(sema->vypis);
	if (info->serfOnMolo == 4)
	{
		isCaptain = 1;
		info->serfOnMolo = 0;
		sem_post(sema->boarding);
                sem_post(sema->boarding);
                sem_post(sema->boarding);

	}
	else if (info->serfOnMolo == 2 && info->hackerOnMolo >= 2)
	{
		isCaptain = 1;
		info->hackerOnMolo -= 2;
		info->serfOnMolo = 0;
		sem_post(sema->boarding);
                sem_post(sema->boarding);
                sem_post(sema->boarding);

	}
	else
	{
		sem_post(sema->molo);
		sem_wait(sema->boarding);
	}
	sem_wait(sema->waitReady);
	info->serfReady++;
	sem_post(sema->waitReady);

	if (info->hackerReady + info->serfReady != 4)
	{
		sem_wait(sema->mainstreamSemaphore);
	} 
	else
	{	
		info->hackerReady = 0;
		info->serfReady = 0;
		sem_post(sema->mainstreamSemaphore);
		sem_post(sema->mainstreamSemaphore);
		sem_post(sema->mainstreamSemaphore);		
	}	
	sem_wait(sema->vypis);
	fprintf(info->file, "%d: serf: %d: boarding: %d: %d\n", ++(info->counter), serfID, info->hackerOnMolo, info->serfOnMolo);
	fflush(info->file);
	info->reallyBoarded++;
	sem_post(sema->vypis);
	if (info->reallyBoarded != 4)
	{
		sem_wait(sema->mainstreamSemaphoreX);
	}
	else
	{
		info->reallyBoarded = 0;
		sem_post(sema->mainstreamSemaphoreX);
		sem_post(sema->mainstreamSemaphoreX);
		sem_post(sema->mainstreamSemaphoreX);		
	}	
	sem_wait(sema->vypis);
	if (isCaptain == 1)
	{
		fprintf(info->file, "%d: serf: %d: captain\n", ++(info->counter), serfID);
		info->randomNumber++;
	}
	else
	{
		fprintf(info->file, "%d: serf: %d: member\n", ++(info->counter), serfID);
		info->randomNumber++;
	}	
	fflush(info->file);	
	sem_post(sema->vypis);
	if (info->randomNumber != 4)
	{
		sem_wait(sema->onBoard);
	}
	else
	{
		info->randomNumber = 0;
		sem_post(sema->onBoard);
		sem_post(sema->onBoard);
		sem_post(sema->onBoard);		
	}
	if (isCaptain == 1)
	{
		usleep(info->captainTime*1000);
		sem_post(sema->waitCaptain);
		sem_post(sema->waitCaptain);
		sem_post(sema->waitCaptain);
	}
	else
	{
		sem_wait(sema->waitCaptain);
	}
	sem_wait(sema->vypis);
	fprintf(info->file, "%d: serf: %d: landing: %d: %d\n", ++(info->counter), serfID, info->hackerOnMolo, info->serfOnMolo);
	info->anotherRandom++;
	fflush(info->file);
	sem_post(sema->vypis);
	if (info->anotherRandom != 4)
	{
		sem_wait(sema->waitDecrement);
	}
	else
	{
		info->anotherRandom = 0;
		sem_post(sema->waitDecrement);
		sem_post(sema->waitDecrement);
		sem_post(sema->waitDecrement);		
	}
	if (isCaptain == 1)
	{		
		sem_post(sema->molo);		
		info->pocetPlaveb++;
	}
	if (info->pocetPlaveb == 0.5 * (*p))
	{
		sem_post(sema->waitFinished);
	}
	sem_wait(sema->waitFinished);
	sem_post(sema->waitFinished);
	sem_wait(sema->vypis);
	fprintf(info->file, "%d: serf: %d: finished\n", ++(info->counter), serfID);
	fflush(info->file);
	sem_post(sema->vypis);
}
//Zde se vytvareji nove procesy (hackeri)
void hackerMake(int p, int hackertime, processinfo *info, semafory *sema, int shm_fd) {
	pid_t h;
	//Pole, kde se budou ukladat PID potomku	
	int hackerArray[p];
	int hackerID = 0;
	//Cyklus for vytvari pocet hackeru zadanych uzivatelem
	for (int i = 0; i < p; i++)	 
	{	
		//Novy hacker -> jeho ID se zvysi o 1	
		hackerID++;
		//Proces tvorby se uspi na nahodnou hodnotu z intervalu od nuly az po uzivatelem zadany parametr
		usleep(rand() % (hackertime*1000 + 1));		
		h = fork();
		//Akce potomka
		if (h == 0)
		{				
			hacker(info, sema, hackerID, &p);
			exit(0);			
		}
		//Akce parenta - navratova hodnota parenta po forku = PID potomka -> ulozim do pole
		else if (h > 0)
		{
			hackerArray[i] = h;
		}
		//Fork se nezdaril		
		else if (h < 0)
		{
			//Dealokace semaforu, uvolneni pameti
			unlinkSemShm(info, sema, shm_fd);
			//Pokud se fork nezdari ne na zacatku, je nutne zabit deti vytvorene pred timto forkem
			for (int k = 0; k < i; k++)
			{		
				kill(hackerArray[k], SIGTERM);
			}
            
            fprintf(stderr,"Unsuccessful fork\n");
            exit(2);
		}
	}	
}
//Velmi podobny popis jako u funkce hackerMake();
void serfMake(int p, int serftime, processinfo *info, semafory *sema, int shm_fd) {	
	pid_t j;
	int serfID = 0;
	int serfArray[p];
	for (int i = 0; i < p; i++)
	{	    	
		serfID++;
		usleep(rand() % (serftime*1000 + 1));
		j = fork();
        	if (j == 0)
       		{	        		
        		serf(info, sema, serfID, &p);
			exit(0);
     	  	}
		else if (j > 0)
		{
			serfArray[i] = j;
		}
		else if (j < 0)			
		{
			unlinkSemShm(info, sema, shm_fd);
			for (int k = 0; k < i; k++)
			{			
				kill(serfArray[k], SIGTERM);
			}			
			fprintf(stderr,"Unsuccessful fork\n");
			exit(2);               	
		}
	}
}

int main(int argc, char *argv[]) {
	int p;		
	char *interror;
	//Prevedeni argumentu na integer
	p = strtoul(argv[1], &interror, 0);	
	int hackertime = strtoul(argv[2], &interror, 0);
	int serftime = strtoul(argv[3], &interror, 0);
	int captainTime1 = strtoul(argv[4], &interror, 0);
	//Kontrola argumentu - pokud je jeden z nich spatny, program skonci s chybovou hlaskou
	if ((hackertime > 5000 || hackertime < 0) || (serftime > 5000 || serftime < 0) || (captainTime1 > 5000 || captainTime1 < 0) || (argc != 5) || (strlen(interror) != 0) || (p % 2 != 0)) 	
	{
		fprintf(stderr,"Invalid arguments.\n");
		exit(1);
	}
	//Kontrola argumentu uspesna
	else
	{
		int shm_fd = shm_open("/xpawlu00Memory", O_CREAT | O_EXCL | O_RDWR, 0644);
		if (shm_fd == -1)
		{
			fprintf(stderr,"Shared memory opening failed\n");
			close(shm_fd);
			exit(2);
		}	
		//Misto ve sdilene pameti pro strukturu
		ftruncate(shm_fd, sizeof(processinfo));
		processinfo *info;
		//Namapovani sdilene pameti do adresneho prostoru procesu
		info = mmap(NULL, sizeof(processinfo), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
		info->file=fopen("rivercrossing.out","w");
		fflush(info->file);

		semafory sema;
		//Otevreni semaforu		
		sema.vypis = sem_open("/xpawlu00sema1", O_CREAT | O_EXCL, 0644, 1);
		sema.molo = sem_open("/xpawlu00sema2", O_CREAT | O_EXCL, 0644, 1);
		sema.boarding = sem_open("/xpawlu00sema3", O_CREAT | O_EXCL, 0644, 0);
		sema.waitCaptain = sem_open("/xpawlu00sema4", O_CREAT | O_EXCL, 0644, 0);
		sema.onBoard = sem_open("/xpawlu00sema5", O_CREAT | O_EXCL, 0644, 0);
		sema.waitForOthers = sem_open("/xpawlu00sema6", O_CREAT | O_EXCL, 0644, 0);
		sema.mainstreamSemaphore = sem_open("/xpawlu00sema7", O_CREAT | O_EXCL, 0644, 0);		
		sema.mainstreamSemaphoreX = sem_open("/xpawlu00sema9", O_CREAT | O_EXCL, 0644, 0);
		sema.waitFinished = sem_open("/xpawlu00sema10", O_CREAT | O_EXCL, 0644, 0);
		sema.waitDecrement = sem_open("/xpawlu00sema11", O_CREAT | O_EXCL, 0644, 0);
		sema.waitReady = sem_open("/xpawlu00sema12", O_CREAT | O_EXCL, 0644, 1);

		pid_t f;
		pid_t g;
		//Podobny popis jako u funkce hackerMake();
		f = fork();
		if (f == 0)
		{
			hackerMake(p, hackertime, info, &sema, shm_fd);
		}
		else if (f > 0)
		{	
			g = fork();
			if (g == 0)
			{
				serfMake(p, serftime, info, &sema, shm_fd);
			}
			else if (g > 0)
			{
				wait(NULL);
				exit(0);
			}
			else if (g < 0)
			{
				unlinkSemShm(info, &sema, shm_fd);
				fprintf(stderr,"Unsuccessful fork\n");
				exit(2);
			}		
			
			wait(NULL);
			exit(0);			
		}	
		else if (f < 0)
		{
			unlinkSemShm(info, &sema, shm_fd);
			fprintf(stderr,"Unsuccessful fork\n");
			exit(2);
		}
		//Dealokace semaforu, uvolenni sdilene pameti, uzavreni souboru		
		unlinkSemShm(info, &sema, shm_fd);		
		return 0;	
	}	
}
