/*
Class: CPSC 346-01
Team Member 1: Thomas McDonald
Team Member 2: N/A 
GU Username of project lead: tmcdonald
Pgm Name: proj5.c 
Pgm Desc: simulates two processes running controled by Peterson's 
Usage: 1) ./a.out
Usage: 2) ./a.out int int int int
*/
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <pthread.h>
struct Flags
{
    int pr0,pr1,turn;
};
void parent(int, int, struct Flags*);
void child(int, int, struct Flags*);
void cs(char, int);
void non_crit_sect(int);
void main(int argc, char* argv[])
{
    int time_parent;
    int time_child;
    int time_parent_non_cs;
    int time_child_non_cs;
    //check for proper arguments
    if(argc == 1)
    {
        time_parent=2;
        time_child=2;
        time_parent_non_cs=2;
        time_child_non_cs=2;
    }else if(argc == 5)
    {
        time_parent = atoi(argv[1]);
        time_child = atoi(argv[2]);
        time_parent_non_cs = atoi(argv[3]);
        time_child_non_cs = atoi(argv[4]);
    }else
    {
        printf("Sorry, invalid number of arguments");
        return;
    }
    int shmid = shmget(0, 1, 0777 | IPC_CREAT);
    struct Flags*  flags;
    flags = (struct Flags*)shmat(shmid,0,0);
    flags->pr0 = 0;
    flags->pr1 = 0;

    pid_t pid = fork();
    if(pid<0)
    {
        fprintf(stderr, "Fork Error");
        return;
    }else if (pid == 0)
    {
        child(time_child, time_child_non_cs, flags);
    }else
    {
        parent(time_parent, time_parent_non_cs, flags);
    }
    shmdt(flags);
    shmctl(shmid, IPC_RMID, 0);
}
void parent(int time_crit_sect, int time_non_crit_sect, struct Flags* flags)
{
    for (int i = 0; i < 10; i++)
    {
        flags->pr1 = 1;
        flags->turn = 0;
        while(flags->pr0 && !flags->turn);
        cs('p', time_crit_sect);
        flags->pr1 = 0;
        non_crit_sect(time_non_crit_sect);        
    }
}
void child(int time_crit_sect, int time_non_crit_sect, struct Flags* flags)
{
    for (int i = 0; i < 10; i++)
    {
        flags->pr0 = 1;
        flags->turn = 1;
        while(flags->pr1 && flags->turn);
        cs('c', time_crit_sect);
        flags->pr0 = 0;   
        non_crit_sect(time_non_crit_sect);   
    }
}
void cs(char process, int time_crit_sect)
{
    if (process == 'p')
    {
        printf("parent in critical sction\n");
        //printf("%d\n", time_crit_sect);
        sleep(time_crit_sect);
        printf("parent leaving critical section\n");
    }
    else
    {
        printf("child in critical sction\n");
        sleep(time_crit_sect);
        printf("child leaving critical section\n");
    }
}
void non_crit_sect(int time_non_crit_sect)
{
    sleep(time_non_crit_sect);
}