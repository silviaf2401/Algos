/*
 * File: boggle.cpp
 * --------------------------
 * Name: Silvia Fernandez (SUNet ID: silviaf)
 * Section leader: Nick Bowman
 * This file implements the Boggle class, which includes the following methods
 * - Boggle object constructor
 * - class instance variables getters and setters
 * - method to check if a word is valid (> 4 letters, found in dictionary, not found previously by user)
 * - method to check if a word can be made by a chain of letters in a boggle object board
 * - method to find all the valid words that can be found in a board and that have not been found
 * previously by the human player.
 * - method to update the list of words found by the user.
 * This file does not contain any extension functionality. Please refer to files with "extension"
 * suffix for extension functionality.
 */
 
#include "Boggle.h"
#include "shuffle.h"
#include "random.h"
#include "bogglegui.h"
 
// letters on all 6 sides of every cube
static string CUBES[16] = {
    "AAEEGN", "ABBJOO", "ACHOPS", "AFFKPS",
    "AOOTTW", "CIMOTU", "DEILRX", "DELRVY",
    "DISTTY", "EEGHNW", "EEINSU", "EHRTVW",
    "EIOSST", "ELRTTY", "HIMNQU", "HLNNRZ"
};
 
// letters on every cube in 5x5 "Big Boggle" version (extension)
static string BIG_BOGGLE_CUBES[25] = {
    "AAAFRS", "AAEEEE", "AAFIRS", "ADENNN", "AEEEEM",
    "AEEGMU", "AEGMNN", "AFIRSY", "BJKQXZ", "CCNSTW",
    "CEIILT", "CEILPT", "CEIPST", "DDLNOR", "DDHNOT",
    "DHHLOR", "DHLNOR", "EIIITT", "EMOTTT", "ENSSSU",
    "FIPRSY", "GORRVW", "HIPRRY", "NOOTUW", "OOOTTU"
};
 
/*Grid dimensions*/
const int GRID_SIZE = 4;
 
/*Minimum word length*/
const int MIN_WORD_LENGTH = 4;
 
/** Method: Boggle
 * Usage: Boggle(dictionary, boardText)
 * -----------------------------------------------------
 * Instantiates one boggle object and sets default value for instance variables.
 * Constructor initializes the Boggle oject board to use the given dictionary
 * lexicon to look up words and uses the string passed as parameter to initialize the board cubes
 * from top left to bottom right. If the string passed as parameter is an empty string, the method
 * generates a random shuffled board. The random shuffled board is made by positioning each of the
 * 16 strings of letter cubes in CUBES randomly first and then selecting a random letter in each of
 * the cubes. Board is populated in upper case.
 * Assumptions: string passed as parameter is a 16 letter string or an empty string.
 * This method is case-insensitive and accepts board text in upper, lower and mixed case.
 * Default vaues for other instance variables are 0 for userScore and computerScore, empty sets
 * for userWordsFound and computerWordsFound, and dictionary for the dictionary.
 * @param dictionary - Lexicon type. Lexicon used to check if the words provided by user
 * or found during word search are valid English words.
 * @param boardText: string type
 */
Boggle::Boggle(const Lexicon& dictionary, string boardText) {
    this->board = Grid<char>(GRID_SIZE, GRID_SIZE);
    shuffle(CUBES, 16);
    int index = 0;
    for (int r = 0; r < GRID_SIZE ; r++) {
        for (int c = 0; c < GRID_SIZE; c++) {
            // random grid generation if empty string passed as parameter
            if (boardText.length() == 0) {
                string cubeLetters = CUBES[GRID_SIZE * r + c];
                int randomNumber = randomInteger(0, 5);
                this->board[r][c] = cubeLetters[randomNumber];
            } else {
                string textboard = toUpperCase(boardText);
                this->board[r][c] = textboard[index];
                index++;
            }
 
         }
     }
 
    this->dictionary = dictionary;
    this->userWordsFound = {};
    this->computerWordsFound = {};
    this->userScore = 0;
    this->computerScore = 0;
}
 
