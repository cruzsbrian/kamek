#pragma once

#include "common.h"

struct SearchResult {
    SearchNode node;
    long nodes;
    double time_spent;
};


class CPU {
public:
    CPU(int s, double t, int e, bool p):
        max_depth(s),
        max_time(t),
        endgame_depth(e),
        print_search_info(p) {};
    SearchResult next_move(board::Board b, int ms_left);
private:
    SearchResult search(board::Board b, int empties, double time_budget, bool try_endgame);
    SearchNode midgame_search(board::Board b, int empties, double time_limit, long *nodes, bool forward_prune);
    SearchNode endgame_search(board::Board b, int empties, double time_limit, long *nodes, bool wld);
    double est_eg_time(int empties);
    int est_eg_empties(double time);
    double avg_nps();

    const int max_depth;
    const double max_time;
    const int endgame_depth;
    const bool print_search_info;

    long total_nodes = 0L;
    double total_time = 0;
};
