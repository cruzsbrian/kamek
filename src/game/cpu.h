#pragma once

#include "../search/hashtable.h"


class CPU {
public:
    CPU(int s, int e): max_depth(s), endgame_depth(e) {};
    int next_move(board::Board b, int ms_left);
private:
    HashTable ht;
    int max_depth;
    int endgame_depth;
};
