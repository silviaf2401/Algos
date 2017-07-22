/*
 * File: grammarsolver.cpp
 * --------------------------
 * Name: Silvia Fernandez (SUNet ID silviaf)
 * Section leader: Nick Bowman
 * This file contains the code for the grammar solver problem in CS106 B assigmnent 3.
 * Specifically it implements the function grammarGenerate, which accepts a BNF language grammar file
 * input and generates a given number of random expansions of a symbol in the language following
 * the grammar rules in the language file.
 * Symbol and number of times are  also specified by the parameters passed to the method.
 * This file only includes homework core functionality. Please refer to files with "extension" suffix
 * for extension functionality.
 */
 
#include "grammarsolver.h"
#include <iostream>
#include <fstream>
#include "strlib.h"
#include "map.h"
#include "string.h"
#include "random.h"
#include "filelib.h"
 
using namespace std;
 
/* Function prototypes */
Vector<string> generateGrammarElementsRandomly(Map<string, Vector<string>> &grammar, string symbol, int times);
void generateGrammarElementsRandomly(Map<string, Vector<string>> &grammar, string symbol, string &finalString);
void buildMap(string fileLines, Map<string, Vector<string>> &grammar);

 
/** Function grammarGenerate()
 * Usage: grammarGenerate(input, symbol, times)
 * ---------------------------------------------
 * Generates a given symbol's expansion randomly a given number of times following the grammar rules in
 * a BNF input file. Specifically it reads the BNF file input stream passed as parameter to the
 * function, stores it in a Map, and creates times (passed as parameter) random expansions of
 * the symbol also passed as parameter.
 * Relies on a recursive function, generateGrammarElementsRandomly to do this.
 * Assumption: file name provided by user and stored in input is a BNF file.
 * This will be called by grammarmain.cpp.
 * This function takes as parameters:
 * @param input - Input stream of BNF file.
 * @param symbol - Symbol to generate
 * @param times - Number of times grammar is generated
 * This function returns:
 * @return Vector of strings of size times with times random generations of symbol
 */
 
Vector<string> grammarGenerate(istream& input, string symbol, int times) {
    // will hold each line of our grammar
    string line;
    // map associating each symbol to expansion rules
    Map<string, Vector<string>> grammar;
    // once we reach end of file fail indicator is set for the stream and it is interpreted as false
    while(getline(input,line)){
         buildMap(line,grammar);
    }

    Vector<string> randomGrammarElements;
    string finalString ="";
    for (int i = 0; i < times; i++) {
        // generates one sentence only
        generateGrammarElementsRandomly(grammar,symbol,finalString);
        randomGrammarElements.add(finalString);
        finalString ="";
    }
    // once done times times, return what will be printed in the console
}
 
/** Function buildMap()
 * Usage: buildMap(fileLines, grammar)
 * -----------------------------------
 * Builds a map associating all non-terminal names in the original grammar file to expansion
 * rules or to terminal names.
 * Takes as parameters:
 * @param grammar - map being built. Type Map<string, Vector<string>>.
 * @param fileLines -  a string containining one line from our grammar file.
 * After this function is called, grammar will be modified to contain all non
 * terminal names associated to their expansion rules as specified in the original input istream.
 */
void buildMap(string fileLines, Map<string, Vector<string>> &grammar) {
        Vector<string> splitLine = stringSplit(fileLines, "::=");
        // splitLine has key as string in first index, then one long string with what that key is composed of.
        string splitLineKey = splitLine.get(0);
        Vector<string> splitValues = stringSplit(splitLine.get(1),"|");
        // assume file properly formatted and no duplicate lines with same non-terminal elements.
        grammar.put(splitLineKey,splitValues);
}

 
/** Function generateGrammarElementsRandomly()
 * Usage: generateGrammarElementsRandomly(grammar, symbol, finalString)
 * ---------------------------------------------
 * Recursively generates, for a given symbol, a string based on grammar rules.
 * Specifically it looks for the symbol in grammar passed as parameter to the function
 * until it reaches a terminal, in which case the terminal is added at the end of
 * the string that is passed as reference parameter to the function.
 * This will be called by grammarmain.cpp.
 * This function takes as parameters:
 * @param finalString - string that will contain a string grammar for the symbol passed as parameter.
 * @param symbol - Symbol for which to generate the string.
 * @param grammar - map used to look for grammar rules associated to a given symbol.
 */
 
void generateGrammarElementsRandomly(Map<string, Vector<string>> &grammar, string symbol, string &finalString) {
    // base case: we are at a terminal
    if (!grammar.containsKey(symbol)) {
        finalString = finalString + " " + symbol;
    } else {
        // recursive case: build list of strings needed until terminal position reached
        Vector<string> temp = grammar.get(symbol);
        // we select one of the possible elements randomly
        int randomInt = randomInteger(0, temp.size()-1);
        string chosenString = temp.get(randomInt);
        // get all the options inside our chosen element and apply generateGrammarElementsRandomly again.
        Vector<string> tempSplit = stringSplit(chosenString," ");
        for (string newSymbol: tempSplit) {
            generateGrammarElementsRandomly(grammar,newSymbol, finalString);
        }
     }
}
 
 
 
 
 
 
    
