#include "negamax.h"

#include "../eval/eval.h"
#include "../util.h"

#include <climits>
#include <iostream>


using namespace std;


NegamaxSearch::NegamaxSearch(const Eval &e, int depth) {
    /* eval = e; */
    max_depth = depth;
}


int NegamaxSearch::next_move(board::Board b, bool c, int ms_left) {
    int max_score = -INT_MAX; // use -INT_MAX so it can be negated without overflow
    int best_move = -1;

    uint64_t move_mask = board::get_moves(b, c);

    while (move_mask != 0ULL) {
        int m = __builtin_ctzll(move_mask);
        move_mask &= move_mask - 1;

        int score = -negamax_score(board::do_move(b, m, c), !c, -INT_MAX, -max_score, 0);

        if (score > max_score) {
            best_move = m;
            max_score = score;

            cout << "Score for move " << move_to_notation(m) << ": " << score << "\n";
        } else {
            cout << "Score for move " << move_to_notation(m) << ": --\n";
        }
    }

    return best_move;
}


int NegamaxSearch::negamax_score(board::Board b, bool c, int alpha, int beta, int depth) {
    if (depth == max_depth) {
        return eval.score(b, c);
    }

    uint64_t move_mask = board::get_moves(b, c);

    if (move_mask == 0ULL) {
        return -negamax_score(b, !c, -beta, -alpha, depth + 1);
    }

    while (move_mask != 0ULL) {
        int m = __builtin_ctzll(move_mask);
        move_mask &= move_mask - 1;

        int score = -negamax_score(board::do_move(b, m, c), !c, -beta, -alpha, depth + 1);
        if (score >= beta) return beta;
        if (score > alpha) alpha = score;
    }

    return alpha;
}
