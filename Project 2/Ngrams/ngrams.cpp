// This is the CPP file you will edit and turn in. (TODO: Remove this comment!)

#include <iostream>
#include <fstream>
#include "console.h"
#include "simpio.h"
#include "filelib.h"
#include "vector.h"
#include "queue.h"
#include "map.h"
#include "random.h"
using namespace std;

Map<Queue<string>, Vector<string> > buildMapFromFile(string fileName, int N);
Queue<string> chooseStartingPoint(string fileName, int N, int mapSize, int& rand);
Vector<string> createRandomText(Map<Queue<string>, Vector<string> > NGramMap, Queue<string> startingQueue, int numOfWords);

int main() {
    // TODO: Finish the program!
    cout << "Welcome to CS106B/X Random Writer ('N-Grams')!" << endl;
    cout << "This program generates random text based on a document." << endl;
    cout << "Give me an input file and an 'N' value for groups" << endl;
    cout << "of words, and I'll create random text for you." << endl;
    cout << endl;

    // Read the input file and ask for N value
    string fileName = getLine("Input file name? ");
    while (!fileExists(fileName)) {
        cout << "Unable to open that file. Try again." << endl;
        fileName = getLine("Input file name? ");
    }
    int N = getInteger("Value of N? ");
    while (N <= 1) {
        cout << "Please enter an integer which is greater than 2." << endl;
        N = getInteger("Value of N? ");
    }

    // Ask for number of random texts to generate
    int numOfWords = getInteger("# of random words to generate (0 to quit): ");
    while (numOfWords != 0) {
        while (numOfWords < 4) {
            cout << "Must be at least 4 words." << endl;
            numOfWords = getInteger("# of random words to generate (0 to quit): ");
        }

        // Build a map of N-Grams
        Map<Queue<string>, Vector<string> > NGramMap = buildMapFromFile(fileName, N);
        int mapSize = NGramMap.size();

        // Choose a random starting point from the document
        int rand = 0;
        Queue<string> startingQueue = chooseStartingPoint(fileName, N, mapSize, rand);

        // Create random text by looking up all possible next words in the map we built
        Vector<string> randomText = createRandomText(NGramMap, startingQueue, numOfWords);

        // Print out the random text
        for (int i=0; i<randomText.size(); i++) {
            cout << randomText[i] << " ";
        }
        cout << endl;
        cout << endl;
        numOfWords = getInteger("# of random words to generate (0 to quit): ");
    }

    cout << "Exiting." << endl;
    return 0;
}

Map<Queue<string>, Vector<string> > buildMapFromFile(string fileName, int N) {
    // Since we need to keep a window of length N-1,
    // we can use a queue in this case
    // Initialize the queue
    ifstream input;
    input.open(fileName);
    Map<Queue<string>, Vector<string> > NGramMap;
    string word;
    Queue<string> window;
    Vector<string> wrap;        // used for the wrap-around part of the map
    for (int i=0; i<N-1; i++) {
        input >> word;
        wrap.add(word);
        window.enqueue(word);
    }

    // Build the map until we finish reading the file
    while (input >> word) {
        // Put the corresponding word value into the map
        Vector<string> wordFreq;
        wordFreq.add(word);
        if (!NGramMap.containsKey(window)) {
            NGramMap[window] = wordFreq;
        } else {
            NGramMap[window].add(word);
        }

        // Every time we move the window forward,
        // we dequeue one word at the front and enqueue one word from the back
        window.enqueue(word);
        window.dequeue();
    }

    // Map should wrap around
    for (int i=0; i<wrap.size(); i++) {
        Vector<string> wrapWordFreq;
        wrapWordFreq.add(wrap[i]);
        if (!NGramMap.containsKey(window)) {
            NGramMap[window] = wrapWordFreq;
        } else {
            NGramMap[window].add(wrap[i]);
        }
        window.enqueue(wrap[i]);
        window.dequeue();
    }

    return NGramMap;
}

Queue<string> chooseStartingPoint(string fileName, int N, int mapSize, int& rand) {
    ifstream input;
    input.open(fileName);
    string word;
    rand = randomInteger(0, mapSize-1);
    Vector<string> wrap;
    Queue<string> startingWindow;

    if (rand <= mapSize-N) {
        for (int i=0; i<rand; i++) {
            input >> word;
        }
        for (int i=rand; i<rand+N-1; i++) {
            input >> word;
            startingWindow.enqueue(word);
        }
    } else {
        for (int i=0; i<N-(mapSize-rand+1); i++) {
            input >> word;
            wrap.add(word);
        }
        for (int i=N-(mapSize-rand+1); i<rand; i++) {
            input >> word;
        }
        for (int i=rand; i<mapSize; i++) {
            input >> word;
            startingWindow.enqueue(word);
        }
        for (int i=0; i<wrap.size(); i++) {
            startingWindow.enqueue(wrap[i]);
        }
    }

    return startingWindow;
}

Vector<string> createRandomText(Map<Queue<string>, Vector<string> > NGramMap, Queue<string> startingQueue, int numOfWords) {
    Vector<string> retVal;
    retVal.add("...");
    Queue<string> head = startingQueue;
    while (!head.isEmpty()) {
        retVal.add(head.peek());
        head.dequeue();
    }

    // To create new words, we randomly select words by using the map we build
    for (int i=0; i<numOfWords; i++) {
        Vector<string> possibleSuffix = NGramMap[startingQueue];
        int rand = randomInteger(0, possibleSuffix.size()-1);
        string suffix = possibleSuffix[rand];
        retVal.add(suffix);

        // Every time we choose a new word, we discard the first word in the window
        // and append the suffix to the end of the window
        startingQueue.dequeue();
        startingQueue.enqueue(suffix);
    }
    retVal.add("...");

    return retVal;
}


