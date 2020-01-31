#include <thread>
#include <atomic>
#include "Producer.h"
#include "Consumer.h"

static const std::string NGRAMS = "ngrams";
static const std::string CONSUMERS_NO = "consumersNo";
static const std::string PRODUCERS_NO = "producersNo";
static const std::string LINES = "lines";

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

    // n-grams to extract
    int n = 2;
    if (arguments.find(NGRAMS) != arguments.end()) {
        n = std::stoi(arguments[NGRAMS]);
    }

    // number of consumer threads
    int consumersNo = 6;
    if (arguments.find(CONSUMERS_NO) != arguments.end()) {
        consumersNo = std::stoi(arguments[CONSUMERS_NO]);
    }

    // number of producer threads
    // to increase the number of producers, please activate the done_books map usage in Producer class
    int producersNo = 1;
    if (arguments.find(PRODUCERS_NO) != arguments.end()) {
        producersNo = std::stoi(arguments[PRODUCERS_NO]);
        /*if (std::stoi(arguments[PRODUCERS_NO]) > 1) {
            std::cout << "To increase the number of producers, please activate the done_books map usage in Producer class." << std::endl;
        }*/
    }

    // number of lines to process
    std::string lines = "10,000";
    if (arguments.find(LINES) != arguments.end()) {
        lines = arguments[LINES];
    }

    // poisonPill string
    std::string poisonPill = "end-of-games";

    auto t1 = std::chrono::high_resolution_clock::now();

    MyQueue shared_queue;
    MyMap global_ngrams;
    MyMap done_books;

    std::vector<std::thread> consumerThreads;
    consumerThreads.reserve(consumersNo);

    std::vector<std::thread> producerThreads;
    producerThreads.reserve(producersNo);

    std::atomic<int> activeProducers;
    for (int i = 0; i < producersNo; i++) {
        producerThreads.emplace_back(Producer(i, producersNo, consumersNo, poisonPill, &shared_queue, &done_books, lines, &activeProducers));
    }

    for (int i = 0; i < consumersNo; ++i) {
        consumerThreads.emplace_back(Consumer(i, n, poisonPill, &global_ngrams, &shared_queue));
    }

    for (auto &pt : producerThreads)
        pt.join();

    for (auto &ct : consumerThreads)
        ct.join();

    auto t2 = std::chrono::high_resolution_clock::now();

    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(t2-t1).count()
              << " milliseconds\n";

    global_ngrams.printReport();

    return 0;
}


