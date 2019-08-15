#include "bogglegui.h"
#include <string>
#include "grid.h"
#include "lexicon.h"
#include "set.h"
using namespace std;

Vector<Vector<int> > findNeighbors(Grid<char>& board, int row, int col, Set<Vector<int> > chosen);
bool humanWordSearchHelper(Grid<char>& board, int row, int col, string word, string found, Set<Vector<int> > chosen, int word_length);
void computerWordSearchHelper(Grid<char>& board, Lexicon& dictionary, int row, int col, string prefix,
                              Set<Vector<int> > chosen, Set<string>& words, Set<string> humanWords);

bool humanWordSearch(Grid<char>& board, Lexicon& dictionary, string word) {
    // Clear highlighting on all cubes
    BoggleGUI::clearHighlighting();

    // If the word is not in the dictionary, return false
    if (!dictionary.contains(word)) {
        return false;
    }

    // If the word length is less than the specified minimum, return false
    if (word.size() < BoggleGUI::MIN_WORD_LENGTH) {
        return false;
    }

    // Iterate over the board and search for the input word
    int word_length = word.size();
    for (int i=0; i<board.numRows(); i++) {
        for (int j=0; j<board.numCols(); j++) {
            Set<Vector<int> > chosen;
            if (humanWordSearchHelper(board, i, j, word, "", chosen, word_length)) {
                // Add points for human player
                if (word.size() <= 4) {
                    BoggleGUI::scorePointsHuman(1);
                } else if (word.size() == 5) {
                    BoggleGUI::scorePointsHuman(2);
                } else if (word.size() == 6) {
                    BoggleGUI::scorePointsHuman(3);
                } else if (word.size() == 7) {
                    BoggleGUI::scorePointsHuman(5);
                } else {
                    BoggleGUI::scorePointsHuman(11);
                }
                return true;
            }
        }
    }
    return false;
}


Set<string> computerWordSearch(Grid<char>& board, Lexicon& dictionary, Set<string>& humanWords) {
    Set<string> words;
    // Search for all suitable words using the cubes on the board
    for (int i=0; i<board.numRows(); i++) {
        for (int j=0; j<board.numCols(); j++) {
            Set<Vector<int> > chosen;
            computerWordSearchHelper(board, dictionary, i, j, "", chosen, words, humanWords);
        }
    }

    return words;
}

Vector<Vector<int> > findNeighbors(Grid<char>& board, int row, int col, Set<Vector<int> > chosen) {
    int startRow = max(0, row-1);
    int endRow = min(row+1, board.numRows()-1);
    int startCol = max(0, col-1);
    int endCol = min(col+1, board.numCols()-1);

    Vector<Vector<int> > retVal;
    for (int i=startRow; i<=endRow; i++) {
        for (int j=startCol; j<=endCol; j++) {
            Vector<int> currentLocation{i, j};
            if (!chosen.contains(currentLocation) && (i!=row || j!=col)) {
                retVal.add(currentLocation);
            }
        }
    }

    return retVal;
}

bool humanWordSearchHelper(Grid<char>& board, int row, int col, string word, string found, Set<Vector<int> > chosen, int word_length) {
    // Base case: all words are found
    if (found.size() == word_length) {
        return true;
    } else {
        char c = word[0];
        if (board[row][col] == c) {
            // Choose
            BoggleGUI::setHighlighted(row, col, true);
            found += c;
            word.erase(0, 1);

            // Explore in the neighbors of the chosen character
            Vector<int> currentLocation{row, col};
            chosen.add(currentLocation);
            Vector<Vector<int> > neighbors = findNeighbors(board, row, col, chosen);
            for (int i=0; i<neighbors.size(); i++) {
                bool wordIsFound = humanWordSearchHelper(board, neighbors[i][0], neighbors[i][1], word, found, chosen, word_length);
                if (wordIsFound) {
                    return true;
                }
            }

            // Un-choose (Backtracking)
            BoggleGUI::setHighlighted(row, col, false);
            word.insert(0, 1, c);
            found.erase(found.size()-1, 1);
        }
        return false;
    }
}

void computerWordSearchHelper(Grid<char>& board, Lexicon& dictionary, int row, int col, string prefix,
                              Set<Vector<int> > chosen, Set<string>& words, Set<string> humanWords) {
    char c = board[row][col];
    prefix += c;
    // Base case: when the prefix cannot be found in the dictionary
    // Do nothing
    if (dictionary.containsPrefix(prefix)) {
        if (dictionary.contains(prefix) && prefix.size()>=4 &&
                !humanWords.contains(prefix) && !words.contains(prefix)) {
            words.add(prefix);
            // Add points for computer player
            if (prefix.size() <= 4) {
                BoggleGUI::scorePointsComputer(1);
            } else if (prefix.size() == 5) {
                BoggleGUI::scorePointsComputer(2);
            } else if (prefix.size() == 6) {
                BoggleGUI::scorePointsComputer(3);
            } else if (prefix.size() == 7) {
                BoggleGUI::scorePointsComputer(5);
            } else {
                BoggleGUI::scorePointsComputer(11);
            }
        }

        // Explore in the neighbors of the chosen character
        Vector<int> currentLocation{row, col};
        chosen.add(currentLocation);
        Vector<Vector<int> > neighbors = findNeighbors(board, row, col, chosen);
        for (int i=0; i<neighbors.size(); i++) {
            computerWordSearchHelper(board, dictionary, neighbors[i][0], neighbors[i][1], prefix, chosen, words, humanWords);
        }

        // Un-choose (Backtracking)
        prefix.erase(prefix.size()-1, 1);
    }
}
