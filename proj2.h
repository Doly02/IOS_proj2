/***********************
 *  IOS projekt 2
 *  nazev:      proj2.h
 *  vytvoril:   Tomas Dolak 
 *  
 * 
 ************************/

/**
 * @author Tomas Dolak
 * @file proj2.h
 * 
 * @brief hlavickovy soubor pro projekt proj2 do predmety IOS na synchronizaci procesu za pomoci semaforu,
 * 
*/

/*          KNIHOVNY                */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
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

/**
 * @brief Počet argumentů, které musí být zadány pro tento program.
 */
#define FIXED_NUM_OF_ARGS 6
/**
 * @brief Pokud program selze
 */
#define PROGRAM_FAILED -1
#define EXIT_CODE 1

/**
 * @brief dopisy
 */
#define LETTERS 1
/**
 * @brief financni sluzby
 */ 
#define FINANCE 2
/**
 * @brief balikove sluzby
 */
#define PACKAGES 3

#define UREDNIK 'U'
#define ZAKAZNIK 'Z'

/**
 * @brief makro pro otevreni semaforu
 */
#define init(name,value)(sem_open(name, O_CREAT | O_WRONLY, 0666, value))


/*          DEFINICE ZPRAV            */
/**
 * @brief definice zprav
 */
#define STARTED 21
#define SERVING_SERVICE_OF_TYPE_X 22
#define SERVICE_FINISHED 23
#define TAKING_BREAK 24
#define BREAK_FINISHED 25
#define GOING_HOME 26
#define CALLED_BY_OFFICE_WORKER 27
#define ENTERING_OFFICE_FOR_SERVICE_X 28
#define CLOSING 29

/**
 * @defgroup DEFINICE_STRUKTUR
 * @{
 */

/**
 * @brief Struktura argumentu.
 */
typedef struct {
    int NZ;     ///< pocet zakazniku
    int NU;     ///< pocet uredniku
    int TZ;     ///< maximalni cas v (ms) po kterem zakaznik po svem vytvoreni ceka, nez vez vejde na postu 
    int TU;     ///< maximalni delka prestavka urednika v (ms)
    int F;      ///< maximalni cas v milisekundach, po kterem je uzavrena posta pro neve prichozi 
} args;

/**
 * @brief Struktura pro sdilenou pamet.
 */
typedef struct {
    int line;               ///< radek souboru na ktery se bude zapisovat vypis 
    int no_officers;        ///< pocet uredniku kterych ma byt vytvoreno
    int act_no_costumers;   ///< aktualni pocet zakazniku
    int no_costumers;       ///< pocet zakazniku ktery ma byt vytvoren 
    int cos_in_line_one;    ///< pocet zakazniku ve fronte na dopisy
    int cos_in_line_two;    ///< pocet zakazniku ve fronte na fin.sluzby
    int cos_in_line_three;  ///< pocet zakazniku ve fronte na baliky
    bool open;              ///< informace o tom zda je posta otevrena(true)/zavrena(false)
    key_t shmid;            ///< identifikacni cislo sdilene pameti 

} ipc_t;

/**
 * @brief Struktura pro uchování semaforů potřebných pro synchronizaci procesů.
 */
typedef struct {
    sem_t *sem_que_packege;     ///< Semafor pro frontu balíků
    sem_t *sem_que_letter;      ///< Semafor pro frontu dopisů
    sem_t *sem_que_finance;     ///< Semafor pro frontu finančních služeb
    sem_t *costumer;            ///< Semafor pro řadu zákazníků
    sem_t *officer_one;         ///< Semafor ktery posti zakaziky k urednikum ve fronte LETTERS
    sem_t *officer_two;         ///< Semafor ktery posti zakaziky k urednikum ve fronte FINANCE
    sem_t *officer_three;       ///< Semafor ktery posti zakaziky k urednikum ve fronte PACKAGES
    sem_t *manager;             ///< Řídící semafor pro výpis
    sem_t *mutex;               ///< Semafor pro hlavní proces
    sem_t *costumer_going_home; ///< Semafor pro zákazníky, kteří odcházejí domů
    sem_t *front_one;           ///< Semafor zarizujici kritickou sekci pro prvni frontu (aby dva urednici nesli obsluhovat stejneho zakaznika)
    sem_t *front_two;           ///< Semafor zarizujici kritickou sekci pro druhou frontu (aby dva urednici nesli obsluhovat stejneho zakaznika)
    sem_t *front_three;         ///< Semafor zarizujici kritickou sekci pro treti frontu (aby dva urednici nesli obsluhovat stejneho zakaznika)

} semaphores;

