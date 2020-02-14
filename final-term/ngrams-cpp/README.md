# n-grams calculation with Cpp

This project contains both a sequential and a parallel versions.

### Following steps show how to run the applications by command line

#### First clone the project and move to main directory. 

### Run Sequential version

(lines parameter should be: 10,000 - 100,000 - 1,000,000 - 6,000,000)

```
./ngrams_cpp_sequential n_grams=2 lines=100,000   
```

### Run Parallel version

```
./ngrams_cpp_parallel n_grams=2 lines=100,000 producers_no=2 consumers_no=6
```