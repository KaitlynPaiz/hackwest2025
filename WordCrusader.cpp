#include <iostream>
#include <string>
#include <vector>
#include <chrono>    // time
#include <set>       // guessed letters
#include <random>    // RNG
#include <cctype>    // tolower / isalpha
#include <limits>    // numeric_limits
#include <cstdlib>   // system()

// ---------------- Config ----------------
constexpr bool END_AFTER_LAST_MONSTER = true; // set false to loop Bat->...->Imp->Bat->...

// ---------------- State -----------------
enum GameState { TITLE_SCREEN, PLAYING, GAME_OVER };

// ---- Small utilities ----
inline char tolower_safe(char c) {
    return static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
}
inline bool isalpha_safe(char c) {
    return std::isalpha(static_cast<unsigned char>(c)) != 0;
}

void clearConsole() {
#ifdef _WIN32
    std::system("cls");
#else
    std::system("clear");
#endif
}

void waitForEnter(const char* prompt = "Press Enter to continue...") {
    std::cout << prompt;
    // Flush any leftover chars (including multiple) up to newline
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    // Now actually wait for a real Enter press
    std::cin.get();
}

// Function to display the masked word
std::string getMaskedWord(const std::string& word, const std::set<char>& guessedLetters) {
    std::string masked;
    masked.reserve(word.size() * 2);
    for (char c : word) {
        if (guessedLetters.count(tolower_safe(c))) masked += c;
        else                                      masked += '_';
        masked += ' ';
    }
    return masked;
}

// Function to check if the word is completely guessed
bool isWordGuessed(const std::string& word, const std::set<char>& guessedLetters) {
    for (char c : word) {
        if (!guessedLetters.count(tolower_safe(c))) return false;
    }
    return true;
}

