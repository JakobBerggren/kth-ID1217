/* The Hungry Bird Problem with semaphores

    One producer - multiple consumers

    usage under Linux:
        gcc hungryBird.c -lpthread 
        ./a.out nBirds nWorms

*/

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <semaphore.h>

#define SHARED 1
#define MAXBIRDS 16
#define MAXWORMS 10000
#define TRUE 1

void *Producer(void*);
void *Consumer(void*);

sem_t empty, mutex;
int dish;

int nBirds;
int nWorms;

int main(int argc, char *argv[]) {
    int i;

    pthread_t babyBird[MAXBIRDS];
    pthread_t parentBird;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM); //TODO TRY WITHOUT ATTR

    nBirds = (argc > 1)? atoi(argv[1]) : MAXBIRDS;
    nWorms = (argc > 2)? atoi(argv[2]) : MAXWORMS;
    if (nBirds > MAXBIRDS) nBirds = MAXBIRDS;
    if (nWorms > MAXWORMS) nWorms = MAXWORMS;

    sem_init(&empty, SHARED, 0);    //init as full
    sem_init(&mutex, SHARED, 1);

    pthread_create(&parentBird, &attr, Producer, NULL);

    for(i = 0; i < nBirds; i++)
        pthread_create(&babyBird[i], &attr, Consumer, NULL);

    pthread_join(parentBird, NULL);
    for(i = 0; i < nBirds; i++)
        pthread_join(babyBird[i], NULL);

    printf("Main thread done");
}



void *Producer(void *arg){
    printf("parentBird created\n");

    while(TRUE){
        sem_wait(&empty);
        dish = nWorms;
        printf("The parent has returned with fresh worms.. %d worms are served\n", dish);      
        sem_post(&mutex);   //Return the mutex which called empty
    }
}



void *Consumer(void *arg){
    printf("babyBird created\n");

    while(TRUE){
        sem_wait(&mutex);

        if(dish == 0){
            printf("The last worm has been eaten, and the babies are still hungry..\n");
            sem_post(&empty);
        } else { 
            
            dish--;
            printf("A worm has been eaten by bird nr.%lu.. there are now %d worms left\n",pthread_self(), dish);
            sem_post(&mutex); 
        }
        sleep(1);
    }
}