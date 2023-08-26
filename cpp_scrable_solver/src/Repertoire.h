#include <cstdint>
#include <string>
#include <vector>
#include <regex>
// #include <map>
#include <ankerl/unordered_dense.h>

struct matchLocationAndCase
{
    size_t cas;
    size_t location;
};

struct WhichMatchAndWords
{
    std::vector<matchLocationAndCase> regexIndexes;
    std::vector<size_t> words;
};

class Repertoire
{
public:
    Repertoire(const std::string& path);

    const std::string& operator[](const size_t i) const;

    bool isInside(const std::string& word) const;
    int points(const std::string& word) const;
    int points(const char c) const;

    std::vector<size_t> filter(const std::string& word, const size_t jokerCount, const std::regex* re, const size_t minimumLength = 0) const;
    WhichMatchAndWords advancedFilter(const std::string& lettersInHand, const size_t jockerCount, const std::vector<std::regex>& res, const std::vector<std::string>& lettersOnBoard) const;


private:
    ankerl::unordered_dense::map<std::string, uint16_t> m_WordsMap;
    // std::map<std::string, uint16_t> m_WordsMap;

    std::string m_Path;
    std::vector<std::string> m_Words;
    std::vector<uint16_t> m_Points;
    std::vector<uint32_t> m_IR;
    std::vector<uint8_t> m_Letters;

};
