#include <iostream>
#include "utility.h"
#include <omp.h>

int main(int argc, char* argv[])
{
    std::unordered_map<std::string, std::string> arguments;

    for (int i = 1; i < argc; ++i)
    {
        std::string parameter = argv[i];
        int position = parameter.find('=');

        arguments.insert(std::pair<std::string, std::string>(
                parameter.substr(0, position),
                parameter.substr(position+1, parameter.length())
        ));
    }

    // number of elements
    int elements_no = 10000;
    if (arguments.find(ELEMENTS_NO) != arguments.end())
    {
        elements_no = std::stoi(arguments[ELEMENTS_NO]);
    }

    // number of clusters
    int clusters_no = 4;
    if (arguments.find(CLUSTERS_NO) != arguments.end())
    {
        clusters_no = std::stoi(arguments[CLUSTERS_NO]);
    }

    // number of threads
    int threads_no = 4;
    if (arguments.find(THREADS_NO) != arguments.end())
    {
        threads_no = std::stoi(arguments[THREADS_NO]);
    }

    std::vector<Cluster> clusters;
    std::vector<Element> elements;

    // choose initialization type
    bool use_file_elements = false;
    if (arguments.find(USE_FILE_ELEMENS) != arguments.end())
    {
        std::string s = arguments[USE_FILE_ELEMENS];
        std::transform(s.begin(), s.end(), s.begin(), ::tolower);
        use_file_elements = (s == "true") ;
    }
    if (!use_file_elements)
    {
        // initializing with random values
        init_clusters(clusters, clusters_no);
        init_elements(elements, elements_no);
    } else {
        // initialize with file elements.txt
        elements_no = 100000;
        clusters_no = 4;

        init_with_file_elements(elements_no, clusters_no, "elements.txt", elements, clusters);
    }

    #ifdef _OPENMP
    std::cout << "_OPENMP defined" << std::endl;
    std::cout << "Num processors (Phys+HT): " << omp_get_num_procs() << std::endl;
    omp_set_num_threads(threads_no);
    #endif

    bool has_changed = true;

    auto t1 = std::chrono::high_resolution_clock::now();
    #pragma omp parallel default(none) shared(has_changed, elements, clusters) firstprivate(elements_no, clusters_no)
    {
        while(has_changed) {
        #pragma omp barrier
        #pragma omp single nowait
        {
            has_changed = false;
        }
            // First k-means step
            #pragma omp for
            for (int i = 0; i < elements_no; i++)
            {
                int cluster_id = clusters[0].index;
                double min_distance = get_distance(elements[i], clusters[0]);
                for (int j = 1; j < clusters_no; j++)
                {
                    double distance = get_distance(elements[i], clusters[j]);
                    if (distance < min_distance)
                    {
                        min_distance = distance;
                        cluster_id = clusters[j].index;
                    }
                }
                elements[i].cluster_id = cluster_id;
            }
            #pragma omp barrier
            // Second k-means step
            #pragma omp for
            for (int i = 0; i < clusters_no; i++)
            {
                clusters[i].new_centroid_x = 0;
                clusters[i].new_centroid_y = 0;
                clusters[i].elements_no = 0;
                for (int j = 0; j < elements_no; j++)
                {
                    if (elements[j].cluster_id == clusters[i].index)
                    {
                        clusters[i].new_centroid_x += elements[j].x;
                        clusters[i].new_centroid_y += elements[j].y;
                        clusters[i].elements_no ++;
                    }
                }
                if (clusters[i].elements_no > 0)
                {
                    clusters[i].new_centroid_x = clusters[i].new_centroid_x / clusters[i].elements_no;
                    clusters[i].new_centroid_y = clusters[i].new_centroid_y / clusters[i].elements_no;
                }
                if (clusters[i].centroid_x != clusters[i].new_centroid_x
                    || clusters[i].centroid_y != clusters[i].new_centroid_y)
                {
                    clusters[i].centroid_x = clusters[i].new_centroid_x;
                    clusters[i].centroid_y = clusters[i].new_centroid_y;
                    #pragma omp flush(has_changed)
                    #pragma omp atomic write
                    has_changed = true;
                    #pragma omp flush(has_changed)
                }
            }
        }
    }
    auto t2 = std::chrono::high_resolution_clock::now();

    std::cout   << "\nParallel execution time: "
                << std::chrono::duration_cast<std::chrono::milliseconds>(t2-t1).count()
                << " milliseconds\n" << std::endl;

    int tot = 0;
    for(int i = 0; i < clusters_no; i++) {
        printf("Cluster: %d - Centroid: (%f,%f) - #Elements: %d\n",
                clusters[i].index, clusters[i].centroid_x, clusters[i].centroid_y, clusters[i].elements_no);
        tot += clusters[i].elements_no;
    }
    printf("\nTot elements: %d\n", tot);

    /*std::ofstream myfile;
    myfile.open ("elements-result.txt");

    for (int i = 0; i < elements_no; i++) {
        myfile << elements[i].x << " " << elements[i].y << " " << elements[i].cluster_id << std::endl;
    }

    myfile.close();

    myfile.open("clusters-result.txt");

    for (int i = 0; i < clusters_no; i++) {
        myfile << clusters[i].index << " " << clusters[i].centroid_x << " " << clusters[i].centroid_y << std::endl;
    }
    myfile.close();*/

    return 0;

}
