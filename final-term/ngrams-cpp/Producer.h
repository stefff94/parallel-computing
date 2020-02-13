//
// Created by Stefano Vannucchi on 19/12/2019.
//

#ifndef BIGRAM_FINAL_TERM_CPP_PRODUCER_H
#define BIGRAM_FINAL_TERM_CPP_PRODUCER_H

#endif //BIGRAM_FINAL_TERM_CPP_PRODUCER_H

#include <thread>
#include <unistd.h>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <sstream>
#include "MyQueue.h"
#include "MyMap.h"


class Producer {
private:
    int id;
    int consumer_no;
    std::string current_directory;
    std::string poison_pill;
    long lines_no = 0;

    // pointers to shared data stuctures
    MyQueue* shared_queue;
    MyMap* done_books;

    std::atomic<int>* active_producers;

    /**
     * This method represents the core of the thread, called when it starts.
     * It loop over all the files contained in the current directory, and process them.
     *
     */
    void prepare()
    {
        printf("Producer %d start work\n", id);
        for (const auto & entry : std::__fs::filesystem::directory_iterator(current_directory))
        {
            if (done_books != nullptr)
            {
                if (!done_books->contains(entry.path().string())) {
                    done_books->insert(entry.path().string(), 1);
                    process_file(entry.path());
                }
            } else {
                process_file(entry.path());
            }

        }
        if (*active_producers == 1)
        {
            for(int i = 0; i < consumer_no; i++)
            {
                shared_queue->push(poison_pill);
            }
        }
        (*active_producers) --;
        printf("Producer %d stop work\n", id);
        printf("Producer %d added %ld lines\n", id, lines_no);
    }

    /**
     * Method that extracts each lines of a file, and push them into a queue shared among all threads.
     *
     * @param path: string variable that contains the path of the current file to be processed.
     */
    void process_file(const std::string& path)
    {
        std::ifstream infile(path);
        for( std::string line; getline( infile, line ); )
        {
            shared_queue->push(line);
            lines_no ++;
        }
    }

public:
    Producer(int id, int p_no, int c_no, std::string pp, MyQueue* sq, MyMap* db,
            const std::string& path_part, std::atomic<int>* a_p)
    {
        this->id = id;
        current_directory = "../../done-books/" + path_part + "_lines";
        consumer_no = c_no;
        poison_pill = pp;
        shared_queue = sq;

        if (p_no > 1) {
            done_books = db;
        } else {
            done_books = nullptr;
        }
        this->active_producers = a_p;
        (*this->active_producers) ++;
    }

    void operator()()
    {
        prepare();
    }
};
