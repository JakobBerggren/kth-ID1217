# matrixSum.c

## How to run:
**usage with gcc (version 4.2 or higher required):**  
gcc -O -fopenmp -o matrixSum-openmp matrixSum-openmp.c   
./matrixSum-openmp size numWorkers


## Performance

### 1 process
1000 - 1.49409ms  
5000 - 32.7661ms  
10000 - 129.825ms  

### 2 processes
1000 - .964473ms    - 54% speedup  
5000 - 17.465ms     - 88% speedup  
10000 - 67.5362ms   - 92% speedup  

### 3 processes
1000 - 1.08999ms    - 37% speedup   
5000 - 22.3726ms    - 46% speedup  
10000 - 84.1448ms   - 54% speedup  


### 4 processes
1000 - 3.90922ms    - no speedup  
5000 - 24.189ms     - 35% speedup  
10000 - 69.5116ms   . 87% speedup  

## Discussion of result

At smaller matrices it may not be worth it to use extra threads due to overhead cost. Larger matrices allows for greater use of processes.
Disturbance when running may be the reason for discrepancies.




# quicksort.c

## How to run: 
**usage with gcc (version 4.2 or higher required):**  
gcc -O -fopenmp -o quicksort-openmp quicksort-openmp.c   
./quicksort-openmp size numWorkers

## Performance

### 1 process
1000 - 4.29463ms  
10000 - 42.1837ms  
100000 - 222.456ms  

### 2 processes
1000 - 4.37952ms  
10000 - 68.2656ms  
100000 - 239.342ms  

### 3 processes
1000 - 4.76656ms  
10000 - 85.0897ms  
100000 - 246.666ms  


### 4 processes
1000 - 5.17337ms  
10000 - 99.4739ms  
100000 - 246.196ms  

### Discussion of result

It is clear that no speedup is achieved with this implementation. I believe the reason for that is the lack of a threshold for when the number of elements is so small that it is unneccessary to do the sorting in parallel.