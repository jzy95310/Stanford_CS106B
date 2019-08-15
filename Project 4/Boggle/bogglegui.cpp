/*
 * CS 106B/X Boggle
 * This file implements the graphical user interface for Boggle.
 *
 * Please do not modify this provided file. Your turned-in files should work
 * with an unmodified version of all provided code files.
 *
 * @version 2018/11/05
 * - changed behavior of scorePoints function to not throw error when passed 0
 * @version 2018/11/03
 * - 106B 18au autograder version
 * @version 2018/10/25
 * - added threading to improve animation responsiveness on Mac systems
 * - additional decomp of human player handling code
 * @version 2018/10/22
 * - added repaint() calls to ask___() methods to remove stale GUI appearance
 * @version 2018/10/20
 * - 18au 106B initial version
 * @version 2016/10/25
 * - 16au initial version; heavily refactored into fully graphical UI
 *
 * @author original version by Eric Roberts and Julie Zelenski;
 *         modified by Marty Stepp for Autumn 2013 and Winter 2014;
 *         further modified by Marty Stepp for Autumn 2016 106X
 *
 * Known open issue: Leaks memory for all GObjects created.  Oops.
 */

#include "bogglegui.h"
#include "error.h"
#include "filelib.h"
#include "gevents.h"
#include "gthread.h"
#include "sound.h"
#include "strlib.h"

// these are the functions you must implement in bogglesearch.cpp
// (DO NOT MODIFY THESE HEADERS IN ANY WAY!)
bool humanWordSearch(Grid<char>& board,
                     Lexicon& dictionary,
                     std::string word);

Set<std::string> computerWordSearch(Grid<char>& board,
                                    Lexicon& dictionary,
                                    Set<std::string>& humanWords);


static Grid<char> mystery(int xyz, const Vector<std::string>& v);


// declare/initialize static variables and constants
const std::string BoggleGUI::BOGGLE_WINDOW_TITLE = "CS 106B/X Boggle";
const std::string BoggleGUI::DICTIONARY_FILE = "dictionary.txt";
const std::string BoggleGUI::CUBE_FONT = "Helvetica-Bold";
const std::string BoggleGUI::SCORE_FONT = "Helvetica";
#ifdef _WIN32
const std::string BoggleGUI::WORD_FONT = "Arial";
#else
const std::string BoggleGUI::WORD_FONT = "Geneva";
#endif
const Vector<std::string> BoggleGUI::LETTER_CUBES {
    "AAEEGN", "ABBJOO", "ACHOPS", "AFFKPS",
    "AOOTTW", "CIMOTU", "DEILRX", "DELRVY",
    "DISTTY", "EEGHNW", "EEINSU", "EHRTVW",
    "EIOSST", "ELRTTY", "HIMNQU", "HLNNRZ"
};
const Vector<std::string> BoggleGUI::LETTER_CUBES_BIG {
    "AAAFRS", "AAEEEE", "AAFIRS", "ADENNN", "AEEEEM",
    "AEEGMU", "AEGMNN", "AFIRSY", "BJKQXZ", "CCNSTW",
    "CEIILT", "CEILPT", "CEIPST", "DDLNOR", "DDHNOT",
    "DHHLOR", "DHLNOR", "EIIITT", "EMOTTT", "ENSSSU",
    "FIPRSY", "GORRVW", "HIPRRY", "NOOTUW", "OOOTTU"
};
const Vector<std::string> BoggleGUI::LETTER_CUBES_SUPER_BIG {
    "AAAFRS", "AAEEEE", "AAEEOO", "AAFIRS", "ABDEIO", "ADENNN",
    "AEEEEM", "AEEGMU", "AEGMNN", "AEILMN", "AEINOU", "AFIRSY",
    /* "AnErHeInQuTh" */ "AEHIQT", "BBJKXZ", "CCENST", "CDDLNN", "CEIITT", "CEIPST",
    "CFGNUY", "DDHNOT", "DHHLOR", "DHHNOW", "DHLNOR", "EHILRS",
    "EIILST", "EILPST", /* "EIO###" */ "EIOEIO", "EMTTTO", "ENSSSU", "GORRVW",
    "HIRSTV", "HOPRST", "IPRSYY", /* "JKQuWXZ" */ "JKQWXZ", "NOOTUW", "OOOTTU",
};

BoggleGUI* BoggleGUI::_instance = nullptr;

