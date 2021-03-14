#include "../board/board.h"

namespace endgame {

struct EndgameStats {
    long nodes;
    float time_spent;
};

int solve(board::Board b, EndgameStats &stats, bool display);

int eg_deep(board::Board b, int alpha, int beta, int empties, bool passed, long *n);
int eg_medium(board::Board b, int alpha, int beta, int empties, bool passed, long *n);
int eg_shallow(board::Board b, int alpha, int beta, int empties, bool passed, long *n);

}
