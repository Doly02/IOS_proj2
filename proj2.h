/***********************
 *  IOS projekt 2
 *  nazev:      proj2.h
 *  vytvoril:   Tomas Dolak 
 *  
 * 
 ************************/

/*          KNIHOVNY                */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <threads.h>
#include <time.h>

#include <stdbool.h>
#include <errno.h>
#include <ctype.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/mman.h>

/*          DEFINICE MAKRA            */
#define FIXED_NUM_OF_ARGS 6
#define PROGRAM_FAILED -1

#define LETTERS 1 
#define FINANCE 2
#define PACKAGES 3

#define UREDNIK 'U'
#define ZAKAZNIK 'Z'

#define init(name,value)(sem_open(name, O_CREAT | O_WRONLY, 0666, value))


/*          DEFINICE ZPRAV            */
#define STARTED 21
#define SERVING_SERVICE_OF_TYPE_X 22
#define SERVICE_FINISHED 23
#define TAKING_BREAK 24
#define BREAK_FINISHED 25
#define GOING_HOME 26
#define CALLED_BY_OFFICE_WORKER 27
#define ENTERING_OFFICE_FOR_SERVICE_X 28
#define CLOSING 29



/*          DEFINOVANI PROMENNYCH     */

// promenna pro soubor 
FILE *file; 
// struktura argumentu 
typedef struct {
    int NZ;     // pocet zakazniku
    int NU;     // pocet uredniku
    int TZ;     // maximalni cas v (ms) po kterem zakaznik po svem vytvoreni ceka, nez vez vejde na postu 
    int TU;     // maximalni delka prestavka urednika v (ms)
    int F;      // maximalni cas v milisekundach, po kterem je uzavrena posta pro neve prichozi 
} args;

// struktura pro sdilenou pamet 
typedef struct {
    int line;
    int no_officers;
    int act_no_costumers; // aktualni pocet zakazniku
    int no_costumers;
    int cos_in_line_one; // pocet zakazniku ve fronte na dopisy
    int cos_in_line_two; // pocet zakazniku ve fronte na fin.sluzby
    int cos_in_line_three; // pocet zakazniku ve fronte na baliky
    bool open;
    key_t shmid;

} ipc_t;

//  struktura semaforu
typedef struct {
    sem_t *sem_que_packege; // fronta baliky
    sem_t *sem_que_letter;  // fronta dopisy
    sem_t *sem_que_finance; // fronta financni sluzby
    sem_t *costumer;        // rada zakazniku
    sem_t *officer;         // rada uredniku 
    sem_t *manager;         // ridici semafor vypisu 
    sem_t *mutex;           // hlavni proces
    sem_t *costumer_going_home;

} semaphores;


/*          DEKLARACE FUNKCI         */

// FUNKCE overi zda je argument, cele cislo
bool is_integer(const char *str);

// FUNKCE overi zda jsou argumenty platne cisla, overi zda je jich presny pocet a vlozi je do struktury typu args
int arguments(int argc, char **argv, args *arguments);

// FUNKCE vytvori potrebne semafory
int create_semaphores(semaphores *samaphor);

// FUNKCE znici semafory a uklidi svincik
void destroy_semaphores(semaphores *semaphor);


ipc_t  *initialize_shared_mem();

void destroy_shared_memory(ipc_t *ipc);



// FUNKCE nahlasi na std_out, odpovidajici hlasku
// potreba dodelat nejakou strukturu pro proces a sdilenou pamet 
void put_message(int id, char type, int action, ipc_t *ipc);

void put_message_extra(int ID, int action, ipc_t *ipc, int service);

void put_message_for_main_process(int action, ipc_t *ipc);

void officer_generator(args all_arguments, ipc_t *ipc);

void officer_function(args all_arguments, ipc_t *ipc);

void costumer_generator(args all_arguments, ipc_t *ipc);

void costumer_function(args all_arguments, ipc_t *ipc);

// funkce pro nahodny cas
int random_time(int time);

int choose_me_random_line(int max_time);