// declare/initialize member variables
BoggleGUI::BoggleGUI()
        : _window(nullptr),
          _promptLabel(nullptr),
          _textField(nullptr),
          _yesButton(nullptr),
          _noButton(nullptr),
          _humanScoreLabel(nullptr),
          _computerScoreLabel(nullptr),
          _statusLabel(nullptr),
          _animationDelay(DEFAULT_ANIMATION_DELAY),
          _totalScore {0, 0},
          _wordCount {0, 0},
          _lastBoardSize(BOARD_SIZE_DEFAULT),
          _consoleOutput(true),
          _graphicalOutput(true),
          _bigBoggleSupport(false),
          _autograderMode(false) {
    // empty
}

BoggleGUI::~BoggleGUI() {
    // TODO: delete
    _window = nullptr;
    _promptLabel = nullptr;
    _textField = nullptr;
    _yesButton = nullptr;
    _noButton = nullptr;
    _humanScoreLabel = nullptr;
    _computerScoreLabel = nullptr;
    _statusLabel = nullptr;
}

/*static*/ BoggleGUI* BoggleGUI::instance() {
    if (!_instance) {
        _instance = new BoggleGUI();
    }
    return _instance;
}

// implementation of static member functions
void BoggleGUI::askNone() {
    if (!_graphicalOutput) {
        return;
    }
    _promptLabel->setText(" ");   // empty, but retain its height
    _yesButton->setVisible(false);
    _noButton->setVisible(false);
    _textField->setVisible(false);
    _window->repaint();
}

void BoggleGUI::askText(const std::string& prompt, const std::string& initialValue) {
    std::string goodPrompt = trim(prompt) + " ";
    _promptLabel->setText(goodPrompt);
    if (_consoleOutput && !trim(prompt).empty()) {
        std::cout << goodPrompt;
        std::cout.flush();
    }

    if (!_graphicalOutput) {
        return;
    }
    _yesButton->setVisible(false);
    _noButton->setVisible(false);
    _textField->setVisible(true);

    _yesButton->setAccelerator("");
    _noButton->setAccelerator("");
    _textField->setText(initialValue);
    _textField->setEnabled(true);
    _textField->requestFocus();
    _window->repaint();
}

void BoggleGUI::askYesOrNo(const std::string& prompt) {
    std::string goodPrompt = trim(prompt) + " ";
    if (_consoleOutput && !trim(prompt).empty()) {
        std::cout << goodPrompt;
        std::cout.flush();
    }

    if (!_graphicalOutput) {
        return;
    }

    _promptLabel->setText(goodPrompt);
    _yesButton->setEnabled(true);
    _noButton->setEnabled(true);
    _yesButton->setAccelerator("y");
    _noButton->setAccelerator("n");

    _yesButton->setVisible(true);
    _noButton->setVisible(true);
    _textField->setVisible(false);

    _yesButton->requestFocus();
    _window->repaint();
}

/*
 * This internal helper does all the messy math to work out how to divide
 * up the space within the current graphics window to neatly fit the board,
 * the cubes, and the word lists.
 * This function does not actually add any GObjects to the GWindow;
 * but it does set the state of the ' structure to be used later.
 */
void BoggleGUI::calculateGeometry() {
    if (!_graphicalOutput) {
        return;
    }
    double boardSize = std::min(_window->getWidth() / 3 - 2 * INDENT,
                                _window->getHeight() - 2 * LABEL_HEIGHT);
    _cubeSize = std::min((boardSize - BOARD_BORDER) / rowCount(),
                        (boardSize - BOARD_BORDER) / columnCount());
    _boardRect.w = _boardRect.h = boardSize;
    _boardRect.y = LABEL_HEIGHT + LABEL_HEIGHT;
    double leftover = _window->getWidth() - _boardRect.w - 2 * INDENT;
    _scoreBox[HUMAN].x = INDENT;
    _scoreBox[HUMAN].y = _scoreBox[COMPUTER].y = _boardRect.y;
    _scoreBox[HUMAN].h = _scoreBox[COMPUTER].h = LABEL_HEIGHT;
    _scoreBox[HUMAN].w = leftover * HUMAN_PERCENTAGE;
    _boardRect.x = _scoreBox[HUMAN].x + _scoreBox[HUMAN].w + INDENT;
    _scoreBox[COMPUTER].x = _boardRect.x + _boardRect.w + INDENT;
    _scoreBox[COMPUTER].w = _window->getWidth() - _scoreBox[COMPUTER].x - INDENT;
    _statusBox.x = INDENT;
    _statusBox.y = LABEL_HEIGHT;
    _statusBox.w = _window->getWidth() - 2 * INDENT;
    _statusBox.h = LABEL_HEIGHT;

    _fontSize = _cubeSize / 2;
    _wordColumnWidth = _cubeSize * 1.45;
}

