/* A program to calculate jacobi matrices sequentially
    @Author Jakob Berggren, Oskar Hahr

    usage with gcc (version 4.2 or higher required):
        gcc -o jacobi_seq jacobi_seq.c
        ./jacobi_seq size iters

*/

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <sys/time.h>
#include <math.h>
#include <limits.h>

/* MAX for: number for grid size and number of iterations */
#define MAXSIZE 1000
#define MAXITERS 1000000


int size, iters, workers;
double start_time, end_time;
FILE* output;

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
/* Function for calculating the max difference error between grids a & b */
double maxDiff(double** a, double** b){
    int i, j;
    double maxdiff;
    double temp;

    maxdiff = 0.0;
    
    for(i = 0; i < size; i++)
    {
        for(j = 0; j < size; j++){

            temp = a[i][j] - b[i][j];
            if(temp < 0)
                temp = -temp;
            if(temp > maxdiff)
                maxdiff =temp;
        }
    }
    return maxdiff;
}


/* An iterative Jacobi Method function that iterates over the grids a & b updating their values
over a number of iterations specified as input data. 
*/
void jacobi(double** a, double** b){
    int i, j, count;
    int interiorSize = size - 1;
    for(count = 0; count < iters; count++)
    {
        for(i = 1; i < interiorSize; i++){
            for(j = 1; j < interiorSize; j++){
                b[i][j] = (a[i-1][j] + a[i+1][j] + a[i][j-1] +a[i][j+1]) *0.25;
            }   
        }

        for(i = 1; i < interiorSize; i++){
            for(j = 1; j < interiorSize; j++){
                a[i][j] = (b[i-1][j] + b[i+1][j] + b[i][j-1] +b[i][j+1]) * 0.25;
            }   
        }   
    }   
}

/* Function for printing the results of a grid to the output file */
void print(double** a){
    int i,j;
    output = fopen("jacobi_seq_matrix.txt","w");
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
    double maxdiff;

    /* initialize input values*/
    size = (argc > 1)? atoi(argv[1]) : MAXSIZE;
    iters = (argc > 2)? atoi(argv[2]) : MAXITERS;
    if(size > MAXSIZE) size = MAXSIZE; 
    if(iters > MAXITERS) iters = MAXITERS;


    /* The specified input variable size is defined as the size of the interior grid,
    add 2 to this size to retrieve the total size of the grid including outer boundary points 
    */
    size += 2;      

    /* Allocate memory for the grids */
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
            outer boundary points are = 1 
            */
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
    start_time = read_timer();
    /* Jacobi iteration between a & b*/
    jacobi(a, b);
    /* Calculate max difference error between a & b*/
    maxdiff = maxDiff(a,b);
    /* End of computational part, read the end time */
    end_time = read_timer();

    print(a);
    printf("%d %d\t", size-2, iters);
    printf("%g\t", end_time - start_time);
    printf("%g\n", maxdiff);

    free(a);
    free(b);
    return 0;
}