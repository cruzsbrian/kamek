#include "../board/board.h"

namespace endgame {

const int MOVE_LOSE = 255;

struct EndgameStats {
    long nodes;
    float time_spent;
};

int solve(board::Board b, EndgameStats &stats, bool display);
int best_move(board::Board b, EndgameStats &stats);

int eg_deep(board::Board b, int alpha, int beta, int empties, bool passed, long *n);
int eg_medium(board::Board b, int alpha, int beta, int empties, bool passed, long *n);
int eg_shallow(board::Board b, int alpha, int beta, int empties, bool passed, long *n);

}
