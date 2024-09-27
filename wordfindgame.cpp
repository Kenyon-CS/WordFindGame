#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <random>

const int GRID_SIZE = 20;

// Directions for placing words
enum Direction { HORIZONTAL, VERTICAL, DIAGONAL };

// Structure to represent a word placement
struct WordPlacement {
    std::string word;
    int row;
    int col;
    Direction dir;
};

void loadWordsFromFile(const std::string& filename, std::vector<std::string>& words);
void selectRandomWords(const std::vector<std::string>& allWords, std::vector<std::string>& selectedWords, int minWords, int maxWords);
bool placeWordInGrid(std::vector<std::vector<char>>& grid, const std::string& word, int row, int col, Direction dir);
void generateWordGrid(const std::vector<std::string>& words, std::vector<std::vector<char>>& grid);
void fillEmptySpaces(std::vector<std::vector<char>>& grid);
void printGrid(const std::vector<std::vector<char>>& grid);
void writeGridToFile(const std::vector<std::vector<char>>& grid, const std::vector<std::string>& words, const std::string& filename);

int main() {
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    std::string wordFilename;
    std::cout << "Enter the filename containing the list of words: ";
    std::getline(std::cin, wordFilename);

    // Load words from file
    std::vector<std::string> allWords;
    loadWordsFromFile(wordFilename, allWords);

    if (allWords.empty()) {
        std::cerr << "No words loaded. Exiting program." << std::endl;
        return 1;
    }

    // Select between 10 to 20 words randomly
    std::vector<std::string> selectedWords;
    selectRandomWords(allWords, selectedWords, 10, 20);

    // Initialize grid
    std::vector<std::vector<char>> grid(GRID_SIZE, std::vector<char>(GRID_SIZE, ' '));

    // Generate the word grid
    generateWordGrid(selectedWords, grid);

    // Fill empty spaces with random letters
    fillEmptySpaces(grid);

    // Output the grid to the screen
    std::cout << "\nGenerated Word Find Grid:\n" << std::endl;
    printGrid(grid);

    // Ask user for output filename
    std::string outputFilename;
    std::cout << "\nEnter the filename to save the grid: ";
    std::getline(std::cin, outputFilename);

    // Write the grid and words to the file
    writeGridToFile(grid, selectedWords, outputFilename);

    std::cout << "\nGrid and word list have been written to " << outputFilename << std::endl;

    // Output the list of words to find
    std::cout << "\nWords to find:\n";
    for (const auto& word : selectedWords) {
        std::cout << word << std::endl;
    }

    return 0;
}

void loadWordsFromFile(const std::string& filename, std::vector<std::string>& words) {
    std::ifstream infile(filename);
    if (!infile) {
        std::cerr << "Unable to open word file: " << filename << std::endl;
        return;
    }
    std::string word;
    while (std::getline(infile, word)) {
        if (!word.empty()) {
            // Convert word to uppercase
            std::transform(word.begin(), word.end(), word.begin(), ::toupper);
            if (word.length() <= GRID_SIZE) { // Ensure word fits in the grid
                words.push_back(word);
            }
        }
    }
    infile.close();
}

void selectRandomWords(const std::vector<std::string>& allWords, std::vector<std::string>& selectedWords, int minWords, int maxWords) {
    int numWords = std::min(static_cast<int>(allWords.size()), maxWords);
    if (numWords < minWords) {
        numWords = minWords;
    }
    selectedWords = allWords;
    std::shuffle(selectedWords.begin(), selectedWords.end(), std::default_random_engine(std::rand()));
    selectedWords.resize(numWords);
}

bool placeWordInGrid(std::vector<std::vector<char>>& grid, const std::string& word, int row, int col, Direction dir) {
    int len = word.length();
    int dRow = 0, dCol = 0;

    switch (dir) {
        case HORIZONTAL: dCol = 1; break;
        case VERTICAL: dRow = 1; break;
        case DIAGONAL: dRow = 1; dCol = 1; break;
    }

    // Check boundaries
    int endRow = row + dRow * (len - 1);
    int endCol = col + dCol * (len - 1);

    if (endRow < 0 || endRow >= GRID_SIZE || endCol < 0 || endCol >= GRID_SIZE) {
        return false;
    }

    // Check if the word can be placed
    for (int i = 0; i < len; ++i) {
        char currentChar = grid[row + i * dRow][col + i * dCol];
        if (currentChar != ' ' && currentChar != word[i]) {
            return false;
        }
    }

    // Place the word
    for (int i = 0; i < len; ++i) {
        grid[row + i * dRow][col + i * dCol] = word[i];
    }

    return true;
}

void generateWordGrid(const std::vector<std::string>& words, std::vector<std::vector<char>>& grid) {
    for (const auto& word : words) {
        bool placed = false;
        int attempts = 0;

        while (!placed && attempts < 100) { // Limit attempts to avoid infinite loops
            int dir = std::rand() % 3; // 0: Horizontal, 1: Vertical, 2: Diagonal
            int row = std::rand() % GRID_SIZE;
            int col = std::rand() % GRID_SIZE;

            placed = placeWordInGrid(grid, word, row, col, static_cast<Direction>(dir));
            ++attempts;
        }

        if (!placed) {
            std::cerr << "Unable to place word: " << word << std::endl;
        }
    }
}

void fillEmptySpaces(std::vector<std::vector<char>>& grid) {
    for (auto& row : grid) {
        for (auto& cell : row) {
            if (cell == ' ') {
                cell = 'A' + std::rand() % 26; // Random uppercase letter
            }
        }
    }
}

void printGrid(const std::vector<std::vector<char>>& grid) {
    // Print column numbers
    std::cout << "   ";
    for (int col = 0; col < GRID_SIZE; ++col) {
        std::cout << std::setw(2) << col << " ";
    }
    std::cout << "\n";

    // Print the grid
    for (int row = 0; row < GRID_SIZE; ++row) {
        std::cout << std::setw(2) << row << " ";
        for (int col = 0; col < GRID_SIZE; ++col) {
            std::cout << " " << grid[row][col] << " ";
        }
        std::cout << "\n";
    }
}

void writeGridToFile(const std::vector<std::vector<char>>& grid, const std::vector<std::string>& words, const std::string& filename) {
    std::ofstream outfile(filename);
    if (!outfile) {
        std::cerr << "Unable to open output file: " << filename << std::endl;
        return;
    }

    // Write the grid
    outfile << "GRID:" << std::endl;
    // Write column numbers
    outfile << "   ";
    for (int col = 0; col < GRID_SIZE; ++col) {
        outfile << std::setw(2) << col << " ";
    }
    outfile << "\n";

    // Write the grid
    for (int row = 0; row < GRID_SIZE; ++row) {
        outfile << std::setw(2) << row << " ";
        for (int col = 0; col < GRID_SIZE; ++col) {
            outfile << " " << grid[row][col] << " ";
        }
        outfile << "\n";
    }

    // Write the words to find
    outfile << "\nWORDS TO FIND:\n";
    for (const auto& word : words) {
        outfile << word << std::endl;
    }

    outfile.close();
}
