#ifndef K_MEANS_MID_TERM_UTILITY_V2_H
#define K_MEANS_MID_TERM_UTILITY_V2_H

#endif //K_MEANS_MID_TERM_UTILITY_V2_H

#include <unordered_map>
#include <fstream>
#include <sstream>
#include <vector>

static const std::string ELEMENTS_NO = "elements_no";
static const std::string CLUSTERS_NO = "clusters_no";
static const std::string USE_FILE_ELEMENS = "use_file_elements";

static const std::string THREADS_NO = "threads_no";

/**
 * Data structure that holds the elements.
 *
 */
class Elements {
public:
    int* x;
    int* y;
    int* cluster_id;

    Elements (int size)
    {
        this->x = new int [size];
        this->y = new int [size];
        this->cluster_id = new int [size];
    }
};

/**
 * Data structure that holds the clusters.
 *
 */
class Clusters {
public:
    double* centroid_x;
    double* centroid_y;
    double* new_centroid_x;
    double* new_centroid_y;
    int* elements_no;

    Clusters (int size)
    {
        this->centroid_x = new double [size];
        this->centroid_y = new double [size];
        this->new_centroid_x = new double [size];
        this->new_centroid_y = new double [size];
        this->elements_no = new int [size];
    }

};

// Functions declarations
void init_elements(Elements* elements , int elements_no);
void init_clusters(Clusters* clusters, int clusters_no);
void init_with_file_elements
(
        int elements_no,
        int clusters_no,
        std::string filename,
        Elements* elements,
        Clusters* clusters
);
double get_distance(int el_x, int el_y, double cl_x, double cl_y);

/**
 * Function used to initialize the elements with random values.
 *
 * @param elements: pointer to the Elements object
 * @param elements_no: number of elements
 */
void init_elements(Elements* elements, int elements_no)
{
    for (int i = 0; i < elements_no; i++)
    {
        elements->x[i] = rand()%500;
        elements->y[i] = rand()%500;
        elements->cluster_id[i] = -1;
    }
}

/**
 * Function used to initialize the clusters with random values
 *
 * @param clusters: pointer to the Clusters object
 * @param clusters_no: number of clusters
 */
void init_clusters(Clusters* clusters, int clusters_no)
{
    for (int i = 0; i < clusters_no; i++)
    {
        clusters->centroid_x[i] = rand()%500;
        clusters->centroid_y[i] = rand()%500;
        clusters->new_centroid_x[i] = 0;
        clusters->new_centroid_y[i] = 0;
        clusters->elements_no[i] = 0;
    }
}

/**
 * Function used to initialize elements and clusters with values contained in a file.
 * The file must be written line by line, where each line contains x_coordinate
 * and y_coordinate separated by a single space.
 * (Example line: "10 20")
 *
 * @param elements_no: number of elements
 * @param clusters_no: number of clusters
 * @param filename: name of the file
 * @param elements: pointer to the Elements object
 * @param clusters: pointer to the Cluster object
 */
void init_with_file_elements
(
        int elements_no,
        int clusters_no,
        std::string filename,
        Elements* elements,
        Clusters* clusters
)
{
    std::ifstream infile(filename);

    int num = elements_no / clusters_no;
    int count = 0;

    for( std::string line; getline( infile, line ); )
    {
        std::istringstream is(line);
        std::string part;
        std::vector<int> parts;
        while (getline(is, part, ' '))
        {
            parts.push_back(std::stoi(part));
        }
        if ( (count % num) == 0)
        {
            clusters->centroid_x[count/num] = part[0];
            clusters->centroid_y[count/num] = part[1];
            clusters->new_centroid_x[count/num] = 0;
            clusters->new_centroid_y[count/num] = 0;
            clusters->elements_no[count/num] = 0;
        }
        elements->x[count] = parts[0];
        elements->y[count] = parts[1];
        elements->cluster_id[count] = -1;
        count ++;
    }
}

/**
 * Function used to calculate distance between an element and a cluster.
 * It returns the Euclidean distance.
 *
 * @param el_x: element x coordinate
 * @param el_y: element y coordinate
 * @param cl_x: cluster centroid x coordinate
 * @param cl_y: cluster centroid y coordinate
 * @return
 */
double get_distance(int el_x, int el_y, double cl_x, double cl_y)
{
    return sqrt(pow(el_y - cl_y, 2) + pow(el_x-cl_x, 2));
}
