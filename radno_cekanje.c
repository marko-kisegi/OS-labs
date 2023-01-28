/*
Dretve i procesi
Zadatak: Sinkronizacija ulazne, radne i izlazne dretve radnim čekanjem

Napisati program koji stvara ukupno 
tri dretve u dva procesa 
(jedan proces ima dvije, a 
rugi jednu dretvu). 
Prva dretva je ulazna dretva i ona svakih jednu do pet sekundi generira
nasumičan broj između 1 i 100 koji upisuje u zajedničku varijablu koju dijeli s 
radnom dretvom. 
Radna dretva čita taj broj, povećava ga za jedan i rezultat upisuje u drugu 
zajedničku varijablu s izlaznom dretvom. 

Izlazna dretva pročita taj broj i zapisuje ga u
datoteku. 

Broj nasumičnih brojeva koje generira ulazna dretva zadaje se kao argument
prilikom poziva glavnog programa. 

Sinkronizaciju obaviti radnim čekanjem na način da 
radna dretva čeka kada je upisana vrijednost 0 u zajedničkoj varijabli za ulaznu i radnu 
dretvu, ulazna dretva radno čeka kada je taj broj različit od 0. Na jednak način
sinkronizirati radnu i izlaznu dretvu drugom zajedničkom varijablom.

b) Studenti čija je zadnja znamenka JMBAG-a neparna trebaju napisati program u kojem jedan 
od dva procesa ima jednu dretvu i to ulaznu, a drugi proces dvije dretve: radnu i izlaznu.

Uputa: Dretve istog procesa komuniciraju i sinkroniziraju se preko zajedničke globalne 
varijable unutar tog procesa, dok dretve različitih procesa komuniciraju preko zajedničke 
memorije koju treba stvoriti na početku izvođenja programa.
Sve što nije zadano u tekstu zadatka riješiti na proizvoljan način.
Primjer ispisa prilikom pokretanja programa:

$ ./a.out 2
Pokrenuta RADNA DRETVA
Pokrenut IZLAZNI PROCES
Pokrenuta ULAZNA DRETVA

ULAZNA DRETVA: broj 57
RADNA DRETVA: pročitan broj 57 i povećan na 58
IZLAZNI PROCES: broj upisan u datoteku 58

ULAZNA DRETVA: broj 80
RADNA DRETVA: pročitan broj 80 i povećan na 81
IZLAZNI PROCES: broj upisan u datoteku 81

Završila RADNA DRETVA
Završila ULAZNA DRETVA
Završio IZLAZNI PROCES

Ispis izlazne datoteke:

$ cat ispis.txt
58
81
**/

#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<pthread.h>

int Id; /*identifikacijski broj segmenta */
int *ZajednickaVarijablaUlaznaRadna = 0;
int ZajednickaVarijablaRadnaIzlazna = 0;

void brisi(int sig){
	/* oslobadanje zajednicke memorije */
	(void) shmdt((char *) ZajednickaVarijablaUlaznaRadna);
	(void) shmctl(Id, IPC_RMID, NULL);
	exit(0);
}


/* ulazna dretva i ona svakih jednu do pet sekundi generira
nasumičan broj između 1 i 100 koji upisuje u zajedničku varijablu koju dijeli s 
radnom dretvom. 
*/

/*Sinkronizaciju obaviti radnim čekanjem na način 
da radna dretva čeka kada je upisana vrijednost 
0 u zajedničkoj varijabli za ulaznu i radnu dretvu, 
ulazna dretva radno čeka kada je taj broj različit 
od 0. Na jednak način sinkronizirati radnu
 i izlaznu dretvu drugom zajedničkom varijablom. */

void *ULAZNA_DRETVA(void *x){
	printf("Pokrenuta ULAZNA DRETVA\n");
	int i = x;
	srand(time(NULL));
	for(int j = i; j > 0 ; j--) {	
		int random;
		while( (random = rand()%100) == 0);
		*ZajednickaVarijablaUlaznaRadna = random;
		printf("Random broj je %d\n", *ZajednickaVarijablaUlaznaRadna);
		while(*ZajednickaVarijablaUlaznaRadna != 0);
	} 
	*ZajednickaVarijablaUlaznaRadna = -1;
	exit(1);
}

