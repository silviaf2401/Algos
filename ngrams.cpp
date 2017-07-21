/*
 * File: ngrams.cpp
 * -------------------
 * Name: Silvia Fernandez (SUNet ID: silviaf)
 * Section Leader: Nick Bowman
 * This file implements the Random Writer (N-Grams) part of assignment 2 in CS 106B, including
 * extension functionality.
 * Specifically, this program reads an input file and uses it to build random text
 * composed of word groups of N words, N-Grams, and that sounds like it came from the same
 * author as the original text file. The text input file is provided by the user, the 'N' value for
 * groups of words used when generating the random text, and the total number of words to be printed
 * are also provided by user. Interaction with user (welcome message, input request and output and
 * error message printing) occurs via the console. Incorrect user input is messaged to user,
 * who is reprompted for input until correctly entered.
 * After an original input file and N value for groups of words is provided, program promtps
 * user for number of words to print and prints generated text in the console until the user
 * enters "0" for the number of words to print, in which case the program exits and a closing
 * message is displayed.
 * This file implements the following extension functionality: making the N-grams be complete
 * sentences: when we are creating our map of N-1 word prefixes, we keep track of which prefixes
 * are at the start of a sentence (begin with uppercase letter) and which words are at the end of
 * a sentence (end with period, question mark or exclamation mark). We use this information to begin
 * randomly generated text with a random sentence starter, and instead of generating the exact number
 * of words requested by the user, if we are not a the end of a sentence,
 * we keep adding words to the sentence until we end the sentence. A message is displayed before the text
 * is printed to let the user know we printed a different number of words (and the length).
 * --------------------------------------
 */
 
#include <cctype>
#include <cmath>
#include <fstream>
#include <iostream>
#include <string>
#include "console.h"
#include "simpio.h"
#include "vector.h"
#include "map.h"
#include "hashmap.h"
#include "filelib.h"
#include "random.h"
#include "strlib.h"
 
using namespace std;
 
/* Holds program run characteristics: filename of file containing seed text, size of NGrams, number
 * of words to display, all provided by user. */
struct userInput {
    string fileName;
    int valueN;
    int countWordsDisplayed;
};
 
/* Function prototypes */
void displayWelcomeMessage();
void generateNgrams();
void importSeedText(Vector<string> &allWords, const userInput &userAnswer, Vector<int> &indexBegSentence);
userInput getUserInput();
void wrapSeedText (Vector<string> &allWords, const userInput &userAnswer);
void getCountWordsToPrint(userInput &userAnswer);
void buildMap(HashMap<Vector<string>, Vector<string>> &map, const Vector<string> &allWords,
              const userInput &userAnswer, const int origSize);
string buildStringToPrint(const userInput &userAnswer, const Vector<string> &allWords,
                          const int origSize, HashMap<Vector<string>, Vector<string>> &map, const Vector<int> &indexBegSentence);
 
/* Text to display on console */
const string WELCOME_ONE = "Welcome to CS 106B Random Writer ('N-Grams')."; // Welcome message line 1
/* Welcome message line 2 */
const string WELCOME_TWO = "This program makes random text based on a document.";
/* Welcome message line 3 */
const string WELCOME_THREE = "Give me an input file and an \'N\' value for groups";
const string WELCOME_FOUR = "of words, and I'll create random text for you."; // Welcome message line 4
const string EXIT = "Exiting"; // Displayed when user exits game.
const string PROMPT_FILE = "Input file name?"; // Question requesting input file name from user.
const string PROMPT_N = "Value of N?"; // Question requesting N from user.
const string N_ERROR = "N must be 2 or greater."; // Error displayed if N value entered is too small.
/* Prompts user for number of words to generate */
const string COUNT_WORDS_GENERATE = "# of random words to generate (0 to quit)?";
 
/* Game constants */
/* Value which, when entered by user when prompted for count of words to display, causes the game to exit. */
const int QUIT_VALUE = 0;
const int MIN_NGRAM_LENGTH = 2; // Miminum N-Gram length.
 
/* Main program */
int main() {
    displayWelcomeMessage();
    generateNgrams();
    cout << EXIT << endl;
    return 0;
}
 
/*
 * Function: displayWelcomeMessage
 * Usage: displayWelcomeMessage();
 * -------------------------------------
 * Prints in the console a welcome message: four lines of text, followed by an empty line.
 */
void displayWelcomeMessage(){
    cout << WELCOME_ONE << endl << WELCOME_TWO << endl << WELCOME_THREE << endl
         << WELCOME_FOUR << endl << endl;
}
 
/*
 * Function: generateNgrams()
 * Usage: generateNgrams();
 * -------------------------------------
 * This function generates N-Grams as specified by the header of this file.
 * It requests input by the user (filename of seed text to import and N value for size of groups
 * of words used to generate random text starting from seed text), then continues to ask user
 * for the count of words to print and prints those words until user enters 0 for that count.
 * In this case the function returns.
 *
 */
