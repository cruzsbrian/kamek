#include "util.h"

#include <cmath>


string move_to_notation(int move) {
    if (move == -1) {
        return "pass";
    } else {
        int row = move / 8 + 1;
        char col = 'a' + move % 8;

        string s(1, col);
        return s + to_string(row);
    }
}

int notation_to_move(const string &input) {
    if (input == "pass") return -1;

    int row = input[1] - '1';
    char col = input[0] - 'a';

    return row * 8 + col;
}


float score_to_pts(int score) {
    /* float adj_score = float(score) / 500.; */
    /* return 1 / (1 + exp(-adj_score)); */
    return (float)score / 50.;
}
