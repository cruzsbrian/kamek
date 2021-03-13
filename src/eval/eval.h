#pragma once 

#include "../board/board.h"

class Eval {
public:
    virtual int score(board::Board b) const { return 0; }
};
