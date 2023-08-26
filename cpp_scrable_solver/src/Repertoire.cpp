#include "Repertoire.h"
#include "SimpleTimer.h"

#include <cctype>
#include <fstream>
#include <iterator>
#include <iostream>
#include <algorithm>
#include <regex>

std::vector<std::string> readFileP(const std::string& path)
{
    std::ifstream file{path};
    if (!file.is_open())
        throw std::runtime_error("failed to read: " + path);

    std::vector<std::string> buffer;
    buffer.reserve(411430 + 1);

    std::copy(std::istream_iterator<std::string>(file), std::istream_iterator<std::string>(), std::back_inserter(buffer));

    file.close();
    return buffer;
}

uint16_t getPoint(const std::string& word)
{
    const uint8_t values[] = {1, 3, 3, 2, 1, 4, 2, 4, 1, 8, 10, 1, 2, 1, 1, 3, 8, 1, 1, 1, 1, 4, 10, 10, 10, 10};

    uint32_t value = 0;

    for(size_t i = 0; i < word.length(); i++)
        value += values[word[i] - 'A'];

    return value;
}

uint16_t getPoint(const char c)
{
    const uint8_t values[] = {1, 3, 3, 2, 1, 4, 2, 4, 1, 8, 10, 1, 2, 1, 1, 3, 8, 1, 1, 1, 1, 4, 10, 10, 10, 10};
    return values[c - 'A'];
}

void getLetters(const std::string& word, uint8_t* letters)
{
    for(size_t i = 0; i < word.length(); i++)
        letters[word[i] - 'A']++;
}

void getLetters(const std::string& lettersInHand, const std::vector<std::string>& otherLetters, const size_t start, const size_t size, uint8_t* letters)
{
    getLetters(lettersInHand, letters);
    for(size_t i = 0; i < size; i++)
        getLetters(otherLetters[i + start], letters);
}

size_t sizesHelper(const std::vector<std::string>& otherLetters, const size_t start, const size_t size)
{
    size_t ret = 0;
    for(size_t i = 0; i < size; i++)
        ret += otherLetters[i + start].size();
    return ret;
}

bool isFeasible(const uint8_t* test, const uint8_t* against, int jokerCount)
{
    for(size_t i = 0; i < 26; i++)
    {
        const int dif = test[i] - against[i];
        if(dif < 0)
        {
            jokerCount += dif;
            if(jokerCount < 0)
                return false;
        }
    }

    return true;
}


uint32_t IR(const std::string& word)
{
    uint32_t ir = 0;
    for(size_t i = 0; i < word.length(); i++)
        ir |= 0x80000000 >> (word[i] - 'A');

    return ir;
}


