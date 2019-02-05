#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <sys/time.h>

#define MAXSIZE 10000  /* maximum matrix size */
#define MAXTHREADS 10   /* maximum number of workers */


double start_time, end_time; /* start and end times */
int array[MAXSIZE];
int size, counter, threads;

pthread_mutex_t count;

void *quicksort(void *);
void* swap(int a,int b);
int partition(int low, int high, int pivot);

typedef struct{
    int start;
    int end;
} info;


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

int main(int argc, char *argv[]){
    pthread_attr_t attr;
    info data;

    int i;
    long l;
    
    pthread_attr_init(&attr);
    pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);

    size = (argc > 1)? atoi(argv[1]) : MAXSIZE;
    threads = (argc > 2)? atoi(argv[2]) : MAXTHREADS;
    if (size > MAXSIZE) size = MAXSIZE;
    if (threads > MAXTHREADS) threads = MAXTHREADS;

    data.start = 0;
    data.end = size-1;
    for(i=0;i<size;i++){
        array[i] = rand()%99;
    }

    start_time = read_timer();
    quicksort(&data);
    end_time =read_timer();

    printf("Sorting %d elements with %d threads took %gs\n",size, threads, end_time-start_time);
    if(size < 200){
        for (i = 0; i < size; i++) {
	        printf(" %d", array[i]);
	        printf(",");
        }  
    }

    pthread_exit(NULL);
}

void* quicksort(void *data){
    info *info_in = (info*) data;
    pthread_t new_thread;

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
        
        pthread_mutex_lock(&count);
        if(counter < MAXTHREADS){
            counter++;
            pthread_mutex_unlock(&count);

            pthread_create(&new_thread, NULL, quicksort, &info1);
            quicksort(&info2);
            pthread_join(new_thread,NULL);

            pthread_mutex_lock(&count);
            counter--;
            pthread_mutex_unlock(&count);
        }
        else{
            pthread_mutex_unlock(&count);
            quicksort(&info1);
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

   