/* Distributed pairing using MPI

    usage under Linux:         
        mpicc -o dining diningPhilosophers.c
        mpiexec -np rank ./dining

*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "mpi.h"

void table();
void philosopher(int rank);


int main(int argc, char *argv[]){
    int rank, size;

    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if(rank == 0){
        table();
    } else{
        philosopher(rank);
    }

    MPI_Finalize();
    return 0;
}

void table(){
   
    int chopsticks[5] = {1,1,1,1,1};    // 1 == free, 0 == taken
    int diningPhilo[5];                 // 1 == eating, 0 == thinking
    
    int chop, left, right, current;
    while(1){
        MPI_Recv(&current, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        current--;      //Normalize to zero-indexing because of rank

        left = current;
        right = current+1;

        if(right == 5){
            right = 0;
        }
        

        if(diningPhilo[current] == 1){  //already eating
            //return chopsticks
            chopsticks[left] = 1;
            chopsticks[right] = 1;

            diningPhilo[current] = 0;
        }
        else{   //HUNGRY
           chop = chopsticks[left] * chopsticks[right];

            if(chop == 1){  //can eat, make chops unavailable
                diningPhilo[current] = 1;
                chopsticks[left] = 0;
                chopsticks[right] = 0;
            }
            MPI_Send(&chop, 1, MPI_INT, current+1, 0, MPI_COMM_WORLD);
        }
    }
}

void philosopher(int rank){
    int chop;

    while(1){
        MPI_Send(&rank, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        MPI_Recv(&chop, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        if(chop == 1){
            printf("philosopher %d is eating\n",rank); 
            sleep(rand() % 5);      //Eating

                                    //Return the chopsticks
            printf("philosopher %d returns his chopsticks\n", rank);
            MPI_Send(&rank, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);

        } else{ printf("philosopher %d couldnt eat :(\n", rank); }
        //printf("philosopher %d is thinking\n", rank); 
        sleep(rand() % 5);          //Thinking
    }
}