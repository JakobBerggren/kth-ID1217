/* A program to calculate jacobi matrices in parallel using openmp
    @Author Jakob Berggren, Oskar Hahr

    usage with gcc (version 4.2 or higher required) and :
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

/* MAX for: Grid size, Number of Iterations and Working threads */

#define MAXSIZE 1000
#define MAXITERS 1000000
#define MAXWORKERS 4

int size, iters, workers;
double maxdiff;
double start_time, end_time;
FILE* output;

/* Iterative parallel function for calculating the max difference error between grids a & b */
void maxDiff(double** a, double** b){
    int i, j;
    double temp;
    
    #pragma omp parallel for private(j, temp)
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

/* Parallelized Jacobi Method function that iterates over the grids a & b updating their values
over a number of iterations. 
*/
void jacobi(double** a, double** b){
    int i, j, count;
    int interiorSize = size - 1;

    for(count = 0; count < iters; count++)
    {
        /* launch threads */
        #pragma omp parallel
        {
            /* Update all the values in grid b */
            #pragma omp for private(j)
            for(i = 1; i < interiorSize; i++){
                for(j = 1; j < interiorSize; j++){
                    b[i][j] = (a[i-1][j] + a[i+1][j] + a[i][j-1] +a[i][j+1])* 0.25;
                }   
            }
            /* Update all the values in grid a*/
            #pragma omp for private(j)
            for(i = 1; i < interiorSize; i++){
                for(j = 1; j < interiorSize; j++){
                    a[i][j] = (b[i-1][j] + b[i+1][j] + b[i][j-1] +b[i][j+1])*0.25;
                }   
            }
        }
    }   
}

/* Function for printing the results of a grid to the output file */
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

    /* initialize input variables */
    size = (argc > 1)? atoi(argv[1]) : MAXSIZE;
    iters = (argc > 2)? atoi(argv[2]) : MAXITERS;
    workers = (argc > 3)? atoi(argv[3]) : MAXWORKERS;
    if(size > MAXSIZE) size = MAXSIZE; 
    if(iters > MAXITERS) iters = MAXITERS;
    if(workers > MAXWORKERS) workers = MAXWORKERS;

   
    /* set number of workers */
    omp_set_num_threads(workers);

    /* The specified input variable: Size, is defined as the size of the interior grid.
    By adding 2 to this size the total size of the grid is retrieve, including outer boundary points 
    */
    size += 2;   
    
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
            /* condition for init with boundary points 
            outer boundary points are = 1 */
            if(i == 0 || j == 0 || i == size-1 || j == size-1){
                a[i][j] = 1;    
                b[i][j] = 1;
            }
            /* interior points are = 0 */
            else{
                a[i][j] = 0;
                b[i][j] = 0;
            }
        }
    }
    /* Beginning of computational part, read start time */
    start_time = omp_get_wtime();


    /* Jacobi iteration between a & b*/
    jacobi(a, b);
    /* Calculate max difference error between a & b*/
    maxDiff(a,b);

    /* End of computational part, read the end time */
    end_time = omp_get_wtime();


    print(a);
    printf("%d %d %d\t", size-2, iters, workers);
    printf("%g\t", end_time - start_time);
    printf("%g\n", maxdiff);
    free(a);
    free(b);

    return 0;
}