void generateNgrams() {
    userInput userAnswer = getUserInput();
    cout << endl;
    // vector to store all words in file
    Vector<string> allWords;
    Vector<int> indexBegSentence;
    importSeedText(allWords, userAnswer, indexBegSentence);
    int origSize = allWords.size();
    wrapSeedText(allWords, userAnswer);
    HashMap<Vector<string>, Vector<string>> map;
    buildMap(map, allWords, userAnswer, origSize);
    while (true) {
        getCountWordsToPrint(userAnswer);
        if (userAnswer.countWordsDisplayed == 0) {
            return;
        }
        string toPrint = buildStringToPrint(userAnswer, allWords, origSize, map, indexBegSentence);
        cout << toPrint << endl << endl;
        userAnswer.countWordsDisplayed = -1;
    }
}
 
/*
 * Function: getUserInput
 * Usage: getuserInput();
 * -------------------------------------
 * This method prompts user via console for a file name with seed text that will serve as start for random
 * text generation and for the length N (integer) of the sequence of words to use during random text
 * generation. Displays an error message in the console and reprompts user until correct input is
 * provided: file needs to be found and N is at least of MIN_NGRAM_LENGTH). Returns a userInput
 * type which holds the file name provided by the user in its fileName parameter and the length of the
 * sequence of words to use during random text generation (provided by user) in its valueN parameter.
 * At this point the countWordsDisplayed parameter is initialized to -1 so that we have control over what
 * that parameter stores until it stores user-provided input.
 */
userInput getUserInput() {
    userInput userAnswer;
    userAnswer.fileName = promptUserForFile(PROMPT_FILE);
    // Initialize userAnswer.valueN to be smaller than MIN_NGRAM_LENGTH
    userAnswer.valueN = MIN_NGRAM_LENGTH - 1;
    while (userAnswer.valueN < MIN_NGRAM_LENGTH) {
        userAnswer.valueN = getInteger(PROMPT_N);
        if (userAnswer.valueN < MIN_NGRAM_LENGTH) {
            cout << N_ERROR << endl;
        }
    }
    userAnswer.countWordsDisplayed = -1;
    return userAnswer;
}
 
/*
 * Function: importSeedText
 * Usage: importSeedText(allWords, userAnswer);
 * -------------------------------------
 * Opens the file of name provided by userAnswer.filename's parameter and stores each word read
 * in it in the allWords object.  The text in this file will be used as seed to generate random text
 * in the program.
 * Takes two parameters:
 * - userAnswer (userInput type): contains the filename to open and use to populate allWords.
 * - allWords (Vector<string> type): stores all the words from userAnswer file in it.
 * Assumption: filename stored in userInput exists.
 */
void importSeedText(Vector<string> &allWords, const userInput &userAnswer, Vector<int> &indexBegSentence) {
    string word;
    ifstream filestream;
    openFile(filestream, userAnswer.fileName);
    int index = 0;
    while (filestream >> word) {
        allWords.add(word);
        char temp = word[0];
        if (isalpha(temp) && temp == toupper(word[0])) {
            indexBegSentence.add(index);
        }
        index++;
    }
}
 
/*
 * Function: wrapSeedText
 * Usage: wrapSeedText(allWords, userAnswer);
 * -------------------------------------
 * Modifies allWords to address text wrapping in this program. Appends the first N - 1
 * words of allWords to the end of allWords. N is the number for the length of groups
 * of words which was entered by the user in the console. Takes 2 parameters:
 * - allWords (type Vector<string>): contains all the words read from the seed text file.
 * At the end of this method its length will have increased by userAnswer.valueN - 1.
 * - userAnswer (type userInput): its userAnswer.valueN parameter contains the number of groups of words
 * entered by the user in the console. Remains unchanged.
 */
void wrapSeedText (Vector<string> &allWords, const userInput &userAnswer) {
    string temp;
    for (int i = 0; i < userAnswer.valueN - 1; i++) {
        temp = allWords[i];
        allWords.add(temp);
    }
}
 
/*
 * Function: buildMap
 * Usage: buildMap(map, allWords, userAnswer, origSize);
 * --------------------------------------------------------------
 * Builds a map of N-grams, ie of all the possible word windows(type Vector<string>) of size N -1 (N provided by
 * user input) in the seed text stored in allWords to the list (type Vector<string>)
 * of all the next word that immediately follow each occurrence of that window in the seed text (with
 * wrapping). The list of words is of type Vector<string> and can contain duplicate words.
 * What we mean by wrapping is that any word in the seed text file can be at the beginning of the
 * window selected, and thus, if when selecting the beginning window, we are at the end of the text
 * file, we take any additional words needed to build our map from the beginning of the seed text file.
 * This method takes 4 parameters:
 * - map ((type HashMap<Vector<string>, Vector<string>>): stores the map defined above.
 * - allWords (type Vector<string>): where we search for all the possible word windows
 * of size N-1. Remains unchanged.
 * - userAnswer (type userInput): its valueN parameter stores the size (+1) of the word windows
 *  that are map keys. Remains unchanged.
 * - origSize (type int): count of words in seed text file. This constants allow us to traverse
 * allWords to the correct index to find word windows and matching words for our map.
 * Remains unchanged.
 */
