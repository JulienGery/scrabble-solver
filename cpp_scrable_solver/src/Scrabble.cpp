#include "Scrabble.h"
#include <algorithm>
#include <iostream>
#include <fstream>
#include <iterator>
#include <string>
// #include <execution>
#include "SimpleTimer.h"

auto getColor(bonuses ca)
{
    if(ca == None)
        return "\033[1;37m";
    if(ca == start)
        return "\033[1;35m";
    if(ca == letterDouble)
        return "\033[1;36m";
    if(ca == letterTriple)
        return "\033[1;34m";
    if(ca == wordDouble)
        return "\033[1;33m";

    return "\033[1;31m";
}

void printBoardBonuses(const std::vector<std::vector<bonuses>>& board)
{
    for(const auto& row : board)
    {
        for(const auto& ca: row)
            std::cout << getColor(ca) << '#' << "\033[0m ";
        std::cout << '\n';
    }
}

void pretyPrintHelper(const std::vector<std::string>& board)
{
    std::cout << '\t';
    for(size_t i = 0; i < board[0].size(); i++)
        std::cout << i << ' ';
    std::cout << '\n';

    for(size_t j = 0; j < board.size(); j++)
    {
        std::cout << j << '\t';
        for(size_t i = 0; i < board[j].size(); i++)
            std::cout << getColor(Scrabble::board[j][i]) << board[j][i] << "\033[0m ";
        std::cout << '\n';
    }
    std::cout << '\n';

}

void Scrabble::pretyPrint() const
{
    pretyPrintHelper(m_board);
}

std::vector<std::string> readFile(const std::string& path)
{
    std::ifstream file{path};
    if (!file.is_open())
        throw std::runtime_error("failed to read: " + path);

    const std::regex re("(\\??\\w|#)");
    std::vector<std::string> buffer(15);

    std::string line;
    size_t j = 0;
    while (std::getline(file, line))
    {
        auto words_begin = std::sregex_iterator(line.begin(), line.end(), re);
        auto words_end = std::sregex_iterator();

        for(std::sregex_iterator i = words_begin; i != words_end; i++)
        {
            std::smatch match = *i;
            std::string match_str = match.str();
            buffer[j] += match_str[0];
        }

        j++;
    }

    file.close();
    return buffer;

}


std::vector<LettersAndRange> buildGroupsHelper(const std::string& row)
{
    std::vector<LettersAndRange> ret;
    LettersAndRange buff{};
    buff.letters = "";
    buff.end = row.size() - 1;
    buff.start = 0;

    for(size_t i = 0; i < row.size(); i++)
    {
        if(row[i] != '#')
        {
            if(buff.letters.empty())
                buff.start = i;
            buff.letters += row[i];
        }
        else if (!buff.letters.empty())
        {
            buff.end = i - 1;

            ret.push_back(buff);
            buff = LettersAndRange{"", 0, row.size() - 1};

        }
    }

    if (!buff.letters.empty())
        ret.push_back(buff);

    return ret;
}

std::vector<size_t> buildSpacesHelper(const std::vector<LettersAndRange>& group, const size_t rowSize)
{
    std::vector<size_t> spaces(group.size() + 1);

    size_t start = 0;
    for(size_t i = 0; i < group.size(); i++)
    {
        spaces[i] = group[i].start - start;
        start = group[i].end + 1;
    }

    spaces[spaces.size() - 1] = rowSize - start;
    return spaces;
}


std::regex subRegex(const std::vector<LettersAndRange>& groups, const std::vector<size_t>& spaces, const size_t start, const size_t batchSize)
{
    const int startOffset = start == 0? 0 : -1;

    std::string re = "^";
    if(spaces[start] + startOffset)
        re += "\\w{0," + std::to_string(spaces[start] + startOffset) + '}';
    re += '(' + groups[start].letters + ')';

    for(size_t i = 1; i < batchSize; i++)
        re += "\\w{" + std::to_string(spaces[i + start]) + '}' + groups[i + start].letters;

    const size_t lastIndex = batchSize + start;
    const int offset = lastIndex == spaces.size() - 1? 0 : -1;
    if (spaces[lastIndex] + offset)
        re += "\\w{0," + std::to_string(spaces[lastIndex] + offset) + '}';

    re += "$";

    // std::cout << re << '\n';
    return std::regex(re);
}


