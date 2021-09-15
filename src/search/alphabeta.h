#include <vector>

#include "../board/board.h"
#include "../util.h"
#include "hashtable.h"

SearchNode ab_deep(board::Board b, int alpha, int beta, int depth, HashTable &ht, bool passed, long *n, clock_t start, float time_limit);
int ab_medium(board::Board b, int alpha, int beta, int empties, HashTable &ht, bool passed, long *n);
int ab(board::Board b, int alpha, int beta, int empties, bool passed, long *n);

vector<ScoredMove> get_sorted_moves(board::Board b, int depth, HashTable &ht, long *n);