// --- Main Game Logic ---
int main() {
    GameState currentState = TITLE_SCREEN;

    // Content
    const std::vector<std::string> words    = {"dragon", "knight", "castle", "sword", "shield", "bat"};
    const std::vector<std::string> monsters = {"Bat", "Goblin", "Ghoul", "Imp"};

    // State
    int currentMonsterIndex = 0;
    std::string currentWord;
    std::set<char> guessedLetters;
    int hearts = 7;
    const int TIME_LIMIT_SECONDS = 60;
    int score = 0;

    // Timing
    std::chrono::time_point<std::chrono::high_resolution_clock> startTime;

    // RNG
    std::random_device rd;
    std::mt19937 generator(rd());

    auto pickRandomWord = [&]() {
        std::uniform_int_distribution<int> dist(0, static_cast<int>(words.size() - 1));
        return words[dist(generator)];
    };

    while (true) {
        clearConsole();

        if (currentState == TITLE_SCREEN) {
            std::cout << "*****************************************\n";
            std::cout << "           WELCOME TO WORDCRUSADER       \n";
            std::cout << "             A MEDIEVAL WORD GAME        \n";
            std::cout << "*****************************************\n\n";
            std::cout << "Decipher words to defeat monsters!\n";
            std::cout << "You have " << TIME_LIMIT_SECONDS << " seconds and 7 hearts per monster.\n";
            std::cout << "Each wrong letter costs a heart.\n\n";
            std::cout << "Press 'P' to Play or 'Q' to Quit: ";

            char choice;
            if (!(std::cin >> choice)) return 0;
            choice = tolower_safe(choice);

            if (choice == 'p') {
                // Initialize a fresh game
                currentState = PLAYING;
                hearts = 7;
                guessedLetters.clear();
                currentWord = pickRandomWord();
                startTime = std::chrono::high_resolution_clock::now();
                score = 0;
                currentMonsterIndex = 0; // reset progression on new game

                // Eat trailing newline once after reading choice so next reads behave
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            } else if (choice == 'q') {
                break;
            } else {
                // Invalid key; flush line and show title again
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            }

        } else if (currentState == PLAYING) {
            auto now = std::chrono::high_resolution_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - startTime).count();
            int timeLeft = TIME_LIMIT_SECONDS - static_cast<int>(elapsed);

            if (timeLeft <= 0) {
                std::cout << "\nTIME'S UP! The " << monsters[currentMonsterIndex] << " overwhelmed you!\n";
                currentState = GAME_OVER;
                waitForEnter();
                continue;
            }
            if (hearts <= 0) {
                std::cout << "\nYou ran out of hearts! The " << monsters[currentMonsterIndex] << " defeats you!\n";
                currentState = GAME_OVER;
                waitForEnter();
                continue;
            }

            std::cout << "-----------------------------------\n";
            std::cout << "Monster: " << monsters[currentMonsterIndex] << "\n";
            std::cout << "Hearts:  " << std::string(hearts, '<') << "  " << hearts << "/7\n";
            std::cout << "Time Left: " << timeLeft << "s\n";
            std::cout << "Score:   " << score << "\n";
            std::cout << "Word:    " << getMaskedWord(currentWord, guessedLetters) << "\n";
            std::cout << "Guessed Letters: ";
            for (char l : guessedLetters) std::cout << l << ' ';
            std::cout << "\n\nEnter your guess (a single letter): ";

            char guess;
            if (!(std::cin >> guess)) return 0;
            guess = tolower_safe(guess);

            if (!isalpha_safe(guess)) {
                std::cout << "Please enter a letter (A-Z).\n";
                waitForEnter();
                continue;
            }

            if (guessedLetters.count(guess)) {
                std::cout << "You already guessed that letter!\n";
            } else {
                guessedLetters.insert(guess);
                if (currentWord.find(guess) == std::string::npos) {
                    std::cout << "Incorrect guess!\n";
                    --hearts;
                } else {
                    std::cout << "Correct guess!\n";
                    if (isWordGuessed(currentWord, guessedLetters)) {
                        score += 100 + (hearts * 10) + (timeLeft * 5);
                        std::cout << "\n-------------------------------\n";
                        std::cout << "You completed the word: " << currentWord << "!\n";
                        std::cout << "-------------------------------\n";

                        std::cout << "\nCONGRATULATIONS! You defeated the " << monsters[currentMonsterIndex] << "!\n";

                        // Advance monster (either stop after last, or wrap around)
                        if (END_AFTER_LAST_MONSTER) {
                            currentMonsterIndex++;
                            if (currentMonsterIndex >= static_cast<int>(monsters.size())) {
                                std::cout << "\nYou have defeated all the monsters! You are the ultimate WordCrusader!\n";
                                currentState = GAME_OVER;
                                waitForEnter();
                                continue;
                            }
                        } else {
                            currentMonsterIndex = (currentMonsterIndex + 1) % static_cast<int>(monsters.size());
                        }

                        // Prepare next challenge
                        hearts = 7;
                        guessedLetters.clear();
                        currentWord = pickRandomWord();
                        startTime = std::chrono::high_resolution_clock::now();

                        std::cout << "Next Monster: " << monsters[currentMonsterIndex] << "\n";
                        waitForEnter();
                    }
                }
            }

            //waitForEnter();

        } else if (currentState == GAME_OVER) {
            clearConsole();
            std::cout << "\n***********************************\n";
            std::cout << "            GAME OVER              \n";
            std::cout << "          Final Score: " << score << "\n";
            std::cout << "***********************************\n\n";
            std::cout << "Press 'R' to Play Again or 'Q' to Quit: ";

            char choice;
            if (!(std::cin >> choice)) return 0;
            choice = tolower_safe(choice);

            if (choice == 'r') {
                currentState = TITLE_SCREEN; // New game will re-init
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            } else if (choice == 'q') {
                break;
            } else {
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            }
        }
    }

    std::cout << "Thanks for playing WordCrusader!\n";
    return 0;
}