/*static*/ void BoggleGUI::clearHighlighting() {
    instance()->clearHighlightingImpl();
}

void BoggleGUI::clearHighlightingImpl() {
    ensureInitialized();
    if (!_graphicalOutput) {
        return;
    }
    for (int row = 0; row < rowCount(); row++) {
        for (int col = 0; col < columnCount(); col++) {
            setHighlighted(row, col, false, /* animate */ false);
        }
    }
}

void BoggleGUI::close() {
    if (isInitialized() && _window) {
        _window->setVisible(false);
        _window->close();
        delete _window;
        _window = nullptr;
        _humanScoreLabel = nullptr;
        _computerScoreLabel = nullptr;
        _statusLabel = nullptr;
        _letterCubes.resize(0, 0);
        _recordedWordLabels.clear();
    }
}

int BoggleGUI::columnCount() const {
    return _gameBoard.numCols();
}

int BoggleGUI::cubeCount() const {
    return rowCount() * columnCount();
}

void BoggleGUI::ensureInitialized() {
    if (!isInitialized()) {
        error("BoggleGUI has not been initialized yet");
    }
}

const Grid<bool>& BoggleGUI::getHighlighting() {
    return _highlighting;
}

int BoggleGUI::getScore(Player player) const {
    return _totalScore[player];
}

void BoggleGUI::handleNoClick() {
    // echo user input for logging and debugging
    if (_consoleOutput) {
        std::cout << "N" << std::endl;
    }

    if (_gameState == STATE_ASK_RANDOM_BOARD) {
        _gameState = STATE_TYPE_BOARD;
        askText("Type the " + integerToString(cubeCount())
                + " letters on the board:", _lastBoard);
    } else if (_gameState == STATE_GAME_OVER) {
        // quit out
        shutdown();
    }
}

void BoggleGUI::handleTextFieldEnterKeyPress() {
    // echo user input for logging and debugging
    if (_consoleOutput) {
        std::cout << _textField->getText() << std::endl;
    }

    if (_gameState == STATE_ASK_BOARD_SIZE) {
        // user is done typing board size
        if (!_textField->valueIsInteger()
                || !setBoardSize(_textField->getValueAsInteger())) {
            setStatusMessage("Board size must be an integer between "
                             + integerToString(BOARD_SIZE_MIN) + " and "
                             + integerToString(BOARD_SIZE_MAX), /* isError */ true);
        }
    } else if (_gameState == STATE_TYPE_BOARD) {
        // user is done typing board text
        Grid<char> newBoard(rowCount(), columnCount());
        std::string text = trim(toUpperCase(_textField->getText()));
        bool valid = false;
        if (makeBoard(newBoard, text)) {
            _gameBoard = newBoard;
            _textField->setText("");
            labelAllCubes(text);
            _gameState = STATE_HUMAN_TURN;

            if (_consoleOutput) {
                std::cout << std::endl;
            }

            setStatusMessage("Human player's turn:");
            askText("Type a word (or Enter to stop): ");
            valid = true;
        }

        if (!valid) {
            setStatusMessage("Board string must be exactly " + integerToString(cubeCount())
                             + " letters from A-Z.", /* isError */ true);
        }
    } else if (_gameState == STATE_HUMAN_TURN) {
        handleTextFieldEnterKeyPress_humanTurn();
    }
}

