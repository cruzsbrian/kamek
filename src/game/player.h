#pragma once

#include "../board/board.h"


class Player {
public:
    virtual int next_move(board::Board b, int ms_left) {return -1;};
};
