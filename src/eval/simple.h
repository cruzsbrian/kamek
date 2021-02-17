#pragma once 

#include "eval.h"


class SimpleEval : public Eval {
public:
    int score(board::Board b, bool c);
private:
    const static int w_mobility = 15;
    const static int w_frontier = -10;
    const static int w_stable = 25;
    const static int w_corner = 25;
    const static int w_edge = 5;
    const static int w_pieces = 1;
};