/**
 * @defgroup DEKLARACE_FUNKCI
 * @{
 */

/**
 * @brief funkce overi zda je argument, cele cislo 
 * @param str ukazatel na cislo, ktere se bude proverovat
 * @return v pripade ze je cislo cele cislo vraci true, jinak false 
*/
bool is_integer(const char *str);

/**
 * @brief funkce overi zda jsou argumenty platne cisla, overi zda je jich presny pocet tzn.5 a vlozi jen struktury typu args
 * @param argc pocet argumentu programu
 * @param argv ukazatel na ukazatele argumentu
 * @param arguments ukazatel na strukturu do, ktere budou vlozeny argumenty programu
 * @return v pripada ze nenastal zadny problem s argumenty vraci 0, jinak -1
 * 
*/
int arguments(int argc, char **argv, args *arguments);

/**
 * @brief funkce vytvori sdilene semafory
 * @param semaphor ukazatel na strukturu semaforu, ktere maji byt vytvoreny
 * @return v pripade uspesneho vytvoreni vraci 0, jinak -1
*/
int create_semaphores(semaphores *semaphor);

/**
 * @brief funkce znici vytvorene semafory programu
 * @param semaphor ukazatel na strukturu semaforu, ktere se maji vyt zniceny a odlinkovany
*/
void destroy_semaphores(semaphores *semaphor);

/**
 * @brief funkce vytvori sdilenou pamet pro program
 * @return ukazatel na sdilenou pamet
*/
ipc_t  *initialize_shared_mem();

/**
 * @brief funkce znici sdilenou pamet programu
 * @param ipc pamet, ktera ma byt znicena 
*/
void destroy_shared_memory(ipc_t *ipc);

/**
 * @brief funkce, posila zpravy z programu do  proj2.out, jedna se o vsechny zpravy, ktere se netykaji prime interakce zakaznika s urednikem a zprav pro hlavni proces
 * 
 * @param id indentifikacni cislo zakaznika nebo urednika
 * @param type zda je podproces urednik nebo zakaznik
 * @param action ukon, ktery se vypise do  proj2.out
 * @param ipc ukazatel na sdilenou pamet
*/ 
void put_message(int id, char type, int action, ipc_t *ipc);

/**
 * @brief funkce pro generovani zprav s vice udaji jedna se o (zpravu SERVING...,ENTERING OFFICE...)
 * @param ID identifikacni cislo zakaznika nebo urednika
 * @param action akce, ktera se bude vypisovat do file proj2.out
 * @param ipc ukazatel na sdilenou pamet
 * @param service fronta ve ktere interakce zakaznika a urednika probiha
*/
void put_long_message(int ID, int action, ipc_t *ipc, int service);

/**
 * @brief funkce pro generovani zprav hlavniho procesu 
 * @param action akce, ktera se bude vypisovat
 * @param ipc sdilena pamet 
*/
void put_message_for_main_process(int action, ipc_t *ipc);

/**
* @brief funkce pomoci fork() vytvori uredniky a dale u nich spousti funkci officer_function @see officer_function
* @param all_arguments argumenty se kterymi se pracuje 
* @param ipc ukazatel na sdilenou pamet 
*/
void officer_generator(args all_arguments, ipc_t *ipc);

/**
* @brief funkce predstavujici chovani urednika 
* @param all_arguments argumenty se kterymi se pracuje 
* @param ipc ukazatel na sdilenou pamet 
*/
void officer_function(args all_arguments, ipc_t *ipc);

/**
* @brief funkce pomoci fork() vytvori zakazniky a dale u nich spousti funkci costumer_function @see costumer_function 
* @param all_arguments argumenty se kterymi se pracuje 
* @param ipc ukazatel na sdilenou pamet 
*/
void costumer_generator(args all_arguments, ipc_t *ipc);

/**
* @brief funkce predstavujici chovani zakaznika
* @param all_arguments argumenty se kterymi se pracuje 
* @param ipc ukazatel na sdilenou pamet 
*/
void costumer_function(args all_arguments, ipc_t *ipc);

/**
* @brief funkce generujici nahodny cas
* @param time maximalni cas 
* @return nahodny cas v intervalu <0,time>
*/
int random_time(int time);

/**
* @brief funkce vybirajici nahodnou frontu, pro urednika do ktere se podiva zda ma jit obsluhovat, pro zakaznika, do ktere se zaradi
* @param line pocet front, ze kterych je mozne vybrat
* @return nahodna fronta v intervalu <1,line>
*/
int choose_me_random_line(int max_time);