/** Method: getLetter
 * Usage: b.getLetter(row, col)
 * -----------------------------------------------------
 * Returns the character stored in the row and column of the board of the boggle object
 * the method is called on. Throws an int exception if row,col are not within the bounds of the board.
 * @param row - int specifying the row of the position in the Boggle board for which we're returning
 * the character.
 * @param col - int specifying the column of the position in the Boggle board for which we're
 * returning the character.
 * @return board[row][col]: character of the board at position row, col
 */
char Boggle::getLetter(int row, int col) {
    if (!board.inBounds(row, col)) {
        throw -1;
    } else {
        return board[row][col];
    }
}
 
/** Method: checkWord
 * Usage: b.checkWord(word)
 * -----------------------------------------------------
 * Checks if the string word passed as parameter is a suitable word, i.e. if it is in the dictionary,
 * if it is long enough, and if it has not been already found. Returns true if the word is suitable,
 * false if not. Accepts words in upper, lower and mixed case.
 * @param word: type string. Word we're checking the validity for.
 * @return bool: true if the word was valid, false if not
 */
bool Boggle::checkWord(string word) {
    string wordUpperCase = toUpperCase(word);
    return (dictionary.contains(wordUpperCase) && (word.length() >= MIN_WORD_LENGTH) &&
            (!userWordsFound.contains(wordUpperCase)));
}
 
/** Method: humanWordSearch
 * Usage: b.humanWordSearch(word)
 * -----------------------------------------------------
 * Checks to see of an individual word can be formed on the board of the Bogle object this method is
 * called on, by tracing a path through neighoring letters. Two letters are neighbors if they are next
 * to each other horizontally, vertically or diagonally. Each cube can be used at most once in a word.
 * Finds where the word can start, and from each starting point it recursively tries to extend to find
 * the target word "word" with a recursive helper function.
 * Returns true if the word can be formed on the board, false otherwise. Uses recursive backtracking
 * and ahelper function to search for words. As each cube is explored it is highlighted and
 * subsequently unhighlighted if the cube is not part of a successful path to form the target word.
 * Once the word has been found the cubes selected to build it remain highlighted.
 * Assumptions: word passed is in uppercase. Letters in grid are in uppercase.
 * @param word: string type. Word we're searching in the board.
 * @return boolean: true if the word was found in the board, false otherwise.
 */
bool Boggle::humanWordSearch(string word) {
    BoggleGUI::clearHighlighting();
    string currentWord;
    // Keeps track of which cubes have been used
    Grid<bool> hasBeenSelected(GRID_SIZE, GRID_SIZE);
    // Keeps track of position in the word we're considering
    int index = 0;
    // search for first letter
    for (int r = 0; r < board.numRows(); r++) {
        for (int c = 0; c < board.numCols(); c++) {
            BoggleGUI::setHighlighted(r, c, true);
            if (board[r][c] == word[index]) {
                //choose
                hasBeenSelected[r][c] = true;
                currentWord += board[r][c];
                //recurse/explore
                if (humanWordHelper(word, currentWord, r, c, hasBeenSelected, index + 1)) {
                    return true;
                }
                // unchoose/undo changes
                hasBeenSelected[r][c] = false;
                currentWord = currentWord.substr(0, currentWord.length() - 1);
            }
            BoggleGUI::setHighlighted(r, c, false);
        }
    }
    return false;
}
 
/** Method: humanWordHelper
 * Usage: humanWordHelper(word, currentWord, r, c, hasBeelSelected, currentIndex)
 * ---------------------------------------------------------------------------------
 * Recursively tries to extend currentWord to find word, adding letters to currentWord from cubes that
 * have not been used yet and that are neighbors of position r,c.
 * If currentWord is equal to word we were able to find the word in the board and the function returns true.
 * @param word: string type. Word we're searching for in the board.
 * @param currentWord: string type. Word we're buiding by exploring neighoring letters from a given position.
 * @param hasBeenSelected: Grid<bool> type keeping track of cubes that have been used (one cube can be used at most
 * once in a word).
 * @param currentIndex: index of the character from our target word we're looking for in the board.
 * @return boolean: true if the word was found in the board, false otherwise.
 */
