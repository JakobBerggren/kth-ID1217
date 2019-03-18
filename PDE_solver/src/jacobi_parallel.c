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
#define MAXITERS 1000000
#define MAXWORKERS 10

#define max(a,b) \
({__typeof__ (a) _a = (a); \
__typeof__ (b) _b = (b); \
_a > _b ? _a : _b; })

int size, iters, workers;
double maxdiff;
double start_time, end_time;
FILE* output;

void maxDiff(double** a, double** b){
    int i, j;
    double temp;
    
    #pragma omp for private(j, temp)
    for(i = 1; i < size; i++)
    {
        for(j = 1; j < size; j++){
            temp = a[i][j] - b[i][j];
            if(temp < 0)
                temp = -temp;
            if(temp > maxdiff){
                #pragma omp critical
                if(temp > maxdiff)
                    maxdiff =temp;
            }            
        }
    }
}

void jacobi(double** a, double** b){
    int i, j, count;
    int interiorSize = size - 1;

    for(count = 0; count < iters*0.5; count++)
    {
        #pragma omp for private(j)
        for(i = 1; i < interiorSize; i++){
            for(j = 1; j < interiorSize; j++){
                b[i][j] = (a[i-1][j] + a[i+1][j] + a[i][j-1] +a[i][j+1])* 0.25;
            }   
        }
        
        #pragma omp for private(j)
        for(i = 1; i < interiorSize; i++){
            for(j = 1; j < interiorSize; j++){
                a[i][j] = (b[i-1][j] + b[i+1][j] + b[i][j-1] +b[i][j+1])*0.25;
            }   
        }
    }   
}


void print(double** a){
    int i,j;
    output = fopen("./result/jacobi_parallel_matrix.txt","w");
    for(i = 0; i < size; i++){
        for(j = 0; j < size ; j++){
            fprintf(output,"%g, ",a[i][j]);
        }
        fprintf(output,"\n");
    }
    fclose(output);
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


    //print(a);
    printf("Size: %d, Iterations: %d, Number of Workers: %d,\t", size-2, iters, workers);
    printf("Runtime was: %gs,\t", end_time - start_time);
    printf("Maximum error: %g\n", maxdiff);
    free(a);
    free(b);

    return 0;
}
