#pragma once

#include "common.h"

struct SearchResult {
    int best_move;
    int depth;
    int score;
    long nodes;
    float time_spent;
    vector<int> pv;
};


class CPU {
public:
    CPU(int s, float t, int e, bool p):
        max_depth(s),
        max_time(t),
        endgame_depth(e),
        print_search_info(p) {};
    SearchResult next_move(board::Board b, int ms_left);
private:
    const int max_depth;
    const float max_time;
    const int endgame_depth;
    const bool print_search_info;
};