void BoggleGUI::handleTextFieldEnterKeyPress_humanTurn() {
    std::string text = trim(toUpperCase(_textField->getText()));
    _textField->setEnabled(false);

    if (_humanWords.contains(text)) {
        setStatusMessage("Duplicate word.", /* isError */ true);
        _textField->setText("");
        _textField->setEnabled(true);
        _textField->requestFocus();

    } else if (text.empty()) {
        // pressing Enter on empty string ends human's turn
        handleTextFieldEnterKeyPress_humanTurnComplete();
    } else {
        for (int i = 0; i < (int) text.length(); i++) {
            if (text[i] < 'A' || text[i] > 'Z') {
                handleTextFieldEnterKeyPress_humanTurnWordEntryComplete(/* wasValid */ false);
                return;
            }
        }

        setStatusMessage("Searching ...");

        if (_consoleOutput) {
            std::cout << "humanWordSearch for \"" << text << "\" ... ";
            std::cout.flush();
        }

        // calls your recursive human word search function
        // (in its own thread, so that it doesn't lock up the GUI)
        GThread::runInNewThreadAsync([this, text]() {
            bool result = humanWordSearch(_gameBoard, _dictionary, text);

            if (_consoleOutput) {
                std::cout << "returned " << std::boolalpha << result << std::endl;
                if (result) {
                    std::cout << "Human score is now " << _totalScore[HUMAN] << "." << std::endl;
                }
            }

            if (result) {
                _humanWords.add(text);
                setStatusMessage("You found a new word! \"" + text + "\"");
                BoggleGUI::recordWord(text, HUMAN);
            }

            handleTextFieldEnterKeyPress_humanTurnWordEntryComplete(/* wasValid */ result);
        });
    }
}

void BoggleGUI::handleTextFieldEnterKeyPress_humanTurnWordEntryComplete(bool wasValid) {
    if (!wasValid) {
        BoggleGUI::setStatusMessage("Invalid word or not found on the board.", /* isError */ true);
    }

    if (_consoleOutput) {
        std::cout << std::endl;
        std::cout << "Type a word (or Enter to stop): ";
        std::cout.flush();
    }

    _textField->setText("");
    _textField->setEnabled(true);
    _textField->requestFocus();
}

void BoggleGUI::handleTextFieldEnterKeyPress_humanTurnComplete() {
    // human player is done
    if (_consoleOutput) {
        std::cout << std::endl;
    }
    clearHighlighting();

    setStatusMessage("Computer player's turn:");
    _gameState = STATE_COMPUTER_TURN;
    askNone();

    if (_consoleOutput) {
        std::cout << "computerWordSearch ..." << std::endl;
    }

    // calls your recursive computer word search function
    // (in its own thread, so that it doesn't lock up the GUI)
    GThread::runInNewThreadAsync([this]() {
        Set<std::string> allWords = computerWordSearch(_gameBoard, _dictionary, _humanWords);
        if (_consoleOutput) {
            std::cout << "returned " << allWords << std::endl;
            std::cout << "Computer score is " << _totalScore[COMPUTER] << "." << std::endl;
            std::cout << std::endl;
        }

        // display words found on GUI
        for (const std::string& word : allWords) {
            recordWord(word, COMPUTER);
        }

        int humanScore = _totalScore[HUMAN];
        int computerScore = _totalScore[COMPUTER];
        if (humanScore > computerScore) {
            // not likely brah
            setStatusMessage("Wow, you beat me. It must be a glitch.", /* isError */ true);
        } else if (computerScore > humanScore) {
            setStatusMessage("I crushed you, puny human!");
        } else {
            setStatusMessage("We tied. Consider yourself lucky.");
        }

        _gameState = STATE_GAME_OVER;
        askYesOrNo("Play again?");
    });
}

void BoggleGUI::handleYesClick() {
    // echo user input for logging and debugging
    if (_consoleOutput) {
        std::cout << "Y" << std::endl;
    }

    if (_gameState == STATE_ASK_RANDOM_BOARD) {
        const Vector<std::string>& letterCubeVec =
                rowCount() == BOARD_SIZE_MIN ? LETTER_CUBES :
                rowCount() == BOARD_SIZE_MAX ? LETTER_CUBES_SUPER_BIG :
                LETTER_CUBES_BIG;
        Grid<char> newBoard = mystery(rowCount(), letterCubeVec);

        // verify that this is a legal Boggle board
        if (makeBoard(newBoard, newBoard.toString2D("", "", "", ""))) {
            _gameBoard = newBoard;
            labelAllCubes(newBoard);
            _gameState = STATE_HUMAN_TURN;

            if (_consoleOutput) {
                std::cout << _lastBoard << std::endl;
                std::cout << std::endl;
            }

            setStatusMessage("Human player's turn:");
            askText("Type a word (or Enter to stop): ");
        } else {
            setStatusMessage("Invalid board.", /* isError */ true);
        }
    } else if (_gameState == STATE_GAME_OVER) {
        // start over; clear words, score, labels of words found
        reset();

        if (_bigBoggleSupport) {
            // ask user for board size
            _gameState = STATE_ASK_BOARD_SIZE;
            askText("Board size? ", integerToString(_lastBoardSize));
        } else {
            // assume default board size
            setBoardSize(BOARD_SIZE_DEFAULT);
        }
    }
}

