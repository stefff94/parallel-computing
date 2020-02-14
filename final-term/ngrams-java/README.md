# n-grams calculation with Java

This project contains both a sequential and a parallel versions.

### Following steps show how to run the applications by command line

#### First clone the project and move to main directory. 

### Run Sequential version

(lines parameter should be: 10,000 - 100,000 - 1,000,000 - 6,000,000)

```
cd out/production/ngrams-java

java it.sv.ngrams.sequential.Application ngrams=2 lines=100,000   
```

### Run Parallel version

```
cd out/production/ngrams-java

java it.sv.ngrams.parallel.Application ngrams=2 lines=100,000 producersNo=2 consumersNo=6
```