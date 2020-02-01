//
// Created by Stefano Vannucchi on 23/12/2019.
//

#ifndef BIGRAM_FINAL_TERM_CPP_MYMAP_H
#define BIGRAM_FINAL_TERM_CPP_MYMAP_H

#include <map>
#include <mutex>
#include <unordered_map>

class MyMap {

private:
    std::mutex mut;
    std::unordered_map<std::string, int> map;

public:

    void merge(std::unordered_map<std::string, int> mapToMerge) {
        std::unordered_map<std::string, int>::iterator it;

        for ( it = mapToMerge.begin(); it != mapToMerge.end(); it++ ) {

            std::unique_lock<std::mutex> lk(mut);
            auto global_it = map.find(it->first);
            if(global_it != map.end()) {
                // map.operator[](global_it->first) = global_it->second + it->second;
                map[global_it->first] = global_it->second + it->second;
            } else {
                map.insert(std::pair<std::string, int>(it->first, it->second));
            }
            lk.unlock();
        }
    }

    std::unordered_map<std::string, int> getMap() {
        return map;
    }

    void printReport() {
        std::lock_guard<std::mutex> lk(mut);
        std::unordered_map<std::string, int>::iterator it;
        long long count = 0;
        for (it = map.begin(); it != map.end(); it++) {
            count += it->second;
        }
        printf("count: %lld \n", count);

        printf("qu: %d \n", map["qu"]);
    }

    bool contains(std::string key) {
        std::lock_guard<std::mutex> lk(mut);
        return map.count(key) == 1;
    }

    void insert(std::string key, int value) {
        std::lock_guard<std::mutex> lk(mut);
        map.insert(std::pair<std::string, int>(key, value));
    }

/*    void printElements() {
        std::unordered_map<std::string, int>::iterator it;
        for ( it = map.begin(); it != map.end(); it++ ) {
            std::cout << "ngram: " << it->first << " - value: " << it->second << std::endl;
        }
    }*/

};

#endif //BIGRAM_FINAL_TERM_CPP_MYMAP_H