std::vector<std::regex> buildRegexHelper(const std::vector<LettersAndRange>& group, const std::vector<size_t>& spaces)
{
    const size_t n = group.size();

    std::vector<std::regex> regexes(n * (n + 1) / 2);

    size_t track = 0;
    for(size_t i = group.size(); i > 0; i--)
        for(size_t j = 0; j <= group.size() - i; j++)
        {
            regexes[track] = subRegex(group, spaces, j, i);
            track++;
        }

    return regexes;
}

void Scrabble::buildGroups()
{
    m_horizontalGroups.resize(m_board.size());
    for(size_t i = 0; i < m_board.size(); i++)
        m_horizontalGroups[i] = buildGroupsHelper(m_board[i]);

    m_verticalGroups.resize(m_VerticalBoard.size());
    for(size_t j = 0; j < m_VerticalBoard.size(); j++)
        m_verticalGroups[j] = buildGroupsHelper(m_VerticalBoard[j]);
}

void Scrabble::buildSpaces()
{
    m_horizontalSpaces.resize(m_horizontalGroups.size());
    for(size_t i = 0; i < m_horizontalGroups.size(); i++)
        m_horizontalSpaces[i] = buildSpacesHelper(m_horizontalGroups[i], m_board[i].size());

    m_verticalSpaces.resize(m_verticalGroups.size());
    for(size_t j = 0; j < m_verticalGroups.size(); j++)
        m_verticalSpaces[j] = buildSpacesHelper(m_verticalGroups[j], m_VerticalBoard[j].size());

}

void Scrabble::buildRegex()
{
    m_horizontalRegex.resize(m_board.size());
    for(size_t i = 0; i < m_board.size(); i++)
        m_horizontalRegex[i] = buildRegexHelper(m_horizontalGroups[i], m_horizontalSpaces[i]);

    m_verticalRegex.resize(m_VerticalBoard.size());
    for(size_t j = 0; j < m_VerticalBoard.size(); j++)
        m_verticalRegex[j] = buildRegexHelper(m_verticalGroups[j], m_verticalSpaces[j]);
}

std::vector<std::string> buildLettersHelper(const std::vector<LettersAndRange>& groups)
{
    std::vector<std::string> ret(groups.size());
    for(size_t i = 0; i < ret.size(); i++)
        ret[i] = groups[i].letters;

    return ret;
}

void Scrabble::buildLetters()
{
    m_horizontalLetters.resize(m_horizontalGroups.size());
    for(size_t i = 0; i < m_horizontalLetters.size(); i++)
        m_horizontalLetters[i] = buildLettersHelper(m_horizontalGroups[i]);

    m_verticalLetters.resize(m_verticalGroups.size());
    for(size_t i = 0; i < m_verticalLetters.size(); i++)
        m_verticalLetters[i] = buildLettersHelper(m_verticalGroups[i]);
}

std::string getWordHelper(std::string row, const size_t startIndex, const size_t insertIndex, const char c)
{
    std::string ret = "";
    ret.reserve(insertIndex - startIndex);
    for(size_t i = startIndex; i < row.size(); i++)
    {
        if(row[i] != '#')
            ret += row[i];
        else if (i == insertIndex)
            ret += c;
        else
            return ret;
    }
    return ret;
}


std::string getWord(const std::string& row, const size_t rowIndex, const char c)
{
    std::string testWord;
    for(int i = rowIndex - 1;; i--)
        if (i == -1 || row[i] == '#')
            return getWordHelper(row, i + 1, rowIndex, c);

}

