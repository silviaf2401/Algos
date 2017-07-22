*
 * File: boggleplay.cpp
 * --------------------------
 * Name: Silvia Fernandez (SUNet ID: silviaf)
 * Section leader: Nick Bowman
 * This file implements the playOneGame function called by the overall Boggle program in assignment
 * 4 in CS 106B. Specifically this file implements all user interaction via the console
 * and console updates to reflect current state of the game for one game of Boggle between a human
 * player and the computer. One game of boggle consists in:
 * - setting up a GRID_SIZE x GRID_SIZE grid of uppercase letters. Letters can be randomly selected
 * or specified by the user.
 * - Human player turn: game prompts human player for words and stores unique 4+ letter English words
 * provided that exist in Lexicon used and that can be formed by chains of neighboring cells in the grid
 * (search for a given word in grid is illustrated via letter highlight.). Score and game
 * status in console and GUI are updated as game progresses.
 * - Computer turn: doing an exhaustive search (recursive backtracking) for all unique 4+ letter
 * English words that the human player did not find. Once exhaustive search is completed, score and game
 * status in console and GUI are updated.
 * - Human and computer score are calculated and the winner (player with highest score) is declared.
 * In case of tie, human player wins.
 * Interaction with user (gathering grid, messaging game status/progress) is done via the console and
 * via a GUI.
 * This file does not implement highlighting and unhighlighting letters in GUI when human player words
 * are being searched in the game grid, this functionality is included in the Boggle class file code.
 * This file does not include any extension functionality. Please refer to files with suffix "extension"
 * for extension functionality.
 */
 
 
#include "lexicon.h"
#include "console.h"
#include "simpio.h"
#include "Boggle.h"
#include "bogglegui.h"
#include <cctype>
 
/* Function prototypes */
void playOneGame(const Lexicon& dictionary);
string getGridChoice();
void initialBoardSetup(Boggle &b);
void updateConsole(const Boggle &b, string message);
void humanTurn(Boggle &b);
void computerTurn(Boggle &b);
void declareWinner(const Boggle &b);
bool validBoard(string customBoard);
 
/* Text to display in console and GUI */
const string RANDOM_BOARD_Q = "Do you want to generate a random board?";
const string YES_NO_QUESTION_ERROR = "Please type a word that begins with \'y\' or \'n\'";
const string TYPE_WORD_PROMPT = "Type a word (or Enter to stop):";
const string INVALID_WORD = "You must enter an unfound 4+ letter word from the dictionary.";
const string PLAYER_TURN_WORDS = "Your words (";
const string PLAYER_TURN_SCORE = "Your score: ";
const string PLAY_AGAIN_Q = "Play again (Y/N)?";
const string YOUR_TURN = "It's your turn!";
const string NOT_IN_BOARD = "That word cannot be found on this board.";
const string FOUND_NEW_WORD = "You found a new word! ";
const string MANUAL_GRID_REQUEST = "Type the 16 letters to appear on the board:";
const string MANUAL_GRID_ERROR = "That is not a valid 16-letter string. Try again.";
const string COMPUTER_TURN = "It's my turn!";
const string COMPUTER_WORDS ="My words (";
const string COMPUTER_SCORE = "My score: ";
const string COMPUTER_WON_TAUNT = "Ha ha ha, I destroyed you. Better luck next time, puny human!";
const string HUMAN_WON_TAUNT = "Mwahaha! I won. Your backtracking was no match for my love of dictionaries.";
 
/*Grid dimensions*/
const int GRID_SIZE = 4;
 
/*Animation delay (ms)*/
const int ANIMATION_DELAY = 100;
 
/** Function: playOneGame
 * Usage: playOneGame(dictionary)
 * -----------------------------------------------------
 * Plays one game of Boggle using the Lexicon passed as reference to check for word validity.
 * Specifically this function gets grid choice from player, initializes GUI and console, lets
 * human player play one turn, lets computer play one turn and declares a winner.
 * @param dictionary - Lexicon type. Lexicon used to check if the words provided by user
 * or found during the search are valid English words.
 */
