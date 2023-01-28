/*
Vrtuljak
Zadatak
Modelirati vrtuljak (ringišpil) s dva  procesa:
 procesima posjetitelj (koje predstavljaju posjetitelje koji žele na vožnju) 
 te procesom vrtuljak. Procesima posjetitelj 
 se ne smije dozvoliti ukrcati na vrtuljak kada više nema praznih mjesta 
 (kojih je ukupno N) te prije nego li svi prethodni posjetitelji siđu. 
 Vrtuljak se može pokrenuti tek kada je pun. 
 Za sinkronizaciju koristiti opće semafore i dodatne varijable.

Dretva posjetitelj() {
    ...
    sjedi;
    ...
    ustani; // ili sidji
    ...
}
Dretva vrtuljak() {
   dok je(1) {
      ...
      pokreni vrtuljak;
      zaustavi vrtuljak;
      ...
   }
}
*/

#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<signal.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<semaphore.h>

#define BR_MJESTA 3
int ID1;
int ID2;
int ID3;
int ID4;
/* globalne varijable, kazaljke na objekte u zajedničkoj
memoriji */

sem_t *vrtuljak;
sem_t *sjeli;
sem_t *smije_izaci;
sem_t *izasli;

void vrtuljakp(){
    /* kreće vrtuljak */
    int j = 1;
    while(1){
        /* broj slobodnih mjesta vrtuljka */
        printf("Krug broj %d\n", j++);
        for(int i = 0 ; i < BR_MJESTA; i++){
            sem_post(vrtuljak);        
        }
        /* čekamo da svi posjetitelji zauzmu mjesta */
        for(int i = 0 ; i < BR_MJESTA; i++){
            sem_wait(sjeli);
        }
        /* vrtimo se */
        printf("Krenuo vrtuljak.\n");
        sleep(2);
        printf("Vrtuljak stao.\n");
        /* dajemo do znanja posjetitelji smiju van */
        for(int i = 0; i < BR_MJESTA; i++){
            sem_post(smije_izaci);
        }
        /* cekamo da su svi izasli */
        for(int i = 1 ; i<  BR_MJESTA; i++){
            sem_wait(izasli);
        }
        printf("Posjetitelji izasli.\n");
    }
}
void posjetiteljp(){
    while(1){
        /* cekamo vrtuljak da nam dodjeli slobodno mj */

        sem_wait(vrtuljak);
        printf("Usao.\n");
        /* sjedamo na vrtuljak */
        sem_post(sjeli);
        // voznja traje 

        /*cekamo smijemo li izaci */
        sem_wait(smije_izaci);
        /* izlazimo iz vrtuljka  
        */
        printf("Izasao\n");
        sem_post(izasli);
    }
}

void brisi(int sig){
    /* oslobađanje zajedničke memorije */
    sem_destroy(vrtuljak);
    sem_destroy(izasli);
    sem_destroy(smije_izaci);
    sem_destroy(sjeli);
    /*
    Segment se može otpustiti sustavskim pozivom shmdt:
    int shmdt(char *addr) ;
    Zajednički spremnički prostor ostaje nedirnut 
    i može mu se opet pristupiti tako da se ponovno veže na adresni prostor procesa, 
    mada je moguće da pri tome dobije drugu adresu u njegovom adresnom prostoru. 
    addr je adresa segmenta dobivena pozivom shmat.


    */
   (void) shmdt((char *) vrtuljak);
   (void) shmdt((char *) smije_izaci);
   (void) shmdt((char *) sjeli);
   (void) shmdt((char *) izasli);

    /*
    Uništavanje segmenta zajedničke memorije izvodi se sustavskim pozivom shmctl:
    int shmctl(int segid, int cmd, struct shmid_ds *sbuf) ;
    Za uništavanje segmenta treba za segid staviti identifikacijski broj dobiven sa shmget, 
    cmd treba biti IPC_RMID, a sbuf može biti NULL. Greška je uništiti segment koji nije 
    otpušten iz adresnog prostora svih procesa koji su ga koristili. shmctl, 
    kao i shmdt vraća 0 ako je sve u redu, a -1 u slučaju greške. (Detaljnije o ovim pozivima u: man shmget, man shmop, man shmctl)
    */


   (void) shmctl(ID1, IPC_RMID, NULL);
   (void) shmctl(ID2, IPC_RMID, NULL);
   (void) shmctl(ID3, IPC_RMID, NULL);
   (void) shmctl(ID4, IPC_RMID, NULL);
   exit(0);
}

