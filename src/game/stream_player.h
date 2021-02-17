#include "player.h"
#include "../board/board.h"


class StreamPlayer : public Player {
public:
    int next_move(board::Board b, bool c, int ms_left);
};
