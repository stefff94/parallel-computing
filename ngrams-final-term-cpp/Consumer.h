//
// Created by Stefano Vannucchi on 20/12/2019.
//

#ifndef BIGRAM_FINAL_TERM_CPP_CONSUMER_H
#define BIGRAM_FINAL_TERM_CPP_CONSUMER_H

#endif //BIGRAM_FINAL_TERM_CPP_CONSUMER_H

#include <map>
#include <string>
#include <regex>
#include <mutex>
#include "MyMap.h"
#include "MyQueue.h"

class Consumer {
public:

private:
    int id;
    int n;
    bool canWork;
    std::string poisonPill;
    std::unordered_map<std::string, int> ngrams;

    MyMap* global_ngrams;
    MyQueue* lines;

    void consume() {
        printf("Consumer %d start work\n", id);
        while(canWork) {
            if(!lines->empty()) {
                std::string s;

                lines->wait_and_pop(s);

                if(s == poisonPill) {
                    canWork = false;
                    // finalize work
                    global_ngrams->merge(ngrams);
                } else {
                    processString(s);
                }
            }
        }
        printf("Consumer %d stop work\n", id);
    }

    void processString(std::string s) {
        std::transform(s.begin(), s.end(), s.begin(), ::tolower);
        s.erase( std::remove(s.begin(), s.end(), '\r'), s.end() );
        s.erase( std::remove(s.begin(), s.end(), '\xbf'), s.end() );

        if (s.size() >= n) {
            for(int i = 0; i < s.size() - n + 1; ++i) {
                std::string w;
                for(int j = 0; j < n; j++) {
                    w += s[i+j];
                }

                if (isNgramGood(w)) {
                    ngrams[w]++;
                }
            }
        }
    }

    static bool isNgramGood(std::string s) {
        std::string const rule = " ^[^,.-;:?!«»_'-*()@]+$\"";
        for (char i : rule) {
            if (s.find(i) != std::string::npos )
                return false;
        }
        return true;
    }

public:
    Consumer(int id, int n, std::string pp, MyMap* gn, MyQueue* q) {
        this->id = id;
        this->n = n;
        lines = q;
        poisonPill = pp;
        canWork = true;
        global_ngrams = gn;
    }

    void operator()(){
        consume();
    }

};
