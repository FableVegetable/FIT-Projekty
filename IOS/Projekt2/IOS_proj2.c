/**  
  * Názov programu:	2. projekt z IOS
  * Názov projektu: 	Synchronizacia procesov
  * Autor:		Tomáš Zubrik (c) 2017
  * Popis ulohy:	Implementujte v jazyce C modifikovaný synchronizační problém Child Care	(můžete se inspirovat
  *			knihou The Little Book of Semaphores). Existuje centrum, které se stará o děti, a dva typy 
  *			procesů: dospělý člověk (adult) a dítě (child). V centru se dospělí lidé starají o děti, přičemž 
  *			jeden dospělý se může starat nejvýše o tři děti. Např. pokud je v centru 5 dětí, musí být přítomni 
  *			alespoň 2 dospělí, pokud jsou pouze 3 děti, stačí jeden dospělý. Do centra přicházejí a z centra 
  *			odcházejí dospělí lidé a děti, ovšem tak, aby nebyla porušena výše uvedená výjimka. Pokud by byla 
  *			odchodem dospělého porušena podmínka, musí s odchodem vyčkat, než odejde dostatečný počet dětí. 
  *			Podobně, pokud chce vstoupit dítě a v centru není dostatek dospělých, musí vyčkat, až příjde další dospělý.
  */

//Sytemove knihovny
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <fcntl.h>

//Definovanie makier
#define UPPERBOUND (5000)
#define LOWERBOUND (0)

//Subor na vypis akcii procesov
FILE *output=NULL;	 

//Semafory
sem_t *mutex;
sem_t *childQueue; 
sem_t *adultQueue; 
sem_t *finishing;

//Zdielane premenne
int *action;
int *waiting; 
int *leaving; 
int *adults;
int *children;
int *CCount;
int *ACount;

//Miesto v pamati pre prislusne zdielane premenne
int shm_action;
int shm_waiting; 
int shm_leaving;
int shm_adults;
int shm_children;
int shm_CCount;
int shm_ACount;

//Globalne premenne
int A,C,AGT,CGT,AWT,CWT;

//Prototypy funkci

/**
  *    @brief Zatvori a odlinkuje z pamati vsetky vytvorene semafory
  */
void closeSemaphores();

/**
  *    @brief Odmapuje a odlinkuje z pamati vsetky zdielane premenne    
  */
void cleanUpMemory();

/**
  *    @brief Vypise chybu v pripade zlyhania vytvorenia potomka procesu a ukonci program    
  *    @param name Nazov prislusneho procesu, child alebo adult
  *    @param maker Nazov procesu, ktory vytvara podprocesy, s ktorymi pracujeme 
  */
void writeSystemError1(char *name, pid_t maker);

/**
  *    @brief Vypise chybu v pripade zlyhania vytvarania zdroja
  */
void writeSystemError2();

/**
  *    @brief Alokuje miesto v pamati prislusnej velkosti pre vsetky potrebne semafory a zdielane premenne
  */
void allocateMemory();

/**
  *    @brief Skontroluje spravny pocet a interval zadanych argumentov
  *    @param argc Pocet zadanych argumentov
  *    @param argv Jednotlive argumenty v poli
  *    @return Vracia 0, ak je kontrola argumentov korektna, v pripade chyby vracia 1 
  */
int checkArguments(int argc, char **argv);

/**
  *    @brief Zabitie procesov a ukoncenie programu so systemovou chybou
  *    @param handler Obsluha preruseni procesoru
  */
void terminate(int handler);

/**
  *    @brief Simuluje aktivitu procesu CHILD v centre, je zodpovedna za vypis jednotlivych akcii
  *    @param childOrder Poradie generovaneho procesu CHILD pri vypise
  *    @param count Pocet procesov CHILD, ktore budeme generovat
  *    @param sleeptime Cas, z ktoreho pouzijeme nahodny cas na uspanie procesu CHILD
  */
void ChildProcessing(int childOrder, int count, int sleeptime);

/**
  *    @brief Simuluje aktivitu procesu ADULT v centre, je zodpovedna za vypis jednotlivych akcii
  *    @param adultOrder Poradie generovaneho procesu ADULT pri vypise
  *    @param count Pocet procesov ADULT, ktore budeme generovat
  *    @param sleeptime Cas, z ktoreho pouzijeme nahodny cas na uspanie procesu ADULT
  */
