// This is the CPP file you will edit and turn in. (Remove this comment!)

#include <iostream>
#include <fstream>
#include "console.h"
#include "simpio.h"
#include "filelib.h"
#include "grid.h"
#include "lifegui.h"
using namespace std;

Grid<char> getGridFromFile(string fileName);
Grid<char> computeNextGeneration(Grid<char> oldGrid);
int countNeighbors(int x, int y, Grid<char> lifeGrid);

int main() {
    // TODO: Finish the program!
    // Print the introduction
    cout << "Welcome to the CS106B/X Game of Life!" << endl;
    cout << "This program simulates the lifecycle of a bacterial colony." << endl;
    cout << "Cells (X) live and die by the following rules:" << endl;
    cout << "* A cell with 1 or fewer neighbors dies." << endl;
    cout << "* Locations with 2 neighbors remain stable." << endl;
    cout << "* Locations with 3 neighbors will create life." << endl;
    cout << "* A cell with 4 or more neighbors dies." << endl;
    cout << endl;

    bool addFile = true;
    while (addFile) {
        // Get the file and turn it into a Grid
        string fileName = getLine("Grid input file name? ");
        while (!fileExists(fileName)) {
            cout << "Unable to open that file. Try again." << endl;
            fileName = getLine("Grid input file name? ");
        }
        Grid<char> lifeGrid = getGridFromFile(fileName);
        LifeGUI::initialize();
        LifeGUI::resize(lifeGrid.numRows(), lifeGrid.numCols());    // Implement the GUI
        for (int i=0; i<lifeGrid.numRows(); i++) {
            for (int j=0; j<lifeGrid.numCols(); j++) {
                cout << lifeGrid[i][j];
                if (lifeGrid[i][j] == 'X') {
                    LifeGUI::fillCell(i, j);
                }
            }
            cout << endl;
        }
        LifeGUI::repaint();

        // Ask for user's action
        string action = getLine("a)nimate, t)ick, q)uit? ");

        while (action != "q") {
            while (action != "a" && action != "t" && action != "") {
                cout << "Please enter a valid command." << endl;
                action = getLine("a)nimate, t)ick, q)uit? ");
            }
            if (action == "t" || action == "") {
                // Compute and print the next generation
                Grid<char> newLifeGrid = computeNextGeneration(lifeGrid);
                LifeGUI::clear();
                for (int i=0; i<newLifeGrid.numRows(); i++) {
                    for (int j=0; j<newLifeGrid.numCols(); j++) {
                        cout << newLifeGrid[i][j];
                        if (newLifeGrid[i][j] == 'X') {
                            LifeGUI::fillCell(i, j);
                        }
                    }
                    cout << endl;
                }
                LifeGUI::repaint();
                lifeGrid = newLifeGrid;
            }
            if (action == "a") {
                int numOfFrames = getInteger("How many frames? ");
                while (numOfFrames < 0) {
                    cout << "Please enter a positive integer." << endl;
                    numOfFrames = getInteger("How many frames? ");
                }
                // Animates the update for each generation of cells
                // based on the input of numOfFrames
                for (int i=0; i<numOfFrames; i++) {
                    clearConsole();
                    Grid<char> newLifeGrid = computeNextGeneration(lifeGrid);
                    LifeGUI::clear();
                    for (int i=0; i<newLifeGrid.numRows(); i++) {
                        for (int j=0; j<newLifeGrid.numCols(); j++) {
                            cout << newLifeGrid[i][j];
                            if (newLifeGrid[i][j] == 'X') {
                                LifeGUI::fillCell(i, j);
                            }
                        }
                        cout << endl;
                    }
                    LifeGUI::repaint();
                    lifeGrid = newLifeGrid;
                    pause(100);
                }
            }
            action = getLine("a)nimate, t)ick, q)uit? ");
        }

        // Ask the user if he/she wants to upload another file
        string userInput = getLine("Load another file? ");
        while (userInput != "Y" && userInput != "N" &&
               userInput != "y" && userInput != "n") {
            cout << "Please type a word that starts with 'Y' or 'N'." << endl;
            userInput = getLine("Load another file? ");
        }
        if (userInput == "N" || userInput == "n") {
            addFile = false;
        }
        LifeGUI::shutdown();
    }

    cout << "Have a nice Life!" << endl;
    return 0;
}

Grid<char> getGridFromFile(string fileName) {
    ifstream input;
    input.open(fileName);
    string line;
    getline(input, line);
    int rows = stringToInteger(line);
    getline(input, line);
    int cols = stringToInteger(line);
    Grid<char> lifeGrid(rows, cols);

    for (int i=0; i<rows; i++) {
        getline(input, line);
        for (int j=0; j<cols; j++) {
            lifeGrid[i][j] = line[j];
        }
    }
    return lifeGrid;
}

Grid<char> computeNextGeneration(Grid<char> oldGrid) {
    int rows = oldGrid.numRows();
    int cols = oldGrid.numCols();
    Grid<char> newGrid(rows, cols);

    for (int i=0; i<rows; i++) {
        for (int j=0; j<cols; j++) {
            int neighbors = countNeighbors(i, j, oldGrid);
            if (neighbors <= 1) {
                // A cell with 1 or fewer neighbors dies
                newGrid[i][j] = '-';
            } else if (neighbors == 2) {
                // Locations with 2 neighbors remain stable
                newGrid[i][j] = oldGrid[i][j];
            } else if (neighbors == 3) {
                // Locations with 3 neighbors will create life
                newGrid[i][j] = 'X';
            } else if (neighbors >= 4) {
                // A cell with 4 or more neighbors dies
                newGrid[i][j] = '-';
            }
        }
    }
    return newGrid;
}

int countNeighbors(int x, int y, Grid<char> lifeGrid) {
    int startRow = max(0, x-1);
    int endRow = min(x+1, lifeGrid.numRows()-1);
    int startCol = max(0, y-1);
    int endCol = min(y+1, lifeGrid.numCols()-1);

    int neighbors = 0;
    for (int i=startRow; i<=endRow; i++) {
        for (int j=startCol; j<=endCol; j++) {
            if (lifeGrid[i][j] == 'X') {
                neighbors++;
            }
        }
    }

    if (lifeGrid[x][y] == 'X') {
        neighbors--;
    }
    return neighbors;
}
