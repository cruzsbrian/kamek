#include <vector>
#include <time.h>

#include "board.h"
#include "common.h"
#include "hashtable.h"

struct SearchInfo {
    HashTable *ht;
    long nodes;
    clock_t start;
    float time_limit;
    bool forward_prune;

    SearchInfo(HashTable *ht, float time_limit, bool forward_prune) {
        this->ht = ht;
        this->nodes = 0L;
        this->start = clock();
        this->time_limit = time_limit;
        this->forward_prune = forward_prune;
    }
};

SearchNode ab_deep(
    board::Board b,
    int alpha,
    int beta,
    int depth,
    bool passed,
    SearchInfo &si
);
int ab_medium(board::Board b, int alpha, int beta, int depth, bool passed, SearchInfo &si);
int ab(board::Board b, int alpha, int beta, int depth, bool passed, SearchInfo &si);

vector<ScoredMove> get_sorted_moves(board::Board b, int depth, SearchInfo &si);
