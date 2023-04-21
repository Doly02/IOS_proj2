/***********************
 *  IOS projekt 2
 *  nazev:      proj2.c
 *  vytvoril:   Tomas Dolak 
 *  
 *  projekt na synchronizaci procesu
 * 
 ************************/
#include "proj2.h"
semaphores sems;

/*          ZPRACOVANI ARGUMENTU     */

// funkce ktera zkontroluje zda je argument vubec cele cislo 
bool is_integer(const char *str){
    while (*str) {
        if (!isdigit((unsigned char)*str)) {
            return false;
        }
        str++;
    }

    return true;
}


int arguments(int argc, char **argv, args *all_arguments)
{
    // kontrola poctu argumentu 
    if (argc != FIXED_NUM_OF_ARGS) //zde se kouknout na pocitani argumentu 
    {
        printf("Not right num of arguments\n");
        return -1;
    }

    // kontrola zda jsou argumenty, cela cisla
    for (int i = 1; i < FIXED_NUM_OF_ARGS; i++)
    {
        if(!is_integer(argv[i]))
            return -1;
    }

    // kontrola hodnot argumentu a jejich ulozeni do struktury
    for (int i = 1; i < FIXED_NUM_OF_ARGS; i++)
    {
        int arg_value = atoi(argv[i]);

        if (i == 1){
            if (arg_value < 1)
                return -1;
            all_arguments->NZ = arg_value;
        } 

        if (i == 2){
            if (arg_value < 1)
                return -1;
            all_arguments->NU = arg_value;
        }

        if (i == 3){
            if (arg_value < 0 || arg_value > 10000)
                return -1;
            all_arguments->TZ = arg_value;
        }
        
        if (i == 4){
            if (arg_value < 0 || arg_value > 100)
                return -1;
             all_arguments->TU = arg_value;
        }
        if (i == 5){
            if (arg_value < 0 || arg_value > 10000)
                return -1;
            all_arguments->F = arg_value;
        }

    }

    return 0;
}

/*          FUNKCE SEMAFORU          */
int create_semaphores(semaphores *semaphors)
{    
    semaphors->sem_que_finance = init("semaphore_que_finance", 1);
    if (semaphors->sem_que_finance == SEM_FAILED)
        return -1;

    semaphors->sem_que_letter = init("semaphore_que_letter",1);
    if(semaphors->sem_que_letter == SEM_FAILED)
        return -1;

    semaphors->sem_que_packege = init("semaphore_que_package",1);
    if(semaphors->sem_que_packege == SEM_FAILED)
        return -1;

    semaphors->costumer = init("costumer", 0);
    if(semaphors->costumer == SEM_FAILED)
        return -1;

    semaphors->officer = init("officer", 1);
    if(semaphors->officer == SEM_FAILED)
        return -1;

    semaphors->manager = init("manager_of_processes",1);
    if(semaphors->manager == SEM_FAILED)
        return -1;

    semaphors->mutex = init("mutex_xdolak09",1);
    if(semaphors->manager == SEM_FAILED)
        return -1;

    return 0;
}

void destroy_semaphores(semaphores *semaphors)
{

    sem_destroy(semaphors->sem_que_finance);
    sem_unlink("semaphore_que_finance");
    sem_destroy(semaphors->sem_que_letter);
    sem_unlink("semaphore_que_letter");
    sem_destroy(semaphors->sem_que_packege);
    sem_unlink("semaphore_que_package");
    sem_destroy(semaphors->manager);
    sem_unlink("manager_of_processes");
    sem_destroy(semaphors->costumer);
    sem_unlink("costumer");
    sem_destroy(semaphors->officer);
    sem_unlink("officer");
    sem_destroy(semaphors->mutex);
    sem_unlink("mutex_xdolak09");

}

void put_message(int ID,char type, int action, ipc_t *ipc)
{
    ++ipc->line;

    /*
    VZOR:
    if (action == STARTED)
        fprintf(file, "%u: %c %u: started\n", ipc->line, proces->typ.type, proces->process_id);    
    
    */

    // spolecne zpravy
    if (action == STARTED)
    {
        fprintf(stdout,"%u: %c %u: started\n", ipc->line, type,ID);
        fflush(stdout);
    }
    if (action == GOING_HOME)
    {
        fprintf(stdout, "%u: %c %u: going home\n",ipc->line, type, ID);
        fflush(stdout);
    }
    if (action == SERVICE_FINISHED)
    {
        fprintf(stdout,"%u: %c %u: finished\n",ipc->line, type,ID);
        fflush(stdout);
    }
    if (action == TAKING_BREAK)
    {
        fprintf(stdout,"%u: %c %u: taking break\n",ipc->line, type, ID);
        fflush(stdout);
    }
    if (action == BREAK_FINISHED)
    {
        fprintf(stdout,"%u: %c %u: break finished\n",ipc->line, type, ID);
        fflush(stdout);
    }
    // zpravy konkretne pro zakaznika

    if (action == CALLED_BY_OFFICE_WORKER)
    {
        fprintf(stdout,"%u: %c %u: called by office worker\n",ipc->line, type, ID);
        fflush(stdout);
    }
}

