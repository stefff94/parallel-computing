//
// Created by Stefano Vannucchi on 25/01/2020.
//

#include <iostream>
#include <unordered_map>
#include <filesystem>
#include <fstream>
#include <regex>


bool isNgramGood(const std::string& s) {
    std::string const rule = " ^[^,.-;:?!«»_'-*()@]+$\"";
    for (char i : rule) {
        if (s.find(i) != std::string::npos )
            return false;
    }
    return true;
}

void getNGrams(std::unordered_map<std::string, int>& ngrams, std::ifstream& infile, int n) {
    std::string s;
    while (getline(infile, s))
    {
        std::transform(s.begin(), s.end(), s.begin(), ::tolower);
        s.erase( std::remove(s.begin(), s.end(), '\r'), s.end() );
        s.erase( std::remove(s.begin(), s.end(), '\xbf'), s.end() );

        if (s.size() >= n) {
            for(int i = 0; i < s.size() - n + 1; ++i) {
                std::string w;
                for(int j = 0; j < n; j++) {
                    w += s[i+j];
                }

                if (isNgramGood(w))
                    ngrams[w]++;
            }
        }
    }
}

int main() {

    int n = 2;

    std::unordered_map<std::string, int> ngrams;

    //std::string currentDirectory = "/Users/stefano/Documents/Università/Ing-Inf-LM/1° anno/Parallel Computing/exam-projects/bigram-final-term-cpp/done-books/test";
    std::string currentDirectory = "/Volumes/Disco Esterno/parallel-v2/done-books/1,000,000_lines/";

    auto t1 = std::chrono::high_resolution_clock::now();
    printf("Sequential worker start \n");

    for (const auto & entry : std::__fs::filesystem::directory_iterator(currentDirectory)) {
        std::ifstream infile(entry.path());

        getNGrams(ngrams, infile, n);

    }

    auto t2 = std::chrono::high_resolution_clock::now();
    printf("Sequential worker end \n");

    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(t2-t1).count()
              << " milliseconds\n";

    std::unordered_map<std::string, int>::iterator it;
    /*for (it = ngrams.begin(); it != ngrams.end(); ++it) {
        printf("key: %s - value: %d \n", it->first.c_str(), it->second);
    }*/

    long long count = 0;
    for (it = ngrams.begin(); it != ngrams.end(); it++) {
        count += it->second;
    }
    printf("count: %lld \n", count);

    printf("qu: %d \n", ngrams["qu"]);

}