template<typename T, typename Compare>
std::vector<size_t> sort_permutation(const std::vector<T>& vec, Compare& compare)
{
    std::vector<size_t> perm(vec.size());

    for(size_t i = 0; i < perm.size(); i++)
        perm[i] = i;

    std::sort(perm.begin(), perm.end(),
            [&](size_t i, size_t j){ return compare(vec[i], vec[j]);});

    return perm;
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




//init repertoire
Repertoire::Repertoire(const std::string& path)
{
    m_Path = path;
    {
        SimpleTimer timer("read dictionary");
        m_Words = readFileP(path);
    }

    {
        m_Points.resize(m_Words.size());
        SimpleTimer timer{"get points"};
        for(size_t i = 0; i < m_Words.size(); i++)
            m_Points[i] = getPoint(m_Words[i]);
    }

   //  const std::vector<size_t> perm = sort_permutation(m_Points,
   //          *[](const size_t &a, const size_t &b){ return a > b;});
   //
   //
   // apply_permutation_in_place(m_Points, perm);
   // apply_permutation_in_place(m_Words, perm);

    {
        SimpleTimer timer("IR");
        m_IR.resize(m_Words.size());
        for(size_t i = 0; i < m_Words.size(); i++)
            m_IR[i] = IR(m_Words[i]);
    }

    {
        SimpleTimer timer("letters");
        m_Letters.resize(m_Words.size() * 26);
        for(size_t i = 0; i < m_Words.size(); i++)
            getLetters(m_Words[i], &m_Letters[i * 26]);
    }

    {
        SimpleTimer timer("hashmap");
        for(size_t i = 0; i < m_Words.size(); i++)
            m_WordsMap[m_Words[i]] = m_Points[i];
    }
}

const std::string& Repertoire::operator[](const size_t i) const {
    return m_Words[i];
}


bool Repertoire::isInside(const std::string& word) const
{
    if(m_WordsMap.count(word))
        return true;
    return false;
}

int Repertoire::points(const std::string& word) const
{
    if (word.size() == 1)
        return getPoint(word);
    else if (m_WordsMap.count(word))
        return m_WordsMap.at(word);
    return -1;
}

int Repertoire::points(const char c) const
{
    return getPoint(c);
}


void lettersInside(const uint32_t ir)
{
    std::cout << "dans ce mot il y a:";
    for(size_t i = 0; i < 26; i++)
    {
        if (ir & (0x80000000 >> (32 - i)))
            std::cout << (char)('A' + i) << ' ';
    }
    std::cout << '\n';
}

//minimum length exclusive
std::vector<size_t> Repertoire::filter(const std::string& word, const size_t jokerCount, const std::regex* reMustMatch, const size_t minimumLength) const
{
    std::vector<size_t> ret(m_Words.size());

    const uint32_t wordIR = IR(word);
    const size_t wordLength = word.length() + jokerCount;


    if(jokerCount == 0)
    {
        size_t valide = 0;
        for(size_t i = 0; i < m_IR.size(); i++)
            if(!(~(~m_IR[i] | wordIR)) && wordLength >= m_Words[i].length() && m_Words[i].length() > minimumLength)
            {
                ret[valide] = i;
                valide++;
            }

        ret.resize(valide);
    }
    else
    {
        size_t valide = 0;
        for(size_t i = 0; i < m_IR.size(); i++)
            if(wordLength >= m_Words[i].length())
            {
                uint32_t tmp = ~(~m_IR[i] | wordIR);
                for(size_t j = 0; j < jokerCount; j++)
                    tmp &= (tmp - 1);

                if(!tmp)
                {
                    ret[valide] = i;
                    valide++;
                }
            }

        ret.resize(valide);
    }

    uint8_t letters[26] = {0};
    getLetters(word, letters);

    std::smatch match;
    if (reMustMatch)
    {
        size_t valide = 0;
        for(const size_t i : ret)
        {
            if (std::regex_search(m_Words[i], match, *reMustMatch))
            {
                ret[valide] = i;
                valide++;
            }
        }

        ret.resize(valide);
    }

    size_t valide = 0;
    for(const size_t i : ret)
    {
        if(isFeasible(letters, &m_Letters[i * 26], jokerCount))
        {
            ret[valide] = i;
            valide++;
        }
    }

    ret.resize(valide);

    return ret;
}

WhichMatchAndWords Repertoire::advancedFilter(const std::string& lettersInHand, const size_t jokerCount, const std::vector<std::regex>& res, const std::vector<std::string>& lettersOnboard) const
{
    std::string allLetters = lettersInHand;
    for(size_t i = 0; i < lettersOnboard.size(); i++)
        allLetters += lettersOnboard[i];

    const size_t wordLength = allLetters.size() + jokerCount;

    std::vector<size_t> ret(m_Words.size());
    const uint32_t wordIR = IR(allLetters);


    const size_t n = lettersOnboard.size();
    std::vector<size_t> sizes(n * (n + 1) / 2);
    std::vector<uint8_t> attention(n * (n + 1) / 2 * 26);
    size_t track = 0;
    for(size_t i = lettersOnboard.size(); i > 0; i--)
        for(size_t j = 0; j <= lettersOnboard.size() - i; j++)
        {
            getLetters(lettersInHand, lettersOnboard, j, i, &attention[track * 26]);
            sizes[track] = sizesHelper(lettersOnboard, j, i);
            track++;
        }

    if(jokerCount == 0)
    {
        size_t valide = 0;
        for(size_t i = 0; i < m_IR.size(); i++)
            if(!(~(~m_IR[i] | wordIR)) && wordLength >= m_Words[i].length())
            {
                ret[valide] = i;
                valide++;
            }

        ret.resize(valide);
    }
    else
    {
        size_t valide = 0;
        for(size_t i = 0; i < m_IR.size(); i++)
            if(wordLength >= m_Words[i].length())
            {
                uint32_t tmp = ~(~m_IR[i] | wordIR);
                for(size_t j = 0; j < jokerCount; j++)
                    tmp &= (tmp - 1);

                if(!tmp)
                {
                    ret[valide] = i;
                    valide++;
                }
            }

        ret.resize(valide);
    }

    size_t valide = 0;
    std::vector<matchLocationAndCase> regexMatch(ret.size());
    std::smatch match;
    std::cmatch cmatch;
    for(const auto i : ret)
    {
        for(size_t j = 0; j < res.size(); j++)
        {
            if(m_Words[i].length() > sizes[j] && isFeasible(&attention[j * 26], &m_Letters[i * 26], jokerCount) && std::regex_match(m_Words[i], match, res[j]))
            {
                //rework here one word may match multiples regexes
                //
                const size_t maxMatch = match.position(1);
                //
                // if (std::regex_match(m_Words[i].data(), m_Words[i].data() + maxMatch, cmatch, res[j]))
                // {
                //     std::string wordCopy = m_Words[i];
                //     wordCopy[maxMatch] = std::tolower(wordCopy[maxMatch]);
                //     std::cout << "could be valide: " << wordCopy << ' ' << cmatch.position(1) << '\n';
                //     if(std::regex_match(wordCopy, match, res[j]))
                //         std::cout << "valide!!: " << match.position(1) << '\n';
                //
                // }

                regexMatch[valide] = {j, maxMatch};
                ret[valide] = i;
                valide++;
                break;
            }
        }

    }

    regexMatch.resize(valide);

    ret.resize(valide);
    return { regexMatch, ret };
}
