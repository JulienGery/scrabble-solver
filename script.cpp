#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
#include <iterator>
#include <assert.h>
#include <algorithm>
#include <map>
#include <numeric>

auto convertStrToRepresentation(const std::string &str) {
    uint32_t rp = 0;
    for(const auto &i : str){
        rp |= 0x80000000 >> (i - 'A');
    }
    return rp;
}

void readFile(const std::string &path, std::vector<std::string> &buffer) {
    std::ifstream file{path};
    if (!file.is_open()) throw std::runtime_error("\033[1;31mFailed to open file: " + path + "\033[0m");
    std::copy(
        std::istream_iterator<std::string>(file),
        std::istream_iterator<std::string>(),
          std::back_inserter(buffer)
    );
    file.close();
}

template<class t>
void writeFile(const std::string &path, std::vector<t> &buffer) {
    assert(buffer.size() > 0 && "\033[1;31mNo data to write\033[0m");

    std::ofstream out(path, std::ios::binary | std::ios::out);
    out.write((char *)&buffer[0], buffer.size() * sizeof(t));
    out.close();
}

void writeFile(const std::string &path, std::vector<std::string> &buffer) {
    assert(buffer.size() > 0 && "\033[1;31mNo data to write\033[0m");

    std::ofstream out(path, std::ios::out);
    std::ostream_iterator<std::string> output_iterator(out, "\n");

    std::copy(buffer.begin(), buffer.end(), output_iterator);
    
    out.close();
}


// https://stackoverflow.com/questions/17074324/how-can-i-sort-two-vectors-in-the-same-way-with-criteria-that-uses-only-one-of
template <typename T, typename Compare>
std::vector<std::size_t> sort_permutation(
    const std::vector<T>& vec,
    Compare& compare)
{
    std::vector<std::size_t> p(vec.size());
    std::iota(p.begin(), p.end(), 0);
    std::sort(p.begin(), p.end(),
        [&](std::size_t i, std::size_t j){ return compare(vec[i], vec[j]); });
    return p;
}

template <typename T>
void apply_permutation_in_place(
    std::vector<T>& vec,
    const std::vector<std::size_t>& p)
{
    std::vector<bool> done(vec.size());
    for (std::size_t i = 0; i < vec.size(); ++i)
    {
        if (done[i])
        {
            continue;
        }
        done[i] = true;
        std::size_t prev_j = i;
        std::size_t j = p[i];
        while (i != j)
        {
            std::swap(vec[prev_j], vec[j]);
            done[j] = true;
            prev_j = j;
            j = p[j];
        }
    }
}

int main(int argc, char* argv[]) {
    if(argc != 2) throw std::runtime_error("\033[1;31mGive a file!\033[0m");
    
    std::map<char, uint16_t> values{{'A', 1},{'B', 3},{'C', 3},{'D', 2},{'E', 1}, {'F', 4}, {'G', 2}, {'H', 4}, {'I', 1}, {'J', 8}, {'K', 10}, {'L', 1}, {'M', 2}, {'N', 1}, {'O', 1}, {'P', 3}, {'Q', 8}, {'R', 1}, {'S', 1}, {'T', 1}, {'U', 1}, {'V', 4}, {'W', 10},{'X', 10}, {'Y', 10}, {'Z', 10}};

    std::vector<std::string> words;
    readFile(argv[1], words);

    const size_t lines = words.size();
    std::vector<uint32_t> WBRp(lines, 0);
    std::vector<uint16_t> sizes(lines);
    std::vector<uint16_t> points(lines);

    for (size_t i = 0; i < words.size(); i++) {
        const auto &word = words[i];
        sizes[i] = word.size();
        WBRp[i] = convertStrToRepresentation(word);
        
        uint16_t point = 0;
        for(const auto &letter : word) {
            point += values[letter];
        }
        points[i] = point;
    }

    
    auto p = sort_permutation(points, 
                                    *[](const size_t &a, const size_t &b) { return a > b;});

    apply_permutation_in_place(points, p);
    apply_permutation_in_place(words, p);
    apply_permutation_in_place(sizes, p);
    apply_permutation_in_place(WBRp, p);


    writeFile("wordsSorted.txt", words);
    writeFile("WBRp.bin", WBRp);
    writeFile("lengths.bin", sizes);
    writeFile("points.bin", points);

}