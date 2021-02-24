#pragma once

#include "eval.h"

#define N_PARAMS_SIMPLE 6

struct SimpleEvalParams {
    int p[N_PARAMS_SIMPLE] = {0, 0, 0, 0, 0, 0};
};

class SimpleEval : public Eval {
public:
    SimpleEval() {};
    SimpleEval(SimpleEvalParams p) : params(p) {};
    int score(board::Board b, bool c) const;
private:
    const SimpleEvalParams params = {10, -10, 25, 25, 5, 1};
};
