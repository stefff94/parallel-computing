//
// Created by Stefano Vannucchi on 20/12/2019.
//

#ifndef BIGRAM_FINAL_TERM_CPP_CONSUMER_H
#define BIGRAM_FINAL_TERM_CPP_CONSUMER_H

#endif //BIGRAM_FINAL_TERM_CPP_CONSUMER_H

#include <string>
#include "MyMap.h"
#include "MyQueue.h"

class Consumer {
private:
    int id;
    int n;
    bool can_work;
    std::string poison_pill;

    // private hash map
    std::unordered_map<std::string, int> n_grams;

    // pointers to shared data structures
    MyMap* global_n_grams;
    MyQueue* lines;

    /**
     * Core method of the consumer thread. It process each line, extracted from the shared queue,
     * and calculate the ngrams.
     *
     */
    void consume()
    {
        printf("Consumer %d start work\n", id);
        while(can_work)
        {
            if(!lines->empty())
            {
                std::string s;
                lines->wait_and_pop(s);
                if(s == poison_pill)
                {
                    can_work = false;

                    // finalize work
                    global_n_grams->merge(n_grams);

                } else {
                    process_string(s);
                }
            }
        }
        printf("Consumer %d stop work\n", id);
    }

    /**
     * Method used to process the single line.
     *
     * @param s: line extracted from the shared queue.
     */
    void process_string(std::string s)
    {
        std::transform(s.begin(), s.end(), s.begin(), ::tolower);
        if (s.size() >= n)
        {
            for(int i = 0; i < s.size() - n + 1; ++i)
            {
                std::string w;
                for(int j = 0; j < n; j++)
                {
                    w += s[i+j];
                }

                if (is_n_gram_good(w))
                {
                    n_grams[w]++;
                }
            }
        }
    }

    /**
     * Method used to check if an n-gram is valid.
     * @param s: string that contains the n-gram to check
     * @return (true) if it is valid, (false) otherwise.
     */
    static bool is_n_gram_good(std::string s)
    {
        for(char& c : s) {
            if (!isalpha(c)) {
                return false;
            }
        }
        return true;
    }

public:
    Consumer(int id, int n, std::string pp, MyMap* gn, MyQueue* q)
    {
        this->id = id;
        this->n = n;
        lines = q;
        poison_pill = pp;
        can_work = true;
        global_n_grams = gn;
    }

    void operator()()
    {
        consume();
    }

};
