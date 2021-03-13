#include "negamax.h"

#include "../eval/eval.h"
#include "../util.h"

#include <climits>
#include <iostream>
#include <time.h>


using namespace std;


int endgame_score(board::Board b) {
    return board::popcount(b.own) - board::popcount(b.opp);
}


NegamaxSearch::NegamaxSearch(Eval *e, int depth) {
    eval = e;
    max_depth = depth;
}


int NegamaxSearch::next_move(board::Board b, int ms_left) {
    int max_score = -INT_MAX; // use -INT_MAX so it can be negated without overflow
    int best_move = -1;

    clock_t start = clock();
    long n_nodes = 0L;

    uint64_t move_mask = board::get_moves(b);

    while (move_mask != 0ULL) {
        int m = __builtin_ctzll(move_mask);
        move_mask &= move_mask - 1;

        int score = -negamax_score(board::do_move(b, m), -INT_MAX, -max_score, 0, false, &n_nodes);

        if (score > max_score) {
            best_move = m;
            max_score = score;

            cerr << "Score for move " << move_to_notation(m) << ": " << score << "\n";
        } else {
            cerr << "Score for move " << move_to_notation(m) << ": --\n";
        }
    }

    clock_t end = clock();
    float time_spent = (float)(end - start) / CLOCKS_PER_SEC;
    float nps = (float)n_nodes / time_spent;
    cerr << n_nodes << " nodes in " << time_spent << "s @ " << nps << " node/s\n";

    return best_move;
}


int NegamaxSearch::negamax_score(board::Board b, int alpha, int beta, int depth, bool passed, long *n) {
    (*n)++;

    if (depth == max_depth) {
        return eval->score(b);
    }

    uint64_t move_mask = board::get_moves(b);

    if (move_mask == 0ULL) {
        // If the opponent passed, the game is over.
        if (passed) return endgame_score(b);
        return -negamax_score(board::Board{b.opp, b.own}, -beta, -alpha, depth + 1, true, n);
    }

    while (move_mask != 0ULL) {
        int m = __builtin_ctzll(move_mask);
        move_mask &= move_mask - 1;

        int score = -negamax_score(board::do_move(b, m), -beta, -alpha, depth + 1, false, n);
        if (score >= beta) return beta;
        if (score > alpha) alpha = score;
    }

    return alpha;
}