void put_message_for_main_process(int action, ipc_t *ipc)
{
    ++ipc->line;
    if (action == CLOSING)
    {
        fprintf(stdout,"%u: closing\n",ipc->line);
        fflush(stdout);
    }
}

void put_message_extra(int ID, int action, ipc_t *ipc, int service)
{
    ++ipc->line;
    // zpravy konkretne pro urednika
    if (action == SERVING_SERVICE_OF_TYPE_X)
    {
        fprintf(stdout,"%u: U %u: serving a service of type %u\n", ipc->line, ID, service);
        fflush(stdout);
    }
    if (action == ENTERING_OFFICE_FOR_SERVICE_X)
    {
        fprintf(stdout,"%u: Z %u: entering office for a service %u\n", ipc->line, ID, service);
        fflush(stdout);
    }
}



/*          FUNKCE PRO SDILENOU PAMET           */
ipc_t  *initialize_shared_mem()
{
    // generovani klice pro nahodnou pamet, jeste neexistuje FILE_KEY!
    key_t key = ftok("proj2.c",'c');

    // vytvoreni segmentu sdilene pameti
    int shmem_key = shmget(key, sizeof(ipc_t),0666 | IPC_CREAT);
    // pripojeni segmentu sdilene pameti k adresnimu prostoru procesu 
    ipc_t *ipc = shmat(shmem_key,NULL,0);

    ipc->shmid = shmem_key;

    return ipc;
}

void destroy_shared_memory(ipc_t *ipc)
{
    key_t key = ipc->shmid;

    shmctl(key, IPC_RMID, NULL);

    shmdt(ipc);
}

void officer_generator(args all_arguments, ipc_t *ipc)
{
    for(int i = 0; i < all_arguments.NU;i++)
    {
        pid_t off_id = fork();
        if (off_id == -1)
            fprintf(stdout, "Error w generating children\n");
        else if(off_id == 0){
            officer_function(all_arguments,ipc);
            exit(0);
        }
        
    }
    exit(0);
}

void officer_function(args all_arguments, ipc_t *ipc)
{

    int officers_id;
    //uzamkonout semafor pro vypis
    sem_wait(sems.manager);
    ++(ipc->no_officers);
    officers_id = ipc->no_officers;
    put_message(officers_id,s_U,STARTED,ipc);
    sem_post(sems.manager);

    // pokud mame nejake zakazniky, ktere je potreba obsouzit 
    while (ipc->act_no_costumers >= 0)
    {
        // jelikoz muze pracovat vice uredniku, musi vsak pracovat alespon jeden 
            int Ran_time = random_time(10);
            //vyber nahodne fronty, kterou bude urednik obsluhovat
            int rand_choise = random_time_wout_zero(3);
            if (ipc->act_no_costumers > 0)
            {
                //printf("log c.1\n");
                if(rand_choise == 1)
                {
                    //printf("log c.2\n");
                    sem_wait(sems.sem_que_letter);
                    if(ipc->cos_in_line_one > 0)
                    {
                        //printf("log c.3\n");
                        //ubereme zakaznika z fronty
                        --(ipc->cos_in_line_one);
                        // otevreni semaforu pro zakazniky
                        sem_post(sems.costumer);

                        sem_wait(sems.manager);
                        put_message_extra(officers_id,SERVING_SERVICE_OF_TYPE_X,ipc, LETTERS);
                        sem_post(sems.manager);

                        usleep(Ran_time);

                        sem_wait(sems.manager);
                        put_message(officers_id,s_U,SERVICE_FINISHED,ipc);
                        sem_post(sems.manager);
                        --(ipc->act_no_costumers); // zakaznik odchazi spokojeny dom
                        sem_wait(sems.costumer);

                    }
                    sem_post(sems.sem_que_letter);
                }   
                else if(rand_choise == 2)
                {
                    sem_wait(sems.sem_que_finance);
                    //printf("log c.4\n");
                    if(ipc->cos_in_line_two > 0)
                    {
                        //snizime pocet cekajicich zakazniku z fronty na dopisy
                        --(ipc->cos_in_line_two);
                        // otevreni semaforu pro zakazniky
                        sem_post(sems.costumer);
                        //printf("log c.5\n");
                        sem_wait(sems.manager);
                        put_message_extra(officers_id,SERVING_SERVICE_OF_TYPE_X,ipc, FINANCE);
                        sem_post(sems.manager);

                        usleep(Ran_time);

                        sem_wait(sems.manager);
                        put_message(officers_id,s_U,SERVICE_FINISHED,ipc);
                        sem_post(sems.manager);
                        --(ipc->act_no_costumers); // zakaznik odchazi spokojeny dom

                        sem_wait(sems.costumer);
                    }
                    sem_post(sems.sem_que_finance);
                }   
                else if(rand_choise == 3)
                {
                    sem_wait(sems.sem_que_packege);
                    //printf("log c.6\n");
                    if(ipc->cos_in_line_three > 0)
                    {
                        //printf("log c.7\n");
                        // ubereme zakaznika z fronty na balicky
                        --(ipc->cos_in_line_one);

                        // otevreni semaforu pro zakazniky
                        sem_post(sems.costumer);

                        sem_wait(sems.manager);
                        put_message_extra(officers_id,SERVING_SERVICE_OF_TYPE_X,ipc, PACKAGES);
                        sem_post(sems.manager);

                        usleep(Ran_time);

                        sem_wait(sems.manager);
                        put_message(officers_id,s_U,SERVICE_FINISHED,ipc);
                        sem_post(sems.manager);
                        --(ipc->act_no_costumers); // zakaznik odchazi spokojeny dom

                        sem_wait(sems.costumer);
                    }
                    sem_post(sems.sem_que_packege);
                }

            }  
            else if (ipc->act_no_costumers == 0)
            {
                int break_of_oficer = random_time(all_arguments.TU);
                sem_wait(sems.manager);
                put_message(officers_id,s_U,TAKING_BREAK,ipc);
                sem_post(sems.manager);
                usleep(break_of_oficer);
            } 
        
    }
}