bool Boggle::humanWordHelper(string word, string currentWord, int r, int c, Grid<bool> &hasBeenSelected, int currentIndex) {
    if (word == currentWord) {
        return true;
    } else {
        for (int newr = r - 1; newr <= r + 1; newr++) {
            for (int newc = c - 1; newc <= c + 1; newc++) {
                // exclude current cell
                if (newr == r && newc == c) continue;
                if (board.inBounds(newr, newc) && !hasBeenSelected[newr][newc]) {
                    if (board[newr][newc] == word[currentIndex]) {
                        // choose
                        currentWord += board[newr][newc];
                        hasBeenSelected[newr][newc]= true;
                        BoggleGUI::setHighlighted(newr, newc, true);
                        // explore
                        if(humanWordHelper(word, currentWord, newr, newc, hasBeenSelected, currentIndex + 1)) {
                            return true;
                        }
                        //unchoose
                        hasBeenSelected[newr][newc]= false;
                        currentWord = currentWord.substr(0, currentWord.length() - 1);
                        BoggleGUI::setHighlighted(newr, newc, false);
                    }
                }
            }
        }
        return false;
    }
}
 
/** Method: getScoreHuman
 * Usage: b.getScoreHuman()
 * ---------------------------------------------------------------------------------
 * Returns the human score of the Boggle object this method is applied to.
 * @return userScore: type int - human score of the Boggle object this method is applied to.
 */
int Boggle::getScoreHuman() const {
    return userScore;
}
 
/** Method: getUserWordsFound
 * Usage: b.getUserWordsFound()
 * ---------------------------------------------------------------------------------
 * Returns userWordsFound for the Boggle object this method is applied to.
 * @return userWordsFound: type Set<string> - userWordsFound for the Boggle object this method is
 * applied to.
 */
Set<string> Boggle::getUserWordsFound() const {
    return userWordsFound;
}
 
/** Method: getComputerWordsFound
 * Usage: b.getComputerWordsFound()
 * ---------------------------------------------------------------------------------
 * Returns ComputerWordsFound for the Boggle object this method is applied to.
 * @return ComputerWordsFound: type Set<string> - ComputerWordsFound for the Boggle object this method
 * is applied to.
 */
Set<string> Boggle::getComputerWordsFound() const {
    return computerWordsFound;
}
 
/** Method: computerWordSearch
 * Usage: b.computerWordSearch()
 * ---------------------------------------------------------------------------------
 * Runs an exhaustive recursive search on the board of the Boggle object to which this method is applied
 * for all words that can be formed and that have not been found by the human player. Returns the list of
 * all words as a Set of strings.
 * Each cube can be used at most once in a word.
 * Relies on recursive helper function that prunes searches by not exploring partial paths that will
 * not be able to form a valid word.
 */
Set<string> Boggle::computerWordSearch() {
    Grid<bool> hasBeenSelected(GRID_SIZE, GRID_SIZE);
    string currentWord;
    Set<string> result;
    for (int r =0 ; r < board.numRows(); r++) {
        for (int c =0; c < board.numCols(); c++) {
            // choose
            hasBeenSelected[r][c]= true;
            currentWord += board[r][c];
            // explore
            helperComputer(r, c, hasBeenSelected, result, dictionary, currentWord);
            // unchoose
            hasBeenSelected[r][c]= false;
            currentWord = currentWord.substr(0, currentWord.length() - 1);
        }
    }
    return result;
}
 
/** Method: helperComputer
 * Usage: helperComputer(r, c, hasBeenSelected, result, dictionary, currentWord)
 * ---------------------------------------------------------------------------------
 * Recursively searches paths via unselected neighboring cells to find words in dictionary that are of valid length
 * and that have not been found by human user. Prunes searches by not exploring partial paths that will not be
 * able to form a valid word.
 * If a valid word is found it adds it to the set of words passed as parameter to this method.
 * @param: r (type int) row of position for which we're exploring neighboring cubes in board
 * @param: c (type int) column of position for which we're exploring neighboring cubes in board
 * @param: hasBeenSelected (Grid<bool> type): keeps track of the cubes that have already been used
 * @param: result (Set<string>) list of 4+ letter unique english words that can be found in board and that have
 * not been found by human player.
 * @param: dictionary (type Lexicon) - used to check if word found is an English word
 * @param: currentWord (type string) - word that is being built and for which we check validity
 */
