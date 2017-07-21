/*
 * File: wordladder.cpp
 * -------------------
 * Name: Silvia Fernandez (SUNet ID: silviaf)
 * Section Leader: Nick Bowman
 * This file implements the worldladder game of CS 106B's assignment 2 with extension funtionality.
 * Please refer to submission #10 in paperless for core functionality.
 * Specifically, this game asks the user to provide a text file with a list of words (one word per
 * line) which will become the program's dictionary, then asks for 2 different words of same length
 * that are in that dictionary. If such sequence exists, the program then prints in the console a
 * minimum length word ladder for those two words. A word ladder is a sequence of dictionary words
 * of same length that differ from its neighbors (preceeding them/succeeding them in the ladder) by
 * a letter replacement only. If a ladder is found, this program prints the shortest word ladder from
 * the second word provided by the player to the first word provided by the player. If no ladder is
 * found, a message is displayed in the console to let the user know.
 * The game continues until the user presses "Enter" when prompted for a new word at which point the
 * game exits exits and a closing message is displayed in the console.
 * This file includes the following extension feature:
 * - allow word ladder end points (begining and end of the ladder) to be outside of the dictionary.
 * - messages displayed in the console during using iteraction are adjusted accordingly.
 * --------------------------------------
 */
 
#include <cctype>
#include <cmath>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "console.h"
#include "lexicon.h"
#include "queue.h"
#include "filelib.h"
#include "simpio.h"
#include "strlib.h"
#include "hashset.h"
#include <algorithm>
using namespace std;
 
/* New type definition: holds user input (words) */
struct userInput {
    string wordOne;
    string wordTwo;
};
 
/*Function prototypes */
void displayWelcomeMessage();
void playGame(const Lexicon &dictWords, Vector<string> &allNeighborsecondWord);
userInput getUserWords(const Lexicon &dictWords, Vector<string> &allNeighborsecondWord);
void resetUserWords(userInput &userWords);
void findNeighbors(Vector<string> &allNeighbors, string word, const Lexicon &dictWords);
bool containsWord(Vector<string> allNeighborsecondWord, string word);
 
 
/* Constants: text displayed in console */
const string WELCOME_ONE = "Welcome to CS 106B Word Ladder."; // First line of welcome message.
/* Second line of welcome message. */
const string WELCOME_TWO = "Please give me two words, and I will change the";
/* Third line of welcome message. */
const string WELCOME_THREE = "first into the second by changing one letter at a time.";
const string WELCOME_FOUR = "The words connecting your words will be in the dictionary.";
const string WELCOME_FIVE = "Your words can be outside of the dictionary but need to be of same length.";
const string PROMPT_FILE_NAME = "Dictionary file name?"; // Displayed to prompt user for dictionary file.
/* Message displayed when game exits. */
const string CLOSING_MESSAGE = "Have a nice day.";
/* Message displayed when words of different length entered */
const string NOT_SAME_LENGTH = "The two words must be the same length.";
/* Message displayed when one or 2 words entered are not in dictionary */
const string NOT_IN_DICT = "The two words must be found in the dictionary.";
/* Message displayed when user enters identical words.*/
const string EQUAL_WORDS = "The two words must be different.";
const string ENTER_WORD_ONE = "Word #1 (or Enter to quit): "; // Prompts user for first word.
const string ENTER_WORD_TWO = "Word #2 (or Enter to quit): "; // Prompts user for second word.
 
 
int main() {
    displayWelcomeMessage();
    string dictFile = promptUserForFile(PROMPT_FILE_NAME);
    cout << endl;
    Vector<string> allNeighborsecondWord;
    Lexicon dictWords(dictFile);
    playGame(dictWords, allNeighborsecondWord);
    cout << CLOSING_MESSAGE << endl;
    return 0;
}
 
/*
 * Function: displayWelcomeMessage
 * Usage: displayWelcomeMessage();
 * -------------------------------------
 * This function displays 3 lines of a welcome message in the console to explain to the user what the game
 * consists in, followed by an empty line.
 */
