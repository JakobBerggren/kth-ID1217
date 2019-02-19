/* The Bear and honeybees Problem with semaphores

    Multiple producers - one consumer

    usage under Linux:         
        gcc bearHoneybees.c -lpthread -lposix4
        ./a.out nHoneybees nPortions

*/

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <semaphore.h>

#define SHARED 1
#define MAXHONEYBEES 16
#define MAXPORTIONS 10000
#define TRUE 1

void *Producer(void*);
void *Consumer(void*);

sem_t full, mutex;
int pot;

int nHoneybees;
int nPortions;

int main(int argc, char *argv[]) {
    long i;

    pthread_t honeybees[MAXHONEYBEES];
    pthread_t bear;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM); //TODO TRY WITHOUT ATTR

    nHoneybees = (argc > 1)? atoi(argv[1]) : MAXHONEYBEES;
    nPortions = (argc > 2)? atoi(argv[2]) : MAXPORTIONS;
    if (nHoneybees > MAXHONEYBEES) nHoneybees = MAXHONEYBEES;
    if (nPortions > MAXPORTIONS) nPortions = MAXPORTIONS;

    sem_init(&full, SHARED, 0);    //init as full
    sem_init(&mutex, SHARED, 1);

    pthread_create(&bear, &attr, Consumer, NULL);

    for(i = 0; i < nHoneybees; i++)
        pthread_create(&honeybees[i], &attr, Producer, NULL);

    pthread_join(bear, NULL);
    for(i = 0; i < nHoneybees; i++)
        pthread_join(honeybees[i], NULL);

    printf("Main thread done");
}



void *Producer(void *arg){
    printf("Honeybee created\n");

    while(TRUE){
        sem_wait(&mutex);
        pot++;
        printf("The pot has been filled by bee nr.%lu %d portions\n",pthread_self() , pot);
        if(pot == nPortions){
            printf("The pot has %d portions, and is now full\n", pot);
            sem_post(&full);
        } else { 
            sem_post(&mutex); 
        }
        sleep(1);
    }
}



void *Consumer(void *arg){
    printf("Bear created\n");

    while(TRUE){
        sem_wait(&full);
        printf("The bear has awaken and eats all the honey..\n");
        pot = 0;      
        sem_post(&mutex);   //Return the mutex which called full
    }
}