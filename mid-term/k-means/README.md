# k-means implementation with C++ and OpenMP

This project contains a sequential version written in C+ and two parallel versions implemented with C++ and OpenMP.

### Following steps show how to run the applications by command line

#### First clone the project and move to main directory. 

### Run Sequential version

```
./k-means-sequential elements_no=100000 clusters_no=4   
```

### Run Parallel (AoS) version

```
./k-means-parallel-array-of-structures elements_no=100000 clusters_no=4 threads_no=6
```

### Run Parallel (SoA) version

```
./k-means-parallel-structure-of-arrays elements_no=100000 clusters_no=4 threads_no=6
```
