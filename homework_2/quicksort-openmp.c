/* quick sort using OpenMP

   usage with gcc (version 4.2 or higher required):
     gcc -O -fopenmp -o quicksort-openmp quicksort-openmp.c 
     ./quicksort-openmp size numWorkers

*/

#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

#define MAXSIZE 100000  /* maximum matrix size */
#define MAXTHREADS 10   /* maximum number of workers */


double start_time, end_time; /* start and end times */
int array[MAXSIZE];
int size, counter, threads;

void *quicksort(void *);
void* swap(int a,int b);
int partition(int low, int high, int pivot);

typedef struct{
    int start;
    int end;
} info;

int main(int argc, char *argv[]){
    info data;

    int i;
    long l;

    size = (argc > 1)? atoi(argv[1]) : MAXSIZE;
    threads = (argc > 2)? atoi(argv[2]) : MAXTHREADS;
    if (size > MAXSIZE) size = MAXSIZE;
    if (threads > MAXTHREADS) threads = MAXTHREADS;
    
    omp_set_num_threads(threads);

    data.start = 0;
    data.end = size-1;
    for(i=0;i<size;i++){
        array[i] = rand()%99;
    }

    start_time = omp_get_wtime();
    #pragma omp parallel
    {
    #pragma omp single
    quicksort(&data);
    }
    


    end_time = omp_get_wtime();

    printf("Sorting %d elements with %d threads took %gs\n",size, threads, end_time-start_time);
    if(size < 200){
        for (i = 0; i < size; i++) {
	        printf(" %d", array[i]);
	        printf(",");
        }  
    }
}

void* quicksort(void *data){
    info *info_in = (info*) data;

    int left = info_in->start;
    int right = info_in->end;
    int pivot = (left+right)/2;

    if(left < right){
        pivot = partition(left,right,pivot);
        
        info info1;
        info1.start = left;
        info1.end = pivot-1;
        
        info info2;
        info2.start = pivot+1;
        info2.end = right;
        
        //#pragma omp parallel sections
        {
            #pragma omp task
            quicksort(&info1);

            #pragma omp task
            quicksort(&info2);
        }
    }
    


}
void* swap(int a,int b){
    int tmp = array[a];
    array[a] = array[b];
    array[b] = tmp;
}

int partition(int low, int high, int pivot){
    if(pivot != low)
        swap(low,high);

    pivot = low;
    low++;
    while(low <= high){
        if(array[low] <= array[pivot])
            low++;
        else if(array[high]> array[pivot])
            high--;
        else
            swap(low,high);
    }

    if(high != pivot)
        swap(pivot, high);

    return high;
}

   