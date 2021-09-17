#include "board.h"
#include "common.h"

namespace endgame {

const int MOVE_LOSE = 255;

struct EndgameStats {
    long nodes = 0L;
    float time_spent = 0.;
};

int solve(board::Board b, EndgameStats &stats, bool display);

SearchNode eg_deep(board::Board b, int alpha, int beta, int empties, bool passed, long *n, clock_t start, float time_limit);
int eg_medium(board::Board b, int alpha, int beta, int empties, bool passed, long *n);
int eg_shallow(board::Board b, int alpha, int beta, int empties, bool passed, long *n);

}
