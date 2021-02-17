#pragma once

#include "../board/board.h"


class Player {
public:
    virtual int next_move(board::Board b, bool c, int ms_left) {return -1;};
};
