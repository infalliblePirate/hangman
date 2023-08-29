#include <iostream>
#include "../pugixml-1.13/src/pugixml.cpp"
#include <ctime>
#include <string>
#include <filesystem>


const int WORDS_COUNT = 1000;
const int MAX_WRONG_ATTEMPTS = 7;
const std::string ALREADY_CONTAINS = "character was already guessed";
const std::string WRONG_GUESS = "wrong";
const std::string CORRECT_GUESS = "correct";

std::string getRandomWord() {
    pugi::xml_document doc;
    if (!doc.load_file("data/words.xml")) {
        std::cerr << "Failed to load XML file." << std::endl;
    }

    int index = rand() % WORDS_COUNT;
    pugi::xml_node wordNode = doc.select_node(
            ("/words/word[@index='" + std::to_string(index) + "']").c_str()
    ).node();
    return wordNode.text().get();
}

void initSettings() {
    std::srand(static_cast<unsigned int>(std::time(nullptr)));   // Seed the random number generator
}

char inputUserGuess() {
    std::cout << "Enter a letter" << std::endl;
    char c;
    std::cin >> c;
    c = tolower(c);
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // clear the input buffer
    return c;
}

bool contains(const std::string &s, char ch) {
    for (char c: s) {
        if (c == ch) {
            return true;
        }
    }
    return false;
}

void revealChar(const std::string &word, std::string &hiddenWord, char c) {
    for (int i = 0; i < word.length(); i++) {
        if (word[i] == c) {
            hiddenWord[i] = c;
        }
    }
}

char inputAndValidateGuess(std::string &guessedChar) {
    char c = inputUserGuess();
    while (contains(guessedChar, c)) {
        std::cout << ALREADY_CONTAINS << "\n" << "Try again" << std::endl;
        c = inputUserGuess();
    }
    guessedChar += c;
    return c;
}

int getGameState(const std::string &word, std::string &hiddenWord, std::string &guessedChar) {
    char c = inputAndValidateGuess(guessedChar);
    if (contains(word, c)) {
        revealChar(word, hiddenWord, c);
        return 0; // returns 0 if correct, 1 if wrong
    } else {
        return 1;
    }
}

std::string getHiddenWord(int l) {
    std::string hidden;
    for (int i = 0; i < l; i++) {
        hidden += "_";
    }
    return hidden;
}

void printHangman(int index) {
    pugi::xml_document doc;
    if (!doc.load_file("data/hangman.xml")) {
        std::cerr << "Failed to load XML file." << std::endl;
    }

    pugi::xml_node wordNode = doc.select_node(
            ("/states/state[@index='" + std::to_string(index) + "']").c_str()
    ).node();
    std::cout<<  wordNode.text().get() <<std::endl;
}

void printGameState(const int state, const int counter) {

    if (state == 0) {
        std::cout << CORRECT_GUESS << std::endl;
    } else {
        std::cout << WRONG_GUESS << std::endl;
        printHangman(counter);
    }
}

void startGameLoop() {
    int counter = 0; // counts the num of wrong guesses
    std::string word = getRandomWord();
    std::string guessedChar; // keeps track of guessed characters
    std::string hiddenWord = getHiddenWord(word.length());

    while (true) {
        std::cout << hiddenWord << std::endl;
        int state = getGameState(word, hiddenWord, guessedChar);
        counter += state;
        if (counter == MAX_WRONG_ATTEMPTS) {
            printHangman(MAX_WRONG_ATTEMPTS);
            std::cout << "You lost" << std::endl;
            std::cout << std::endl;
            std::cout << "The word was " << word << std::endl;
            return;
        } else if (!contains(hiddenWord, '_')) {
            std::cout << "You won" << std::endl;
            std::cout << std::endl;
            std::cout << "The word was " << word << std::endl;
            return;
        }
        printGameState(state, counter);
    }

}

void startGameRound() {
    std::cout << "-------NEW ROUND-------" << std::endl;
    startGameLoop();
}

void startNewGame() {
    char userInput;
    do {
        std::cout << std::endl;
        std::cout << "Do you want to continue? [y/n] ";
        std::cin >> userInput;

        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // clear the input buffer

        if (userInput == 'n') {
            break;
        }

        startGameRound();
    } while (true);
}

int main() {
    initSettings();
    startNewGame();
    return 0;
}
