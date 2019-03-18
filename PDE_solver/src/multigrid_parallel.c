/* A program to calculate multigrid jacobi matrices in parallel using openmp
    @Author Jakob Berggren, Oskar Hahr

    usage with gcc (version 4.2 or higher required) and :
        gcc -O -fopenmp -o multigrid_parallel multigrid_parallel.c
        ./multigrid_parallel size iters workers
*/

#include <omp.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <sys/time.h>
#include <math.h>
#include <limits.h>

/* MAX numbers for grid size, number of iterations and workers */
#define MAXSIZE 1000
#define MAXITERS 10000000
#define MAXWORKERS 4

int iters, workers;
double start_time, end_time, maxdiff;
FILE* output;

/* Iterative parallel function for calculating the max difference between grids a & b with the size of size */
void maxDiff(double** a, double** b, int size){
    int i, j;
    double temp;

    #pragma omp parallel for private(j, temp)
    for(i = 0; i < size; i++)
    {
        for(j = 0; j < size; j++){

            temp = a[i][j] - b[i][j];
            /* If value of a - b is negative, flip it */
            if(temp < 0)
                temp = -temp;
            /* if the new value is a larger error, replace it */
            if(temp > maxdiff){
            /* maxdiff is a shared global variable, protected with mutex */
                #pragma omp critical
                if(temp > maxdiff)
                    maxdiff = temp;
            }
        }
    }
}
/* An iterative Jacobi Method function that iterates over the grids a & b updating their values
over a number of iterations. */
void jacobi(double** a, double** b, int size, int iterations){
    int interiorSize = size - 1;
    /* launch parallel threads */
    int count = 0;
        /* initialize private variables */
        int id = omp_get_thread_num();
        for(count = 0; count < iterations; count++)
        {   
            /* First for loop to calculate new values of grid b */
            #pragma omp parallel
            {
            int i,j;
            #pragma omp for
            for(i = 1; i < interiorSize; i++){
                for(j = 1; j < interiorSize; j++){
                b[i][j] = (a[i-1][j] + a[i+1][j] + a[i][j-1] +a[i][j+1]) * 0.25;
                }   
            }
            /* Second for loop to calculate new values of grid a */
            #pragma omp for
            for(i = 1; i < interiorSize; i++){
                for(j = 1; j < interiorSize; j++){
                    a[i][j] = (b[i-1][j] + b[i+1][j] + b[i][j-1] +b[i][j+1])*0.25;
                }   
            }  
        }  
    } 
}


void print(double** a, int s){
    int i,j;
    output = fopen("multigrid_parallel_matrix.txt","w");

    for(i = 0; i < s; i++){
        for(j = 0; j < s; j++){
            fprintf(output,"%g, ",a[i][j]);
        }
        fprintf(output,"\n");
    }
    fclose(output);
}

/* Parallel restriction function, the restriction function is used to project the values of a fine grid onto a coarse grid. 
This function is used to down a level in the V-Cycle */
void restriction(double** fine, double** coarse, int size){

    int i, j, x, y;
    int sizeC = size-1;
    /* iterate over the coarse matrix, mapping has a 1:2 relation between the coarse matrix to the fine matrix in regards to i,j : x,y */
    #pragma omp parallel for private(j, x, y)
    for(i = 1; i < sizeC; i++)
    {
        x = i << 1;
        for(j = 1; j < sizeC; j++)
        {
            y = j << 1;
            coarse[i][j] = fine[x][y]*0.5 + (fine[x-1][y] + fine[x][y-1] + fine[x][y + 1] + fine[x + 1][y]) * 0.125;
        }
    }
}

/* Parallel interpolation function, interpolation is used to project values of a coarse grid onto a fine grid 
This function is called to move up a level in the V-Cycle */
void interpolation(double** coarse, double** fine, int sizeFine, int sizeCoarse){
    

    int i, j, x, y;
    int sizeF = sizeFine - 1;
    int sizeC = sizeCoarse - 1;
    /* launch parallel threads*/
    #pragma omp parallel
    {
        /* Update the fine points that directly map to a coarse point in the grid */
        #pragma omp for private(j, x, y)
        for(i = 1; i < sizeC; i++)
        {
            x = i << 1;
            for(j = 1; j < sizeC; j++)
            {
                y = j << 1;
                fine[x][y] = coarse[i][j]; 
            }
        }
        /* Update the fine points that are in the same columns as a coarse point in the grid */
        #pragma omp for private(j, x, y)
        for(i = 1; i < sizeF; i += 2){
            for(j = 2; j < sizeF; j += 2){
                fine[i][j] = (fine[i-1][j] + fine[i+1][j]) * 0.5; 
            }
        }
        /* Update the rest of the fine points in the grid. */
        #pragma omp for private(j, x, y)
        for(i = 1; i < sizeF; i++){
            for(j = 1; j < sizeF; j += 2){
                fine[i][j] = (fine[i][j-1] + fine[i][j+1]) * 0.5;
            }
        }
    }
}



