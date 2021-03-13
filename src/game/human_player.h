#include "player.h"
#include "../board/board.h"


class HumanPlayer : public Player {
public:
    int next_move(board::Board b, int ms_left);
};
