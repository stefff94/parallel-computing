#include <iostream>
#include "utility-v2.h"
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

    Elements* elements = new Elements(elements_no);
    Clusters* clusters = new Clusters(clusters_no);

    // number of threads
    int threads_no = 4;
    if (arguments.find(THREADS_NO) != arguments.end())
    {
        threads_no = std::stoi(arguments[THREADS_NO]);
    }

    // choose initialization type
    bool use_file_elements = false;
    if (arguments.find(USE_FILE_ELEMENS) != arguments.end())
    {
        std::string s = arguments[USE_FILE_ELEMENS];
        std::transform(s.begin(), s.end(), s.begin(), ::tolower);
        use_file_elements = (s == "true");
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

        elements = new Elements(elements_no);
        clusters = new Clusters(clusters_no);

        init_with_file_elements(elements_no, clusters_no, "elements.txt", elements, clusters);
    }

    #ifdef _OPENMP
    std::cout << "_OPENMP defined" << std::endl;
    std::cout << "Num processors (Phys+HT): " << omp_get_num_procs() << std::endl;
    omp_set_num_threads(threads_no);
    #endif

    bool has_changed = true;

    auto t1 = std::chrono::high_resolution_clock::now();

    #pragma omp parallel default(none) shared(clusters, has_changed) firstprivate(elements, clusters_no, elements_no)
    {
        while(has_changed)
        {
            #pragma omp barrier
            #pragma omp single nowait
            {
                has_changed = false;
            }
            double new_centroid_x[clusters_no];
            double new_centroid_y[clusters_no];
            int c_elements[clusters_no];
            for (int i = 0; i < clusters_no; ++i)
            {
                new_centroid_x[i] = 0;
                new_centroid_y[i] = 0;
                c_elements[i] = 0;
            }

            #pragma omp for
            for (int i = 0; i < elements_no; i++)
            {
                int cluster_id = 0;
                double min_distance = get_distance(
                        elements->x[i], elements->y[i],
                        clusters->centroid_x[0], clusters->centroid_y[0]);
                for (int j = 1; j < clusters_no; j++)
                {
                    double distance = get_distance(
                            elements->x[i], elements->y[i],
                            clusters->centroid_x[j], clusters->centroid_y[j]);
                    if (distance < min_distance)
                    {
                        min_distance = distance;
                        cluster_id = j;
                    }
                }
                new_centroid_x[cluster_id] += elements->x[i];
                new_centroid_y[cluster_id] += elements->y[i];
                c_elements[cluster_id] += 1;
            }

            for (int i = 0; i < clusters_no; ++i)
            {
                #pragma omp critical
                {
                    clusters->new_centroid_x[i] += new_centroid_x[i];
                    clusters->new_centroid_y[i] += new_centroid_y[i];
                    clusters->elements_no[i] += c_elements[i];
                }
            }
            #pragma omp barrier
            #pragma omp single
            {
                for (int i=0; i < clusters_no; ++i)
                {
                    clusters->new_centroid_x[i] = clusters->new_centroid_x[i] / clusters->elements_no[i];
                    clusters->new_centroid_y[i] = clusters->new_centroid_y[i] / clusters->elements_no[i];

                    if (clusters->centroid_x[i] != clusters->new_centroid_x[i] ||
                        clusters->centroid_y[i] != clusters->new_centroid_y[i])
                    {
                        clusters->centroid_x[i] = clusters->new_centroid_x[i];
                        clusters->centroid_y[i] = clusters->new_centroid_y[i];
                        has_changed = true;
                    }
                }
                if (has_changed)
                {
                    for (int i=0; i < clusters_no; ++i)
                    {
                        clusters->new_centroid_x[i] = 0;
                        clusters->new_centroid_y[i] = 0;
                        clusters->elements_no[i] = 0;
                    }
                }
            }
        }
    }
    auto t2 = std::chrono::high_resolution_clock::now();

    std::cout   << "\nParallel execution time: "
                << std::chrono::duration_cast<std::chrono::milliseconds>(t2-t1).count()
                << " milliseconds\n" << std::endl;

    int tot = 0;
    for(int i = 0; i< clusters_no; i++)
    {
        printf("Cluster: %d - Centroid: (%f,%f) - #Elements: %d\n",
                i, clusters->centroid_x[i], clusters->centroid_y[i], clusters->elements_no[i]);
        tot += clusters->elements_no[i];
    }
    printf("\nTot elements: %d\n", tot);

    return 0;
}
