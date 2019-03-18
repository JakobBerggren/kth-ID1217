/* A program to calculate multigrid jacobi matrices sequentially
    @Author Jakob Berggren, Oskar Hahr

    usage with gcc (version 4.2 or higher required):
        gcc -o multigrid_seq multigrid_seq.c
        ./multigrid_seq size iters

*/

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <sys/time.h>
#include <math.h>
#include <limits.h>

/* MAX number for grid size and max number of iterations*/
#define MAXSIZE 1000
#define MAXITERS 10000000

int iters;
double start_time, end_time;
FILE * output;

/* timer */
double read_timer() {
    static bool initialized = false;
    static struct timeval start;
    struct timeval end;
    if( !initialized )
    {
        gettimeofday( &start, NULL );
        initialized = true;
    }
    gettimeofday( &end, NULL );
    return (end.tv_sec - start.tv_sec) + 1.0e-6 * (end.tv_usec - start.tv_usec);
}
/* Function for calculating the max difference between grids a & b with the size of size */
double maxDiff(double** a, double** b, int size){
    int i, j;
    double maxdiff;
    double temp;

    maxdiff = 0.0;
    for(i = 0; i < size; i++)
    {
        for(j = 0; j < size; j++){

            temp = a[i][j] - b[i][j];
            /* If value of (a - b) is negative, flip it */
            if(temp < 0)
                temp = -temp;
            /* If value of maxdiff is smaller than new value, update maxdiff */
            if(temp > maxdiff)
                maxdiff =temp;
        }
    }
    return maxdiff;
}
/* An iterative Jacobi Method function that iterates over the grids a & b updating their values
over a number of iterations. */

void jacobi(double** a, double** b, int size, int iterations){
    int i, j, count;
    int interiorSize = size - 1;
    
    for(count = 0; count < iterations; count++)
    {
        /* First for loop to calculate new values of grid b */
        for(i = 1; i < interiorSize; i++){
            for(j = 1; j < interiorSize; j++){
                b[i][j] = (a[i-1][j] + a[i+1][j] + a[i][j-1] +a[i][j+1]) * 0.25;
            }   
        }
        /* Second for loop to calculate new values of grid a */
        for(i = 1; i < interiorSize; i++){
            for(j = 1; j < interiorSize; j++){
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

/* The restriction function projects the values of a fine grid onto a coarse grid. 
This function is used to down a level in the V-Cycle */
void restriction(double** fine, double** coarse, int size){        
    int i, j, x, y;
    int sizeC = size-1;
    /* iterate over the coarse matrix, mapping has a 1:2 relation between the coarse matrix to the fine matrix in regards to i,j : x,y */
    for(i = 1; i < sizeC; i++)
    {
        x = i << 1;
        for(j = 1; j < sizeC; j++)
        {
            y = j << 1;
            /* coarse value gets part of its value from its direct fire grain mapping, and the rest from the neighbours of the fine grained mapping. */
            coarse[i][j] = fine[x][y]*0.5 + (fine[x-1][y] + fine[x][y-1] + fine[x][y + 1] + fine[x + 1][y]) * 0.125;
        }
    }
}
/* interpolation is used to project values from a coarse grid onto a fine grid 
This function is called to move up a level in the V-Cycle */
void interpolation(double** coarse, double** fine, int sizeFine, int sizeCoarse){
    
    int i, j, x, y;
    int sizeF = sizeFine - 1;
    int sizeC = sizeCoarse - 1;

    /* Update the fine points that directly map to a coarse point in the grid */
    for(i = 1; i < sizeC; i++)
    {
        x = i << 1;
        for(j = 1; j < sizeC; j++)
        {
            y = j << 1;
            /* coarse point directly maps to fine grain point */
            fine[x][y] = coarse[i][j]; 
        }
    }
    /* Update the fine points that are in the same columns as a coarse point in the grid */
    for(i = 1; i < sizeF; i += 2){
        for(j = 2; j < sizeF; j += 2){
            fine[i][j] = (fine[i-1][j] + fine[i+1][j]) * 0.5; 
        }
    }
    /* Update the rest of the fine points in the grid. */
    for(i = 1; i < sizeF; i++){
        for(j = 1; j < sizeF; j += 2){
            fine[i][j] = (fine[i][j-1] + fine[i][j+1]) * 0.5;
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
    if(size1 > MAXSIZE) size1 = MAXSIZE; 
    if(iters > MAXITERS) iters = MAXITERS;

    /* calculate the sizes of all the grids */
    size2 = (size1 * 2) + 1;
    size3 = (size2 * 2) + 1;
    size4 = (size3 * 2) + 1;


    size1 += 2;     
    size2 += 2;
    size3 += 2;
    size4 += 2;

    /* Allocate memory for  grids */

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
    start_time = read_timer();

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
    end_time = read_timer();
    
    print(a4, size4);
    printf("%d %d\t", size1-2, iters);
    printf("%g\t", end_time - start_time);
    printf("%g\n", maxdiff);

    return 0;
}