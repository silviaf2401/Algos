
/*
 * File: encoding.cpp
 * --------------------------
 * Name: Silvia Fernandez (SUNet ID: silviaf)
 * Section leader: Nick Bowman
 * This file implements functions to compress and decompress the contents of an input stream into
 * an output stream using a Huffman encoding algorithm (lossless compression). Huffman encoding
 * uses different length binary encoding for different characters instead of 8-bit-per-character;
 * short encoding lengths are used for characters that appear frequently in the file being encoded
 * and longer encoding lengths for characters that do not appear frequently.
 * Huffman encoding steps are: look at a source file's contents and count the number of occurrences of
 * each character in it (build a frequency table), build a binary tree where each node represents a
 * character in the input file and its count of occurrences in the input file (encoding tree), build
 * an encoding map by traversing the encoding tree - encoding map contains the characters in the input
 * file and their binary encoding, and encode the data: input file is re-examined and encoding map is
 * used to retrieve the binary encoding for each character, which is written encoded into output stream/file.
 * In our implementation, the frequency table is also included as header in our output stream.
 * Please refer to the method headers for more details on these functions.
 * This file includes password extension functionality to CS106B's Huffman encoding assignment.
 * Please refer to submission #6 in paperless for core functionality.
 * User needs either an admin key "CS106B" to create an account with user name and password
 * to be able to use the compressing/decompressing program
 * If the user already has an account, they need to enter their username and password (3 attempts allowed)
 * to be able to run the program.
 * Username and passwords to be used for different program runs are stored in passwords.txt
 * Authentication key for new user is stored in authkey.txt
 * Nick: for this program to run, please change the PATH_RESOURCES_DIRECTORY constant below to the path of your
 * resources folder (what Qt creator uses in your local machine).
 * Thank you :)
 */
 
 
#include "encoding.h"
#include "pqueue.h"
#include "strlib.h"
#include "filelib.h"
 
/* Function prototypes */
void helperTreeTraversal(HuffmanNode *encodingTree, Map<int, string> &encodingMap, string currentString);
int getCharFromTree(HuffmanNode *node, ibitstream &input);
void writeEncodingString(const string encodingString, obitstream&output);
 
/** Function: buildFrequencyTable()
 * Usage: buildFrequencyTable(input)
 * ---------------------------------
 * Reads input from a istream, counts and returns a mapping from each character (represented as int
 * here) to the number of times that character appears in the file. Adds a single occurrence of the
 * fake character PSEUDO_EOF to map.
 * Assumptions: input file exists and can be read, the file might be empty. An empty file
 * returns a map containing only 1 occurrence of PSEUDO_EOF.
 * @param: input, type istream: input stream fromw here we read input
 * @return: freqTable, type Map<int, int> mapping from each character (int) in input to the number of
 * times the character appears in it.
 */
Map<int, int> buildFrequencyTable(istream& input) {
    Map<int, int> freqTable;
    while (true) {
        int c = input.get();
        if (c == EOF) {
            break;
        }
        if (!freqTable.containsKey(c)){
            freqTable.put(c, 1);
        } else {
            freqTable.put(c, freqTable.get(c) + 1);
        }
    }
    freqTable.put(PSEUDO_EOF, 1);
    return freqTable;
}
 
/** Function: buildEncodingTree()
 * Usage: buildEncodingTree(freqTable)
 * ---------------------------------
 * Creates a Huffman encoding tree based on the character frequencies in the freqTable passed as
 * parameter. Returns a pointer to the node representing the root of the Huffman encoding tree.
 * When building the encoding tree, this function uses a priority queue of HuffmanNodes
 * to keep track of which nodes to process next. The priority queue allows each element in the
 * freqTable to be enqueued along with an associated priority and sorts elements by their priority
 * so that elements dequeued always have the lowest priority number. Tree is built by iteratively
 * dequeuing the first 2 elements of the HuffmanNodes queue, creating a new Huffman node of
 * priority the sum of the priorities of those 2 elements and pointing to each of the 2 elements
 * and adding the new node to the priority queue. This process continues until only one element
 * remains in the queue of Huffman nodes, which is the root of the Huffman encoding tree to be
 * returned.
 * Assumptions: the frequency table is valid: does not contain any keys other than char
 * values, PSEUDO_EOF, and NOT_A_CHAR; all counts are positive integers and it is not empty.
 * @param: freqTable, type Map<int, int> mapping from each character (int) in input to the number
 * of times the character appears in it, used to build our Huffman encoding tree.
 * @return: HuffmanNode*: pointer to the root of the Huffman tree built by this function.
 */
