/* Distributed pairing using MPI

    usage under Linux:         
        mpicc -o dist distributedPairing.c
        mpiexec -np rank ./dist

    Solution:
        Solved by allowing the teacher to know the number of students and group size.
        Teacher listens to messages via mpi and for each message gotten it decrements the counter of students
        and increment the groupCount. If groupCount = size then the teacher sets groupCount to 0, since we have a group which 
        is communicated via MPI to the students in that group.

*/


#include "mpi.h"
#include <stdio.h>

void teacher(int size);
void student(int rank);


int main(int argc, char *argv[]){
    int rank, size;

    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if(rank == 0){
        teacher(size);
    } else{
        student(rank);
    }


    MPI_Finalize();
}

void teacher(int size){
    int n = size - 1;   //teacher is -1
    

    int group[2];
    int groupCount = 0;
    
    while(n > 0){
        MPI_Recv(&group[groupCount], 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        n--;
        groupCount++;

        if(groupCount == 2){
            groupCount = 0;
            MPI_Send(&group[0], 1, MPI_INT, group[1], 0, MPI_COMM_WORLD);
            MPI_Send(&group[1], 1, MPI_INT, group[0], 0, MPI_COMM_WORLD);
        }
        else if(groupCount > 0 && n == 0){
            groupCount = 0;
            MPI_Send(&group[0], 1, MPI_INT, group[0], 0, MPI_COMM_WORLD);
        }
    }
}


void student(int rank){
    int partner = 0;
    
    MPI_Send(&rank, 1,	MPI_INT, 0, 0, MPI_COMM_WORLD); 

    MPI_Recv(&partner, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);


    printf("Student %d: My partner is %d\n", rank, partner);


}