size_t getBonusValue(const bonuses bonus)
{
    if (bonus == bonuses::None)
        return 1;
    if (bonus == bonuses::wordDouble || bonus == bonuses::letterDouble)
        return 2;
    if (bonus == bonuses::wordTriple || bonus == bonuses::letterTriple)
        return 3;
    //start case
    return 2;
}


int Scrabble::pointsHelper(const std::string& row, const std::vector<std::string>& rotatedBoard, const std::string& word, const size_t startOfWordInRow, const size_t rowIndex) const
{
    size_t letterPlaced = 0;
    size_t indexInWord = 0;
    size_t otherPoints = 0;
    size_t wordPoint = m_repertoire.points(word);
    size_t wordWideMul = 1;

    for(size_t i = startOfWordInRow; i < word.size() + startOfWordInRow; i++)
    {
        if(row[i] == '#')
        {
            letterPlaced++;
            const std::string testWord = getWord(rotatedBoard[i], rowIndex, word[indexInWord]);

            int tmpPoints = m_repertoire.points(testWord);

            if(tmpPoints == -1)
                return -1;

            const bonuses bonus = Scrabble::board[rowIndex][i];
            const size_t charPoint = m_repertoire.points(word[indexInWord]);

            if (bonus == bonuses::wordTriple || bonus == bonuses::wordDouble)
            {
                const size_t mul = getBonusValue(bonus);
                wordWideMul *= mul;
                tmpPoints *= mul;
            }
            else
            {
                const size_t add = (getBonusValue(bonus) - 1) * charPoint;
                wordPoint += add;
                tmpPoints += add;
            }
            otherPoints += testWord.size() > 1 ? tmpPoints : 0;

        }

        indexInWord++;
    }

    if (letterPlaced == 7)
        otherPoints += 50;

    return int(wordPoint * wordWideMul + otherPoints);
}

bool containe(const StartEnd a, const StartEnd b)
{
    if (a.start <= b.start && a.end >= b.end)
        return true;
    return false;
}

//some cases are not handled but it should be fine
bool overlap(const StartEnd a, const StartEnd b)
{
    if (a.start >= b.start && a.start <= b.end)
        return true;

    if (a.end >= b.start && a.end <= b.end)
        return true;

    if(a.start >= b.start && a.end >= b.end)
        return true;

    return false;
}

void Scrabble::putain(const std::string& row, const std::vector<std::string>& rotatedBoard, const size_t start, const size_t end, const StartEnd ancor, const std::string& word, const size_t wordIndex, const size_t rowIndex, const bool isCollumn, std::vector<wordPointStartInRowRow>& out) const
{
    // std::cout << "start: " << start << " end: " << end << '\n';
    for(size_t i = start; i < ancor.end + 1; i++)
    {
        if (i + word.size() > end)
            return;
        if(overlap({i, i + word.size() - 1}, ancor))
        {
            const int point = pointsHelper(row, rotatedBoard, word, i, rowIndex);
            if (point != -1)
                out.push_back({
                        point,
                        i,
                        wordIndex,
                        rowIndex,
                        isCollumn
                        });

        }
    }
}

void Scrabble::findBestWordHelper(const std::string row, const std::vector<std::string> rotatedBoard, const std::vector<LettersAndRange> groups, const std::vector<StartEnd> ancors, const std::vector<std::string> words, const std::vector<size_t> wordsIndexes, const size_t rowIndex, const bool isCollumn, std::vector<wordPointStartInRowRow>& out) const
{
    size_t start = 0;
    for(const auto ancor : ancors)
    {
        for(const auto& group : groups)
        {
            if(containe({start, group.start}, ancor))
            {
                for(size_t i = 0; i < words.size(); i++)
                    putain(row, rotatedBoard, start != 0 ? start + 2 : 0, group.start - 1, ancor, words[i], wordsIndexes[i], rowIndex, isCollumn, out);
            }
            start = group.end;
        }

        if (containe({start, row.size() - 1}, ancor))
        {
            for(size_t i = 0; i < words.size(); i++)
                putain(row, rotatedBoard, start != 0 ? start + 2 : 0, row.size(), ancor, words[i], wordsIndexes[i], rowIndex, isCollumn, out);
        }
    }

}

