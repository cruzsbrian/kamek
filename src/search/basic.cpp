#include "basic.h"

#include <climits>
#include <iostream>

#include "../eval/pattern_eval.h"
#include "../util.h"


const int FF_CUTOFF = 7;


int ab(board::Board b, int alpha, int beta, int depth, bool passed, long *n) {
    (*n)++;

    if (depth == 0) {
        return eval::score(b);
    }

    uint64_t move_mask = board::get_moves(b);

    if (move_mask == 0ULL) {
        if (passed) return (board::popcount(b.own) - board::popcount(b.opp)) * 50;
        return -ab(board::Board{b.opp, b.own}, -beta, -alpha, depth, true, n);
    }

    while (move_mask != 0ULL) {
        int m = __builtin_ctzll(move_mask);
        move_mask &= move_mask - 1;

        int score = -ab(board::do_move(b, m), -beta, -alpha, depth - 1, false, n);

        if (score >= beta) return beta;
        if (score > alpha) alpha = score;
    }

    return alpha;
}


int ab_deep(board::Board b, int alpha, int beta, int depth, bool passed, long *n) {
    (*n)++;

    if (depth == 0) {
        return eval::score(b);
    }

    uint64_t move_mask = board::get_moves(b);

    if (move_mask == 0ULL) {
        if (passed) return (board::popcount(b.own) - board::popcount(b.opp)) * 50;
        return -ab_deep(board::Board{b.opp, b.own}, -beta, -alpha, depth, true, n);
    }

    // Get all moves, boards, and opponent mobilities in arrays for sorting
    ScoredMove moves[32];
    int n_moves = 0;
    while (move_mask != 0ULL) {
        int m = __builtin_ctzll(move_mask);
        move_mask &= move_mask - 1;

        board::Board after = board::do_move(b, m);
        /* int opp_moves = board::popcount(board::get_moves(after)); */

        /* if (m == 0 || m == 7 || m == 56 || m == 63) opp_moves -= KM_WEIGHT_MED; */
        int sort_depth = max(0, depth - FF_CUTOFF);
        /* int score = eval::score(after); */
        int score = ab(after, -beta, -alpha, sort_depth, false, n);

        moves[n_moves] = ScoredMove{m, score, after};
        n_moves++;
    }

    for (auto i = 0; i < n_moves; i++) {
        // Traverse ahead to find best move index
        int best = INT_MAX;
        int best_idx = i;
        for (auto j = i + 1; j < n_moves; j++) {
            int score = moves[j].score;
            if (score < best) {
                best = score;
                best_idx = j;
            }
        }

        // Swap with current position
        if (best_idx != i) {
            ScoredMove tmp = moves[i];
            moves[i] = moves[best_idx];
            moves[best_idx] = tmp;
        }

        // Get score
        int score;

        if (depth <= FF_CUTOFF) {
            score = -ab(moves[i].after, -beta, -alpha, depth - 1, false, n);
        } else {
            score = -ab_deep(moves[i].after, -beta, -alpha, depth - 1, false, n);
        }

        if (score >= beta) return beta;
        if (score > alpha) alpha = score; 
    }

    return alpha;
}