void playOneGame(const Lexicon& dictionary) {
    BoggleGUI::initialize(GRID_SIZE,GRID_SIZE);
    string customBoard = getGridChoice();
    Boggle b(dictionary, customBoard);
    updateConsole(b, YOUR_TURN);
    initialBoardSetup(b);
    BoggleGUI::setAnimationDelay(ANIMATION_DELAY);
    humanTurn(b);
    BoggleGUI::clearHighlighting();
    computerTurn(b);
    declareWinner(b);
}
 
/** Function: getGridChoice
 * Usage: getGridChoice(dictionary)
 * -----------------------------------------------------
 * Gets grid choice from user, returns an empty string if user wants a random grid
 * or returns a 16-letter string, if the user does not want a random grid.
 * Specifically this function asks the user via console if a random grid is desired for game,
 * and continues to do so until user provides a correct answer.
 * If user does not want a random grid, function then prompts user via the console for a 16-letter
 * string and continues to do so until a 16-letter string is provided.
 * @return customBoard (string): captures user choice for grid. This can be empty (user desires
 * random grid) or can be a 16-letter string (to use for our game grid).
 * Note: string is returned as entered by user when not empty. Any capitalization changes needed for
 * game are handled by Boggle object constructor.
 */
string getGridChoice() {
    string customBoard ="";
    bool wantRandomBoard = getYesOrNo(RANDOM_BOARD_Q, YES_NO_QUESTION_ERROR);
    if (!wantRandomBoard) {
        bool valid = false;
        while (!valid) {
            customBoard = getLine(MANUAL_GRID_REQUEST);
            valid = validBoard(customBoard);
            if (!valid) {
                cout << MANUAL_GRID_ERROR << endl;
            }
        }
    }
    return customBoard;
}
 
/** Function: initialBoardSetup
 * Usage: initialBoardSetup(b)
 * -----------------------------------------------------
 * Initializes Board at the beginning of a game of boggle.
 * Initialization consists in populating the grid in the GUI with letters using the boggle object
 * passed as parameter to this function, and displaying a "Your turn" message.
 * @param: b, Boggle object whose board contents are used to populate the game grid in the GUI.
 */
void initialBoardSetup(Boggle &b) {
    for (int r = 0; r < GRID_SIZE; r++) {
        for (int c = 0; c < GRID_SIZE; c++) {
            BoggleGUI::labelCube(r, c, b.getLetter(r, c), false);
        }
    }
    BoggleGUI::setStatusMessage(YOUR_TURN);
}
 
/** Function: updateConsole
 * Usage: updateConsole(b, message)
 * -----------------------------------------------------
 * Updates the console to reflect game status when human player is playing. Specifically this function
 * will clear the console, display most recent game status which includes a message summarizing
 * result of most recent action/next step, the letter grid, and the human player scores and words found.
 * @param: b, Boggle object which is used display the grid and access human player scores and list
 * of word founds.
 * @param: message (string) summarizing most recent game action/next step (displayed in console)
 */
void updateConsole(const Boggle &b, string message) {
    clearConsole();
    cout << message << endl;
    cout << b << endl;
    cout << PLAYER_TURN_WORDS << b.getUserWordsFound().size() << "): " <<
            b.getUserWordsFound().toString() << endl;
    cout << PLAYER_TURN_SCORE << b.getScoreHuman() << endl;
}
 
/** Function: humanTurn
 * Usage: humanTurn(b)
 * -----------------------------------------------------
 * Plays human player turn. Specifically, until the user presses "enter" instead of entering a word,
 * it promts the user for a word via the console. Once word has been provided, it checks if the word
 * provided is valid (is in the dictionary, is at least four letters long and has not been provided by user
 * yet). If the word is not valid, a message letting the user know that is the case is displayed in
 * the console and in the GUI, and the user is prompted for a new word. If the word is valid, the
 * function checks to see if the word can be formed by a chain of letters in the grid, if that is the case,
 * the word is added to the list of words found by the human player, the human score is updated and
 * the console and GUI messages are updated.
 * @param: b, Boggle object, whose parameters and methods are used to do checks on the words provided
 * by the user, to update the user score and to update the list of words found.
 */
