/* matrix summation using OpenMP

   usage with gcc (version 4.2 or higher required):
     gcc -O -fopenmp -o matrixSum-openmp matrixSum-openmp.c 
     ./matrixSum-openmp size numWorkers

*/

#include <omp.h>

double start_time, end_time;
#include <stdlib.h>
#include <stdio.h>
#define MAXSIZE 10000  /* maximum matrix size */
#define MAXWORKERS 8   /* maximum number of workers */

int numWorkers;
int size; 
int matrix[MAXSIZE][MAXSIZE];
void *Worker(void *);

typedef struct{
    int value;
    int x, y;
} info;

/* read command line, initialize, and create threads */
int main(int argc, char *argv[]) {
  int i, j, total=0;
  info max, min;

  /* read command line args if any */
  size = (argc > 1)? atoi(argv[1]) : MAXSIZE;
  numWorkers = (argc > 2)? atoi(argv[2]) : MAXWORKERS;
  if (size > MAXSIZE) size = MAXSIZE;
  if (numWorkers > MAXWORKERS) numWorkers = MAXWORKERS;

  omp_set_num_threads(numWorkers);

  /* initialize the matrix */
  for (i = 0; i < size; i++) {
      printf("[ ");
	  for (j = 0; j < size; j++) {
      matrix[i][j] = rand()%99;
      	  printf(" %d", matrix[i][j]);
	  }
	  	  printf(" ]\n");
  }

  max.value = matrix[0][0];
  max.x = 0;
  max.y = 0;
  min.value = matrix[0][0];
  min.x = 0;
  min.y = 0;
  start_time = omp_get_wtime();
#pragma omp parallel for reduction (+:total) private(j)
  for (i = 0; i < size; i++)
    for (j = 0; j < size; j++){
      total += matrix[i][j];

      #pragma omp critical
      if(max.value < matrix[i][j]){
        max.value = matrix[i][j];
        max.y = i;
        max.x = j;
      }
      
      #pragma omp critical
      if(min.value > matrix[i][j]){
        min.value = matrix[i][j];
        min.y = i;
        min.x = j;
      }
        
    }
// implicit barrier

  end_time = omp_get_wtime();

  printf("the total is %d\n", total);
  printf("The maximum is %d located at [%d,%d]\n", max.value, max.y, max.x);
  printf("The minimum is %d located at [%d,%d]\n", min.value, min.y, min.x);
  printf("it took %g seconds\n", end_time - start_time);


}

