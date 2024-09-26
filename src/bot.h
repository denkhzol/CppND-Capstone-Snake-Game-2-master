#ifndef BOT_H
#define BOT_H
#include "player.h"

class Bot : public Player{
    public:
        Bot();

        // Generate random number between 1 and 4
        int GenRndmDirection();
};

#endif // BOT_H