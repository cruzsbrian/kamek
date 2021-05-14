#include "endgame.h"

#include <time.h>
#include <iostream>
#include <climits>

#include "../util.h"
#include "../eval/simple_eval.h"


namespace endgame {


const int DEEP_CUTOFF = 10;
const int MED_CUTOFF = 7;

const int KM_WEIGHT_DEEP = 3;
const int KM_WEIGHT_MED = 1;


int solve(board::Board b, EndgameStats &stats, bool display) {
    clock_t start = clock();
    long nodes = 0L;

    int empties = 64 - board::popcount(b.own | b.opp);
    int score = eg_deep(b, -1, 1, empties, false, &nodes);

    clock_t end = clock();
    float time_spent = (float)(end - start) / CLOCKS_PER_SEC;

    stats.nodes += nodes;
    stats.time_spent += time_spent;

    if (display) {
        float nps = (float)nodes / time_spent;
        cerr << nodes << " nodes in " << time_spent << "s @ " << nps << " node/s\n";
    }

    return score;
}


int best_move(board::Board b, EndgameStats &stats) {
    clock_t start = clock();
    long nodes = 0L;

    int empties = 64 - board::popcount(b.own | b.opp);

    uint64_t move_mask = board::get_moves(b);

    if (move_mask == 0ULL) {
        #ifdef PRINT_SEARCH_INFO
        cerr << "Must pass\n";
        #endif
        return -1;
    }

    // Get all moves, boards, and opponent board scores in arrays for sorting
    ScoredMove moves[32];
    int n_moves = 0;
    while (move_mask != 0ULL) {
        int m = __builtin_ctzll(move_mask);
        move_mask &= move_mask - 1;

        board::Board after = board::do_move(b, m);
        int score = eval::score(after);

        moves[n_moves] = ScoredMove{m, score, after};
        n_moves++;
    }


    int max_score = -1;
    int best_move = MOVE_LOSE; // if no winning moves are found, return this code

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
        int score = -eg_deep(moves[i].after, -1, -max_score, empties - 1, false, &nodes);

        #ifdef PRINT_SEARCH_INFO
        cerr << "Score for move " << move_to_notation(moves[i].move) << ": " << score << "\n";
        #endif

        if (score > max_score) {
            max_score = score;
            best_move = moves[i].move;
        }

        if (score >= 1) { // accept any win
            clock_t end = clock();
            float time_spent = (float)(end - start) / CLOCKS_PER_SEC;

            stats.nodes += nodes;
            stats.time_spent += time_spent;

            return moves[i].move;
        }
    }

    clock_t end = clock();
    float time_spent = (float)(end - start) / CLOCKS_PER_SEC;

    stats.nodes += nodes;
    stats.time_spent += time_spent;

    return best_move;
}


int eg_deep(board::Board b, int alpha, int beta, int empties, bool passed, long *n) {
    (*n)++;

    uint64_t move_mask = board::get_moves(b);

    if (move_mask == 0ULL) {
        if (passed) return board::popcount(b.own) - board::popcount(b.opp);
        return -eg_deep(board::Board{b.opp, b.own}, -beta, -alpha, empties, true, n);
    }

    // Get all moves, boards, and opponent mobilities in arrays for sorting
    ScoredMove moves[32];
    int n_moves = 0;
    while (move_mask != 0ULL) {
        int m = __builtin_ctzll(move_mask);
        move_mask &= move_mask - 1;

        board::Board after = board::do_move(b, m);
        int opp_moves = board::popcount(board::get_moves(after));

        /* if (m == 0 || m == 7 || m == 56 || m == 63) opp_moves -= KM_WEIGHT_DEEP; */
        opp_moves += eval::score(after) / 40;

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

        if (empties <= DEEP_CUTOFF) {
            score = -eg_medium(moves[i].after, -beta, -alpha, empties - 1, false, n);
        } else {
            score = -eg_deep(moves[i].after, -beta, -alpha, empties - 1, false, n);
        }

        if (score >= beta) {
            return beta;
        }
        if (score > alpha) {
            alpha = score;
        }
    }

    return alpha;
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
