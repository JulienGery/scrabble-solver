#include <algorithm>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <chrono>
#include <iterator>


auto convertStrToRepresentation(const std::string &str) {
    uint32_t rp = 0;
    for(const auto &i : str){
        rp |= 0x80000000 >> (i - 'A');
    }
    return rp;
}

auto aa(const std::string &letters, std::vector<uint16_t> &buffer) {
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

bool match(const std::vector<uint16_t> &letters, short unsigned int word[26]) {
    for(size_t i = 0; i < 26; i++) {
        if(letters[i] < word[i]) {
            return false;
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
    std::vector<uint16_t[26]> jsp(words.size()); //size must be set at init time
    readFile("jsp.bin", jsp);


    auto letters = std::string(argv[1]); //vteafsjxdr
    std::transform(letters.begin(), letters.end(), letters.begin(), ::toupper);
    const uint16_t length = letters.size();
    const auto representation = convertStrToRepresentation(letters);
    std::vector<uint16_t> word(26);
    aa(letters, word);

    size_t eligible = 0;
    std::vector<size_t> indexes(sizes.size());
    
    const auto start = std::chrono::high_resolution_clock::now();
    
    for (size_t i = 0; i < sizes.size(); i++) {
        if ((!(~(~WBRp[i] | representation))) && (length >= sizes[i])){
            indexes[eligible] = i;
            eligible++;
        }
    }

    indexes.resize(eligible);

    size_t valid = 0;
    std::vector<size_t> finalIndexes(eligible);
    for(const auto &index : indexes) {
        if (match(word, jsp[index])) {
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