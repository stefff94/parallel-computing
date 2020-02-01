#include <math.h>
#include <vector>
#include <fstream>
#include <sstream>
#include <unordered_map>

static const std::string ELEMENTS_NO = "elementsNo";
static const std::string CLUSTERS_NO = "clustersNo";
static const std::string USE_FILE_ELEMENS = "useFileElements";

static const std::string THREADS_NO = "threadsNo";

class Cluster {
public:
    double centroid_x;
    double centroid_y;
    int index;
    double newCentroid_x;
    double newCentroid_y;
    int elementNo;

    explicit Cluster(int index, int x, int y) {
        this->index = index;
        this->centroid_x = x;
        this->centroid_y = y;
        this->newCentroid_x = 0;
        this->newCentroid_y = 0;
        this->elementNo = 0;
    }
};

class Element{
public:
    int x;
    int y;
    int clusterId;

    explicit Element(int x, int y) {
        this->x = x;
        this->y = y;
        clusterId = 0;
    }
};

void initElements(std::vector<Element> &elements, int elementsNo);
void initClusters(std::vector<Cluster> &clusters, int clustersNo);

void initWithFileElements
(
        int elementsNo,
        int clustersNo,
        std::string filename,
        std::vector<Element> &elements,
        std::vector<Cluster> &clusters
);

double getDistance(Element element, Cluster cluster);

void initElements(std::vector<Element> &elements, int elementsNo){
    for (int i = 0; i < elementsNo; i++) {
        elements.push_back(*(new Element(rand()%500, rand()%500)));
    }
}

void initClusters(std::vector<Cluster> &clusters, int clustersNo) {
    for (int i = 0; i < clustersNo; i++) {
        clusters.push_back(*(new Cluster(i, rand()%500, rand()%500)));
    }
}

void initWithFileElements
(
        int elementsNo,
        int clustersNo,
        std::string filename,
        std::vector<Element> &elements,
        std::vector<Cluster> &clusters
)
{
    std::ifstream infile(filename);

    int num = elementsNo / clustersNo;
    int count = 0;

    for( std::string line; getline( infile, line ); )
    {
        std::istringstream is(line);
        std::string part;
        std::vector<int> parts;
        while (getline(is, part, ' ')) {
            parts.push_back(std::stoi(part));
        }
        if ( (count % num) == 0) {
            clusters.push_back(*new Cluster((count / num),parts[0], parts[1]));
        }
        count ++;
        elements.push_back(*new Element(parts[0], parts[1]));
    }
}

double getDistance(Element element, Cluster cluster) {
    return sqrt(pow(element.y - cluster.centroid_y,2) + pow(element.x-cluster.centroid_x,2));
}