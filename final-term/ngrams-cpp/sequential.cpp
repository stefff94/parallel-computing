//
// Created by Stefano Vannucchi on 25/01/2020.
//

#include <iostream>
#include <unordered_map>
#include <filesystem>
#include <fstream>

static const std::string NGRAMS = "n_grams";
static const std::string LINES = "lines";

/**
 * Method used to check if an n-gram is valid.
 *
 * @param s: string that contains the n-gram to check
 * @return (true) if it is valid (false) otherwise
 */
bool is_n_gram_good(std::string s)
{
    for(char& c : s) {
        if (!isalpha(c)) {
            return false;
        }
    }
    return true;
}

/**
 * Method used to extract the n-grams from the lines.
 *
 * @param n_grams: reference to map that holds the n-grams
 * @param infile: input stream file
 * @param n: n-gram defined
 */
void get_n_grams(std::unordered_map<std::string, int>& n_grams, std::ifstream& infile, int n)
{
    std::string s;
    while (getline(infile, s))
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
}

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

    // number of lines to process
    std::string lines = "10,000";
    if (arguments.find(LINES) != arguments.end())
    {
        lines = arguments[LINES];
    }

    std::string current_directory = "../../done-books/" + lines + "_lines";

    // hash map to store the n-grams
    std::unordered_map<std::string, int> n_grams;

    auto t1 = std::chrono::high_resolution_clock::now();
    printf("Sequential worker start \n");

    for (const auto & entry : std::__fs::filesystem::directory_iterator(current_directory))
    {
        std::ifstream infile(entry.path());

        get_n_grams(n_grams, infile, n);
    }

    auto t2 = std::chrono::high_resolution_clock::now();
    printf("Sequential worker end \n");

    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(t2-t1).count()
              << " milliseconds\n";

    std::unordered_map<std::string, int>::iterator it;

    long long count = 0;
    for (it = n_grams.begin(); it != n_grams.end(); it++) {
        count += it->second;
    }

    printf("count: %lld \n", count);
    printf("qu: %d \n", n_grams["qu"]);

    /*for (it = n_grams.begin(); it != n_grams.end(); ++it) {
        printf("key: %s - value: %d \n", it->first.c_str(), it->second);
    }*/

}