void AdultProcessing(int adultOrder, int count, int sleeptime);

/**
  *    @brief Hlavna funkcia, ktora uskutocni celu simulaciu detskeho centra
  *    @param argc Pocet argumentov
  *    @param argv Jednotlive argumenty v poli
  */
int main(int argc, char **argv)
{
    signal(SIGTERM, terminate);
    signal(SIGINT, terminate);
    
    srand(time(0));
    srandom(getpid());
    
    if(checkArguments(argc, argv) == 1)
        return 1;
    
    if((output = fopen("proj2.out","w")) == NULL)
    {
        fprintf(stderr,"Subor \"proj2.out\" sa nepodarilo otvorit!\n");
        return 2;
    }

    setbuf(output,NULL);
    allocateMemory();

    (*CCount) = atoi(argv[2]);    
    (*ACount) = atoi(argv[1]);
    
    int totalnum = (*ACount) + (*CCount);

    pid_t adult;
    pid_t adultMaker = fork();
    if(adultMaker == 0) //potomok procesu 
    {
        for(int i=0; i<A; i++)
        {
            adult = fork();
            if(adult == 0)
            {   
                if(AGT) usleep((rand()%AGT + 1)*1000);
                AdultProcessing(i+1, totalnum, AWT);
                closeSemaphores();
                fclose(output);
                exit(0);
            }
            if(adult < 0)
                writeSystemError1("ADULT", adultMaker);
        }
        if(adult > 0) //parent proces
        {
            while(wait(NULL) > 0);
        }
        closeSemaphores();
        fclose(output);
        exit(0);
    }

    if(adultMaker < 0 )
        writeSystemError1("ADULT", adultMaker);
 
    pid_t child;
    pid_t childMaker = fork();
    if(childMaker == 0) //potomok procesu
    {
        for(int i=0; i<C; i++)
        {
            child = fork();
            if(child == 0)
            {
                if(CGT) usleep((rand()%CGT + 1)*1000);
                ChildProcessing(i+1, totalnum, CWT);
                closeSemaphores();
                fclose(output);
                exit(0);
            }
            if(child < 0)
                writeSystemError1("CHILD", childMaker);
        }
        if(child > 0) //parent proces
        {
            while(wait(NULL) > 0);
        }
        closeSemaphores();
        fclose(output);
        exit(0);
        
    }
    if(childMaker < 0 )
        writeSystemError1("CHILD", childMaker);
    
    if(childMaker > 0 && adultMaker > 0)
    {
        while(wait(NULL) >0);
    }

    cleanUpMemory();
    return 0;
}

void writeSystemError1(char *name, pid_t maker)
{
    fprintf(stderr, "Nepodarilo sa vytvorit potomka procesu %s\n", name);
    kill(maker, SIGTERM);
    terminate(0);
    exit(2);
}

void writeSystemError2()
{
    fprintf(stderr, "Vytvaranie zdroja zlyhalo \n");
    cleanUpMemory();
    exit(2); 
}

void terminate(int handler)
{
    (void)handler;
    kill(getpid(), SIGTERM);
    cleanUpMemory();
    exit(2);
}

void closeSemaphores()
{
    //Zatvorenie semeforov
    sem_close(mutex);
    sem_close(childQueue);
    sem_close(adultQueue);
    sem_close(finishing);
   
    //Odalokovanie miesta semaforov
    sem_unlink("/xzubri00mutex");
    sem_unlink("/xzubri00childQueue");
    sem_unlink("/xzubri00adultQueue");
    sem_unlink("/xzubri00finishing");
}

void cleanUpMemory()
{
    closeSemaphores();
    fclose(output);
     
    //Odmapovanie pamate
    munmap(waiting, sizeof(int));
    munmap(leaving, sizeof(int));
    munmap(action, sizeof(int));
    munmap(adults, sizeof(int));
    munmap(children, sizeof(int));
    munmap(CCount, sizeof(int)); 
    munmap(ACount, sizeof(int));
    
     //Vymazanie pamate a zatvorenie
    shm_unlink("/xzubri00waiting");     close(shm_waiting);
    shm_unlink("/xzubri00leaving");     close(shm_leaving);
    shm_unlink("/xzubri00action");      close(shm_action);
    shm_unlink("/xzubri00adults");      close(shm_adults);
    shm_unlink("/xzubri00children");    close(shm_children); 
    shm_unlink("/xzubri00CCount");    close(shm_CCount);   
    shm_unlink("/xzubri00ACount");    close(shm_ACount); 
}