void displayWelcomeMessage() {
    cout << WELCOME_ONE << endl;
    cout << WELCOME_TWO << endl;
    cout << WELCOME_THREE << endl;
    cout << WELCOME_FOUR << endl;
    cout << WELCOME_FIVE << endl << endl;
}
 
/*
 * Function: getUserWords
 * Usage: getUserWords(dictWords);
 * -------------------------------------
 * Prompts the user via console for 2 words and stores them in lowercase in userWords(userInput type)
 * as wordOne and wordTwo parameters. These will be used for the end and beginning of the word ladder
 * respectively (if ladder exists).
 * Displays a console error message & re-prompts user for two new words if the words provided
 * are not different words of same length in dictWords(Lexicon type) passed as parameter to this
 * function. If the user enters "Enter" for either of the words, the function returns.
 * This function takes one parameter: dictWords, of Lexicon type, containing the list of valid words.
 * This function returns userWords (type userInput) which contains the words provided by user
 * for the ladder as wordOne and wordTwo parameters (or an empty string if user pressed enter).
 * This function is not case sensitive to user input. Assumption: words in dictWords are in lowercase.
 */
userInput getUserWords(const Lexicon &dictWords, Vector<string> &allNeighborsecondWord) {
    userInput userWords;
    resetUserWords(userWords);
    while (userWords.wordOne.length() == 0 && userWords.wordTwo.length() == 0) {
        userWords.wordOne = toLowerCase(getLine(ENTER_WORD_ONE));
        if (userWords.wordOne == "") {
            return userWords;
        }
        userWords.wordTwo = toLowerCase(getLine(ENTER_WORD_TWO));
        if (userWords.wordTwo == "") {
            return userWords;
        }
        if (userWords.wordOne.length() != userWords.wordTwo.length()) {
            cout << NOT_SAME_LENGTH << endl << endl;
            resetUserWords(userWords);
        } else if (!dictWords.contains(userWords.wordOne)) {
            Vector<string> allNeighborsfirstWord;
            findNeighbors(allNeighborsfirstWord, userWords.wordOne, dictWords);
            if (allNeighborsfirstWord.size() == 0) {
                cout << "The first word you entered has no direct word neighbors in our dictionary. Please try again." << endl;
                resetUserWords(userWords);
             }
        } else if (!dictWords.contains(userWords.wordTwo)) {
            findNeighbors(allNeighborsecondWord, userWords.wordTwo, dictWords);
            if (allNeighborsecondWord.size() == 0) {
                cout << "The second word you entered has no direct word neighbors in our dictionary. Please try again." << endl;
                resetUserWords(userWords);
             }
        } else {
            if (userWords.wordOne == userWords.wordTwo) {
                cout << EQUAL_WORDS << endl << endl;
                resetUserWords(userWords);
            }
        }
    }
    return userWords;
}
 
/* Function: resetUserWords
 * Usage: resetUserWords(userWords)
 * -----------------------------------
 * This function sets the wordOne and wordTwo parameters of the userInput variable
 * passed as reference parameter to this method to the empty string.
 */
void resetUserWords(userInput &userWords) {
    userWords.wordOne = "";
    userWords.wordTwo = "";
}
bool containsWord(Vector<string> allNeighborsecondWord, string word) {
    return (find(allNeighborsecondWord.begin(),allNeighborsecondWord.end(),word) != allNeighborsecondWord.end());
}
 
/*
 * Function: playGame
 * Usage: playGame(dictWords);
 * -------------------------------------
 * Plays the word ladder game. Until the user presses "Enter" as word input, program promtps
 * user via the console to enter 2 different words of the same length and in dictWords passed
 * as parameter. Once a correct set of words has been entered by the user, the program will find
 * the shortest word ladder between the 2 words (if it exists) using a breadth first search algorithm.
 * This function ignores any word included in a previous shorter ladder preventing us from building
 * a circular program. Once a shortest word ladder has been found, it is printed backwards in the
 * console (second word provided by user to first word provided by user).
 * If no word ladder has been found, a message stating that is displayed in the console.
 * This function takes a parameter dictWords, list of valid words.
 * Assumption: all words in dictWords are in lowercase.
 */
