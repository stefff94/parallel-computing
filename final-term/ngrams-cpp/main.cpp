#include <thread>
#include <atomic>
#include "Producer.h"
#include "Consumer.h"

static const std::string NGRAMS = "n_grams";
static const std::string CONSUMERS_NO = "consumers_no";
static const std::string PRODUCERS_NO = "producers_no";
static const std::string LINES = "lines";

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

    // n-grams to extract
    int n = 2;
    if (arguments.find(NGRAMS) != arguments.end())
    {
        n = std::stoi(arguments[NGRAMS]);
    }

    // number of consumer threads
    int consumers_no = 6;
    if (arguments.find(CONSUMERS_NO) != arguments.end())
    {
        consumers_no = std::stoi(arguments[CONSUMERS_NO]);
    }

    // number of producer threads
    int producers_no = 1;
    if (arguments.find(PRODUCERS_NO) != arguments.end())
    {
        producers_no = std::stoi(arguments[PRODUCERS_NO]);
    }

    // number of lines to process
    std::string lines = "10,000";
    if (arguments.find(LINES) != arguments.end())
    {
        lines = arguments[LINES];
    }

    // poison pill string
    std::string poison_pill = "end-of-games";

    auto t1 = std::chrono::high_resolution_clock::now();

    // shared data structures
    MyQueue shared_queue;
    MyMap global_ngrams;
    MyMap done_books;

    std::vector<std::thread> consumer_threads;
    consumer_threads.reserve(consumers_no);

    std::vector<std::thread> producer_threads;
    producer_threads.reserve(producers_no);

    std::atomic<int> active_producers(0);
    for (int i = 0; i < producers_no; i++)
    {
        producer_threads.emplace_back(Producer(i, producers_no, consumers_no, poison_pill, &shared_queue,
                                               &done_books, lines, &active_producers));
    }

    for (int i = 0; i < consumers_no; ++i)
    {
        consumer_threads.emplace_back(Consumer(i, n, poison_pill, &global_ngrams, &shared_queue));
    }

    for (auto &pt : producer_threads)
    {
        pt.join();
    }

    for (auto &ct : consumer_threads)
    {
        ct.join();
    }

    auto t2 = std::chrono::high_resolution_clock::now();

    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(t2-t1).count()
              << " milliseconds\n";

    global_ngrams.print_report();

    return 0;
}
