/***********************
 *  IOS projekt 2
 *  nazev:      proj2.c
 *  vytvoril:   Tomas Dolak 
 *  
 *  popis: projekt na synchronizaci procesu, program demonstruje reseni barbershop problem
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
        return PROGRAM_FAILED;
    }

    // kontrola zda jsou argumenty, cela cisla
    for (int i = 1; i < FIXED_NUM_OF_ARGS; i++)
    {
        if(!is_integer(argv[i]))
            return PROGRAM_FAILED;
    }

    // kontrola hodnot argumentu a jejich ulozeni do struktury
    for (int i = 1; i < FIXED_NUM_OF_ARGS; i++)
    {
        int arg_value = atoi(argv[i]);

        if (i == 1){
            if (arg_value < 1)
                return PROGRAM_FAILED;
            all_arguments->NZ = arg_value;
        } 

        if (i == 2){
            if (arg_value < 1)
                return PROGRAM_FAILED;
            all_arguments->NU = arg_value;
        }

        if (i == 3){
            if (arg_value < 0 || arg_value > 10000)
                return PROGRAM_FAILED;
            all_arguments->TZ = arg_value;
        }
        
        if (i == 4){
            if (arg_value < 0 || arg_value > 100)
                return PROGRAM_FAILED;
             all_arguments->TU = arg_value;
        }
        if (i == 5){
            if (arg_value < 0 || arg_value > 10000)
                return PROGRAM_FAILED;
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
        return PROGRAM_FAILED;

    semaphors->sem_que_letter = init("semaphore_que_letter",1);
    if(semaphors->sem_que_letter == SEM_FAILED)
        return PROGRAM_FAILED;

    semaphors->sem_que_packege = init("semaphore_que_package",1);
    if(semaphors->sem_que_packege == SEM_FAILED)
        return PROGRAM_FAILED;

    semaphors->costumer = init("costumer", 0);
    if(semaphors->costumer == SEM_FAILED)
        return PROGRAM_FAILED;

    semaphors->officer = init("officer", 0);
    if(semaphors->officer == SEM_FAILED)
        return PROGRAM_FAILED;

    semaphors->manager = init("manager_of_processes",1);
    if(semaphors->manager == SEM_FAILED)
        return PROGRAM_FAILED;

    semaphors->mutex = init("mutex_xdolak09",0);
    if(semaphors->manager == SEM_FAILED)
        return PROGRAM_FAILED;

    semaphors->costumer_going_home = init("costumer_going_home",0);
    if(semaphors->manager == SEM_FAILED)
        return PROGRAM_FAILED;


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
    sem_destroy(semaphors->costumer_going_home);
    sem_unlink("costumer_going_home");
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

// FUNKCE ktera vytvori sdilenou pamet a vraci ukazatel 

ipc_t  *initialize_shared_mem()
{
    // generovani klice pro nahodnou pamet, jeste neexistuje FILE_KEY!
    key_t key = ftok("test_version2.c",'c');

    // vytvoreni segmentu sdilene pameti
    int shmem_key = shmget(key, sizeof(ipc_t),0666 | IPC_CREAT);
    // pripojeni segmentu sdilene pameti k adresnimu prostoru procesu 
    ipc_t *ipc = shmat(shmem_key,NULL,0);

    ipc->shmid = shmem_key;

    return ipc;
}

// FUNKCE ktera znici vytvorenou sdilenou pamet

void destroy_shared_memory(ipc_t *ipc)
{
    key_t key = ipc->shmid;

    shmctl(key, IPC_RMID, NULL);

    shmdt(ipc);
}

/*          FUNKCE PRO UREDNIKA                 */

// FUNKCE ktera generuje podprocesy typu UREDNIK a nasledne u nich spousti funkci urednika

void officer_generator(args all_arguments, ipc_t *ipc)
{
    for(int i = 0; i < all_arguments.NU;i++)
    {
        pid_t off_id = fork();
        if (off_id == PROGRAM_FAILED)
            fprintf(stdout, "Error w generating children\n");
        else if(off_id == 0){
            officer_function(all_arguments,ipc);
            exit(0);
        }
        
    }
    
}

// FUNKCE urednika, dokud je posta otevrena nebo stoji nejaky zakaznik ve fronte tak urednik pracuje, pokud
// je posta zavrena a ve fronte uz nestoji zadny zakaznik, tak jde domu (konci)

