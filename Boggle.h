/*
 * File: Boggle.h
 * --------------
*  Name: Silvia Fernandez (SUNet ID: silviaf)
 * Section leader: Nick Bowman
 * This file declares a set of methods and states instance variables for
 * the class Boggle.
 */
 
#ifndef _boggle_h
#define _boggle_h
#include <iostream>
#include <string>
#include "lexicon.h"
#include "grid.h"
using namespace std;
 
class Boggle {
public:
    Boggle(const Lexicon& dictionary, string boardText = "");
    bool checkWord(string word);
    bool gridContains(char currentChar);
    bool humanWordSearch(string word);
    Set<string> computerWordSearch();
    char getLetter(int row, int col);
    Set<string> getComputerWordsFound() const;
    Set<string> getUserWordsFound() const;
    int getScoreHuman() const;
    int getScoreComputer() const;
    void setComputerWordsFound(Set<string> set);
    void setComputerScore(int score);
    void addtoUserWordsFound(string word);
    void setScoreHuman(int score);
    friend ostream& operator<<(ostream& out, const Boggle& boggle);
 
private:
    // instance variables
    Set<string> userWordsFound;
    Set<string> computerWordsFound;
    Grid<char> board;
    int userScore;
    int computerScore;
    Lexicon dictionary;
    bool humanWordHelper(string word, string currentWord, int r, int c, Grid<bool> &hasBeenSelected, int currentIndex);
    void helperComputer(int r, int c, Grid<bool> &hasBeenSelected, Set<string> &result, const Lexicon &dictionary, string currentWord);
};
 
#endif // _boggle_h
 