void BoggleGUI::initialize(int rows, int cols) {
    if (rows == -1) {
        rows = BoggleGUI::BOARD_SIZE_DEFAULT;
    }
    if (cols == -1) {
        cols = BoggleGUI::BOARD_SIZE_DEFAULT;
    }

    if (rows < BOARD_SIZE_MIN || rows > BOARD_SIZE_MAX
            || cols < BOARD_SIZE_MIN || cols > BOARD_SIZE_MAX) {
        error("BoggleGUI::initialize: invalid dimensions");
    }

    _gameState = _bigBoggleSupport ? STATE_ASK_BOARD_SIZE : STATE_ASK_RANDOM_BOARD;

    if (!_window && _graphicalOutput) {
        // create entire GUI
        _window = new GWindow(BOGGLE_WINDOW_WIDTH, BOGGLE_WINDOW_HEIGHT, /* visible */ false);
        _window->setWindowTitle(BOGGLE_WINDOW_TITLE);
        _window->setResizable(true);
        _window->setExitOnClose(!_autograderMode);

        _promptLabel = new GLabel("");
        _promptLabel->setFont("*-*-20");
        _window->setRegionAlignment(GWindow::REGION_NORTH, ALIGN_RIGHT);
        _window->addToRegion(_promptLabel, GWindow::REGION_NORTH);
        _promptLabel->setVisible(false);

        // we kind of hack a horizontal padding into the buttons using HTML and nbsp
        _yesButton = new GButton("Yes");
        // yesButton->setMnemonic('Y');
        // yesButton->setAccelerator("Ctrl-Y");
        _yesButton->setFont("*-20");
        _yesButton->setActionListener([this]() {
            handleYesClick();
        });
        _window->addToRegion(_yesButton, GWindow::REGION_NORTH);
        _yesButton->setVisible(false);

        _noButton = new GButton("No");
        // noButton->setMnemonic('N');
        // yesButton->setAccelerator("Ctrl-N");
        _noButton->setFont("*-20");
        _noButton->setActionListener([this]() {
            handleNoClick();
        });
        _window->addToRegion(_noButton, GWindow::REGION_NORTH);
        _noButton->setVisible(false);

        _textField = new GTextField(20);
        _textField->setActionListener([this]() {
            handleTextFieldEnterKeyPress();
        });
        _textField->setFont("Monospaced-Plain-18");
        _window->addToRegion(_textField, GWindow::REGION_NORTH);
        _textField->setVisible(false);
    }

    _gameBoard.resize(rows, cols);
    _highlighting.resize(rows, cols);
    calculateGeometry();

    _wordCount[HUMAN] = 0;
    _wordCount[COMPUTER] = 0;

    // draw the Boggle board's blue background
    if (_window && _graphicalOutput) {
        GRect* boardBackground = new GRect(_boardRect.x, _boardRect.y, _boardRect.w, _boardRect.h);
        boardBackground->setColor(BOARD_COLOR);
        boardBackground->setFilled(true);
        boardBackground->setFillColor(BOARD_COLOR);
        _window->add(boardBackground);

        if (!_humanScoreLabel || _computerScoreLabel) {
            // draw the labels of player names and score labels
            setupPlayerLabels(HUMAN, "Human");
            setupPlayerLabels(COMPUTER, "Computer");
        }

        if (!_statusLabel) {
            // set up status label
            _statusLabel = new GText("");
            _statusLabel->setFont(SCORE_FONT + "-" + integerToString(SCORE_FONT_SIZE));
            _statusLabel->setColor(LABEL_COLOR);
            _window->add(_statusLabel, _statusBox.x, _statusBox.y);
        }

        // draw the NxN grid of Boggle letter cubes
        setupLetterCubes();

        if (!_window->isVisible()) {
            _window->center();
            _window->setVisible(true);
        }

        _textField->requestFocus();
    }

    // clear out any in-progress game state
    reset();

    if (_dictionary.isEmpty()) {
        _dictionary.addWordsFromFile(DICTIONARY_FILE);
    }

    if (_bigBoggleSupport) {
        _gameState = STATE_ASK_BOARD_SIZE;
        askText("Board size? ", integerToString(_lastBoardSize));
    } else {
        _gameState = STATE_ASK_RANDOM_BOARD;
        askYesOrNo("Generate a random board?");
    }
}

