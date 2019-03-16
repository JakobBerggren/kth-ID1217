/* A program to calculate jacobi matrices sequentially
    @Author Jakob Berggren, Oskar Hahr

    usage with gcc (version 4.2 or higher required):
        gcc -o jacobi_seq jacobi_seq.c
        ./jacobi_seq size iters workers

*/

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <sys/time.h>
#include <math.h>
#include <limits.h>


#define MAXSIZE 1000
#define MAXITERS 100
#define MAXWORKERS 1

#define max(a,b) \
({__typeof__ (a) _a = (a); \
__typeof__ (b) _b = (b); \
_a > _b ? _a : _b; })

int size, iters, workers;
double start_time, end_time;

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

void jacobi(double** a, double** b){
    int i, j, count;
    int interiorSize = size - 1;
    for(count = 0; count < iters*0.5; count++)
    {
        for(i = 1; i < interiorSize; i++){
            for(j = 1; j < interiorSize; j++){
                b[i][j] = (a[i-1][j] + a[i+1][j] + a[i][j-1] +a[i][j+1]);
            }   
        }

        for(i = 1; i < interiorSize; i++){
            for(j = 1; j < interiorSize; j++){
                a[i][j] = (b[i-1][j] + b[i+1][j] + b[i][j-1] +b[i][j+1])*0.0625;
            }   
        }   
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
    double maxdiff;
    size = (argc > 1)? atoi(argv[1]) : MAXSIZE;
    iters = (argc > 2)? atoi(argv[2]) : MAXITERS;
    workers = (argc > 3)? atoi(argv[3]) : MAXWORKERS;
    if(size > MAXSIZE) size = MAXSIZE; 
    if(iters > MAXITERS) iters = MAXITERS;
    if(workers > MAXWORKERS) workers = MAXWORKERS;

    size += 2;      // makes room for boundary points :)  


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

    start_time = read_timer();
    jacobi(a, b);
    maxdiff = maxDiff(a,b);
    end_time = read_timer();


    print(a, b,maxdiff);

    return 0;
}
