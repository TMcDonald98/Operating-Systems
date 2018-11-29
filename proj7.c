/*
Class: CPSC 346-01
Team Member 1: Thomas McDonald
Team Member 2: Luke Hartman
GU Username of project lead: tmcdonald
Pgm Name: proj7.c 
Pgm Desc: simulates reader writer problem controlled by threads
Compile: gcc -pthread proj7.c -lm
Usage: 1) ./a.out
*/
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>
#include <math.h>

_Bool isPrime(int);
void getRandomNum(char*);
void* writePipe(void*);
void* readPipe1(void*);
void* readPipe2(void*);
void* readPipe3(void*);

struct params
{
    int num, aPipe[2], threadNum;
    char* randNum;
    
};
struct paramsR
{
    int aPipe[2], threadNum;
    
};

typedef struct params params;

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Error: Options required\n"); 
        fprintf(stderr, "usage: ./a.out (int)\n\n"); 
        exit(EXIT_FAILURE);
    }else
    {
        pthread_t t1, t2, t3, t4;
        params p1;
        int status;

        p1.num = atoi(argv[1]);
        if (pipe(p1.aPipe) < 0)
            exit(1);
        //Set Up Writing thread
        status = pthread_create(&t1, NULL, writePipe, (void*) &p1);
        if(status != 0)
        {
            printf("Error in thread 1!");
            exit(-1);
        }
         pthread_join(t1, NULL);
        //Set up Reading thread
        status = pthread_create(&t2, NULL, readPipe1, (void*) &p1);
        if(status != 0)
        {
            printf("Error in thread 2!");
            exit(-1);
        }
        status = pthread_create(&t3, NULL, readPipe2, (void*) &p1);
        if(status != 0)
        {
            printf("Error in thread 2!");
            exit(-1);
        }
        status = pthread_create(&t4, NULL, readPipe3, (void*) &p1);
        if(status != 0)
        {
            printf("Error in thread 2!");
            exit(-1);
        }
        pthread_join(t2, NULL);
        pthread_join(t3, NULL);
        pthread_join(t4, NULL);
        return 0;
    }
}
void getRandomNum(char* str)
{
    int num = (rand() % 90000000) + 10000000;
    sprintf(str, "%d", num);
}
_Bool isPrime(int num)
{
    int i;
    double root = sqrt((double) num);
    for(i = 2; 1.0 * i < num/2; i++)
    {
        if (num % i == 0) 
            return 0;
    }
    return 1;      
}
void* writePipe(void* params_i)
{
    printf("Writing\n");
    params* p = (params*) params_i;
    int count;
    char str[8];
    //getRandomNum(str);
    //printf("")
    //char* str = (char*)calloc(8,sizeof(char*));
    for(count = 0; count < p->num; count++)
    {
        getRandomNum(p->randNum);
        write(p->aPipe[1], p->randNum, sizeof(p->randNum));
        printf("Generated: %s\n", p->randNum);
    }
    printf("Leaving Writing\n");
    close(p->aPipe[1]);
    
    pthread_exit(NULL);
}
void* readPipe1(void* params_i)
{
    //sleep(2); 
    printf("Reading\n");
    params* p = (params*) params_i;
    int size;
    char readNum[8];
    while(size = read(p->aPipe[0], readNum, sizeof(readNum)) > 0)
    {
        if(isPrime(atoi(readNum)))
            printf("Reader %d: %s is Prime.\n",1, readNum);
    }
    if(size != 0)
        exit(2);
    printf("Leaving Reading\n");
    pthread_exit(NULL);
}
void* readPipe2(void* params_i)
{
    sleep(1); 
    printf("Reading\n");
    params* p = (params*) params_i;
    int size;
    char readNum[8];
    while(size = read(p->aPipe[0], readNum, sizeof(readNum)) > 0)
    {
        if(isPrime(atoi(readNum)))
            printf("Reader %d: %s is Prime.\n",2, readNum);
    }
    if(size != 0)
        exit(2);
    printf("Leaving Reading\n");
    pthread_exit(NULL);
}
void* readPipe3(void* params_i)
{
    sleep(2); 
    printf("Reading\n");
    params* p = (params*) params_i;
    int size;
    char readNum[8];
    while(size = read(p->aPipe[0], readNum, sizeof(readNum)) > 0)
    {
        if(isPrime(atoi(readNum)))
            printf("Reader %d: %s is Prime.\n",3, readNum);
    }
    if(size != 0)
        exit(2);
    printf("Leaving Reading\n");
    pthread_exit(NULL);
}