void allocateMemory()
{
    //Vytvorenie semaforov
    mutex = sem_open("/xzubri00mutex", O_CREAT | O_EXCL, 0644, 1);  //mutual exclusion semaphore
    childQueue = sem_open("/xzubri00childQueue", O_CREAT | O_EXCL, 0644, 0);
    adultQueue = sem_open("/xzubri00adultQueue", O_CREAT | O_EXCL, 0644, 0);
    finishing = sem_open("/xzubri00finishing", O_CREAT | O_EXCL, 0644, 0);
    
    if(mutex == SEM_FAILED || childQueue == SEM_FAILED || adultQueue == SEM_FAILED || finishing == SEM_FAILED)
        writeSystemError2();
    
    //Vytvorenie zdielanych premmenych
    shm_waiting = shm_open("/xzubri00waiting", O_CREAT | O_EXCL | O_RDWR, 0644);
    shm_leaving = shm_open("/xzubri00leaving", O_CREAT | O_EXCL | O_RDWR, 0644);
    shm_action = shm_open("/xzubri00action", O_CREAT | O_EXCL | O_RDWR, 0644);
    shm_adults = shm_open("/xzubri00adults", O_CREAT | O_EXCL | O_RDWR, 0644);
    shm_children = shm_open("/xzubri00children", O_CREAT | O_EXCL | O_RDWR, 0644);
    shm_CCount = shm_open("/xzubri00CCount", O_CREAT | O_EXCL | O_RDWR, 0644);
    shm_ACount = shm_open("/xzubri00ACount", O_CREAT | O_EXCL | O_RDWR, 0644);
    
    if(shm_leaving == -1 || shm_waiting == -1 || shm_action == -1 || shm_adults == -1 || shm_children == -1 || shm_ACount == -1 || shm_CCount == -1)
        writeSystemError2();
    
    //Vytvorenie miesta pre zdielanu premmenu velkosti int
    ftruncate(shm_action, sizeof(int));
    ftruncate(shm_waiting, sizeof(int));
    ftruncate(shm_leaving, sizeof(int));
    ftruncate(shm_adults, sizeof(int));
    ftruncate(shm_children, sizeof(int));
    ftruncate(shm_CCount, sizeof(int));
    ftruncate(shm_ACount, sizeof(int));

    //Namapovanie zdielanej pamate
    action = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shm_action, 0);
    leaving = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shm_leaving, 0);
    waiting = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shm_waiting, 0);
    adults = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shm_adults, 0);
    children = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shm_children, 0);
    CCount = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shm_CCount, 0);
    ACount = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shm_ACount, 0);
    
    if(action == MAP_FAILED || leaving == MAP_FAILED || waiting == MAP_FAILED || adults == MAP_FAILED || children == MAP_FAILED || ACount == MAP_FAILED || CCount == MAP_FAILED)
        writeSystemError2();
}

int checkArguments(int argc, char **argv)
{
    if(argc == 7)
    {
        char *errors[6];
        A = strtol(argv[1], &errors[0], 10);
        C = strtol(argv[2], &errors[1], 10);
        AGT = strtol(argv[3], &errors[2], 10);
        CGT = strtol(argv[4], &errors[3], 10);
        AWT = strtol(argv[5], &errors[4], 10);
        CWT = strtol(argv[6], &errors[5], 10);
        
        for(int i=0; i<6; i++)
        {
            if(*errors[i] != '\n' && *errors[i] != 0)
            {
                fprintf(stderr, "Argumenty musia byt cele kladne cisla!\n");
                return 1;
            }
        }
        
        if(A <= LOWERBOUND || C <= LOWERBOUND)
        {
            fprintf(stderr, "Pocet procesov ADULTS a CHILDREN musi byt kladne cele cislo!\n");
            return 1;
        }
        
        if(AGT > UPPERBOUND || AGT < LOWERBOUND || CWT > UPPERBOUND || CGT < LOWERBOUND)
        {
            fprintf(stderr, "Doba generovania a simulovania procesov nie je v rozmedzi <0, 5000> !\n");
            return 1;
        }
    }
    else
    {
        fprintf(stderr, "Zadaj 6 argumentov (A,C,AGT,CGT,AWT,CWT)\n");
        return 1;
    }
    return 0;
}

