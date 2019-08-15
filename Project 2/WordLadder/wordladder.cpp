// This is the CPP file you will edit and turn in. (TODO: Remove this comment!)

#include <iostream>
#include <fstream>
#include "console.h"
#include "simpio.h"
#include "filelib.h"
#include "vector.h"
#include "stack.h"
#include "queue.h"
#include "set.h"
#include "lexicon.h"

using namespace std;

Lexicon loadDictionary(string fileName);
bool isInVector(string target, Vector<string> dict);
bool isWordSameLength(string beginWord, string endWord);
bool isWordIdentical(string beginWord, string endWord);
bool isWordInDict(string beginWord, string endWord, Lexicon dict);
bool isNeighbor(string word1, string word2);
Vector<Stack<string> > findNeighbors(Stack<string> wordLadder, Lexicon dict, Set<string> record);
string toLowerCase(string str);

int main() {
    // TODO: Finish the program!
    // The word ladder project is a classical Breadth First Search problem
    cout << "Welcome to CS106B/X Word Ladder!" << endl;
    cout << "Please give me two English words, and I will convert the" << endl;
    cout << "first into the second by modifying one letter at a time." << endl;
    cout << endl;

    // Choose a dictionary to use
    string dictName = getLine("Dictionary file name: ");
    while (!fileExists(dictName)) {
        cout << "Unable to open that file. Try again." << endl;
        dictName = getLine("Dictionary file name: ");
    }
    cout << endl;

    // Load the dictionary
    Lexicon dict = loadDictionary(dictName);

    while (true) {
        // Ask the user for the begin word and end word
        string word1 = getLine("Word 1 (or Enter to quit): ");
        if (word1 == "") {
            break;
        }
        string word2 = getLine("Word 2 (or Enter to quit): ");
        if (word2 == "") {
            break;
        }

        // Convert both words into lower case
        string beginWord = toLowerCase(word1);
        string endWord = toLowerCase(word2);

        // Check the validity of the two words
        if (!isWordSameLength(beginWord, endWord)) {
            cout << "The length of the two words must be the same." << endl;
            cout << endl;
            continue;
        }
        if (isWordIdentical(beginWord, endWord)) {
            cout << "The two words must be different." << endl;
            cout << endl;
            continue;
        }
        if (!isWordInDict(beginWord, endWord, dict)) {
            cout << "The two words must be found in the dictionary." << endl;
            cout << endl;
            continue;
        }

        Stack<string> result;
        bool wordIsFound = false;
        // Start by creating a queue of stack containing only the begin word
        Queue<Stack<string> > wordLadderQueue;
        Stack<string> beginWordLadder;
        Set<string> record;     // Need to keep track of the words that has already been checked
        beginWordLadder.push(beginWord);
        record.add(beginWord);
        wordLadderQueue.enqueue(beginWordLadder);

        while (!wordLadderQueue.isEmpty()) {
            int queueSize = wordLadderQueue.size();
            // cout << queueSize << endl;
            // For each existing neighbors in the queue, find their neighbors,
            // put them in stacks and enqueue them, then dequeue the previous neighbors
            for (int i=0; i<queueSize; i++) {
                Stack<string> wordToFind = wordLadderQueue.peek();
                Vector<Stack<string> > neighbors = findNeighbors(wordToFind, dict, record);
                for (int j=0; j<neighbors.size(); j++) {
                    wordLadderQueue.enqueue(neighbors[j]);
                    record.add(neighbors[j].peek());
                    if (neighbors[j].peek() == endWord) {
                        result = neighbors[j];
                        wordIsFound = true;
                    }
                }
                wordLadderQueue.dequeue();
            }
            if (wordIsFound) {
                cout << "A ladder from " << endWord << " back to " << beginWord << ":" << endl;
                while (!result.isEmpty()) {
                    string word = result.pop();
                    cout << word << " ";
                }
                cout << endl;
                cout << endl;
                break;
            }
        }

        if (!wordIsFound) {
            cout << "There is no ladder from " << endWord << " back to " << beginWord << "." << endl;
            cout << endl;
        }

    }

    cout << "Have a nice day." << endl;
    return 0;
}

Lexicon loadDictionary(string fileName) {
    ifstream input;
    input.open(fileName);
    string line;
    Lexicon dict;
    while (getline(input, line)) {
        dict.add(line);
    }
    return dict;
}

bool isWordSameLength(string beginWord, string endWord) {
    if (beginWord.length() == endWord.length()) {
        return true;
    } else {
        return false;
    }
}

bool isWordIdentical(string beginWord, string endWord) {
    if (beginWord == endWord) {
        return true;
    } else {
        return false;
    }
}

bool isWordInDict(string beginWord, string endWord, Lexicon dict) {
    if (dict.contains(beginWord) && dict.contains(endWord)) {
        return true;
    } else {
        return false;
    }
}

Vector<Stack<string> > findNeighbors(Stack<string> wordLadder, Lexicon dict, Set<string> record) {
    Vector<Stack<string> > retVal;
    string word = wordLadder.peek();
    for (int i=0; i<word.size(); i++) {
        string possibleNeighbor = word;
        for (char ch = 'a'; ch <= 'z'; ch++) {
            possibleNeighbor[i] = ch;
            if (dict.contains(possibleNeighbor) && !record.contains(possibleNeighbor)) {
                Stack<string> newWordLadder = wordLadder;
                newWordLadder.push(possibleNeighbor);
                retVal.add(newWordLadder);
            }
        }
    }
    return retVal;
}

string toLowerCase(string str) {
    string retVal = "";
    for (int i=0; i<str.size(); i++) {
        retVal += tolower(str[i]);
    }
    return retVal;
}
