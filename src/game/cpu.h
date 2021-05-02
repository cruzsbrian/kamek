#pragma once

#include "player.h"


class CPU : public Player {
public:
    CPU(int search_depth, int endgame_depth);
    int next_move(board::Board b, int ms_left);
private:
    int aspiration_search(board::Board b, int *move_out, int alpha, int beta, long *n);
    int search_depth;
    int endgame_depth;
};