/*
Radna dretva čita taj broj, povećava ga za jedan i rezultat upisuje u drugu 
zajedničku varijablu s izlaznom dretvom. 

*/ 
void *RADNA_DRETVA(void *x){
	int i = 1;
	printf("Pokrenuta RADNA DRETVA\n");	
	while(ZajednickaVarijablaRadnaIzlazna==0){
		/* čitanje iz zajedničke varijable koja se dijeli među procesima
		*/

		while(*ZajednickaVarijablaUlaznaRadna == 0);

		i = *ZajednickaVarijablaUlaznaRadna;

		/* povećanje vrijednosti varijable za 1 
		*/
		if(i == 0) break;

		i = i + 1 ;
		/* spremanje na zajedničku varijablu koja se dijeli s drugom dretvom
		*/
		ZajednickaVarijablaRadnaIzlazna = i;
		while(ZajednickaVarijablaRadnaIzlazna != 0);
		if(ZajednickaVarijablaUlaznaRadna == -1 ) break;
		*ZajednickaVarijablaUlaznaRadna = 0;
	};
	printf("Vani \n");
	ZajednickaVarijablaRadnaIzlazna = -1;
	exit(0);
}

/*
Izlazna dretva pročita taj broj i zapisuje ga u
datoteku.  ispis.txt
*/

void *IZLAZNA_DRETVA(void *x){
	printf("Pokrenuta IZLAZNA DRETVA\n");
	while(ZajednickaVarijablaRadnaIzlazna==0){
		while(ZajednickaVarijablaRadnaIzlazna==0);
		FILE *fptr;
		fptr = fopen("ispis.txt", "a+");
		fprintf(fptr, "%d\n", ZajednickaVarijablaRadnaIzlazna);
		printf("IZLAZNA DRETVA: broj %d upisan u datoteku \n", ZajednickaVarijablaRadnaIzlazna);
		fclose(fptr);
		ZajednickaVarijablaRadnaIzlazna = 0;
	}
	printf("Vani 3");
	exit(0);
}

int main(int argc, char **argv){
	pthread_t thr_id[3];
	/* zauzimanje zajedničke memorije */
	Id = shmget(IPC_PRIVATE, sizeof(int), 0600);
	if(Id == -1) {
		/* greška - nema zajedničke MEMORIJE 
		*/
		exit(0); 
	}
	printf("Ako je segmentation fault(core dumped) zaboravio si argument.\n");
	ZajednickaVarijablaUlaznaRadna = (int *) shmat(Id, NULL, 0);
	*ZajednickaVarijablaUlaznaRadna = 0;
	sigset(SIGINT, brisi);
	// pokretanje procesa 1. 
	int send = atoi(argv[1]);
	if(fork() == 0){
		// pokretanje dretve 1 ULAZNE
		if(pthread_create(&thr_id[0], NULL, ULAZNA_DRETVA, send) != 0) {
			printf("Greska pri stvaranju ULAZNE dretve! \n");
			exit(1);
		}
		pthread_join(thr_id[0] , NULL);
		exit(1);
	}
	// pokretanje procesa 2
		// pokretanje dretve 2 RADNE 
	if(pthread_create(&thr_id[1], NULL, RADNA_DRETVA, NULL) != 0) {
		printf("Greska pri stvaranju RADNE dretve! \n");
		exit(1);
	}
	// pokretanje dretve 3 IZLAZNE
	if(pthread_create(&thr_id[2], NULL, IZLAZNA_DRETVA, NULL) != 0){
		printf("Greska pri stvaranju IZLAZNE dretve ! \n");
		exit(1);
		
	}

	/*
	čekanje završetka dretvi 
	*/
	pthread_join(thr_id[1] , NULL);
	pthread_join(thr_id[2] , NULL);

	/*
	kretanje završetka procesa
	*/
	(void) wait(NULL);

	/* brisanje zajedničke memorije */
	brisi(0);
	
	return 0;
}
