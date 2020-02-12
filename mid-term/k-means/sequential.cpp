#include <iostream>
#include "utility.h"

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
        // initialize with file "elements.txt"
        elements_no = 100000;
        clusters_no = 4;

        init_with_file_elements(elements_no, clusters_no, "elements.txt", elements, clusters);
    }

    // start algorithm
    bool has_changed = true;
    auto t1 = std::chrono::high_resolution_clock::now();

    while(has_changed)
    {
        has_changed = false;

        // First k-means step
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

        // Second k-means step
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
                has_changed = true;
            }
        }
    }
    auto t2 = std::chrono::high_resolution_clock::now();

    std::cout   << "\nSequential execution time: "
                << std::chrono::duration_cast<std::chrono::milliseconds>(t2-t1).count()
                << " milliseconds\n" << std::endl;

    int tot = 0;
    for(int i = 0; i < clusters_no; i++)
    {
        printf("Cluster: %d - Centroid: (%f,%f) - #Elements: %d\n",
                clusters[i].index, clusters[i].centroid_x, clusters[i].centroid_y, clusters[i].elements_no);
        tot += clusters[i].elements_no;
    }
    printf("\nTot elements: %d\n", tot);

    return 0;
}