void ChildProcessing(int childOrder, int count, int sleeptime)
{
    sem_wait(mutex);
    fprintf(output, "%d	: C %d	: started\n", ++(*action), childOrder);
    sem_post(mutex);
    
    sem_wait(mutex);
    if( ((*children) < 3 * (*adults)) || (*ACount == 0))
    {
        (*children)++;
        fprintf(output, "%d	: C %d	: enter\n", ++(*action), childOrder);
        sem_post(mutex);
    }
    else
    {
        (*waiting)++;
        fprintf(output, "%d	: C %d	: waiting : %d : %d\n", ++(*action), childOrder, (*adults) + (*leaving), (*children));
        sem_post(mutex);
        sem_wait(childQueue);

     	sem_wait(mutex);
        fprintf(output, "%d	: C %d	: enter\n", ++(*action), childOrder);
        sem_post(mutex);
    }

    if(sleeptime)    usleep((rand() % sleeptime + 1)*1000);
    
    sem_wait(mutex);
    fprintf(output, "%d	: C %d	: trying to leave\n", ++(*action), childOrder);
    fprintf(output, "%d	: C %d	: leave\n", ++(*action), childOrder);
    (*children)--;
    (*CCount)--;

    if( (*leaving) && (*children) <= (3 * ((*adults) - 1)) )
    {
        sem_post(mutex);
        sem_post(adultQueue);
    }
    else sem_post(mutex);
    
    if((*CCount + *ACount) == 0)
    {
        for(int i=0; i<count; i++)
            sem_post(finishing);
    }
    else sem_wait(finishing);

    sem_wait(mutex);
    fprintf(output, "%d	: C %d	: finished\n", ++(*action), childOrder);
    sem_post(mutex);   
}

void AdultProcessing(int adultOrder, int count, int sleeptime)
{
    sem_wait(mutex);
    fprintf(output, "%d	: A %d	: started\n", ++(*action), adultOrder);
    sem_post(mutex);

    sem_wait(mutex);
    (*adults)++;
    fprintf(output, "%d	: A %d	: enter\n", ++(*action), adultOrder);
    if(*waiting)
    {
        int n = (*waiting <= 3)?(*waiting):3;
        sem_post(mutex);
        for(int i=0; i<n; i++)
	{
	    (*waiting)--;
	    (*children)++;
            sem_post(childQueue);
 	}
    }
    else sem_post(mutex);

    if(sleeptime) usleep((rand() % sleeptime + 1)*1000);
    
    sem_wait(mutex);
    fprintf(output, "%d	: A %d	: trying to leave\n", ++(*action), adultOrder);
    if((*children) <= (3 * ((*adults)-1)))
    {
        (*adults)--;
        (*ACount)--;
        fprintf(output, "%d	: A %d	: leave\n", ++(*action), adultOrder);
        sem_post(mutex);
    }
    else
    {
        (*leaving)++;
        fprintf(output, "%d	: A %d	: waiting : %d : %d\n", ++(*action), adultOrder, (*adults),(*children));

        sem_post(mutex);
        sem_wait(adultQueue);       
 
        sem_wait(mutex);
        (*adults)--;
        (*leaving)--;
        (*ACount)--;
        fprintf(output, "%d	: A %d	: leave\n", ++(*action), adultOrder);
        sem_post(mutex);
    }

    if(*ACount == 0)
        for(int i=0; i<(*waiting); i++)
            sem_post(childQueue);
    
  
    if((*CCount + *ACount) == 0)
    {    
        for(int i=0; i<count; i++)
            sem_post(finishing);
    }
    else sem_wait(finishing);
 
    sem_wait(mutex);
    fprintf(output, "%d	: A %d	: finished\n", ++(*action), adultOrder);
    sem_post(mutex);
}

