#include "bot.h"
#include <cstdlib> // For rand()
#include <ctime>   // For time()
#include "SDL.h"

Bot::Bot(){
    // Initialize random seed
    std::srand(std::time(0));
    _score = 0;
}

int Bot::GenRndmDirection() {
    // Generate a random number between 1 and 4
    return std::rand() % 4 + 1;
}
