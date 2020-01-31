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

#include <vector>
#include "MyQueue.h"
#include "MyMap.h"


class Producer {
private:
    int id;
    std::string currentDirectory;
    std::string poisonPill;
    int consumerNo;

    MyQueue* shared_queue;

    MyMap* done_books;

    long lines_no = 0;

    std::atomic<int>* activeProducers;

    void prepare() {
        printf("Producer %d start work\n", id);
        for (const auto & entry : std::__fs::filesystem::directory_iterator(currentDirectory)) {

            if (done_books != NULL) {
                if (!done_books->contains(entry.path().string())) {
                    done_books->insert(entry.path().string(), 1);

                    processFile(entry.path());
                }
            } else {
                processFile(entry.path());
            }

            /*if(done_books != NULL && !done_books->contains(entry.path().string())) {
                done_books->insert(entry.path().string(), 1);

                std::ifstream infile(entry.path());
                for( std::string line; getline( infile, line ); )
                {
                    shared_queue->push(line);
                    lines_no ++;
                }
            }*/

        }
        if (*activeProducers == 1) {
            for(int i = 0; i < consumerNo; i++) {
                shared_queue->push(poisonPill);
            }
        }
        (*activeProducers) --;
        printf("Producer %d stop work\n", id);
        printf("Producer %d added %ld lines\n", id, lines_no);
    }

    void processFile(std::string path) {
        std::ifstream infile(path);

        for( std::string line; getline( infile, line ); )
        {
            shared_queue->push(line);
            lines_no ++;
        }
    }

public:
    Producer(int id, int pNo, int cNo, std::string pp, MyQueue* sq, MyMap* db, std::string path_part, std::atomic<int>* activeProducers) {
        this->id = id;
        currentDirectory = "/Volumes/Disco Esterno/parallel-v2/done-books/"+ path_part + "_lines";
        // currentDirectory = "/Users/stefano/Desktop/done-books/" + path_part + "_lines";
        consumerNo = cNo;
        poisonPill = pp;
        shared_queue = sq;

        if (pNo > 1) {
            done_books = db;
        }
        this->activeProducers = activeProducers;
        (*this->activeProducers) ++;
    }

    void operator()() {
        prepare();
    }
};



