#pragma once 

#include "player.h"


class Game {
public:
    Game(Player *black, Player *white);
    int play();
private:
    Player *players[2];
    board::Board b;
};