int main(int argc, char const *argv[])
{
    int i,j, size1, size2, size3, size4;
    double maxdiff, **a1, **a2, **a3, **a4, **b1, **b2, **b3, **b4;
    
    /* initialize input variables */
    size1 = (argc > 1)? atoi(argv[1]) : MAXSIZE;
    iters = (argc > 2)? atoi(argv[2]) : MAXITERS;
    workers = (argc > 3)? atoi(argv[3]) : MAXWORKERS;
    if(size1 > MAXSIZE) size1 = MAXSIZE; 
    if(iters > MAXITERS) iters = MAXITERS;
    if(workers > MAXWORKERS) workers = MAXWORKERS;

    /* set number of workers */
    omp_set_num_threads(workers);

    /* calculate the sizes of all the grids */
    size2 = (size1 * 2) + 1;
    size3 = (size2 * 2) + 1;
    size4 = (size3 * 2) + 1;

    /* increment size by 2, the input of size only defines the requested size of the grid for the inner points
    to make room for outer boundary points on all four sides of the grid we have to add 2 to the size */
    size1 += 2;     
    size2 += 2;
    size3 += 2;
    size4 += 2;


    /* Allocate grids */
    a1 = malloc(size1*sizeof(double*));
    b1 = malloc(size1*sizeof(double*));

    a2 = malloc(size2*sizeof(double*));
    b2 = malloc(size2*sizeof(double*));

    a3 = malloc(size3*sizeof(double*));
    b3 = malloc(size3*sizeof(double*));

    a4 = malloc(size4*sizeof(double*));
    b4 = malloc(size4*sizeof(double*));
    
    /* More allocation of grids */
    for(i = 0; i < size1; i++){
        a1[i] = malloc(size1*sizeof(double));
        b1[i] = malloc(size1*sizeof(double));
    }
    for(i = 0; i < size2; i++){
        a2[i] = malloc(size2*sizeof(double));
        b2[i] = malloc(size2*sizeof(double));
    }
    for(i = 0; i < size3; i++){
        a3[i] = malloc(size3*sizeof(double));
        b3[i] = malloc(size3*sizeof(double));
    }
    for(i = 0; i < size4; i++){
        a4[i] = malloc(size4*sizeof(double));
        b4[i] = malloc(size4*sizeof(double));
    }
    /* init matrices */
    for(i = 0; i < size1; i++){
        for( j = 0; j < size1; j++)
        {
            /* condition for init with boundary points 
            outer boundary points are = 1 */
            if(i == 0 || j == 0 || i == size1-1 || j == size1-1){
                a1[i][j] = 1;    
                b1[i][j] = 1;
            }
            /* interior points are = 0 */
            else{
                a1[i][j] = 0;
                b1[i][j] = 0;
            }
        }
    }

    for(i = 0; i < size2; i++){
        for( j = 0; j < size2; j++)
        {
            /* condition for init with boundary points 
            outer boundary points are = 1 */
            if(i == 0 || j == 0 || i == size2-1 || j == size2-1){
                a2[i][j] = 1;    
                b2[i][j] = 1;
            }
                /* interior points are = 0 */
            else{
                a2[i][j] = 0;
                b2[i][j] = 0;
            }
        }
    }

    for(i = 0; i < size3; i++){
        for( j = 0; j < size3; j++)
        {
            /* condition for init with boundary points 
            outer boundary points are = 1 */
            if(i == 0 || j == 0 || i == size3-1 || j == size3-1){
                a3[i][j] = 1;    
                b3[i][j] = 1;
            }
            /* interior points are = 0 */
            else{
                a3[i][j] = 0;
                b3[i][j] = 0;
            }
        }
    }

    for(i = 0; i < size4; i++){
        for( j = 0; j < size4; j++)
        {
            /* condition for init with boundary points 
            outer boundary points are = 1 */
            if(i == 0 || j == 0 || i == size4-1 || j == size4-1){
                a4[i][j] = 1;    
                b4[i][j] = 1;
            }
            /* interior points are = 0 */
            else{
                a4[i][j] = 0;
                b4[i][j] = 0;
            }
        }
    }

    /* computational part, take start time */
    start_time = omp_get_wtime();

    /* begin V-Cycle at the top level, restrict down to the coarsest level
    while doing 4 jacobi iterations on each level. */
    jacobi(a4, b4, size4, 4);
    restriction(a4, a3, size3);

    jacobi(a3, b3, size3, 4);
    restriction(a3, a2, size2);

    jacobi(a2, b2, size2, 4);
    restriction(a2, a1, size1);

    /* Coarsest level reached. Perform iters number of jacobi iterations 
    defined as input argument, and start interpolating back up to the finest grain level. 
    One the way up, perform 4 jacobi iterations on each level. */
    jacobi(a1, b1, size1, iters);
    interpolation(a1, a2, size2, size1);

    jacobi(a2, b2, size2, 4);
    interpolation(a2, a3, size3, size2);

    jacobi(a3, b3, size3, 4);
    interpolation(a3, a4, size4, size3);

    jacobi(a4, b4, size4, 4);

    /* V-Cycle complete, calculate the Max difference error in the finest grids */
    maxDiff(a4, b4, size4);

    /* Computational part of program over, take the end time*/
    end_time = omp_get_wtime();


    print(a4, size4);
    printf("%d %d %d\t", size1-2, iters, workers);
    printf("%g\t", end_time - start_time);
    printf("%g\n", maxdiff);

    free(a1);
    free(a2);
    free(a3);
    free(a4);
    free(b1);
    free(b2);
    free(b3);
    free(b4);


    return 0;
}