void humanTurn(Boggle &b) {
    while (true) {
        string currentWord = getLine(TYPE_WORD_PROMPT);
        if (currentWord == "") {
            break;
        }
        string currentWordUpper = toUpperCase(currentWord);
        // if word not in dictionary, shorter than 4 letters, or already provided
        if (!b.checkWord(currentWordUpper)) {
            updateConsole(b, INVALID_WORD);
            BoggleGUI::setStatusMessage(INVALID_WORD);
            // if word in dictionary, longer than 3 letters and not already provided
        } else {
            // if the word cannot be formed with this grid: let user know
            if (!b.humanWordSearch(currentWordUpper)) {
                updateConsole(b, NOT_IN_BOARD);
                BoggleGUI::setStatusMessage(NOT_IN_BOARD);
                /* If the word can be formed with this grid add it to the list of
                 * words and update score, update console, then GUI
                 */
            } else {
                b.addtoUserWordsFound(currentWordUpper);
                b.setScoreHuman(b.getScoreHuman() + 1 + currentWord.length() - 4);
                string messagetoPrint = FOUND_NEW_WORD + "\"" + currentWordUpper + "\"";
                updateConsole(b, messagetoPrint);
                BoggleGUI::setStatusMessage(messagetoPrint);
                BoggleGUI::recordWord(currentWordUpper, BoggleGUI::HUMAN);
                BoggleGUI::setScore(b.getScoreHuman(), BoggleGUI::HUMAN);
            }
        }
    }
}
 
/** Function: computerTurn
 * Usage: computerTurn(b)
 * -----------------------------------------------------
 * Plays one computer turn. Specifically it searches for all the unique English 4+ letter words that can be formed
 * in the grid and that have not been found by the human player, it updates the computerWords and computer score instance
 * variables of the Boggle class and updates the console and the GUI messages to reflect the computer's words found and
 * computer's score.
 * @param: b, Boggle object whose grid is used to find all the words for the dictionary's turn, and whose
 * instance variables are updated and are used to update the console and GUI messages.
 */
void computerTurn(Boggle &b) {
    Set<string> computerWords = b.computerWordSearch();
    b.setComputerWordsFound(computerWords);
    int newScore = 0;
    for (string word: computerWords) {
        newScore += 1 + word.length() - 4;
    }
    b.setComputerScore(newScore);
    cout << endl;
    cout << COMPUTER_TURN << endl;
    cout << COMPUTER_WORDS << computerWords.size() << "): " << computerWords.toString() << endl;
    cout << COMPUTER_SCORE << newScore << endl;
    for (string word: computerWords) {
        BoggleGUI::recordWord(word, BoggleGUI::COMPUTER);
    }
    BoggleGUI::setScore(newScore, BoggleGUI::COMPUTER);
}
 
/** Function: declareWinner
 * Usage: declareWinner(b)
 * -----------------------------------------------------
 * Compares human and computer score and declares the winner by displaying a message in the console and in the GUI.
 * Player with highest score wins, in case of a tie, the human player wins.
 * @param: b, Boggle object whose instance variables are used to find a winner.
 */
void declareWinner(const Boggle &b) {
    if (b.getScoreComputer() > b.getScoreHuman()) {
        cout << COMPUTER_WON_TAUNT << endl;
        BoggleGUI::setStatusMessage(COMPUTER_WON_TAUNT);
    } else {
        cout << HUMAN_WON_TAUNT << endl;
        BoggleGUI::setStatusMessage(HUMAN_WON_TAUNT);
    }
}
 
/** Function: validBoard
 * Usage: validBoard(b)
 * -----------------------------------------------------
 * Checks the string passed as parameter is a 16-letter string. Returns true
 * if it is, returns false if it is not.
 * @param: customBoard (string) - word for which we check validity.
 * @return: boolean - true if the string passed as parameter to this function is a 16-letter string,
 * false if not.
 */
bool validBoard(string customBoard) {
    for (int i = 0; i < (int) customBoard.size(); i++) {
        if (!isalpha(customBoard[i])){
            return false;
        }
    }
    if (customBoard.size() != GRID_SIZE * GRID_SIZE ) {
        return false;
    }
    return true;
}
 