bool BoggleGUI::isInitialized() const {
    if (_graphicalOutput) {
        return _window != nullptr;
    } else {
        return !_gameBoard.isEmpty();
    }
}

void BoggleGUI::labelAllCubes(const Grid<char>& letters) {
    ensureInitialized();
    if (letters.size() != cubeCount()) {
        error("BoggleGUI::labelAllCubes: incorrectly sized grid");
    }
    labelAllCubes(letters.toString2D("", "", "", ""));
}

void BoggleGUI::labelAllCubes(const std::string& letters) {
    ensureInitialized();
    if ((int) letters.length() < cubeCount()) {
        error("BoggleGUI::labelAllCubes: too few letters in string: \"" + letters + "\"");
    }
    int row = 0;
    int col = 0;
    bool labeledAll = false;
    for (int i = 0; i < (int) letters.length(); i++) {
        char letter = letters[i];
        if (!isalpha(letter)) {
            continue;
        }
        labelCube(row, col, letter);
        col++;
        if (col == columnCount()) {
            col = 0;
            row++;
            if (row == rowCount()) {
                labeledAll = true;
                break;
            }
        }
    }
    if (labeledAll) {
        _lastBoard = letters;
    } else {
        error("BoggleGUI::labelAllCubes: didn't label every cube from letters string: \"" + letters + "\"");
    }
}

void BoggleGUI::labelCube(int row, int col, char letter) {
    ensureInitialized();
    if (!_gameBoard.inBounds(row, col)) {
        error("BoggleGUI::labelCube: invalid row or column");
    }
    if (!isalpha(letter) && letter != ' ') {
        error("BoggleGUI::labelCube: invalid non-alphabetic character");
    }

    if (!_graphicalOutput) {
        return;
    }

    GText* label = _letterCubes[row][col].label;
    label->setText(std::string(1, letter));
    if (isalpha(letter)) {
        label->setLocation(
                _letterCubes[row][col].rect->getX() + _cubeSize/2 - 0.5 * label->getWidth(),
                _letterCubes[row][col].rect->getY() + _cubeSize/2 + 0.36 * _fontSize);
    }
}

// this function's implementation is intentionally dumb so you won't copy it
bool BoggleGUI::makeBoard(Grid<char>& board, const std::string& boardText) {
    if (static_cast<int>(boardText.length()) != cubeCount()) {
        return false;
    }
    int numRowsCols = round(sqrt(boardText.length()));
    board.resize(numRowsCols, numRowsCols);

    std::ostringstream out;
    out << "{";
    for (int i = 0, len = (int) boardText.length(); i < len; i++) {
        if (!isalpha(boardText[i])) {
            return false;
        }
        if (i % board.numCols() == 0) {
            out << "{";
        }
        out << (char) toupper(boardText[i]);
        if (i % board.numCols() == board.numCols() - 1) {
            out << "}";
        }
        if (i < len - 1) {
            out << ", ";
        }
    }
    out << "}";

    std::istringstream in(out.str());
    return static_cast<bool>(in >> board);
}

/*static*/ void BoggleGUI::playSound(const std::string& filename) {
    instance()->playSoundImpl(filename);
}

void BoggleGUI::playSoundImpl(const std::string& filename) {
    if (!_graphicalOutput) {
        return;
    }
    Sound s(filename);
    s.play();
}

void BoggleGUI::recordWord(const std::string& word, Player player) {
    ensureInitialized();
    if (player != HUMAN && player != COMPUTER) {
        error("BoggleGUI::recordWord: invalid player argument");
    }

    if (_graphicalOutput) {
        GText* label = new GText(toLowerCase(trim(word)));
        label->setFont(WORD_FONT + "-" + integerToString(WORD_FONT_SIZE));
        label->setColor(WORD_COLOR);
        int wordsPerRow = int((_scoreBox[player].w + 50) / _wordColumnWidth);
        int row = _wordCount[player] / wordsPerRow;
        int col = _wordCount[player] % wordsPerRow;
        int rowHeight = WORD_FONT_SIZE + 3;
        _window->add(label, _scoreBox[player].x + col * _wordColumnWidth,
                      _scoreBox[player].y + (row + 1) * rowHeight + WORD_DY);
        _recordedWordLabels.add(label);
    }
    _wordCount[player]++;
}