void officer_function(args all_arguments, ipc_t *ipc)
{

    int officers_id;
    //uzamkonout semafor pro vypis
    sem_wait(sems.manager);
    ++(ipc->no_officers);
    officers_id = ipc->no_officers;
    put_message(officers_id,UREDNIK,STARTED,ipc);
    sem_post(sems.manager);

    while(1)
    {
        // pokud mam zakazniky, je mou povinnosti je obslouzit 
        if (ipc->act_no_costumers > 0)
        {
            // prvni vyberu do ktere fronty nakouknu
            int my_front = choose_me_random_line(3);
            // jak dlouho budu pracovat?
            int my_time_for_front = random_time(all_arguments.TU);

            if(my_front == 1)
            {
                // vybral jsem si prvni frontu, hura! Pojdme se podivat zda-li tam nekdo stoji
                if(ipc->cos_in_line_one > 0)
                {
                    // skoda, musim pracovat :( tak co ted?
                    sem_post(sems.officer); // pustim dovolim zakaznikovi aby isel dalej
                    // zavolam zakaznika k sobe
                    sem_post(sems.sem_que_letter);
                    --ipc->cos_in_line_one;
                    // po tom co se otevre semafor - costumer, vypisu ze si ho teda beru, zakaznik dosel
                    sem_wait(sems.costumer);

                    sem_wait(sems.manager);
                    put_message_extra(officers_id,SERVING_SERVICE_OF_TYPE_X,ipc,my_front);
                    sem_post(sems.manager);

                    // vykonavam praci
                    usleep(my_time_for_front);

                    sem_wait(sems.manager);
                    put_message(officers_id,UREDNIK,SERVICE_FINISHED,ipc);
                    sem_post(sems.manager);

                    // mel bych dovolit zakaznikovi odejit
                    sem_post(sems.costumer_going_home);
                    --ipc->act_no_costumers;

                    
                }

            }
            if(my_front == 2)
            {
                if(ipc->cos_in_line_two > 0)
                {
                    // skoda, musim pracovat :( tak co ted?
                    sem_post(sems.officer); // pustim dovolim zakaznikovi aby isel dalej
                    // zavolam zakaznika k sobe
                    sem_post(sems.sem_que_finance);
                    // po tom co se otevre semafor - costumer, vypisu ze si ho teda beru, zakaznik dosel
                    --ipc->cos_in_line_two;
                    sem_wait(sems.costumer);


                    sem_wait(sems.manager);
                    put_message_extra(officers_id,SERVING_SERVICE_OF_TYPE_X,ipc,my_front);
                    sem_post(sems.manager);

                    // vykonavam praci
                    usleep(my_time_for_front);

                    sem_wait(sems.manager);
                    put_message(officers_id,UREDNIK,SERVICE_FINISHED,ipc);
                    sem_post(sems.manager);

                    // mel bych dovolit zakaznikovi odejit
                    sem_post(sems.costumer_going_home);
                    --ipc->act_no_costumers;
                }
            }
            if(my_front == 3)
            {
                if(ipc->cos_in_line_three > 0)
                {
                    // skoda, musim pracovat :( tak co ted?
                    sem_post(sems.officer); // pustim dovolim zakaznikovi aby isel dalej
                    // zavolam zakaznika k sobe
                    sem_post(sems.sem_que_packege);
                    --ipc->cos_in_line_three;
                    sem_wait(sems.costumer);

                    // po tom co se otevre semafor - costumer, vypisu ze si ho teda beru, zakaznik dosel
                    sem_wait(sems.manager);
                    put_message_extra(officers_id,SERVING_SERVICE_OF_TYPE_X,ipc,my_front);
                    sem_post(sems.manager);

                    // vykonavam praci
                    usleep(my_time_for_front);

                    sem_wait(sems.manager);
                    put_message(officers_id,UREDNIK,SERVICE_FINISHED,ipc);
                    sem_post(sems.manager);

                    // mel bych dovolit zakaznikovi odejit
                    sem_post(sems.costumer_going_home);
                    --ipc->act_no_costumers;
                }
            } 
        }
        // jde domu jelikoz nejsou zadni zakaznici a posta je jiz zavrena 
        else if(ipc->act_no_costumers == 0 && !(ipc->open))
        {
            sem_wait(sems.manager);
            put_message(officers_id,UREDNIK,GOING_HOME,ipc);
            sem_post(sems.manager);
            exit(0);
        }
        // beru si prestavku jelikoz nemam zadne zakazniky
        else if(ipc->act_no_costumers == 0)
        {
            int break_of_oficer = random_time(all_arguments.TU);
            sem_wait(sems.manager);
            put_message(officers_id,UREDNIK,TAKING_BREAK,ipc);
            sem_post(sems.manager);
            usleep(break_of_oficer);

            sem_wait(sems.manager);
            put_message(officers_id,UREDNIK,BREAK_FINISHED,ipc);
            sem_post(sems.manager);
            
        }
        }

}


