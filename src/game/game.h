#pragma once 

#include "../board/board.h"
#include "player.h"


class Game {
public:
    Game(Player *black, Player *white);
    Game(board::Board start, Player *black, Player *white);
    int play(bool display);
private:
    Player *players[2];
    board::Board b;
};
