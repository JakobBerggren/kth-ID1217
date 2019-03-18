/* A program to calculate multigrid jacobi in parallel
    @Author Jakob Berggren, Oskar Hahr
    usage with gcc (version 4.2 or higher required):
        gcc -O -fopenmp -o multigrid_parallel multigrid_parallel.c
        ./multigrid_parallel size iters workers
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
#define MAXITERS 100000000    
#define MAXWORKERS 10


int size, iters, workers;
double start_time, end_time, maxdiff = 0.0;
FILE * output;

double maxDiff(double** a, double** b, int interiorSize){
    int i, j;
    double temp;

    #pragma omp parallel for private (j, temp)
    for(i = 1; i < interiorSize; i++)
    {
        for(j = 1; j < interiorSize; j++){

            temp = a[i][j] - b[i][j];
            if(temp < 0)
                temp = -temp;
            if(temp > maxdiff){
                #pragma omp critical
                if(temp > maxdiff)
                    maxdiff = temp;
            }
        }
    }
    return maxdiff;
}

void jacobi(double** a, double** b, int size, int it){
    int i, j, count, interiorSize;
    printf("hello \n");
    interiorSize =size - 1;
    for(count = 0; count < it; count++)
    {
        #pragma omp for private(j)
        for(i = 1; i < interiorSize; i++){
            for(j = 1; j < interiorSize; j++){
                b[i][j] = (a[i-1][j] + a[i+1][j] + a[i][j-1] +a[i][j+1])*0.25;
            }   
        }

        #pragma omp for private(j)
        for(i = 1; i < interiorSize - 1; i++){
            for(j = 1; j < interiorSize - 1; j++){
                a[i][j] = (b[i-1][j] + b[i+1][j] + b[i][j-1] +b[i][j+1])*0.25;
            }   
        }   
    }   
}

void print(double** a, int s){
    int i,j;
    output = fopen("../result/multigrid_parallel_matrix.txt","w");

    for(i = 0; i < s; i++){
        for(j = 0; j < s; j++){
            fprintf(output,"%g, ",a[i][j]);
        }
        fprintf(output,"\n");
    }
    fclose(output);
}


void restriction(double** fine, double** coarse, int sizeCoarse){        
    int i, j, x, y, interiorC;
    interiorC = sizeCoarse - 1;

    #pragma omp parallel for private(j, x, y)
    for(i = 1; i < interiorC; i++)
    {
        x = i << 1;
        for(j = 1; j < interiorC; j++)
        {
            y = j << 1;
            coarse[i][j] = fine[x][y]*0.5 + (fine[x-1][y] + fine[x][y-1] + fine[x][y + 1] + fine[x + 1][y]) * 0.125;
        }
    }
}

void interpolation(double** coarse, double** fine, int sizeCoarse, int sizeFine){
    int i, j, x, y, interiorC, interiorF;
    interiorC = sizeCoarse -1;
    interiorF = sizeFine - 1;

    #pragma omp parallel for private (j, x, y)
    for(i = 1; i < interiorC; i++)
    {
        x = i << 1;
        for(j = 1; j < interiorC; j++)
        {
            y = j << 1;
            fine[x][y] = coarse[i][j]; 
        }
    }
    #pragma omp parallel for private (j)
    for(i = 1; i < interiorF; i += 2){
        for(j = 2; j < interiorF; j += 2){
            fine[i][j] = (fine[i-1][j] + fine[i+1][j]) * 0.5; 
        }
    }
    #pragma omp parallel for private (j)
    for(i = 1; i < interiorF; i++){
        for(j = 1; j < interiorF; j += 2){
            fine[i][j] = (fine[i][j-1] + fine[i][j+1]) * 0.5;
        }
    }
}


int main(int argc, char const *argv[])
{
    int i,j, size1, size2, size3, size4;
    double maxdiff;
    double** a1, ** b1, ** a2, ** b2, ** a3, ** b3, ** a4, ** b4;
    size = (argc > 1)? atoi(argv[1]) : MAXSIZE;
    iters = (argc > 2)? atoi(argv[2]) : MAXITERS;
    workers = (argc > 3)? atoi(argv[3]) : MAXWORKERS;
    if(size > MAXSIZE) size = MAXSIZE; 
    if(iters > MAXITERS) iters = MAXITERS;
    if(workers > MAXWORKERS) workers = MAXWORKERS;

    omp_set_num_threads(workers);


    /* define the inner grid sizes of the four different grids in the V-cycle */
    size1 = size;        
    size2 = (size1 * 2) + 1;
    size3 = (size2 * 2) + 1;
    size4 = (size3 * 2) + 1;

    /* The sizes of the interior points range from 1 : n, increment the input grid size by one to enable this */
    size1 += 2;
    size2 += 2;
    size3 += 2;
    size4 += 2;

    /* Allocate & intitialize the matrices to represent the grids,
    the grid sizes are incremented again to make room for boundary points on all 4 sides of the grids
    */
    a1 = malloc(size1*sizeof(double*));
    b1 = malloc(size1*sizeof(double*));

    a2 = malloc(size2*sizeof(double*));
    b2 = malloc(size2*sizeof(double*));

    a3 = malloc(size3*sizeof(double*));
    b3 = malloc(size3*sizeof(double*));

    a4 = malloc(size4*sizeof(double*));
    b4 = malloc(size4*sizeof(double*));

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

    /* initiate the values of the matrices for level 1 */
    for(i = 0; i < size1; i++){
        for( j = 0; j < size1; j++)
        {
            /* condition for init with boundary points */
            if(i == 0 || j == 0 || i == size1-1 || j == size1-1){
                a1[i][j] = 1;    
                b1[i][j] = 1;
            }
            /* interior points */
            else{
                a1[i][j] = 0;
                b1[i][j] = 0;
            }
        }
    }
    /* initiate the values of the matrices for level 2 */
    for(i = 0; i < size2; i++){
        for( j = 0; j < size2; j++)
        {
            /* condition for init with boundary points */
            if(i == 0 || j == 0 || i == size2-1 || j == size2-1){
                a2[i][j] = 1;    
                b2[i][j] = 1;
            }
            /* interior points */
            else{
                a2[i][j] = 0;
                b2[i][j] = 0;
            }
        }
    }
    /* initiate the values of the matrices for level 3 */
    for(i = 0; i < size3; i++){
        for( j = 0; j < size3; j++)
        {
            /* condition for init with boundary points */
            if(i == 0 || j == 0 || i == size3-1 || j == size3-1){
                a3[i][j] = 1;    
                b3[i][j] = 1;
            }
            /* interior points */
            else{
                a3[i][j] = 0;
                b3[i][j] = 0;
            }
        }
    }
    /* initiate the values of the matrices for level 4 */
    for(i = 0; i < size4; i++){
        for( j = 0; j < size4; j++)
        {
            /* condition for init with boundary points */
            if(i == 0 || j == 0 || i == size4-1 || j == size4-1){
                a4[i][j] = 1;    
                b4[i][j] = 1;
            }
            /* interior points */
            else{
                a4[i][j] = 0;
                b4[i][j] = 0;
            }
        }
    }   

    /* start_time is recorded as the V-cycle starts */
    start_time = omp_get_wtime();
    /* Perform four jacobi iterations on the 4th level (finest) & restrict to coarser grain*/

        
            jacobi(a4, b4, size4, 4);
            restriction(a4, a3, size3);

            /* Perform four jacobi iterations on the 3rd level & restrict to coarser grain*/
            jacobi(a3, b3, size3, 4);
            restriction(a3, a2, size2);

            /* Perform four jacobi iterations on the 2nd level & restrict to coarser grain*/
            jacobi(a2, b2, size2, 4);
            restriction(a2, a1, size1);

            /* Perform the input number of jacobi iterations on coarsest level & interpolate to finer grain */
            jacobi(a1, b1, size1, iters);
            interpolation(a1, a2, size1, size2);

            /* Perform four jacobi iterations on the 2nd level level & interpolate to finer grain */
            jacobi(a2, b2, size2, 4);
            interpolation(a2, a3, size2, size3);

            /* Perform four jacobi iterations on the 3rd level level & interpolate to finer grain */
            jacobi(a3, b3, size3, 4);
            interpolation(a3, a4, size3, size4);

            /* perform the last four jacobi iterations on the finest grained level and take the maxdiff */
            jacobi(a4, b4, size4, 4);
            maxdiff = maxDiff(a4, b4, size4);

    end_time = omp_get_wtime();

    //printf("size4 : %d\n", size4);

    //print(a4, size4);
    printf("Size: %d, Iterations: %d, Number of Workers: %d,\t", size, iters, workers);
    printf("Runtime was: %gs,\t", end_time - start_time);
    printf("Maximum error: %g\n", maxdiff);
    
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