void BoggleGUI::reset() {
    ensureInitialized();

    // clear out the counts of words recorded so far and scores
    setStatusMessage("Initializing ...");
    _wordCount[HUMAN] = 0;
    _wordCount[COMPUTER] = 0;
    setScore(0, HUMAN);
    setScore(0, COMPUTER);
    _humanWords.clear();
    _highlighting.fill(false);

    // clear the word cubes
    for (int row = 0; row < rowCount(); row++) {
        for (int col = 0; col < columnCount(); col++) {
            labelCube(row, col, ' ');
        }
    }

    // clear out the words found by each player
    if (_graphicalOutput) {
        for (GText* label : _recordedWordLabels) {
            _window->remove(label);
        }
        _recordedWordLabels.clear();
    }
    setStatusMessage("");
}

int BoggleGUI::rowCount() const {
    return _gameBoard.numRows();
}

/*static*/ void BoggleGUI::scorePoints(int points, Player player) {
    instance()->scorePointsImpl(points, player);
}

void BoggleGUI::scorePointsImpl(int points, Player player) {
    ensureInitialized();
    if (player != HUMAN && player != COMPUTER) {
        error("BoggleGUI::scorePoints: invalid player argument");
    }
    if (points < 0) {
        error("BoggleGUI::scorePoints: number of points must be a positive integer");
    }
    setScore(_totalScore[player] + points, player);
}

/*static*/ void BoggleGUI::scorePointsComputer(int points) {
    instance()->scorePointsComputerImpl(points);
}

void BoggleGUI::scorePointsComputerImpl(int points) {
    scorePoints(points, COMPUTER);
}

/*static*/ void BoggleGUI::scorePointsHuman(int points) {
    instance()->scorePointsHumanImpl(points);
}

void BoggleGUI::scorePointsHumanImpl(int points) {
    scorePoints(points, HUMAN);
}

/*static*/ void BoggleGUI::setAnimationDelay(int ms) {
    instance()->setAnimationDelayImpl(ms);
}

void BoggleGUI::setAnimationDelayImpl(int ms) {
    _animationDelay = ms;
}

void BoggleGUI::setAutograderMode(bool autograderMode) {
    _autograderMode = autograderMode;
    if (_window) {
        _window->setExitOnClose(!_autograderMode);
    }
}

void BoggleGUI::setBigBoggleSupport(bool bigBoggle) {
    _bigBoggleSupport = bigBoggle;
}

bool BoggleGUI::setBoardSize(int size) {
    if (size < BOARD_SIZE_MIN || size > BOARD_SIZE_MAX) {
        return false;
    }
    if (_graphicalOutput) {
        _textField->setEnabled(false);
    }
    _lastBoardSize = size;
    initialize(size, size);
    setStatusMessage("");
    _gameState = STATE_ASK_RANDOM_BOARD;
    askYesOrNo("Generate a random board?");
    return true;
}

void BoggleGUI::setConsoleOutput(bool output) {
    _consoleOutput = output;
}

void BoggleGUI::setGraphicalOutput(bool output) {
    _graphicalOutput = output;
}

/*static*/ void BoggleGUI::setHighlighted(int row, int col, bool highlighted, bool animate) {
    instance()->setHighlightedImpl(row, col, highlighted, animate);
}

void BoggleGUI::setHighlightedImpl(int row, int col, bool highlighted, bool animate) {
    ensureInitialized();
    if (!_gameBoard.inBounds(row, col)) {
        error("BoggleGUI::setHighlighted: invalid row or column");
    }

    _highlighting[row][col] = highlighted;

    if (!_graphicalOutput) {
        return;
    }

    _letterCubes[row][col].rect->setFillColor(highlighted ? DIE_COLOR_H : DIE_COLOR);
    _letterCubes[row][col].label->setColor(highlighted ? LETTER_COLOR_H : LETTER_COLOR);
    if (animate && highlighted && _animationDelay > 0) {
        _window->pause(_animationDelay);
    }
}

void BoggleGUI::setScore(int points, Player player) {
    ensureInitialized();
    _totalScore[player] = points;

    if (_graphicalOutput) {
        GText* scoreLabel = player == COMPUTER ? _computerScoreLabel : _humanScoreLabel;
        scoreLabel->setText(integerToString(points));
    }
}

