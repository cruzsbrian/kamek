#pragma once 

#include "player.h"


class Game {
public:
    Game(Player *black, Player *white, bool show_turns);
    int play();
private:
    Player *players[2];
    board::Board b;

    bool show_turns;
};
