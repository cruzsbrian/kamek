#include "util.h"

#include <cmath>
#include <iostream>

#include "eval/simple_eval.h"


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


float win_prob(int score) {
    /* return (float)score / 50.; */

    float adj_score = float(score) / 500.;
    return 1 / (1 + exp(-adj_score));
}


vector<int> eval_activations(board::Board board) {
    vector<int> ret;

    ret.push_back(eval::corners(board));
    ret.push_back(eval::edge_a(board));
    ret.push_back(eval::edge_b(board));
    ret.push_back(eval::c_sq_corner_empty(board));
    ret.push_back(eval::c_sq_corner_same(board));
    ret.push_back(eval::c_sq_corner_opp(board));
    ret.push_back(eval::x_sq_corner_empty(board));
    ret.push_back(eval::x_sq_corner_same(board));
    ret.push_back(eval::x_sq_corner_opp(board));
    ret.push_back(eval::mobility(board));
    ret.push_back(eval::frontier(board));

    return ret;
}


ProgressBar::ProgressBar(long unsigned steps) {
    this->steps = steps;
    this->last_progress = 0;
    this->current_val = 0;
}

void ProgressBar::start() {
    cerr << "0%";
    for (unsigned i = 0; i < PROGRESS_DOTS - 6; i++) cerr << "-";
    cerr << "100%\n";
}

void ProgressBar::step() {
    current_val++;

    unsigned p = current_val * PROGRESS_DOTS / steps;
    if (p != last_progress) {
        cerr << "." << std::flush;
        last_progress = p;
    }
}
