#include <vector>
#include <string>
#include "Repertoire.h"

enum bonuses
{
    None,
    start,
    letterDouble,
    letterTriple,
    wordDouble,
    wordTriple,
};

struct PointAndStart
{
    int point;
    size_t start;
};

struct StartEnd
{
    size_t start;
    size_t end;
};

struct wordPointStartInRowRow
{
    int point;
    size_t start;
    size_t word;
    size_t row;
    bool isCollumn;
};

struct LettersAndRange
{
    std::string letters;
    size_t start;
    size_t end;
};

class Scrabble
{
public:
    Repertoire m_repertoire;
    std::vector<std::string> m_board;
    std::vector<std::string> m_VerticalBoard;

    Scrabble(const std::string& board, const std::string& repertoire);

    void findBestWord(const std::string& letters, const size_t jokerCount);

    void pretyPrint() const;

    static constexpr bonuses board[15][15] =
    {
        {wordTriple, None, None, letterDouble, None, None, None, wordTriple, None, None, None, letterDouble, None, None, wordTriple},
        {None, wordDouble, None, None, None, letterTriple, None, None, None, letterTriple, None, None, None, wordDouble, None},
        {None, None, wordDouble, None, None, None, letterDouble, None, letterDouble, None, None, None, wordDouble, None, None},
        {letterDouble, None, None, wordDouble, None, None, None, letterDouble, None, None, None, wordDouble, None, None, letterDouble},
        {None, None, None, None, wordDouble, None, None, None, None, None, wordDouble, None, None, None, None},
        {None, letterTriple, None, None, None, letterTriple, None, None, None, letterTriple, None, None, None, letterTriple, None},
        {None, None, letterDouble, None, None, None, letterDouble, None, letterDouble, None, None, None, letterDouble, None, None},

        {wordTriple, None, None, letterDouble, None, None, None, start, None, None, None, letterDouble, None, None, wordTriple},


        {None, None, letterDouble, None, None, None, letterDouble, None, letterDouble, None, None, None, letterDouble, None, None},
        {None, letterTriple, None, None, None, letterTriple, None, None, None, letterTriple, None, None, None, letterTriple, None},
        {None, None, None, None, wordDouble, None, None, None, None, None, wordDouble, None, None, None, None},
        {letterDouble, None, None, wordDouble, None, None, None, letterDouble, None, None, None, wordDouble, None, None, letterDouble},
        {None, None, wordDouble, None, None, None, letterDouble, None, letterDouble, None, None, None, wordDouble, None, None},
        {None, wordDouble, None, None, None, letterTriple, None, None, None, letterTriple, None, None, None, wordDouble, None},
        {wordTriple, None, None, letterDouble, None, None, None, wordTriple, None, None, None, letterDouble, None, None, wordTriple},

    };
private:

    std::vector<Repertoire> m_repertoires;

    std::vector<std::vector<LettersAndRange>> m_horizontalGroups;
    std::vector<std::vector<LettersAndRange>> m_verticalGroups;
    std::vector<std::vector<std::regex>> m_verticalRegex;
    std::vector<std::vector<std::regex>> m_horizontalRegex;
    std::vector<std::vector<size_t>> m_verticalSpaces;
    std::vector<std::vector<size_t>> m_horizontalSpaces;

    std::vector<std::vector<std::string>> m_verticalLetters;
    std::vector<std::vector<std::string>> m_horizontalLetters;

    std::vector<std::vector<StartEnd>> m_verticalAncors;
    std::vector<std::vector<StartEnd>> m_horiztalAncors;

    void buildGroups();
    void buildRegex();
    void buildSpaces();
    void buildLetters();
    void buildAncors();

    void findBestWordHelper(const std::string letters, const size_t jokerCount, const std::vector<std::regex> regexes, const std::vector<std::string> lettersOnBoards, const std::vector<std::string> board, const std::vector<std::string> rotatedBoard, const std::vector<LettersAndRange> group, const size_t row, const bool isCollumn, std::vector<wordPointStartInRowRow>& out) const;
    void findBestWordHelper(const std::string row, const std::vector<std::string> rotatedBoard, const std::vector<LettersAndRange> groups, const std::vector<StartEnd> ancors, const std::vector<std::string> words, const std::vector<size_t> wordsIndexes, const size_t rowIndex, const bool isCollumn, std::vector<wordPointStartInRowRow>& out) const;

    PointAndStart points(const std::string& row, const std::vector<std::string>& rotatedBoard, const std::vector<LettersAndRange>& dodo, const matchLocationAndCase& reResult, const std::string& word, const size_t rowIndex) const;

    int pointsHelper(const std::string& row, const std::vector<std::string>& rotatedBoard, const std::string& word, const size_t startOfWordInRow, const size_t rowIndex) const;

    void putain(const std::string& row, const std::vector<std::string>& rotatedBoard, const size_t start, const size_t end, const StartEnd ancor, const std::string& word, const size_t wordIndex, const size_t rowIndex, const bool isCollumn, std::vector<wordPointStartInRowRow>& out) const;
};
