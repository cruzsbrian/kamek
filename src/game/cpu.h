#pragma once

#include "player.h"
#include "../search/hashtable.h"


class CPU : public Player {
public:
    CPU(int search_depth, int endgame_depth);
    int next_move(board::Board b, int ms_left);
private:
    HashTable ht;
    int search_depth;
    int endgame_depth;
};