void playGame(const Lexicon &dictWords, Vector<string> &allNeighborsecondWord) {
    bool foundLadder;
    while(true){
        foundLadder = false;
        userInput wordsProvided = getUserWords(dictWords, allNeighborsecondWord);
        if (wordsProvided.wordOne == "" || wordsProvided.wordTwo == "") {
            return;
        }
        // Store partial ladders representing all possibilities to explore
        Queue<Stack<string>> q;
        // Temporary stack holding partial ladder with first word entered by user.
        Stack<string> tempStack;
        tempStack.add(wordsProvided.wordOne);
        q.enqueue(tempStack);
        // List of words previously used
        HashSet<string> wordsUsed;
        wordsUsed.add(wordsProvided.wordOne);
        while (q.size() > 0 && !foundLadder) {
            Stack<string> tempLadder = q.dequeue();
            Vector<string> allNeighbors;
            findNeighbors(allNeighbors, tempLadder.peek(), dictWords);
            for (string word : allNeighbors) {
                if (!wordsUsed.contains(word) && !foundLadder) {
                    wordsUsed.add(word);
                    // case where wordTwo is in the dictionary
                    if (word == wordsProvided.wordTwo) {
                        foundLadder = true;
                        tempLadder.push(word);
                        cout << "A ladder from " << wordsProvided.wordTwo <<
                                " back to " << wordsProvided.wordOne << ":" << endl;
                        while (tempLadder.size() > 0) {
                            cout << tempLadder.pop() << " ";
                        }
                        cout << endl;
                        cout << endl;
                    } else if (containsWord(allNeighborsecondWord,word) && !dictWords.contains(wordsProvided.wordTwo)) {
                        foundLadder = true;
                        tempLadder.push(word);
                        tempLadder.push(wordsProvided.wordTwo);
                        cout << "A ladder from " << wordsProvided.wordTwo <<
                                " back to " << wordsProvided.wordOne << ":" << endl;
                        while (tempLadder.size() > 0) {
                            cout << tempLadder.pop() << " ";
                        }
                        cout << endl;
                        cout << endl;
                    } else {
                        Stack<string> tempLadderUpdate = tempLadder;
                        tempLadderUpdate.push(word);
                        q.enqueue(tempLadderUpdate);
                    }
                }
            }
 
        }
        if (!foundLadder) {
            cout << "No word ladder found from " << wordsProvided.wordTwo << " back to "
                 << wordsProvided.wordOne << ". " << endl;
            cout << endl;
        }
    }
 
}
 
/*
 * Function: findNeighbors
 * Usage: findNeighbors(allNeighbors, word, dictWords);
 * -------------------------------------
 * Finds all the words in dictWords lexicon(passed by reference) that are neighbors of the string
 * passed as parameter to the function and stores them in allNeighbors(passed as parameter).
 * A word's neighbor is of same length and differs from it by a single letter.
 * To do so it creates a new neighbor candidate by replacing each single letter of the string word
 * passed as parameter by each of the letters in the alphabet (one letter change at a time)
 * and checks if the neighbor candidate is in dictWords. If it is, it is added to allNeighbors.
 * This method takes 3 parameters:
 * - dictWords: Lexicon used to make sure the neighbor is in the dictionary used in this program.
 * - allNeighbors: Vector of strings holding all valid neighbors of the string passed as parameter
 * to this method.
 * - word: string; word for which we're trying to find neighbors in this function.
 * Assumption: all the words in dictWords and word are lowercase.
 */
void findNeighbors(Vector<string> &allNeighbors, string word, const Lexicon &dictWords) {
    string candidateWord;
    for (int i = 0; i < (int) word.length(); i++) {
        for (char c = 'a'; c <= 'z'; c++) {
            candidateWord = word.substr(0, i) + c + word.substr(i + 1);
            if (dictWords.contains(candidateWord)) {
                allNeighbors.add(candidateWord);
            }
        }
    }
}
 
 
 
 
 