#include <chrono>
#include <iostream>
#include <string>
#include "Scrabble.h"
#include "SimpleTimer.h"

int main(int argc, char** args)
{
    SimpleTimer timer{"total"};
    const size_t jokerCount = 0;
    std::string letters = args[1];
    std::transform(letters.begin(), letters.end(), letters.begin(), ::toupper);

    Scrabble scrabble{"../boardInput.txt", "../French ODS dictionary.txt"};

    {
        SimpleTimer timer{"find best word"};
        // scrabble.pretyPrint();
        scrabble.findBestWord(letters, jokerCount);
    }
}