/*              FUNKCE PRO ZAKAZNIKA            */

// funkce generujici podporocesy, zakazniku a spousti u nich funkci zakaznik

void costumer_generator(args all_arguments, ipc_t *ipc)
{
    for(int i = 0; i < all_arguments.NZ; i++)
    {
        pid_t cos_id = fork();
        if (cos_id == PROGRAM_FAILED)
            fprintf(stdout, "Error w generating children\n");
        else if(cos_id == 0){
            //zavolat funkci pro zakaznika
            costumer_function(all_arguments,ipc);
            exit(0);
        }
    }
    
}

// funkce zakaznik, zakaznik se nejprve koukne zda je posta otevrena, pokud ano rozhodne se do jake fronty se postavi a chova se
// podle definovaneho chovani viz. kod a komentare uvitr funkce, pokud je posta zavrena jde domu

void costumer_function(args all_arguments, ipc_t *ipc)
{
    int costumer_id;
    // uzamknout semafor pro vypis
    sem_wait(sems.manager);
    ++(ipc->no_costumers);
    costumer_id = ipc->no_costumers;
    put_message(costumer_id,ZAKAZNIK, STARTED, ipc);
    sem_post(sems.manager);

    int rnd_time = random_time(all_arguments.TZ);
    usleep(rnd_time);

    int chosen_service;

    // rozhodovani zda je posta otevrena ci ne
    sem_wait(sems.mutex);
        if(ipc->open == false){

            sem_wait(sems.manager);
            put_message(costumer_id,ZAKAZNIK,GOING_HOME,ipc);
            sem_post(sems.manager);
            exit(0);
        }
        else{
            chosen_service = choose_me_random_line(3);
            
            if(chosen_service == 1){
                // zvedneme o jedna pocet cekajicich v dane fronte
                ++ipc->cos_in_line_one;
                // zvedneme o jedna celkovy pocet zakazniku
                ++ipc->act_no_costumers;
                
                sem_wait(sems.manager);
                put_message_extra(costumer_id,ENTERING_OFFICE_FOR_SERVICE_X,ipc,chosen_service);
                
                sem_post(sems.manager);

            }
            else if(chosen_service == 2){
                
                // zvedneme o jedna pocet cekajicich v dane fronte
                ++ipc->cos_in_line_two;
                // zvedneme o jedna celkovy pocet zakazniku
                ++ipc->act_no_costumers;
            
                sem_wait(sems.manager);
                put_message_extra(costumer_id,ENTERING_OFFICE_FOR_SERVICE_X,ipc,chosen_service);
                sem_post(sems.manager);

            }
            if(chosen_service == 3){
                
                // zaradi se do dane fronty
                //sem_wait(sems.sem_que_packege);
                // zvedneme o jedna pocet cekajicich v dane fronte
                ++ipc->cos_in_line_three;
                // zvedneme o jedna celkovy pocet zakazniku
                ++ipc->act_no_costumers;
            
                sem_wait(sems.manager);
                put_message_extra(costumer_id,ENTERING_OFFICE_FOR_SERVICE_X,ipc,chosen_service);
                sem_post(sems.manager);

            }
        }
    sem_post(sems.mutex);

    // pokusi se zamknout si urednika
    sem_wait(sems.officer);
    // kdyz se to podari -> tak si muzu vyridit svoji potrebu
    if (chosen_service == 1){

        sem_wait(sems.sem_que_letter);
        sem_wait(sems.manager);
        // je tedy zavolan urednikem
        put_message(costumer_id,ZAKAZNIK,CALLED_BY_OFFICE_WORKER,ipc);
        sem_post(sems.manager);
        // povolim urednikovi aby taky mohl neco vypsat
        sem_post(sems.costumer);
        //uspat 
        usleep(rnd_time);

        // tak vyridil jsem co jsem potreboval
        sem_wait(sems.manager);
        put_message(costumer_id,ZAKAZNIK,SERVICE_FINISHED,ipc);
        sem_post(sems.manager);
    

        //cekam az mi povoli jetzt dom
        sem_wait(sems.costumer_going_home);

        sem_wait(sems.manager);
        put_message(costumer_id,ZAKAZNIK,GOING_HOME,ipc);
        sem_post(sems.manager);
        sem_post(sems.sem_que_letter);
        exit(0);
    }
    else if (chosen_service == 2){
        sem_wait(sems.sem_que_finance);
        sem_wait(sems.manager);
        // je tedy zavolan urednikem
        put_message(costumer_id,ZAKAZNIK,CALLED_BY_OFFICE_WORKER,ipc);
        sem_post(sems.manager);
        // povolim urednikovi aby taky mohl neco vypsat
        sem_post(sems.costumer);
        //uspat 
        usleep(rnd_time);

        // tak vyridil jsem co jsem potreboval
        sem_wait(sems.manager);
        put_message(costumer_id,ZAKAZNIK,SERVICE_FINISHED,ipc);
        sem_post(sems.manager);
    
        
        //cekam az mi povoli jetzt dom
        sem_wait(sems.costumer_going_home);

        sem_wait(sems.manager);
        put_message(costumer_id,ZAKAZNIK,GOING_HOME,ipc);
        sem_post(sems.manager);
        exit(0);
    }
    else if (chosen_service == 3){
        sem_wait(sems.sem_que_packege);
        sem_wait(sems.manager);
        // je tedy zavolan urednikem
        put_message(costumer_id,ZAKAZNIK,CALLED_BY_OFFICE_WORKER,ipc);
        sem_post(sems.manager);
        // povolim urednikovi aby taky mohl neco vypsat
        sem_post(sems.costumer);
        //uspat 
        usleep(rnd_time);
        // tak vyridil jsem co jsem potreboval
        sem_wait(sems.manager);
        put_message(costumer_id,ZAKAZNIK,SERVICE_FINISHED,ipc);
        sem_post(sems.manager);
    

        //cekam az mi povoli jetzt dom
        sem_wait(sems.costumer_going_home);
        sem_wait(sems.manager);
        put_message(costumer_id,ZAKAZNIK,GOING_HOME,ipc);
        sem_post(sems.manager);
        exit(0);
    }

}

