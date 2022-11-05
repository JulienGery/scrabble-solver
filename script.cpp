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
        rp |= 0x80000000 >> (i - 'a');
    }
    return rp;
}

void readFile(const std::string &path, std::vector<std::string> &buffer) {
    std::ifstream file{path};
    if (!file.is_open()) throw std::runtime_error("failed to open file " + path);
    std::copy(
        std::istream_iterator<std::string>(file),
        std::istream_iterator<std::string>(),
          std::back_inserter(buffer)
    );
    file.close();
}

template<class t>
void writeFile(const std::string &path, std::vector<t> &buffer) {
    assert(buffer.size() > 0 && "no data to write");

    std::ofstream out(path, std::ios::binary | std::ios::out);
    out.write((char *)&buffer[0], buffer.size() * sizeof(t));
    out.close();
}

void writeFile(const std::string &path, std::vector<std::string> &buffer) {
    assert(buffer.size() > 0 && "no data to write");

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

int main() {
    std::map<char, uint16_t> values{{'a', 1},{'b', 3},{'c', 3},{'d', 2},{'e', 1}, {'f', 4}, {'g', 2}, {'h', 4}, {'i', 1}, {'j', 8}, {'k', 10}, {'l', 1}, {'m', 2}, {'n', 1}, {'o', 1}, {'p', 3}, {'q', 8}, {'r', 1}, {'s', 1}, {'t', 1}, {'u', 1}, {'v', 4}, {'w', 10},{'x', 10}, {'y', 10}, {'z', 10}};

    std::vector<std::string> words;
    readFile("fr.txt", words);

    const size_t lines = words.size();
    std::vector<uint32_t> strRepresentation(lines, 0);
    std::vector<uint16_t> sizes(lines);
    std::vector<uint16_t> points(lines);

    for (size_t i = 0; i < words.size(); i++) {
        const auto &word = words[i];
        sizes[i] = word.size();
        strRepresentation[i] = convertStrToRepresentation(word);
        
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
    apply_permutation_in_place(strRepresentation, p);


    writeFile("wordsSorted.txt", words);
    writeFile("WBRp.bin", strRepresentation);
    writeFile("lengths.bin", sizes);
    writeFile("points.bin", points);

}