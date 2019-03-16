/* A program to calculate jacobi matrices in parallel using openmp
    @Author Jakob Berggren, Oskar Hahr

    usage with gcc (version 4.2 or higher required):
        gcc -O -fopenmp -o jacobi_parallel jacobi_parallel.c
        ./jacobi_parallel size iters workers

*/

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <sys/time.h>
#include <math.h>
#include <limits.h>
#include <omp.h>




#define MAXSIZE 1000
#define MAXITERS 100
#define MAXWORKERS 10

#define max(a,b) \
({__typeof__ (a) _a = (a); \
__typeof__ (b) _b = (b); \
_a > _b ? _a : _b; })

int size, iters, workers;
double maxdiff;
double start_time, end_time;

void maxDiff(double** a, double** b){
    int i, j;
    double local_maxdiff;
    
    #pragma omp for private(j)
    for(i = 1; i < size; i++)
    {
        for(j = 0; j < size; j++){
            local_maxdiff = max(maxdiff, fabs(a[i][j] - b[i][j]));
            if(local_maxdiff > maxdiff){
                #pragma omp critical(maxdiff)
                if(local_maxdiff > maxdiff){
                    maxdiff = local_maxdiff;
                }
            }
            
        }
    }
}

void jacobi(double** a, double** b){
    int i, j, count;
    int interiorSize = size - 1;

    for(count = 0; count < iters; count++)
    {
        #pragma omp for private(j)
        for(i = 1; i < interiorSize; i++){
            for(j = 1; j < interiorSize; j++){
                b[i][j] = (a[i-1][j] + a[i+1][j] + a[i][j-1] +a[i][j+1])*0.25;
            }   
        }
                //super sick implicit barrier
        #pragma omp for private(j)
        for(i = 1; i < interiorSize; i++){
            for(j = 1; j < interiorSize; j++){
                a[i][j] = (b[i-1][j] + b[i+1][j] + b[i][j-1] +b[i][j+1])*0.25;
            }   
        }
                //not so slick implicit barrier
    }   
}


void print(double** a, double** b, double maxdiff){
    int i,j;
    if(size < 203){
        printf("Matrix A\n");
        for(i = 0; i < size; i++){
            for(j = 0; j < size ; j++){
                printf("%g, ",a[i][j]);
            }
            printf("\n");
        }

        printf("\nMatrix B\n");
        for(i = 0; i < size; i++){
            for(j = 0; j < size ; j++){
                printf("%g, ",b[i][j]);
            }
            printf("\n");
        }     
    }
    printf("Maxdiff is: %g", maxdiff);
}



int main(int argc, char const *argv[])
{
    int i,j;
    maxdiff = 0.0;
    size = (argc > 1)? atoi(argv[1]) : MAXSIZE;
    iters = (argc > 2)? atoi(argv[2]) : MAXITERS;
    workers = (argc > 3)? atoi(argv[3]) : MAXWORKERS;
    if(size > MAXSIZE) size = MAXSIZE; 
    if(iters > MAXITERS) iters = MAXITERS;
    if(workers > MAXWORKERS) workers = MAXWORKERS;

    size += 2;      // makes room for boundary points :)  
    omp_set_num_threads(workers);


    double** a = malloc(size*sizeof(double*));
    double** b = malloc(size*sizeof(double*));
    for(i = 0; i < size; i++){
        a[i] = malloc(size*sizeof(double));
        b[i] = malloc(size*sizeof(double));
    }

    /* init matrices */
    for(i = 0; i < size; i++){
        for( j = 0; j < size; j++)
        {
            /* condition for init with boundary points */
            if(i == 0 || j == 0 || i == size-1 || j == size-1){
                a[i][j] = 1;    
                b[i][j] = 1;
            }
            /* interior points */
            else{
                a[i][j] = 0;
                b[i][j] = 0;
            }
        }
    }
    start_time = omp_get_wtime();
    #pragma omp parallel
    {
        jacobi(a, b);
        maxDiff(a,b);
    }
    end_time = omp_get_wtime();


    print(a, b,maxdiff);

    free(a);
    free(b);

    return 0;
}
