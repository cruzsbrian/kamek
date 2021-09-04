#pragma once

#include "../search/hashtable.h"


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
    CPU(int s, float t, int e): max_depth(s), max_time(t), endgame_depth(e) {};
    SearchResult next_move(board::Board b, int ms_left);
private:
    HashTable ht;
    int max_depth;
    const float max_time;
    int endgame_depth;
};