/*          FUNKCE GENERUJICI NAHODNE CISLA             */

// FUNKCE generujici nahodny cas od 0 az po max_time
int random_time(int max_time) {
    static bool seed_initialized = false;
    if (!seed_initialized) {
        struct timeval work_time;
        gettimeofday(&work_time, NULL);
        unsigned int micro_seed = work_time.tv_sec * 1000000 + work_time.tv_usec;
        srand(micro_seed);
        seed_initialized = true;
    }
    int ran_time = rand() % (max_time + 1);
    return ran_time;
}

// FUNKCE generujici nahodnou frontu 1 az 3 do ktere se zakaznik zaradi, nebo do ktere
// se urednik podiva 

int choose_me_random_line(int line) {
    static bool seed_initialized = false;
    if (!seed_initialized) {
        struct timeval tv;
        gettimeofday(&tv, NULL);
        unsigned int microsec_seed = tv.tv_sec * 1000000 + tv.tv_usec;
        srand(microsec_seed);
        seed_initialized = true;
    }
    int ran_time = rand() % line + 1; 
    return ran_time;
}

/*                  HLAVNI FUNKCE                  */

int main(int argc, char *argv[])
{

    // globalni promenne
    args all_args;
    int error = 0;
    ipc_t *shared_memory = NULL;
   
    //kontrola argumentu
    error = arguments(argc, argv, &all_args);
    if(error == PROGRAM_FAILED)
        return PROGRAM_FAILED;
    
    // vytvoreni sdilene pameti
    shared_memory = initialize_shared_mem();
    
    // otevreni posty
    shared_memory->open = true;

    error = create_semaphores(&sems);
    if(error == PROGRAM_FAILED)
    {   
        destroy_semaphores(&sems);
        return PROGRAM_FAILED;
    }
    printf("NZ: %d,NU: %d,TZ: %d,TU: %d, F: %d\n",all_args.NZ,all_args.NU,all_args.TZ,all_args.TU,all_args.F);    

    sem_post(sems.mutex);

    costumer_generator(all_args,shared_memory);

    officer_generator(all_args,shared_memory);
            
    
    
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

    sem_wait(sems.mutex);

    sem_wait(sems.manager);
    put_message_for_main_process(CLOSING, shared_memory);
    shared_memory->open = false;
    sem_post(sems.manager);
    sem_post(sems.mutex);

    // cekani na podprocesy
    wait(NULL);
    wait(NULL);

    
    destroy_semaphores(&sems);

    destroy_shared_memory(shared_memory);

    

    return 0;
}



// moznosti co muzou jebat valgrind:
// -    inicializace sdilene pameti po staru
// -    zkusit nahradit return -> exitem
// 