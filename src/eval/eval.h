#pragma once 

#include "../board/board.h"

class Eval {
public:
    virtual int score(board::Board b, bool c) { return 0; }
};
