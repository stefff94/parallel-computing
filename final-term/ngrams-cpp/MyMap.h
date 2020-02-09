//
// Created by Stefano Vannucchi on 23/12/2019.
//

#ifndef BIGRAM_FINAL_TERM_CPP_MYMAP_H
#define BIGRAM_FINAL_TERM_CPP_MYMAP_H

#include <mutex>
#include <unordered_map>

class MyMap {

private:
    std::mutex mut;
    std::unordered_map<std::string, int> map;

public:

    /**
     * Method used to merge the thread's local map with the global shared map.
     *
     * @param map_to_merge: thread's map to merge with the global one.
     */
    void merge(std::unordered_map<std::string, int> map_to_merge)
    {
        std::unordered_map<std::string, int>::iterator it;
        for ( it = map_to_merge.begin(); it != map_to_merge.end(); it++ )
        {
            std::unique_lock<std::mutex> lk(mut);
            auto global_it = map.find(it->first);
            if(global_it != map.end())
            {
                map[global_it->first] = global_it->second + it->second;
            } else {
                map.insert(std::pair<std::string, int>(it->first, it->second));
            }
            lk.unlock();
        }
    }

    /**
     * Getter method to obtain the map object.
     *
     * @return: hash map object
     */
    std::unordered_map<std::string, int> get_map()
    {
        return map;
    }

    /**
     * Print a report of elements contained in the map.
     *
     */
    void print_report()
    {
        std::lock_guard<std::mutex> lk(mut);
        std::unordered_map<std::string, int>::iterator it;
        long long count = 0;
        for (it = map.begin(); it != map.end(); it++)
        {
            count += it->second;
        }

        printf("count: %lld \n", count);
        printf("qu: %d \n", map["qu"]);
    }

    bool contains(const std::string& key)
    {
        std::lock_guard<std::mutex> lk(mut);
        return map.count(key) == 1;
    }

    /**
     * Insert a new pair (key, value) in the map, in a mutually exclusive mode.
     *
     * @param key: string
     * @param value: integer
     */
    void insert(const std::string& key, int value)
    {
        std::lock_guard<std::mutex> lk(mut);
        map.insert(std::pair<std::string, int>(key, value));
    }

    /**
     * Print all the elements contained in the map.
     *
     */
    void print_elements()
    {
        std::unordered_map<std::string, int>::iterator it;
        for ( it = map.begin(); it != map.end(); it++ )
        {
            std::cout << "ngram: " << it->first << " - value: " << it->second << std::endl;
        }
    }

};

#endif //BIGRAM_FINAL_TERM_CPP_MYMAP_H
