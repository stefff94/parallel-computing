#include <iostream>
#include "utility.h"
#include <omp.h>

int main(int argc, char* argv[]) {

    std::unordered_map<std::string, std::string> arguments;

    for (int i = 1; i < argc; ++i) {
        std::string parameter = argv[i];
        int position = parameter.find('=');

        arguments.insert(std::pair<std::string, std::string>(
                parameter.substr(0, position),
                parameter.substr(position+1, parameter.length())
        ));
    }

    // number of elements
    int elementsNo = 100000;
    if (arguments.find(ELEMENTS_NO) != arguments.end()) {
        elementsNo = std::stoi(arguments[ELEMENTS_NO]);
    }

    // number of clusters
    int clustersNo = 6;
    if (arguments.find(CLUSTERS_NO) != arguments.end()) {
        clustersNo = std::stoi(arguments[CLUSTERS_NO]);
    }

    // number of threads
    int threadsNo = 6;
    if (arguments.find(THREADS_NO) != arguments.end()) {
        threadsNo = std::stoi(arguments[THREADS_NO]);
    }

    std::vector<Cluster> clusters;
    std::vector<Element> elements;

    // choose initialization type
    bool useFileElements = false;
    if (arguments.find(USE_FILE_ELEMENS) != arguments.end()) {
        std::string s = arguments[USE_FILE_ELEMENS];
        std::transform(s.begin(), s.end(), s.begin(), ::tolower);
        useFileElements = (s == "true") ;
    }
    if (!useFileElements) {
        // initializing with random values
        initClusters(clusters, clustersNo);
        initElements(elements, elementsNo);
    } else {
        // initialize with file elements.txt
        elementsNo = 100000;
        clustersNo = 4;

        initWithFileElements(elementsNo, clustersNo, "elements.txt", elements, clusters);
    }

#ifdef _OPENMP
    std::cout << "_OPENMP defined" << std::endl;
    std::cout << "Num processors (Phys+HT): " << omp_get_num_procs() << std::endl;
    omp_set_num_threads(threadsNo);
#endif

    long long iterations = 0;
    bool hasChanged = true;

    auto t1 = std::chrono::high_resolution_clock::now();
#pragma omp parallel reduction(+:iterations)
    {
        while(hasChanged) {
#pragma omp barrier
#pragma omp single nowait
        {
            hasChanged = false;
        };
            iterations ++;
            // First k-means step
#pragma omp for
            for (int i = 0; i < elementsNo; i++) {
                int cluster_id = clusters[0].index;
                double min_distance = getDistance(elements[i], clusters[0]);
                for (int j = 1; j < clustersNo; j++) {
                    double distance = getDistance(elements[i], clusters[j]);
                    if (distance < min_distance) {
                        min_distance = distance;
                        cluster_id = clusters[j].index;
                    }
                }
                elements[i].clusterId = cluster_id;
            }
#pragma omp barrier
            // Second k-means step
#pragma omp for
            for (int i = 0; i < clustersNo; i++) {
                clusters[i].newCentroid_x = 0;
                clusters[i].newCentroid_y = 0;
                clusters[i].elementNo = 0;
                for (int j = 0; j < elementsNo; j++) {
                    if (elements[j].clusterId == clusters[i].index) {
                        clusters[i].newCentroid_x += elements[j].x;
                        clusters[i].newCentroid_y += elements[j].y;
                        clusters[i].elementNo ++;
                    }
                }
                if (clusters[i].elementNo > 0) {
                    clusters[i].newCentroid_x = clusters[i].newCentroid_x / clusters[i].elementNo;
                    clusters[i].newCentroid_y = clusters[i].newCentroid_y / clusters[i].elementNo;
                }
                if (clusters[i].centroid_x != clusters[i].newCentroid_x || clusters[i].centroid_y != clusters[i].newCentroid_y) {
                    clusters[i].centroid_x = clusters[i].newCentroid_x;
                    clusters[i].centroid_y = clusters[i].newCentroid_y;
#pragma omp flush(hasChanged)
#pragma omp atomic write
                    hasChanged = true;
#pragma omp flush(hasChanged)
                }
            }
        }
    }
    auto t2 = std::chrono::high_resolution_clock::now();

    printf("Iterations: %lld\n", iterations);

    int tot=0;
    for(int i = 0; i< clustersNo; i++) {
        printf("Cluster: %d - Centroid: (%f,%f) - #Elements: %d\n", clusters[i].index, clusters[i].centroid_x, clusters[i].centroid_y, clusters[i].elementNo);
        tot+=clusters[i].elementNo;
    }
    printf("Tot: %d\n", tot);

    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(t2-t1).count()
              << " milliseconds\n";

    /*std::ofstream myfile;
    myfile.open ("elements-result.txt");

    for (int i = 0; i < elementsNo; i++) {
        myfile << elements[i].x << " " << elements[i].y << " " << elements[i].clusterId << std::endl;
    }

    myfile.close();

    myfile.open("clusters-result.txt");

    for (int i = 0; i < clustersNo; i++) {
        myfile << clusters[i].index << " " << clusters[i].centroid_x << " " << clusters[i].centroid_y << std::endl;
    }
    myfile.close();*/

return 0;

}