void costumer_generator(args all_arguments, ipc_t *ipc)
{
    ipc->act_no_costumers = all_arguments.NZ;
    for(int i = 0; i < all_arguments.NZ; i++)
    {
        pid_t cos_id = fork();
        if (cos_id == -1)
            fprintf(stdout, "Error w generating children\n");
        else if(cos_id == 0){
            //zavolat funkci pro zakaznika
            costumer_function(all_arguments,ipc);
            exit(0);
        }
    }
    exit(0);
}


void costumer_function(args all_arguments, ipc_t *ipc)
{
    int costumer_id;
    // uzamknout semafor pro vypis
    sem_wait(sems.manager);
    ++(ipc->no_costumers);
    costumer_id = ipc->no_costumers;
    put_message(costumer_id,s_Z, STARTED, ipc);
    sem_post(sems.manager);

    int rnd_time = random_time(all_arguments.TZ);
    usleep(rnd_time);

    // jestli je posta uzavrena tak jde domu
    //sem_wait(sems.mutex);
    if(ipc->open == true){        
        // vyber random cinnosti
        int rnd_work = random_time_wout_zero(3);
        //vyber nahodneho casu 0-10 po kterem odejde domu
        int rand_time = random_time(10);

        if (rnd_work == 1)
        {
            sem_wait(sems.sem_que_letter);


            //pro vypis
            sem_wait(sems.manager);
            put_message_extra(costumer_id, ENTERING_OFFICE_FOR_SERVICE_X, ipc, LETTERS);
            ++(ipc->cos_in_line_one);
            sem_post(sems.manager);

            // zakaznik si zabral misto u urednika
            sem_wait(sems.costumer);

            // vypis ze zakaznik je zavolan
            sem_wait(sems.manager);
            put_message(costumer_id,s_Z,CALLED_BY_OFFICE_WORKER,ipc);
            sem_post(sems.manager);

            // uspani podprocesu
            usleep(rand_time);

            //pro vypis
            sem_wait(sems.manager);
            put_message(costumer_id,s_Z,GOING_HOME,ipc);
            sem_wait(sems.manager);

            // uvolneni prostoru z fronty na dopisy
            sem_post(sems.sem_que_letter);
        }
        if (rnd_work == 2)
        {
            sem_wait(sems.sem_que_finance);


            //pro vypis
            sem_wait(sems.manager);
            put_message_extra(costumer_id, ENTERING_OFFICE_FOR_SERVICE_X, ipc, FINANCE);
            ++(ipc->cos_in_line_two);
            sem_post(sems.manager);

            //zakaznik si zabira misto u urednika
            sem_wait(sems.costumer);

            // vypis ze zakaznik je zavolan k urednikovi
            sem_wait(sems.manager);
            put_message(costumer_id,s_Z,CALLED_BY_OFFICE_WORKER,ipc);
            sem_post(sems.manager);

            // uspani podprocesu
            usleep(rand_time);

            // obsazeni prostoru pro vypis
            sem_wait(sems.manager);
            put_message(costumer_id,s_Z,GOING_HOME,ipc);
            sem_post(sems.manager);

            // uvolneni prostoru z fronty na dopisy
            sem_post(sems.sem_que_letter);
        }
        if (rnd_work == 3)
        {
            sem_wait(sems.sem_que_packege);


            //pro vypis
            sem_wait(sems.manager);
            put_message_extra(costumer_id, ENTERING_OFFICE_FOR_SERVICE_X, ipc, PACKAGES);
            ++(ipc->cos_in_line_three);
            sem_post(sems.manager);

            // zakaznik jde k urednikovi 
            sem_wait(sems.costumer);

            // vypis ze zakaznik je zavolan k urednikovi
            sem_wait(sems.manager);
            put_message(costumer_id,s_Z,CALLED_BY_OFFICE_WORKER,ipc);
            sem_post(sems.manager);

            usleep(rand_time);
            sem_wait(sems.manager);
            put_message(costumer_id,s_Z,GOING_HOME,ipc);
            sem_post(sems.manager);
            // uvolneni prostoru z fronty na dopisy
            sem_post(sems.sem_que_packege);
        }


    }
    else {

        sem_wait(sems.manager);
        put_message(costumer_id,s_Z,GOING_HOME,ipc);
        sem_post(sems.manager);
        --(ipc->act_no_costumers); // zakaznik nebyl obslouzen, neni ve fronte 

    }
    
    //sem_post(sems.mutex);


}