HuffmanNode* buildEncodingTree(const Map<int, int>& freqTable) {
    PriorityQueue<HuffmanNode*> pq;
    // place characters and count as Huffman nodes into priority queue
    for (int character : freqTable) {
        int frequency = freqTable.get(character);
        HuffmanNode *node = new HuffmanNode(character, frequency, NULL, NULL);
        pq.enqueue(node, frequency);
    }
    while (pq.size() > 1) {
        HuffmanNode *firstNode = pq.dequeue();
        HuffmanNode *secondNode = pq.dequeue();
        HuffmanNode *newNode = new HuffmanNode(NOT_A_CHAR, firstNode->count + secondNode->count, firstNode, secondNode);
        int frequency = newNode->count;
        pq.enqueue(newNode, frequency);
    }
    return pq.dequeue();
}
 
/** Function: buildEncodingMap()
 * Usage: buildEncodingMap(encodingTree)
 * ---------------------------------
 * Accepts a pointer to the root node of a Huffman tree and uses it to create and return a Huffman encoding map
 * based on the tree's structure. Each key in the map is a character(int), and each value is the binary encoding
 * (string) for that character. If the encoding tree is NULL, this function returns an empty map.
 * Encoding map is built recursively with a helper function.
 * @param: encodingTree: pointer to the root of the Huffman tree built used to create Huffman encoding map
 * @return: encodingMap, type Map<int, string> - encoding map mappint a character to its binary encoding.
 */
Map<int, string> buildEncodingMap(HuffmanNode* encodingTree) {
    Map<int, string> encodingMap;
    if (encodingTree != NULL) {
        helperTreeTraversal(encodingTree, encodingMap, "");
    }
    return encodingMap;
}
 
/** Function: helperTreeTraversal()
 * Usage: helperTreeTraversal(encodingTree, encodingMap, currentString)
 * --------------------------------------------------------------------
 * Traverses Hoffman tree exhaustively until it finds a leaf, builds a binary encoding string for
 * all characters in the tree, and adds the binary encoding and the character it corresponds to
 * to the encoding map, associating a character(int) to its binary encoding(string).
 * Assumption encodingTree is not NULL.
 * @param: encodingTree - type HuffmanNode* - pointer to the node from which we're progressing
 * towards the leaf of the Huffman tree.
 * @param: encodingMap, type Map<int, string> map associating a character(int) to its binary
 * encoding(string).
 * @param: currentString, type string: holds the binary encoding of a given character which will be
 * added to our encodingMap.
 */
void helperTreeTraversal(HuffmanNode *encodingTree, Map<int, string> &encodingMap,
                         string currentString) {
    if (encodingTree -> isLeaf()) {
        encodingMap.put(encodingTree->character, currentString);
    } else {
        helperTreeTraversal(encodingTree->zero, encodingMap, currentString + '0');
        helperTreeTraversal(encodingTree->one, encodingMap, currentString + '1');
    }
}
 
/** Function: encodeData()
 * Usage: encodeData(input, encodingMap, output)
 * ----------------------------------------------
 * Reads one character at a time from a given input stream, and uses the provided encoding map to encode
 * each character to binary. Writes the character's encoded binary bits to the given output obitstream
 * bit stream. After writing the input's contents, it writes a single occurrence of the binary encoding
 * for PSEUDO_EOF into the output.
 * Assumptions: parameters are valid: the encoding map is valid and
 * contains all needed data, input stream is readable, output stream is writable, streams are already
 * opened and ready to be read/written, prompting user or opening/closing files is already taken care of.
 * @param: input, type istream - input stream whose contents are being encoded using the encodingMap
 * @param: encodingMap, type Map<int, string> - map used to encode characters in input to binary
 * @param: output, type obitstream -
 */
void encodeData(istream& input, const Map<int, string>& encodingMap, obitstream& output) {
    // write file encoding
    while (true) {
        int c = input.get();
        // if we reach EOF exit loop
        if (c == EOF) {
            break;
        }
        // get encoding for a given character from encoding map and write to output
        string encodingString = encodingMap.get(c);
        writeEncodingString(encodingString, output);
 
    }
    // get encoding for PSEUDO_EOF from encoding map and write to output
    string encodingStringEOF = encodingMap.get(PSEUDO_EOF);
    writeEncodingString(encodingStringEOF, output);
}
 
/** Function: writeEncodingString()
 * Usage: writeBit(encodingString, output)
 * ---------------------------------------
 * Writes encodingString as bits into output.
 * Assumptions: encodingString is a string of '0' and '1' only.
 * @param: encodingString (type string) - encoding to be written to output.
 * @param: output (type obistream) - stream where we're writing the characters in encodingString,
 * as bits.
 */
void writeEncodingString(const string encodingString, obitstream&output) {
    for (char bit: encodingString) {
        if (bit == '0') {
            output.writeBit(0);
        } else {
            output.writeBit(1);
        }
    }
}
 
