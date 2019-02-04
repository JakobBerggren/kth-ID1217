/* matrix summation using pthreads

   features: uses a barrier; the Worker[0] computes
             the total sum from partial sums computed by Workers
             and prints the total sum to the standard output

   usage under Linux:
     gcc matrixSum.c -lpthread
     a.out size numWorkers

*/
#ifndef _REENTRANT 
#define _REENTRANT 
#endif 
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <sys/time.h>
#define MAXSIZE 10000  /* maximum matrix size */
#define MAXWORKERS 10   /* maximum number of workers */

pthread_mutex_t barrier;  /* mutex lock for the barrier */
pthread_mutex_t max, min, sum;  /* mutex lock for the max, min, sum */
pthread_mutex_t task;
pthread_cond_t go;        /* condition variable for leaving */
int numWorkers;           /* number of workers */ 
int numArrived = 0;       /* number who have arrived */
struct valuepos global_max, global_min; /* global max/min */
int global_sum;
int counter;

double start_time, end_time; /* start and end times */
int size, stripSize;  /* assume size is multiple of numWorkers */
int sums[MAXWORKERS]; /* partial sums */
int matrix[MAXSIZE][MAXSIZE]; /* matrix */

//struct valuepos maximum[MAXWORKERS]; /* maximum value */
//struct valuepos minimum[MAXWORKERS]; /* minimum value */

void *Worker(void *);

/* a reusable counter barrier */
void Barrier() {
  pthread_mutex_lock(&barrier);
  numArrived++;
  if (numArrived == numWorkers) {
    numArrived = 0;
    pthread_cond_broadcast(&go);
  } else
    pthread_cond_wait(&go, &barrier);
  pthread_mutex_unlock(&barrier);
}

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

/* struct to hold max/min value with position in matrix */
struct valuepos{
  int x, y, value;
};

void printSum(){
  end_time = read_timer();

    printf("The total is %d\n", global_sum);
    printf("The maximum is %d located at [%d,%d]\n", global_max.value, global_max.y, global_max.x);
    printf("The minimum is %d located at [%d,%d]\n", global_min.value, global_min.y, global_min.x);
    printf("The execution time is %g sec\n", end_time - start_time);
}

/* read command line, initialize, and create threads */
int main(int argc, char *argv[]) {
  int i, j;
  long l; /* use long in case of a 64-bit system */
  pthread_attr_t attr;
  pthread_t workerid[MAXWORKERS];

  /* set global thread attributes */
  pthread_attr_init(&attr);
  pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);

  /* initialize mutex and condition variable */
  pthread_mutex_init(&barrier, NULL);
  pthread_mutex_init(&max, NULL);
  pthread_mutex_init(&min, NULL);
  pthread_mutex_init(&sum, NULL);
  pthread_mutex_init(&task, NULL);
  pthread_cond_init(&go, NULL);

  /* read command line args if any */
  size = (argc > 1)? atoi(argv[1]) : MAXSIZE;
  numWorkers = (argc > 2)? atoi(argv[2]) : MAXWORKERS;
  if (size > MAXSIZE) size = MAXSIZE;
  if (numWorkers > MAXWORKERS) numWorkers = MAXWORKERS;
  stripSize = size/numWorkers;

  /* initialize the matrix */
  for (i = 0; i < size; i++) {
	  for (j = 0; j < size; j++) {
          matrix[i][j] = rand()%99;
	  }
  }
  global_max.y = 0;
  global_max.x = 0;
  global_max.value = matrix[0][0];

  global_min.y = 0;
  global_min.x = 0;
  global_min.value = matrix[0][0];

  /* print the matrix */
//#ifdef DEBUG
  for (i = 0; i < size; i++) {
	  printf("[ ");
	  for (j = 0; j < size; j++) {
	    printf(" %d", matrix[i][j]);
	  }
	  printf(" ]\n");
  }
//#endif

  /* do the parallel work: create the workers */
  start_time = read_timer();
  for (l = 0; l < numWorkers; l++)
    pthread_create(&workerid[l], &attr, Worker, (void *) l);
  
  /* Make sure every thread is done */
  for(l = 0; l < numWorkers; l++)
    pthread_join(workerid[l], NULL);

  printSum();

  pthread_exit(NULL);
}

void update_max(struct valuepos new){
  pthread_mutex_lock(&max);
  if(new.value > global_max.value)
    global_max = new;
  pthread_mutex_unlock(&max);
}
void update_min(struct valuepos new){
  pthread_mutex_lock(&min);
  if(new.value < global_min.value)
    global_min = new;
  pthread_mutex_unlock(&min);
}
void update_sum(int new){
  pthread_mutex_lock(&sum);
    global_sum += new;
  pthread_mutex_unlock(&sum);
}

int getTask(){
  if(counter < size){
    //counter ++;
    return counter++;
  }

  return -1;
}

/* Each worker sums the values in one strip of the matrix.
   After a barrier, worker(0) computes and prints the total */
void *Worker(void *arg) {
  long myid = (long) arg;
  int total, i, j, first, last;
  struct valuepos max, min;

#ifdef DEBUG
  printf("worker %d (pthread id %d) has started\n", myid, pthread_self());
#endif

  /* determine first and last rows of my strip */
  first = myid*stripSize;
  last = (myid == numWorkers - 1) ? (size - 1) : (first + stripSize - 1);



  /* sum values in my strip */
  total = 0;
  max.y = 0;
  max.x = 0;
  max.value = matrix[0][0];

  min.y = 0;
  min.x = 0;
  min.value = matrix[0][0];

  while(true){

    pthread_mutex_lock(&task);
    int taskval = getTask();
    pthread_mutex_unlock(&task);
    if(taskval == -1)
      break;

    for(int i=0;i<size;i++){
      total += matrix[taskval][i];

      if(matrix[taskval][i] > max.value){
        max.value = matrix[taskval][i];
        max.y = taskval;
        max.x = i;
      }
        
      if(matrix[taskval][i] < min.value){
        min.value = matrix[taskval][i];
        min.y = taskval;
        min.x = i;
      } 
    }
  }

  /* Used for 1st / 2nd
  for (i = first; i <= last; i++){
    for (j = 0; j < size; j++){
      total += matrix[i][j];
      if(matrix[i][j] > max.value){
        max.value = matrix[i][j];
        max.y = i;
        max.x = j;
      }
        
      if(matrix[i][j] < min.value){
        min.value = matrix[i][j];
        min.y = i;
        min.x = j;
      }
        
    }
  }
  */

  /* part 2 and 3 */
  if(max.value > global_max.value)
    update_max(max);
  if(min.value < global_min.value)
    update_min(min);
  
  update_sum(total);

  /* Sum using barrier and arrays
  sums[myid] = total;
  maximum[myid] = max;
  minimum[myid] = min;
  Barrier();  
  if (myid == 0) {
    total = 0;
    max = maximum[0];
    min = minimum[0];
    for (i = 0; i < numWorkers; i++){
      total += sums[i];
      if(maximum[i].value > max.value)
        max = maximum[i];
      if(minimum[i].value < min.value)
        min = minimum[i];
    }
    */
    /* get end time */
    //end_time = read_timer();
    /* print results */
    /*
    printf("The total is %d\n", total);
    printf("The maximum is %d located at [%d,%d]\n", max.value, max.y, max.x);
    printf("The minimum is %d located at [%d,%d]\n", min.value, min.y, min.x);
    printf("The execution time is %g sec\n", end_time - start_time);
    */
}
