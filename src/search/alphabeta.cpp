#include "alphabeta.h"

#include <climits>
#include <iostream>

#include "../eval/pattern_eval.h"


const int DEEP_CUTOFF = 7;


SearchNode ab_deep(board::Board b, int alpha, int beta, int depth, bool passed, long *n) {
    (*n)++;

    uint64_t move_mask = board::get_moves(b);

    if (move_mask == 0ULL) {
        if (passed) { // Game is over: solved node
            int score = board::popcount(b.own) - board::popcount(b.opp);
            return {depth, NodeType::PV, score, -1};
        } else {
            int score = -ab_deep(board::Board{b.opp, b.own}, -beta, -alpha, depth, true, n).score;
            if (score > alpha) {
                return {depth, NodeType::PV, score, -1};
            } else {
                return {depth, NodeType::LOW, alpha, -1};
            }
        }
    }

    // Get all moves, boards, and opponent mobilities in arrays for sorting
    ScoredMove moves[32];
    int n_moves = 0;
    while (move_mask != 0ULL) {
        int m = __builtin_ctzll(move_mask);
        move_mask &= move_mask - 1;

        board::Board after = board::do_move(b, m);

        int sort_depth = max(0, depth - DEEP_CUTOFF);
        int score = ab(after, -beta, -alpha, sort_depth, false, n);

        moves[n_moves] = ScoredMove{m, score, after};
        n_moves++;
    }

    int best_move = -1;
    int best_score = alpha;
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

        if (depth <= DEEP_CUTOFF) {
            score = -ab(moves[i].after, -beta, -best_score, depth - 1, false, n);
        } else {
            score = -ab_deep(moves[i].after, -beta, -best_score, depth - 1, false, n).score;
        }

        if (score >= beta) {
            return {depth, NodeType::HIGH, beta, -1};
        }
        if (score > best_score) {
            best_score = score;
            best_move = moves[i].move;
        }
    }

    if (best_score > alpha) {
        return {depth, NodeType::PV, best_score, best_move};
    } else {
        return {depth, NodeType::LOW, alpha, best_move};
    }
}


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