/** Function: decodeData()
 * Usage: decodeData(input, encodingTree, output)
 * ---------------------------------------
 * Reads bits from the given input stream one at a time, and recursively walks through a
 * Hoffman encodingTree to write the original uncompressed contents of the input stream to the given
 * output stream.
 * Assumptions: streams are already opened and valid and we don't have to close them.
 * Uses recursive helper function. If encodingTree is null, function returns
 * immediately.
 * @param: input, type ibitstream - stream containing encoded characters as a sequence of 0 and 1s
 * @param: encodingTree, type HuffmanNode*, pointer to the root of the Huffman encoding tree used
 * to decode the encoded characters in input.
 * @param: output, type ostream - stream where decoded input is written.
 */
void decodeData(ibitstream& input, HuffmanNode* encodingTree, ostream& output) {
    if (encodingTree == NULL) {
        return;
    }
    while (true){
        int toPrint = getCharFromTree(encodingTree, input);
        // if we found EOF, exit loop
        if (toPrint == -1) {
            return;
        }
        output << (char) toPrint;
    }
}
 
/** Function: getCharFromTree();
 * Usage: getCharFromTree(node, input)
 * -----------------------------------
 * Reads bits from the given input stream one at a time and walks through a Hoffman encoding tree
 * recursively until a leaf is found, to return the original characters corresponding to the
 * encoded characters in the input.
 * If PSEUDO_EOF character is found, function returns -1.
 * Assumptions: stream is already opened and valid, node is not NULL.
 * @param: node, type HuffmanNode* - pointer to the root of the Hoffman encoding tree being used
 * to return the original characters corresponding to the encoded characters in the inout
 * @param: input, type ibitstream - contains bits that form encoded characters.
 * @return: integer corresponding to decoded characters.
 */
int getCharFromTree(HuffmanNode *node, ibitstream &input) {
    if (node -> isLeaf()) {
        if (node->character == PSEUDO_EOF) {
            return -1;
        }
        return node ->character;
    } else {
        int toDecode = input.readBit();
        if (toDecode == 0){
            return getCharFromTree(node->zero, input);
        } else {
            return getCharFromTree(node->one, input);
        }
    }
}
 
/** Function: compress();
 * Usage: compress(input, output)
 * -----------------------------------
 * Compresses the given input stream and writes the compressed version to the given output stream.
 * Reads input one character at a time, builds a frequency table for characters (characters as int
 * map to character occurrences (int)) in input stream, builds a Hoffman encoding tree and an
 * encoding map (characters to binary code) from the encoding tree and writes input characters
 * encoded into output stream.
 * The compressed version of the input also includes a header, which is the frequency table.
 * Assumptions: streams are both valid and read/writeable, the input file might be empty.
 * The streams are already opened and ready to be read/written, and function does not need to
 * open/close the streams.
 * Compressed output is smaller in size than input on average.
 * @param input type istream - stream to be compressed
 * @param output type obitstream - output stream with input contents compressed and frequency table.
 */
void compress(istream& input, obitstream& output) {
    Map<int, int> freqTable = buildFrequencyTable(input);
    rewindStream(input);
    HuffmanNode* encodingTree = buildEncodingTree(freqTable);
    Map<int, string> encodingMap = buildEncodingMap(encodingTree);
    output << freqTable;
    encodeData(input, encodingMap, output);
    freeTree(encodingTree);
}
 
/** Function: decompress()
 * Usage: decompress(input, output)
 * ---------------------------------
 * Reads the bits from the given input stream one at a time, including frequency table header.
 * Builds encoding tree from frequency table, uses it to decode the data in input stream and writes
 * the original contents of that stream to the file specified by the output parameter.
 * Assumptions: streams are valid and read/writeable, but the input file might be empty.
 * The streams are already open and ready to be used; function does not need to prompt ueer or
 * open/close files.
 * @param: input type ibitstream - stream containing bits to be decompressed.
 * @param: output type ostream - stream where decoded contents of input stream are written.
 */
void decompress(ibitstream& input, ostream& output) {
    Map<int, int> freqTable;
    input >> freqTable;
    HuffmanNode* encodingTree = buildEncodingTree(freqTable);
    decodeData(input, encodingTree, output);
    freeTree(encodingTree);
}
 
/** Function: freeTree
 *  Usage: freeTree(node)
 * ----------------------
 * Frees the memory associated with the tree whose root node is represented by the given pointer.
 * Root node and all nodes in its subtrees are freed. If the tree passed is NULL the function
 * returns immediately.
 * @param node: type HuffmanNode* - pointer to the root of the tree to be freed from memory.
 */
void freeTree(HuffmanNode* node) {
    if (node == NULL) {
        return;
    } else {
        freeTree(node->zero);
        freeTree(node->one);
        delete node;
    }
}
 