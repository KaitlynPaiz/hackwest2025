#include <iostream>
#include <string>
#include <vector>
#include <algorithm> // For std::find
#include <chrono>    // For time management
#include <set>       // To store guessed letters efficiently
#include <random>    // For random word selection

// Enum to manage game states
enum GameState {
    TITLE_SCREEN,
    PLAYING,
    GAME_OVER
};

// --- Helper Functions (would be part of a Game class in a more complex setup) ---

// Function to display the masked word
std::string getMaskedWord(const std::string& word, const std::set<char>& guessedLetters) {
    std::string masked = "";
    for (char c : word) {
        if (guessedLetters.count(tolower(c))) {
            masked += c;
        } else {
            masked += '_';
        }
        masked += ' '; // Add a space for readability
    }
    return masked;
}

// Function to check if the word is completely guessed
bool isWordGuessed(const std::string& word, const std::set<char>& guessedLetters) {
    for (char c : word) {
        if (!guessedLetters.count(tolower(c))) {
            return false;
        }
    }
    return true;
}

// --- Main Game Logic (would be within a Game class) ---

int main() {
    GameState currentState = TITLE_SCREEN;

    std::random_device rd;
    std::mt19937 generator(rd());

    std::vector<std::string> words = {"dragon", "knight", "castle", "sword", "shield", 
        "bat", "goblin", "wizard", "troll", "orc", "elf", "dwarf", "phoenix", "griffin", 
        "hydra", "cyclops", "minotaur", "vampire", "skeleton", "giant", "witch", "warlock", 
        "necromancer", "cauldron", "armor", "chainmail", "enchanted", "fortress", "quest", 
        "legend", "myth", "sorcery", "alchemy", "runestone"};

    std::shuffle(words.begin(), words.end(), generator);
    std::string currentWord;
    int wordIndex = 0;
    std::set<char> guessedLetters;
    int hearts;
    std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
    const int TIME_LIMIT_SECONDS = 60;
    int score = 0;
    std::string currentMonster = "Bat"; // Starting monster

    //std::random_device rd;
    //std::mt19937 generator(rd());

    while (true) {
        system("cls"); // Clear console (use "clear" on Linux/macOS)

        if (currentState == TITLE_SCREEN) {
            std::cout << "***********************************\n";
            std::cout << "        WELCOME TO WORDCRUSADER    \n";
            std::cout << "         A MEDIEVAL WORD GAME      \n";
            std::cout << "***********************************\n\n";
            std::cout << "In this perilous quest, you must decipher words to defeat monsters!\n";
            std::cout << "You have " << TIME_LIMIT_SECONDS << " seconds and 7 hearts.\n";
            std::cout << "Each wrong letter costs a heart.\n";
            std::cout << "Guess wisely, brave crusader!\n\n";
            std::cout << "Press 'P' to Play or 'Q' to Quit: ";

            char choice;
            std::cin >> choice;
            if (tolower(choice) == 'p') {
                currentState = PLAYING;
                // Game initialization for a new round
                hearts = 7;
                guessedLetters.clear();
                std::uniform_int_distribution<int> distribution(0, words.size() - 1);
                if (wordIndex < words.size()) {
                    currentWord = words[wordIndex++];
                } else {
                    std::cout << "No more words available! Exiting game.\n";
                    currentState = GAME_OVER;
                    break;
                }
                startTime = std::chrono::high_resolution_clock::now();
                score = 0; // Reset score for a new game
            } else if (tolower(choice) == 'q') {
                break; // Exit game loop
            }
        } else if (currentState == PLAYING) {
            auto currentTime = std::chrono::high_resolution_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(currentTime - startTime).count();
            int timeLeft = TIME_LIMIT_SECONDS - elapsed;

            if (timeLeft <= 0) {
                std::cout << "\nTIME'S UP! The " << currentMonster << " overwhelmed you!\n";
                currentState = GAME_OVER;
                continue;
            }
            if (hearts <= 0) {
                std::cout << "\nYou ran out of hearts! The " << currentMonster << " defeats you!\n";
                currentState = GAME_OVER;
                continue;
            }

            std::cout << "-----------------------------------\n";
            std::cout << "Monster: " << currentMonster << "\n";
            std::cout << "Hearts: ";
            for (int i = 0; i < hearts; ++i) std::cout << "<3 ";
            std::cout << " |" << hearts << "/7|\n";
            std::cout << "Time Left: " << timeLeft << "s\n";
            std::cout << "Score: " << score << "\n";
            std::cout << "Word: " << getMaskedWord(currentWord, guessedLetters) << "\n";
            std::cout << "Guessed Letters: ";
            for (char l : guessedLetters) {
                std::cout << l << " ";
            }
            std::cout << "\n\nEnter your guess (a single letter): ";

            char guess;
            std::cin >> guess;
            guess = tolower(guess);

            if (guessedLetters.count(guess)) {
                std::cout << "You already guessed that letter!\n";
                // Optionally, don't penalize a heart for re-guessing
            } else {
                guessedLetters.insert(guess);
                if (currentWord.find(guess) == std::string::npos) {
                    std::cout << "Incorrect guess!\n";
                    hearts--;
                } else {
                    std::cout << "Correct guess!\n";
                    // If the word is guessed, move to the next word/monster
                    if (isWordGuessed(currentWord, guessedLetters)) {
                        score += 100 + (hearts * 10) + (timeLeft * 5); // Example scoring
                        std::cout << "\nCONGRATULATIONS! You defeated the " << currentMonster << "!\n";
                        // In a full game, you'd advance to the next monster/level here
                        // For this example, let's just pick a new word
                        std::cout << "Prepare for the next challenge...\n";
                        hearts = 7; // Reset hearts for the next challenge
                        guessedLetters.clear();
                        std::uniform_int_distribution<int> newWordDistribution(0, words.size() - 1);
                        if (wordIndex < words.size()) {
                            currentWord = words[wordIndex++];
                        } else {
                            std::cout << "No more words available! Exiting game.\n";
                            currentState = GAME_OVER;
                            break;
                        }
                        startTime = std::chrono::high_resolution_clock::now(); // Reset timer
                        // For monsters, you'd have an array of monster names and cycle through them.
                        // currentMonster = nextMonster();
                    }
                }
            }
            //std::cout << "Press Enter to continue...";
            //std::cin.ignore(); // Consume the newline character left by std::cin >> guess;
            //std::cin.get();    // Wait for user to press enter
        } else if (currentState == GAME_OVER) {
            std::cout << "\n***********************************\n";
            std::cout << "            GAME OVER              \n";
            std::cout << "          Final Score: " << score << "\n";
            std::cout << "***********************************\n\n";
            std::cout << "Press 'R' to Play Again or 'Q' to Quit: ";
            char choice;
            std::cin >> choice;
            if (tolower(choice) == 'r') {
                currentState = TITLE_SCREEN; // Go back to title to re-initialize
            } else if (tolower(choice) == 'q') {
                break; // Exit game loop
            }
        }
    }

    std::cout << "Thanks for playing WordCrusader!\n";
    return 0;
}