PointAndStart Scrabble::points(const std::string& row, const std::vector<std::string>& rotatedBoard, const std::vector<LettersAndRange>& group, const matchLocationAndCase& reResult, const std::string& word, const size_t rowIndex) const
{
    size_t index;
    {
        size_t i = 0;
        size_t cas = reResult.cas;
        while (i <= cas)
        {
            cas -= i;
            i++;
        }
        index = cas;
    }

    const size_t startOfWordInRow = group[index].start - reResult.location;

    return {pointsHelper(row, rotatedBoard, word, startOfWordInRow, rowIndex), startOfWordInRow};
}



std::vector<std::string> rotateBoard(const std::vector<std::string>& boardInput)
{
    std::vector<std::string> ret(boardInput[0].size());

    for(size_t j = 0; j < boardInput.size(); j++)
    {
        ret[j].resize(boardInput[j].size());
        for(size_t i = 0; i < boardInput[j].size(); i++)
            ret[j][i] = boardInput[i][j];
    }

    return ret;

}

Scrabble::Scrabble(const std::string& board, const std::string& repertoire) : m_repertoire{repertoire}
{
    m_board = readFile(board);
    m_VerticalBoard = rotateBoard(m_board);
}


void Scrabble::findBestWordHelper(const std::string letters, const size_t jokerCount, const std::vector<std::regex> regexes, const std::vector<std::string> lettersOnBoards, const std::vector<std::string> board, const std::vector<std::string> rotatedBoard, const std::vector<LettersAndRange> group, const size_t row, const bool isCollumn, std::vector<wordPointStartInRowRow>& out) const
{
    const auto candidates = m_repertoire.advancedFilter(letters, jokerCount, regexes, lettersOnBoards);
    for(size_t c = 0; c < candidates.words.size(); c++)
    {
        const PointAndStart point = points(board[row], rotatedBoard, group, candidates.regexIndexes[c], m_repertoire[candidates.words[c]], row);
        if (point.point != -1 )
            out.push_back({
                    point.point,
                    point.start,
                    candidates.words[c],
                    row,
                    isCollumn
                    });
    }


    // std::sort(out.begin(), out.end(), [](const auto a, const auto b){return a.point > b.point;});
}


uint32_t convertRow(const std::string& row)
{
    uint32_t ret = 0;
    for(size_t i = 0; i < row.size(); i++)
        if (row[i] != '#')
            ret |= 1 << i;
    return ret;
}

// 32 not padded but it's ok
uint32_t convertRowPadded(const std::string& row)
{
    uint32_t ret = 0;

    if (row[1] != '#' || row[0] != '#')
        ret |= 1;
    if (row[row.size() - 2] != '#' || row[row.size() - 1] != '#')
        ret |= 1 << (row.size() - 1);

    for(size_t i = 1; i < row.size() - 1; i++)
        if (row[i + 1] != '#' || row[i - 1] != '#' || row[i] != '#')
            ret |= 1 << i;

    return ret;
}

void printBin(const uint32_t a)
{
    for(size_t i = 0; i < 15; i++)
        std::cout << (bool)(a & (1 << i)) << ' ';
    std::cout << '\n';
}

std::vector<StartEnd> getAncorsHelper(const uint32_t _ancors, const size_t rowSize)
{
    std::vector<StartEnd> ancors;
    int start = -1;
    for(size_t i = 0; i < rowSize; i++)
        if (_ancors & (1 << i))
        {
            if (start == -1)
                start = i;

        }
        else if (start != -1)
        {
            ancors.push_back({(size_t)start, i - 1});
            start = -1;
        }

    if (start != -1)
        ancors.push_back({(size_t)start, rowSize - 1});

    return ancors;
}

