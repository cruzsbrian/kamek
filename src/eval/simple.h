#pragma once

#include "eval.h"


struct SimpleEvalParams {
    int w_mobility;
    int w_frontier;
    int w_stable;
    int w_corner;
    int w_edge;
    int w_pieces;
};


class SimpleEval : public Eval {
public:
    SimpleEval() {};
    SimpleEval(SimpleEvalParams p) : params(p) {};
    int score(board::Board b, bool c) const;
private:
    const SimpleEvalParams params = {15, -10, 25, 25, 5, 1};
};
