/*
Class: CPSC 346-01
Team Member 1: Thomas McDonald
Team Member 2: Luke Hartman
GU Username of project lead: tmcdonald
Pgm Name: proj6.c 
Pgm Desc: simulates producer consumer problem controlled by semaphore 
Usage: 1) ./a.out
*/
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <unistd.h>
#include <wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INIT_MUTEX 1 
#define INIT_EMPTY 100 
#define INIT_FULL  0 
#define EMPTY_ARG  0 //used when argument is optional 

void prod_cons();
void producer();
void consumer();
void criticalSection(int);
void set_sem_values(int,int,int);
void get_sem_values(int,int,int);
void set_sem_struct(struct sembuf[],int);
int create_semaphore(int);
void fatal_error(char[]);
void cleanup(int,int,int);

const int PROD = 0;
const int CONS = 1;

int main(int argc, char*argv[])
{
    prod_cons();
    return 0;
}
void prod_cons()
{
    struct sembuf s_wait[1], s_signal[1];
    int mutex, empty, full, ch_stat, status;

    set_sem_struct(s_wait, -1);
    set_sem_struct(s_signal, 1);

    mutex = create_semaphore(0);
    empty = create_semaphore(1);
    full = create_semaphore(2);

    set_sem_values(mutex, empty,full);
    printf("Initial semaphore values:\n");
    get_sem_values(mutex, empty, full);
    
    pid_t pid = fork();
    if(pid<0)
    {
        fprintf(stderr, "Fork Error");
        return;
    }else if (pid == 0)
    {
        consumer(mutex, empty, full);
        printf("Final semaphore values\n");
        get_sem_values(mutex, empty, full);
        cleanup(mutex,empty,full);
    }else
    {
        producer(mutex, empty, full);
        status = wait(&ch_stat);
    }
}
//handels semiphores and critial section 
void producer(int mutex, int empty, int full)
{
    struct sembuf s_wait[1], s_signal[1];
    int ch_stat, status;
    set_sem_struct(s_wait, -1);
    set_sem_struct(s_signal, 1);
    int i;
    for(i = 0; i < 5; i++)
    {   
        if (semop(mutex, s_wait, 1) == -1)
            fatal_error("mutex"); 
        if (semop(empty, s_wait, 1) == -1)
            fatal_error("empty"); 
        criticalSection(PROD);
        if (semop(mutex, s_signal, 1) == -1)
            fatal_error("mutex"); 
        if (semop(full, s_signal, 1) == -1)
            fatal_error("full"); 
    }
}
 //handels semiphores and critial section
void consumer(int mutex, int empty, int full)
{
    struct sembuf s_wait[1], s_signal[1];

    set_sem_struct(s_wait, -1);
    set_sem_struct(s_signal, 1);
    int i;
    for (i = 0; i <5; i++)
    {
        if (semop(mutex, s_wait, 1) == -1)
            fatal_error("mutex"); 
        if (semop(full, s_wait, 1) == -1)
            fatal_error("full"); 
        criticalSection(CONS);
        if (semop(mutex, s_signal, 1) == -1)
            fatal_error("mutex"); 
        if (semop(empty, s_signal, 1) == -1)
            fatal_error("empty"); 
        sleep(1);
    }
}
//handels the producer or consumer
void criticalSection(int who)
{
    if (who == PROD)
    {
        sleep(1);
        printf("Producer making an item\n");
    }else
    {
        sleep(4);
        printf("Consumer consuming an item\n");
    }
}

//create a semaphore set of 1 semaphore specified by key 
int create_semaphore(int key)
{
 int new_sem;
 if ((new_sem = semget(key, 1, 0777 | IPC_CREAT)) == -1)
   {
    perror("semget failed");
    exit(1);
   }
 return new_sem;
}

//set struct values for down/up ops to be used in semop
//By design, we will always refer to the 0th semaphore in the set
void set_sem_struct(struct sembuf sem[], int op_val)
{
 sem[0].sem_num = 0;
 sem[0].sem_op = op_val;
 sem[0].sem_flg = SEM_UNDO;
} 

//set mutex, empty, full to initial values
//these semaphore sets consist of a single semaphore, the 0th 
void set_sem_values(int mutex, int empty, int full)
{
 semctl(mutex, 0, SETVAL, INIT_MUTEX);
 semctl(empty, 0, SETVAL, INIT_EMPTY);
 semctl(full, 0, SETVAL, INIT_FULL);
}

//retrieve value held by the 0th semaphore, which is the second arg.
//0 in the final argument indicates that it is not used 
void get_sem_values(int mutex, int empty, int full)
{
 int m, e, f; 
 m = semctl(mutex, 0, GETVAL, 0);
 e = semctl(empty, 0, GETVAL, 0);
 f = semctl(full, 0, GETVAL, 0);
 printf("mutex: %d empty: %d full: %d\n", m,e,f);
}

//Display error and exit if semop fails
void fatal_error(char sem[])
{
 perror(strcat("Exiting. Failed on ",sem)); 
 exit(1);
}

//remove semaphores
void cleanup(int mutex, int empty, int full)
{
 semctl(mutex, 1, IPC_RMID, EMPTY_ARG);
 semctl(empty, 1, IPC_RMID, EMPTY_ARG);
 semctl(full, 1, IPC_RMID, EMPTY_ARG);
}