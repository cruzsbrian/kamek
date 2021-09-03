#pragma once

#include <string>
#include <vector>
#include <time.h>

#include "board/board.h"

using namespace std;


string move_to_notation(int move);
int notation_to_move(const string &input);

float win_prob(int score);

float get_time_since(clock_t start);


template <typename T> int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}


// Node types:
// PV: principal variation, score is exact
// HIGH: search failed high, score is lower bound
// LOW: search failed low, score is upper bound
// TIMEOUT: search timed out, score and bestmove are invalid
enum NodeType { PV, HIGH, LOW, TIMEOUT };

struct SearchNode {
    int depth;
    NodeType type;
    int score;
    int best_move;
};


struct ScoredMove {
    int move;
    int score;
    board::Board after;
};

bool operator<(const ScoredMove a, const ScoredMove b);


struct ScoredPosition {
    board::Board board;
    int score;
};


class ProgressBar {
public:
    ProgressBar(long unsigned steps);
    void start();
    void step();
private:
    const long unsigned PROGRESS_DOTS = 50;
    long unsigned steps;
    long unsigned current_val;
    long unsigned last_progress;
};