std::vector<StartEnd> getAncors(const std::string& row, const std::string& nextRow)
{
    const uint32_t rowPaddedIR = convertRowPadded(row);
    const uint32_t nextRowIR = convertRow(nextRow);

    const uint32_t _ancors = ~rowPaddedIR & nextRowIR;

    return getAncorsHelper(_ancors, row.size());
}

std::vector<StartEnd> getAncors(const std::string& row, const std::string& previusRow, const std::string& nextRow)
{
    const uint32_t rowPaddedIR = convertRowPadded(row);
    const uint32_t previusRowIR = convertRow(previusRow);
    const uint32_t nextRowIR = convertRow(nextRow);
    const uint32_t ancors = ~rowPaddedIR & (nextRowIR | previusRowIR);


    return getAncorsHelper(ancors, row.size());
}


void Scrabble::buildAncors()
{
    m_verticalAncors.resize(m_VerticalBoard.size());
    m_horiztalAncors.resize(m_board.size());

    m_horiztalAncors[0] = getAncors(m_board[0], m_board[1]);
    m_verticalAncors[0] = getAncors(m_VerticalBoard[0], m_VerticalBoard[1]);

    for(size_t i = 1; i < m_board.size() - 1; i++)
    {
        m_horiztalAncors[i] = getAncors(m_board[i], m_board[i - 1], m_board[i + 1]);
        m_verticalAncors[i] = getAncors(m_VerticalBoard[i], m_VerticalBoard[i - 1], m_VerticalBoard[i + 1]);
    }

    m_horiztalAncors[m_horiztalAncors.size() - 1] = getAncors(m_board[m_board.size() - 1], m_board[m_board.size() - 2]);
    m_verticalAncors[m_verticalAncors.size() - 1] = getAncors(m_VerticalBoard[m_VerticalBoard.size() - 1], m_VerticalBoard[m_VerticalBoard.size() - 2]);
}

void Scrabble::findBestWord(const std::string& letters, const size_t jokerCount)
{
    buildGroups();
    buildSpaces();
    buildRegex();
    buildLetters();
    buildAncors();

    const std::vector<size_t> wordsIndexes = m_repertoire.filter(letters, jokerCount, nullptr);
    std::vector<std::string> words(wordsIndexes.size());
    for(size_t i = 0; i < words.size(); i++)
        words[i] = m_repertoire[wordsIndexes[i]];

    std::vector<size_t> vec(m_board.size());
    for(size_t i = 0; i < vec.size(); i++)
        vec[i] = i;

    std::vector<wordPointStartInRowRow> final;


    {
        SimpleTimer timer{"find Words"};

        std::for_each(vec.begin(), vec.end(), [&](const size_t i)
                {
                findBestWordHelper(letters, jokerCount, m_horizontalRegex[i], m_horizontalLetters[i], m_board, m_VerticalBoard, m_horizontalGroups[i], i, false, final);
                findBestWordHelper(letters, jokerCount, m_verticalRegex[i], m_verticalLetters[i], m_VerticalBoard, m_board, m_verticalGroups[i], i, true, final);
                findBestWordHelper(m_board[i], m_VerticalBoard, m_horizontalGroups[i], m_horiztalAncors[i], words, wordsIndexes, i, false, final);
                findBestWordHelper(m_VerticalBoard[i], m_board, m_verticalGroups[i], m_verticalAncors[i], words, wordsIndexes, i, true, final);
                });


    }

    std::sort(final.begin(), final.end(), [](const wordPointStartInRowRow a, const wordPointStartInRowRow b){return a.point > b.point;});


    for(size_t i = 0; i < std::min((size_t)200, final.size()); i++)
        std::cout << final[i].point << '\t' << (final[i].isCollumn? "col: " : "row: ") << final[i].row << '\t' << m_repertoire[final[i].word] << '\t' << final[i].start << '\n';

    std::cout << final.size() << '\n';
}