void Boggle::helperComputer(int r, int c, Grid<bool> &hasBeenSelected, Set<string> &result, const Lexicon &dictionary,
                            string currentWord) {
    if (dictionary.contains(currentWord) && !userWordsFound.contains(currentWord) && checkWord(currentWord)){
        result.add(currentWord);
        computerScore += 1 + currentWord.length() - MIN_WORD_LENGTH;
    // prune search - we do not explore partial paths that will not be able to form valid word
    } else if (!dictionary.containsPrefix(currentWord)){
       return;
    }
    for (int newr = r - 1; newr <= r + 1; newr++) {
        for (int newc = c - 1; newc <= c + 1; newc++) {
            if (newr == r && newc == c) continue;
            if (board.inBounds(newr, newc) && !hasBeenSelected[newr][newc]) {
                currentWord += board[newr][newc];
                hasBeenSelected[newr][newc]= true;
                helperComputer(newr, newc, hasBeenSelected, result, dictionary, currentWord);
                hasBeenSelected[newr][newc]= false;
                currentWord = currentWord.substr(0, currentWord.length() - 1);
            }
        }
    }
}
 
/** Method: setComputerWordsFound
 * Usage: b.setComputerWordsFound()
 * ---------------------------------------------------------------------------------
 * Sets the computerWordsFound of the Boggle object this method is applied to to the value passed as parameter.
 * @param set: type Set<string> - set of strings which will become the computerWordsFound of the Boggle
 * object this method is applied to.
 */
void Boggle::setComputerWordsFound(Set<string> set) {
    computerWordsFound = set;
}
 
/** Method: getScoreComputer
 * Usage: b.getScoreComputer()
 * ---------------------------------------------------------------------------------
 * Returns the Computer score of the Boggle object this method is applied to.
 * @return userScore: type int - Computer score of the Boggle object this method is applied to.
 */
int Boggle::getScoreComputer() const {
    return computerScore;
}
 
 
/** Method: setComputerScore
 * Usage: b.setComputerScore()
 * ---------------------------------------------------------------------------------
 * Sets the computerScore of the Boggle object this method is applied to to the value passed as parameter.
 * @param score: type int - score which will become the computerScore of the Boggle object this method is
 * applied to.
 */
void Boggle::setComputerScore(int score) {
    computerScore = score;
}
 
/** Method: addtoUserWordsFound
 * Usage: b.addtoUserWordsFound(word)
 * ---------------------------------------------------------------------------------
 * Adds the word passed as parameter to this method to the set of strings userWordsFound
 * of the Boggle object this method is applied to.
 * @param word: type string - word to be added to userWordsFound of the Boggle object
 * to which this method is applied.
 */
void Boggle::addtoUserWordsFound(string word) {
    userWordsFound.add(word);
}
 
/** Method: setScoreHuman
 * Usage: b.setScoreHuman()
 * ---------------------------------------------------------------------------------
 * Sets the userScore of the Boggle object this method is applied to to the value passed as parameter.
 * @param score: type int - score which will become the userScore of the Boggle object this method is
 * applied to.
 */
void Boggle::setScoreHuman(int score) {
    userScore = score;
}
 
/** Method: operator <<
 * Usage: <<
 * ---------------------------------------------------------------------------------
 * << operator for printing a Boggle object to the console as four lines of text, all in uppercase.
 * Assumption: boggle board stores letters in uppercase.
 */
ostream& operator<<(ostream& out, const Boggle& boggle) {
    for (int r = 0; r < boggle.board.numRows(); r++) {
        for (int c = 0 ; c < boggle.board.numCols(); c++) {
            out << boggle.board[r][c];
        }
        out << endl;
    }
    return out;
}
 
 
 
 
 
    