int main(void){
    /* zauzimanje zajednicke memorije
    typedef key_t int;
    int shmget(key_t key, int size, int flags) ;
    Ovaj sustavski poziv pretvara ključ (key) nekog 
    segmenta zajedničkog spremnika u njegov identifikacijski broj
    ili stvara novi segment. Novi segment duljine barem size bajtova
     će biti stvoren ako se kao ključ upotrijebi IPC_PRIVATE. 
     U devet najnižih bitova flags 
     se stavljaju dozvole pristupa (na primjer, 
     oktalni broj 0600 znači da korisnik može čitati i pisati,
      a grupa i ostali ne mogu). 
      shmget vraća identifikacijski broj segmenta koji je potreban u shmat ili -1 u slučaju greške.


     */
    ID1 = shmget(IPC_PRIVATE, sizeof(sem_t), 0600);
    if(ID1 == -1) {
        printf("...\n");
        exit(1); /* nema zajednicke memorije */
    }
    ID2 = shmget(IPC_PRIVATE, sizeof(sem_t), 0600);
    if(ID2 == -1) {
        printf("...\n");
        exit(1); /* nema zajednicke memorije */
    }
    ID3 = shmget(IPC_PRIVATE, sizeof(sem_t), 0600);
    if(ID3 == -1) {
        printf("...\n");
        exit(1); /* nema zajednicke memorije */
    }
    ID4 = shmget(IPC_PRIVATE, sizeof(sem_t), 0600);
    if(ID4 == -1) {
        printf("...\n");
        exit(1); /* nema zajednicke memorije */
    }
    sigset(SIGINT, brisi); /* u slucaju prekida obriši mem */
    // sem_t *sem = (sem_t*)shmat(ID, NULL, 0);

    /*
    shmctl(ID1, IPC_RMID, NULL); 
    shmctl(ID2, IPC_RMID, NULL); 
    shmctl(ID3, IPC_RMID, NULL); 
    shmctl(ID4, IPC_RMID, NULL); 

    */ 
    
    /*
    shmctl moze odmah tu ili na kraju nakon shmdt
    jer IPC_RMID oznacava da segment treba izbrisati
    nakon sto se zadnji proces odijeli od tog segmenta
    (detach)
    
    */
    /* 
    postaljavamo pokazivace na nase semafore koji
    imaju drugacija imena za lakse koristenje
    */ 
    vrtuljak = (sem_t*)shmat(ID1, NULL, 0);    
    sjeli = (sem_t*)shmat(ID2, NULL, 0);    
    smije_izaci = (sem_t*)shmat(ID3, NULL, 0);    
    izasli = (sem_t*)shmat(ID4, NULL, 0);    
    
    sem_init ( vrtuljak, 1 , 0);
    sem_init ( sjeli, 1 , 0);
    sem_init ( smije_izaci, 1 , 0);
    sem_init ( izasli, 1 , 0);

    /* pokretanje paralelenih procesa 1
    vrtuljak */
    if (fork() == 0) {
        printf("Proces vrtuljak\n");
        vrtuljakp();
        exit(0);
    }
    /* posjetitelji */
    int broj_posjetitelja = BR_MJESTA * 2;
    for(int i = 0 ; i < broj_posjetitelja ; i++ ) {
        switch (fork()) { // treba li jedan for > BR_MJESTA
        
        case 0: 
            printf("Posjetitelj %d\n", i + 1);
            posjetiteljp();
            exit(0);
        case -1:
            printf("Nemoguće stvoriti proces.\n");
        }
    }

    /* cekaj kraj pokrenutih procesa */

    for(int i = 0 ; i < broj_posjetitelja+1;i++){
        (void) wait(NULL);
    }

    brisi(0);
    return 0;
}