#include <math.h>
#include <vector>
#include <fstream>
#include <sstream>
#include <unordered_map>

static const std::string ELEMENTS_NO = "elements_no";
static const std::string CLUSTERS_NO = "clusters_no";
static const std::string USE_FILE_ELEMENS = "use_file_elements";

static const std::string THREADS_NO = "threads_no";

/**
 * Data structure used to define a Cluster
 *
 */
class Cluster {
public:
    double centroid_x;
    double centroid_y;
    int index;
    double new_centroid_x;
    double new_centroid_y;
    int elements_no;

    explicit Cluster(int index, int x, int y)
    {
        this->index = index;
        this->centroid_x = x;
        this->centroid_y = y;
        this->new_centroid_x = 0;
        this->new_centroid_y = 0;
        this->elements_no = 0;
    }
};

/**
 * Data structure used to define an Element
 *
 */
class Element {
public:
    int x;
    int y;
    int cluster_id;

    explicit Element(int x, int y)
    {
        this->x = x;
        this->y = y;
        cluster_id = 0;
    }
};

// functions declarations
void init_elements(std::vector<Element> &elements, int elements_no);
void init_clusters(std::vector<Cluster> &clusters, int clusters_no);
void init_with_file_elements
(
        int elements_no,
        int clusters_no,
        std::string filename,
        std::vector<Element> &elements,
        std::vector<Cluster> &clusters
);
double get_distance(Element element, Cluster cluster);

/**
 * Function used to initialize the elements with random values.
 *
 * @param elements: reference to a vector of Element
 * @param elements_no: number of elements
 */
void init_elements(std::vector<Element> &elements, int elements_no)
{
    for (int i = 0; i < elements_no; i++)
    {
        elements.push_back(*(new Element(rand()%500, rand()%500)));
    }
}

/**
 * Function used to initialize the clusters with random values
 *
 * @param clusters: reference to a vector of Cluster
 * @param clusters_no: number of clusters
 */
void init_clusters(std::vector<Cluster> &clusters, int clusters_no)
{
    for (int i = 0; i < clusters_no; i++)
    {
        clusters.push_back(*(new Cluster(i, rand()%500, rand()%500)));
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
 * @param elements: reference to a vector of Element
 * @param clusters: reference to a vector of Cluster
 */
void init_with_file_elements
(
        int elements_no,
        int clusters_no,
        std::string filename,
        std::vector<Element> &elements,
        std::vector<Cluster> &clusters
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
            clusters.push_back(*new Cluster((count / num),parts[0], parts[1]));
        }
        count ++;
        elements.push_back(*new Element(parts[0], parts[1]));
    }
}

/**
 * Function used to calculate distance between an element and a cluster.
 * It returns the Euclidean distance.
 *
 * @param element
 * @param cluster
 * @return
 */
double get_distance(Element element, Cluster cluster)
{
    return sqrt(pow(element.y - cluster.centroid_y,2) + pow(element.x-cluster.centroid_x,2));
}
