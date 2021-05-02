#pragma once

#include <string>

#include "board/board.h"

using namespace std;


string move_to_notation(int move);
int notation_to_move(const string &input);

float score_to_pts(int score);


template <typename T> int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}


struct ScoredMove {
    int move;
    int score;
    board::Board after;
};



struct ScoredPosition {
    board::Board board;
    int score;
};
