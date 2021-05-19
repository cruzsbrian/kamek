#include "../board/board.h"
#include "../util.h"

SearchNode ab_deep(board::Board b, int alpha, int beta, int empties, bool passed, long *n);
int ab(board::Board b, int alpha, int beta, int empties, bool passed, long *n);
