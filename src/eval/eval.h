#pragma once 

#include "../board/board.h"

class Eval {
public:
    virtual int score(board::Board b, bool c) const { return 0; }
};