void buildMap(HashMap<Vector<string>, Vector<string>> &map, const Vector<string> &allWords,
              const userInput &userAnswer, const int origSize) {
    for (int i  = 0; i < origSize; i++) {
        Vector<string> sub = allWords.subList(i, userAnswer.valueN - 1);
        if (map.containsKey(sub)) {
            Vector<string> temp = map.get(sub);
            string toAdd = allWords[i + userAnswer.valueN - 1];
            temp.add(toAdd);
            map.put(sub, temp);
        } else {
            Vector<string> tempTwo;
            tempTwo.add(allWords[i + userAnswer.valueN - 1]);
            map.put(sub, tempTwo);
        }
    }
}
 
/*
 * Function: getCountWordsToPrint
 * Usage: getCountWordsToPrint(userAnswer);
 * --------------------------------------------------------------
 * Prompts user for the number of words to print in the console and stores it in userAnswer's
 * countWordsDisplayed parameter. If user enters 0, function returns, if used enters a value
 * smaller than the value N (size of group of words used to generate random text,
 * also provided by user) the function prints an error message and reprompts user to provide
 * another answer. Takes as parameter userAnswer (type userInput) which contains in its valueN
 * parameter the size of groups of words specified by user.
 */
void getCountWordsToPrint(userInput &userAnswer) {
    while (userAnswer.countWordsDisplayed < userAnswer.valueN) {
        userAnswer.countWordsDisplayed = getInteger(COUNT_WORDS_GENERATE);
        if (userAnswer.countWordsDisplayed == QUIT_VALUE) {
            return;
        }
        if (userAnswer.countWordsDisplayed < userAnswer.valueN) {
            cout << "Must be at least " << userAnswer.valueN << " words."<< endl;
            cout << endl;
        }
    }
}
 
/*
 * Function: buildStringToPrint
 * Usage: buildStringToPrint(userAnswer, allWords, origSize, map);
 * -----------------------------------------------------------------------
 * Builds and returns the random text to print in the console (return type string).
 * Random text is build by randomly selecting one window of text size N - 1 in the map of N-Grams
 * then grabbing one of the words at random in the list of the words that immediately follow
 * each occurrence of that window in the seed text from the map, adding the window and that random
 * word to the text to print and creating a new window by removing the first word of the current
 * window and adding to it the random word that was just selected. This process repeats until the
 * number of words to print has been selected. Random text starts and ends with "...". Words
 * are separated by spaces in it.
 * Takes 4 parameters:
 * - userAnswer (type userInput): its valueN number is a measure of the size of the window
 * we'll be randomly selected (size = valueN-1). Remains unchanged.
 * - allWords(type Vector<string>): contains all the words in the seed text file with wrapping
 * modification. Is used to pick a random starting window of N - 1 words. Remains unchanged.
 * - origSize (type int): count of words in the seed text file. Is used to pick a random index
 * in allWords, which is the starting point of our starting window of N-1 words.
 * Remains unchanged.
 * - map (type  HashMap<Vector<string>, Vector<string>>): map of N-grams. Is used to populate
 * the string returned by this function after populating it with the original window of words.
 * Remains unchanged.
 * Returns the string of random text to print in the console.
 */
string buildStringToPrint(const userInput &userAnswer, const Vector<string> &allWords,
                          const int origSize, HashMap<Vector<string>, Vector<string>> &map, const Vector<int> &indexBegSentence) {
    int counter = 0;
    string toPrint = "";
    int randomInt = randomInteger(0, indexBegSentence.size() - 1);
    Vector<string> window = allWords.subList(indexBegSentence[randomInt], userAnswer.valueN - 1);
    for (string word : window) {
        toPrint = toPrint + word + " ";
        counter++;
    }
    //cout << toPrint.size() << endl;
    toPrint = trimEnd(toPrint);
    //cout << toPrint.size() << " aftertrimming" << endl;
    Vector<string> selectwords;
    string newWord = "a";
    while (counter < userAnswer.countWordsDisplayed) {
        selectwords = map.get(window);
        int randomPosition = randomInteger(0, selectwords.size() - 1);
        newWord = selectwords[randomPosition];
        toPrint = toPrint + " " + newWord ;
        counter++;
        window = window.subList(1, window.size() - 1);
        window.add(newWord);
    }
    while(newWord[newWord.length()-1]!='!' && newWord[newWord.length()-1]!='?'&& newWord[newWord.length()-1]!='.') {
        selectwords = map.get(window);
        int randomPosition = randomInteger(0, selectwords.size() - 1);
        newWord = selectwords[randomPosition];
        toPrint = toPrint + " " + newWord ;
        counter++;
        window = window.subList(1, window.size() - 1);
        window.add(newWord);
    }
    cout << "We printed " << counter << " words instead of " << userAnswer.countWordsDisplayed <<
            " to end the text with a full sentence." << endl;
    return toPrint;
}
 
 
 
    
