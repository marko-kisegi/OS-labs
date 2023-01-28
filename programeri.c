/*MS & Linux programeri
Zadatak
U istoj zgradi rade Microsoftovi programeri i Linux programeri.
Zgrada ima jedan restoran kojega programeri moraju dijeliti.
 U istom trenutku u restoranu smije biti samo jedna vrsta programera
  (ili je restoran prazan). Sinkronizirati programere monitorom
  (ispravna sinkronizacija podrazumijeva i sprječavanje izgladnjivanja.

Svaki programer ima sljedeći oblik:

Programer(vrsta){
     udji(vrsta);
     obavi;
     izadji(vrsta);
}

*/

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

// tri varijable koje prate broj ljudi u restoranu
// broj ljudi koji cekaju iz svakog tima
// te broj ljudi koji su siti iz oba tima
int br[2] = {0, 0};
int ceka[2] = {0, 0};
int siti[2] = {0, 0};

// definiramo MS kao 0 i Linux kao 1
#define MS 0
#define Linux 1
// prag nakon kojeg dajemo idućem timu priliku da uđu u restoran
#define PRAG 3

// deklaracija monitora te reda čekanja

pthread_mutex_t m;
pthread_cond_t uv[2];

void udji(int vrsta)
{
    pthread_mutex_lock(&m);
    ceka[vrsta]++;
    while (br[1 - vrsta] > 0 || (siti[vrsta] >= PRAG && ceka[1 - vrsta] > 0))
    {
        pthread_cond_wait(&uv[vrsta], &m);
    }
    br[vrsta]++;
    ceka[vrsta]--;
    siti[1 - vrsta] = 0;
    siti[vrsta]++;
    pthread_mutex_unlock(&m);
}

void izadji(int vrsta)
{
    pthread_mutex_lock(&m);
    br[vrsta]--;
    if (br[vrsta] == 0)
    {
        pthread_cond_broadcast(&uv[1 - vrsta]);
    }
    pthread_mutex_unlock(&m);
}

void *PROGRAMER(int vrsta)
{
    udji(vrsta);
    printf("U restoranu su programeri %s-a\n", vrsta ? "Linux" : "Microsoft");
    sleep(2);
    izadji(vrsta);
}

int main(void)
{

    /* inicijalizacija monitora
     */
    pthread_mutex_init(&m, NULL);
    pthread_cond_init(&uv[0], NULL);
    pthread_cond_init(&uv[1], NULL);
    int broj_programera = PRAG * 4;

    pthread_t thr_id[broj_programera + broj_programera];
    /* pokretanje programera
     */
    srand( time(NULL));
    for(int i = 0; i < broj_programera; i++){
        if(pthread_create(&thr_id[i] , NULL, PROGRAMER, rand()%2)!=0){
            exit(0);
        }
        if( pthread_create(&thr_id[i+broj_programera] , NULL, PROGRAMER, rand()%2)!=0){
            exit(0);
        }
    }



    /*
    cekanje da svi umru;
    */

    for (int i = 0; i < broj_programera ; i++){
        pthread_join(thr_id[i], NULL);
    }
    


    


    return 0;
}