void BoggleGUI::setStatusMessage(const std::string& status, bool isError) {
    ensureInitialized();
    if (_consoleOutput && !status.empty()) {
        std::cout << status << std::endl;
    }
    if (_graphicalOutput) {
        _statusLabel->setLabel(status);
        _statusLabel->setColor(isError ? LABEL_COLOR_ERROR : LABEL_COLOR);
    }
}

void BoggleGUI::setupLetterCubes() {
    double lineWidth = 2;
    double cubeSize = BoggleGUI::_cubeSize - lineWidth;
    double cubeY = _boardRect.y + BOARD_BORDER / 2;

    // if (cubeCount() != letterCubes.size()) {
        // clear out any old rounded rectangles from the screen
        for (int row = 0; row < _letterCubes.numRows(); row++) {
            for (int col = 0; col < _letterCubes.numCols(); col++) {
                if (_letterCubes[row][col].rect) {
                    _window->remove(_letterCubes[row][col].rect);
                    _window->remove(_letterCubes[row][col].label);
                    delete _letterCubes[row][col].rect;
                    delete _letterCubes[row][col].label;
                    _letterCubes[row][col].rect = nullptr;
                    _letterCubes[row][col].label = nullptr;
                }
            }
        }
    // }

    _letterCubes.resize(rowCount(), columnCount());
    for (int row = 0; row < _letterCubes.numRows(); row++) {
        double cubeX = _boardRect.x + BOARD_BORDER / 2;
        for (int col = 0; col < _letterCubes.numCols(); col++) {
            // display the letter cubes as rounded rectangles
            double cubeRoundRadius = cubeSize / 6;
            GRoundRect* rect = new GRoundRect(cubeX, cubeY, cubeSize,
                                              cubeSize, cubeRoundRadius * 2);
            rect->setLineWidth(lineWidth);
            rect->setColor("Black");
            rect->setFilled(true);
            rect->setFillColor(DIE_COLOR);
            _window->add(rect);
            _letterCubes[row][col].rect = rect;

            // draw the letter on the cube
            GText* label = new GText("M");   // start as "M" for getWidth
            label->setFont(CUBE_FONT + "-" + integerToString(int(_fontSize)));
            label->setColor(LETTER_COLOR);
            label->setLocation(cubeX + cubeSize / 2 - label->getWidth() / 2,
                               cubeY + cubeSize / 2 + 0.4 * _fontSize);
            label->setLabel(" ");
            _window->add(label);
            _letterCubes[row][col].label = label;
            cubeX += cubeSize + lineWidth;
        }
        cubeY += cubeSize + lineWidth;
    }
}

void BoggleGUI::setupPlayerLabels(Player player, const std::string& name) {
    GLine* playerLine = new GLine(_scoreBox[player].x, _scoreBox[player].y,
            _scoreBox[player].x + _scoreBox[player].w,
            _scoreBox[player].y);
    playerLine->setColor(LABEL_COLOR);
    _window->add(playerLine);

    GText* label = new GText(name);
    label->setFont(SCORE_FONT + "-" + integerToString(SCORE_FONT_SIZE));
    _window->add(label, _scoreBox[player].x, _scoreBox[player].y + LABEL_DY);

    GText*& scoreLabel = player == COMPUTER ? _computerScoreLabel : _humanScoreLabel;
    if (scoreLabel) {
        scoreLabel->setText("0");
    } else {
        scoreLabel = new GText("0");
    }
    scoreLabel->setFont(SCORE_FONT + "-" + integerToString(SCORE_FONT_SIZE));
    scoreLabel->setColor(LABEL_COLOR);
    int offset = 32;
    _window->add(scoreLabel,
                _scoreBox[player].x + _scoreBox[player].w - offset,
                _scoreBox[player].y + LABEL_DY);
}

void BoggleGUI::shutdown() {
    if (isInitialized()) {
        if (_consoleOutput) {
            std::cout << "Exiting." << std::endl;
            std::cout.flush();
        }
        close();
    }
}

// what it do?
static Grid<char> mystery(int xyz, const Vector<std::string>& v) {
    Grid<char> g(xyz,xyz);unsigned short boom=xyz*xyz,m=48,s=xyz;
    top:boom--;g[boom/s][boom%s]=v[boom][rand()%(m>>3)];if(boom)goto top;
    shuffle(g);return g;
}

int main() {
    BoggleGUI::instance()->initialize();
    return 0;
}