// FUNKCE generujici nahodny cas od 0 az po time
int random_time(int max_time)
{
    static bool seed_initialized = false;
    if (!seed_initialized) {
        srand(time(NULL));
        seed_initialized = true;
    }
    int ran_time = rand() % (max_time + 1);
    return ran_time;
}
// pouzit timeval, get time

int random_time_wout_zero(int max_time) {
    static bool seed_initialized = false;
    if (!seed_initialized) {
        srand(time(NULL));
        seed_initialized = true;
    }
    int ran_time = rand() % max_time + 1; 
    return ran_time;
}



int main(int argc, char *argv[])
{

    // globalni promenne
    args all_args;
    process proces;
    int error = 0;
    ipc_t *shared_memory = NULL;
   
    //kontrola argumentu
    error = arguments(argc, argv, &all_args);
    if(error == -1)
        return -1;
    
    // vytvoreni sdilene pameti
    shared_memory = initialize_shared_mem();
    shared_memory->open = true;

    error = create_semaphores(&sems);
    if(error == -1)
    {   
        destroy_semaphores(&sems);
        return -1;
    }
    printf("NZ: %d,NU: %d,TZ: %d,TU: %d, F: %d\n",all_args.NZ,all_args.NU,all_args.TZ,all_args.TU,all_args.F);    

    //sem_post(sems.mutex);

    pid_t podprocesses = fork();
    if (podprocesses == 0)
    {

        pid_t childerns_of_podprocesses = fork();
        if(childerns_of_podprocesses == 0)
        {

            officer_generator(all_args,shared_memory);
            exit(0);
        }

        costumer_generator(all_args,shared_memory);
        exit(0);
    }


    // vytvoreni podprocesu pro uredniky
    int F = all_args.F;
    int F_half;
    F_half = F/2;
    int t;

    if (F != 0)
        t = rand() % (F - F_half + 1) + F_half;
    else
        t = F;
    usleep(t);

    // vypise ze zavira a ceka na konec podpocesu
    //sem_wait(sems.mutex);
    sem_wait(sems.manager);
    put_message_for_main_process(CLOSING, shared_memory);
    shared_memory->open = false;
    sem_post(sems.manager);
    //sem_wait(sems.mutex);


    // cekani na podprocesy
    while (wait(NULL) > 0)
    {

    }


    destroy_semaphores(&sems);

    destroy_shared_memory(shared_memory);



    return 0;
}



    // CO JE POTREBA UDELAT?
    /*
    -   funkce ktera zkontroluje argumenty      HOTOVO      OTESTOVANO
    -   funkce na vytvoreni semaforu            HOTOVO      OTESTOVANO
    -   funkce na zniceni semaforu + zameteni   HOTOVO      OTESTOVANO
    -   funkce, ktera bude posilat zpravy       HOTOVO      NEOTESTOVANO(nevim jestli u ni pokazde open/close file)
    -   funkce, ktera bude vypocitava rand.cas  HOTOVO      NEOTESTOVANO    
    -   funkce, ktera vytvori sdilenou pamet    HOTOVO      OTESTOVANO
    -   funkce, ktera znici sdilenou pamet      HOTOVO      OTESTOVANO   
    
    */
