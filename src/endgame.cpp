#include "endgame.h"

#include <time.h>
#include <iostream>
#include <climits>

#include "pattern_eval.h"


namespace endgame {


const int DEEP_CUTOFF = 10;
const int MED_CUTOFF = 7;

const int KM_WEIGHT_DEEP = 3;
const int KM_WEIGHT_MED = 1;


int solve(board::Board b, EndgameStats &stats, bool display) {
    clock_t start = clock();
    long nodes = 0L;

    int empties = 64 - board::popcount(b.own | b.opp);
    SearchNode result = eg_deep(b, -INT_MAX, INT_MAX, empties, false, &nodes, start, 100.);

    clock_t end = clock();
    float time_spent = (float)(end - start) / CLOCKS_PER_SEC;

    stats.nodes += nodes;
    stats.time_spent += time_spent;

    if (display) {
        float nps = (float)nodes / time_spent;
        cerr << nodes << " nodes in " << time_spent << "s @ " << nps << " node/s\n";
    }

    return result.score;
}


SearchNode eg_deep(board::Board b, int alpha, int beta, int empties, bool passed, long *n, clock_t start, float time_limit) {
    (*n)++;

    // Check for timeout.
    if (get_time_since(start) >= time_limit) {
        return {empties, NodeType::TIMEOUT, 0, -1};
    }

    uint64_t move_mask = board::get_moves(b);

    if (move_mask == 0ULL) {
        if (passed) {
            int score = board::popcount(b.own) - board::popcount(b.opp);
            return {DEPTH_100, NodeType::PV, score, -1};
        } else {
            SearchNode result = eg_deep(board::Board{b.opp, b.own}, -beta, -alpha, empties, true, n, start, time_limit);
            if (result.type == NodeType::TIMEOUT) { // propagate timeouts back up
                return {DEPTH_100, NodeType::TIMEOUT, 0, -1};
            }

            return {DEPTH_100, NodeType::PV, -result.score, -1};
        }
    }

    // Get all moves, boards, and opponent mobilities in arrays for sorting
    ScoredMove moves[32];
    int n_moves = 0;
    while (move_mask != 0ULL) {
        int m = __builtin_ctzll(move_mask);
        move_mask &= move_mask - 1;

        board::Board after = board::do_move(b, m);
        int opp_moves = board::popcount(board::get_moves(after));

        if (m == 0 || m == 7 || m == 56 || m == 63) opp_moves -= KM_WEIGHT_DEEP;
        opp_moves += eval::score(after) / 40;

        moves[n_moves] = ScoredMove{m, opp_moves, after};
        n_moves++;
    }

    int best_move = MOVE_LOSE;
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

        if (empties <= DEEP_CUTOFF) {
            score = -eg_medium(moves[i].after, -beta, -best_score, empties - 1, false, n);
        } else {
            SearchNode result = eg_deep(moves[i].after, -beta, -best_score, empties - 1, false, n, start, time_limit);
            if (result.type == NodeType::TIMEOUT) { // propagate timeouts back up
                return {DEPTH_100, NodeType::TIMEOUT, 0, -1};
            }

            score = -result.score;
        }

        if (score >= beta) {
            return {DEPTH_100, NodeType::HIGH, beta, moves[i].move};
        }
        if (score > best_score) {
            best_score = score;
            best_move = moves[i].move;
        }
    }

    if (best_score > alpha) {
        return {DEPTH_100, NodeType::PV, best_score, best_move};
    } else {
        return {DEPTH_100, NodeType::LOW, alpha, best_move};
    }
}

int eg_medium(board::Board b, int alpha, int beta, int empties, bool passed, long *n) {
    (*n)++;

    uint64_t move_mask = board::get_moves(b);

    if (move_mask == 0ULL) {
        if (passed) return board::popcount(b.own) - board::popcount(b.opp);
        return -eg_medium(board::Board{b.opp, b.own}, -beta, -alpha, empties, true, n);
    }

    // Get all moves, boards, and opponent mobilities in arrays for sorting
    ScoredMove moves[32];
    int n_moves = 0;
    while (move_mask != 0ULL) {
        int m = __builtin_ctzll(move_mask);
        move_mask &= move_mask - 1;

        board::Board after = board::do_move(b, m);
        int opp_moves = board::popcount(board::get_moves(after));

        if (m == 0 || m == 7 || m == 56 || m == 63) opp_moves -= KM_WEIGHT_MED;

        moves[n_moves] = ScoredMove{m, opp_moves, after};
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

        if (empties <= MED_CUTOFF) {
            score = -eg_shallow(moves[i].after, -beta, -alpha, empties - 1, false, n);
        } else {
            score = -eg_medium(moves[i].after, -beta, -alpha, empties - 1, false, n);
        }

        if (score >= beta) return beta;
        if (score > alpha) alpha = score; 
    }

    return alpha;
}


int eg_shallow(board::Board b, int alpha, int beta, int empties, bool passed, long *n) {
    (*n)++;

    if (empties == 0) {
        return board::popcount(b.own) - board::popcount(b.opp);
    }

    uint64_t move_mask = board::get_moves(b);

    if (move_mask == 0ULL) {
        if (passed) return board::popcount(b.own) - board::popcount(b.opp);
        return -eg_shallow(board::Board{b.opp, b.own}, -beta, -alpha, empties, true, n);
    }

    while (move_mask != 0ULL) {
        int m = __builtin_ctzll(move_mask);
        move_mask &= move_mask - 1;

        int score = -eg_shallow(board::do_move(b, m), -beta, -alpha, empties - 1, false, n);

        if (score >= beta) return beta;
        if (score > alpha) alpha = score;
    }

    return alpha;
}


}
