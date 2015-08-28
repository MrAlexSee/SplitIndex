#include <boost/format.hpp>
#include <chrono>
#include <iostream>
#include <random>

#include "query_utils.hpp"

using namespace std;

vector<string> QueryUtils::genQueries(const vector<string> &words,
                                      const string &alphabet,
                                      int minWordLength, int nQueries, int maxNErrors)
{
    vector<string> queries;
    queries.reserve(nQueries);

    std::mt19937 generator(chrono::system_clock::now().time_since_epoch().count());
    
    // Inclusive from both sides.
    uniform_int_distribution<> wordRange(0, words.size() - 1);
    uniform_int_distribution<> alphRange(0, alphabet.size() - 1);

    std::bernoulli_distribution fairDist(0.5);
    int nSalted = 0;

    for (int i = 0; i < nQueries; ++i)
    {
        int curIndex = wordRange(generator);
        assert(curIndex >= 0 and curIndex < words.size());

        string curWord = words[curIndex];

        if (curWord.size() < minWordLength)
        {
            i -= 1;
            continue;
        }

        uniform_int_distribution<> charRange(0, curWord.size() - 1);
        bool salted = false;

        for (int iE = 0; iE < maxNErrors; ++iE)
        {
            if (fairDist(generator))
            {
                int charIndex = charRange(generator);
                int alphIndex = alphRange(generator);

                curWord[charIndex] = alphabet[alphIndex];
                salted = true;
            }
        }

        if (salted)
        {
            nSalted += 1;
        }

        queries.push_back(curWord);
    }

    cout << boost::format("Salted %1% words") % nSalted << endl;

    assert(queries.size() == nQueries);
    return queries;
}
