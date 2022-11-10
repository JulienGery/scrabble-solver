#include <algorithm>
#include <boost/algorithm/string/erase.hpp>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <chrono>
#include <iterator>
#include <boost/algorithm/string.hpp>


auto convertStrToRepresentation(const std::string &str) {
    uint32_t rp = 0;
    for(const auto &i : str){
        rp |= 0x80000000 >> (i - 'A');
    }
    return rp;
}

auto getLettersCount(const std::string &letters, std::vector<uint16_t> &buffer) {
    for(const auto &i : letters) {
        buffer[i - 'A']++;
    }
}


template<class t>
void readFile(const std::string &path, std::vector<t> &buffer) {
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file.is_open()) throw std::runtime_error("\033[1;31mFailed to open file " + path + "\033[0m");

    const size_t size = file.tellg();
    buffer.resize(size/sizeof(t));
    file.seekg(0);
    file.read((char *) buffer.data(), size);
    file.close();
}

void readFile(const std::string &path, std::vector<uint16_t[26]> &buffer) {
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file.is_open()) throw std::runtime_error("\033[1;31mFailed to open file " + path + "\033[0m");

    const size_t size = file.tellg();
    file.seekg(0);
    file.read((char *) buffer.data(), size);
    file.close();
}

void readFile(const std::string &path, std::vector<std::string> &buffer) {
    std::ifstream file{path};
    if (!file.is_open()) throw std::runtime_error("\033[1;31mFailed to open file " + path + "\033[0m");

    std::copy(
        std::istream_iterator<std::string>(file),
        std::istream_iterator<std::string>(),
          std::back_inserter(buffer)
    );
    file.close();
}

bool match(const std::vector<uint16_t> &letters, short unsigned int word[26], int16_t jokerCount) {
    for(size_t i = 0; i < 26; i++) {
        const auto tmp = letters[i] - word[i];
        if(tmp < 0) {
            jokerCount += tmp;
            if(jokerCount < 0) {
                return false;
            }
        }
    }
    return true;
}

int main(int argc, char* argv[]) {
    if (argc != 2) throw std::runtime_error("\033[1;31mGive letters!\033[0m");

    std::vector<uint32_t> WBRp;
    readFile("WBRp.bin", WBRp);
    std::vector<uint16_t> sizes;
    readFile("lengths.bin", sizes);
    std::vector<std::string> words;
    readFile("wordsSorted.txt", words);
    std::vector<uint16_t[26]> lettersCount(words.size()); //size must be set at init time
    readFile("lettersCount.bin", lettersCount);

    auto letters = std::string(argv[1]); //vteafsjxdr
    const uint16_t length = letters.size();
    boost::erase_all(letters, "?");
    const int16_t jokerCount = length - letters.size();
    std::transform(letters.begin(), letters.end(), letters.begin(), ::toupper);
    const auto representation = convertStrToRepresentation(letters);
    std::vector<uint16_t> word(26);
    getLettersCount(letters, word);

    size_t eligible = 0;
    std::vector<size_t> indexes(sizes.size());
    
    const auto start = std::chrono::high_resolution_clock::now();
    
    for (size_t i = 0; i < sizes.size(); i++) {
        if (length >= sizes[i]) {
            uint32_t jsp = ~(~WBRp[i] | representation);
            for(size_t a = 0; a < jokerCount+1; a++){
                if (!jsp){
                    indexes[eligible] = i;
                    eligible++;
                    break;
                }
                jsp &= (jsp-1);
            }            
        }
    }

    indexes.resize(eligible);

    size_t valid = 0;
    std::vector<size_t> finalIndexes(eligible);
    for(const auto &index : indexes) {
        if (match(word, lettersCount[index], jokerCount)) {
            finalIndexes[valid] = index;
            valid++;
        }
    }    
    
    finalIndexes.resize(valid);

    const auto end = std::chrono::high_resolution_clock::now();
    const auto delta = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);

    std::cout << "\033[36m" << delta.count() << " nanoseconds \033[1;36mwithout accounting for files reading\n\033[0m";
    std::cout << "\033[32m" << valid << " valids words\n\033[0m";

    // for(const auto &index : finalIndexes) {
    //     std::cout << words[index] << '\t'; 
    // }
}