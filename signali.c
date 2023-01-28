#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <math.h>
/* funkcije za obradu signala, navedene ispod main-a */
void obradi_dogadjaj(int sig);
void obradi_sigterm(int sig);
void obradi_sigint(int sig);

/* globalna varijabla */
int broj;



/* program */
int main()
{
	struct sigaction act;
 	// maskiraj signale 
 	/* 1. maskiranje signala SIGUSR1 */
 
 	act.sa_handler = obradi_dogadjaj; /* kojom se funkcijom signal obrađuje */
 	sigemptyset(&act.sa_mask);
 	sigaddset(&act.sa_mask, SIGTERM); /* blokirati i SIGTERM za vrijeme obrade */
 	act.sa_flags = 0; /* naprednije mogućnosti preskočene */
 	sigaction(SIGUSR1, &act, NULL); /* maskiranje signala preko sučelja OS-a */
 	
 	/* 2. maskiranje signala SIGTERM */
 	act.sa_handler = obradi_sigterm;
 	sigemptyset(&act.sa_mask);
 	sigaction(SIGTERM, &act, NULL);
 	/* 3. maskiranje signala SIGINT */
 	act.sa_handler = obradi_sigint;
 	sigaction(SIGINT, &act, NULL);
 	
 	printf("Program s PID=%ld krenuo s radom\n", (long) getpid());
 	/* neki posao koji program radi; ovdje samo simulacija */

 	// pročitaj broj iz status.txt
 	FILE *fptr;
 	fptr = fopen("status.txt", "r");
 	fscanf(fptr,"%d", &broj);
 	fclose(fptr);
 	int result = 0;
 	// ako je broj == 0 onda čitaj brojeve iz obrada.txt dok ne dođeš do kraja
 	if(broj == 0) {

 		// otvaranje obrada.txt
		fptr = fopen("obrada.txt" , "r+");
		// čitanje obrada.txt dok ne dođem do kraja 
		while (( result = fscanf ( fptr, "%d", &broj)) != EOF){

 		}
 		fclose(fptr);
 		broj = sqrt(broj);
 		// zatvaram obrada.txt jer sam došao do kraja
 	}
 	// otvaram status text i prepisujem što je bilo (ne razumijem što trebam prepisati jer i gdje ali dobro)
 	fptr = fopen("status.txt", "r");
 	int nula = 0;
 	int pamti;
 	fscanf(fptr, "%d" , &pamti);
 	fclose(fptr);
 	fopen("status.txt", "w+");
 	fprintf(fptr,"%d\n", nula);
 	fclose(fptr);
 	printf("Pamti %d \n", pamti);
 	while(1) {
 		int x;
 		broj = broj + 1;
 		x = broj * broj;
 		fptr = fopen("obrada.txt", "a+");
 		fprintf(fptr, "%d\n", x);
 		fclose(fptr);
 		sleep(5);
 	}
 	return 0;
}

void obradi_dogadjaj(int sig)
{
	printf("%d\n", broj);
 	sleep(3);
}

void obradi_sigterm(int sig)
{
	FILE *fptr;
 	fptr = fopen("status.txt", "w");
 	printf("Primio signal SIGTERM, pospremam prije izlaska iz programa\n");
 	fprintf(fptr,"%d",broj);
 	fclose(fptr);
 	exit(1);
}

	void obradi_sigint(int sig)
{
 	printf("Primio signal SIGINT, prekidam rad\n");
 	exit(1);
}