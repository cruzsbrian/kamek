#include "cpu.h"

#include "../search/basic.h"
#include "../search/endgame.h"
#include "../eval/pattern_eval.h"
#include "../util.h"

#include <iostream>
#include <climits>
#include <time.h>


const int ASP_DEPTH_DELTA = 1;
const int ASP_WINDOW = 75;


CPU::CPU(int search_depth, int endgame_depth) {
    this->search_depth = search_depth;
    this->endgame_depth = endgame_depth;
}


int CPU::next_move(board::Board b, int ms_left) {
    int empties = 64 - board::popcount(b.own | b.opp);

    #ifdef PRINT_SEARCH_INFO
    cerr << "\n=======================|   WONKY_KONG   |=======================\n";
    /* cerr << board::to_grid(b, PIECE_OWN) << "\n"; */
    cerr << board::to_str(b) << "\n";

    cerr << empties << " empties\n";
    #endif

    if (empties <= endgame_depth) {
        #ifdef PRINT_SEARCH_INFO
        cerr << "Solving endgame with " << empties << " empties\n";
        #endif

        endgame::EndgameStats stats;

        int best_move = endgame::best_move(b, stats);

        #ifdef PRINT_SEARCH_INFO
        float nps = (float)stats.nodes / stats.time_spent;
        cerr << stats.nodes << " nodes in " << stats.time_spent << "s @ " << nps << " node/s\n";
        #endif

        if (best_move != endgame::MOVE_LOSE) {
            return best_move;
        }
    }

    long nodes = 0L;
    clock_t start = clock();

    // Lower depth search to set aspiration window
    int asp_depth = max(search_depth - ASP_DEPTH_DELTA, 0);
    int asp_score = ab_ff(b, -INT_MAX, INT_MAX, asp_depth, false, &nodes);
    int alpha = asp_score - ASP_WINDOW;
    int beta = asp_score + ASP_WINDOW;

    int best_move;

    // Loop until search succeeds
    while (true) {
        // Try search in current window
        int score = aspiration_search(b, &best_move, alpha, beta, &nodes);

        if (score >= beta) { // Fail-high: increase beta
            beta = beta + ASP_WINDOW * 2;
        } else if (score <= alpha) { // Fail-low: decrease alpha
            alpha = alpha - ASP_WINDOW * 2;
        } else { // alpha < score < beta: success
            break;
        }
    }

    #ifdef PRINT_SEARCH_INFO
    clock_t end = clock();
    float time_spent = (float)(end - start) / CLOCKS_PER_SEC;
    float nps = (float)nodes / time_spent;
    cerr << nodes << " nodes in " << time_spent << "s @ " << nps << " node/s\n";
    #endif

    return best_move;
}


int CPU::aspiration_search(board::Board b, int *move_out, int alpha, int beta, long *n) {
    #ifdef PRINT_SEARCH_INFO
    cerr << "Trying aspiration search in (" << win_prob(alpha) << ", " << win_prob(beta) << ")\n";
    cerr << "Running depth " << search_depth << " search\n";
    #endif

    int best_move = -1;

    uint64_t move_mask = board::get_moves(b);

    if (move_mask == 0ULL) {
        #ifdef PRINT_SEARCH_INFO
        cerr << "Must pass\n";
        #endif
        (*move_out) = -1;
        return 0;
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
        /* int score = eval::score(after); */
        int sort_depth = max(search_depth - 4, 0);
        int score = ab_ff(after, -beta, -alpha, sort_depth, false, n);

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

        int score = -ab_ff(moves[i].after, -beta, -alpha, search_depth, false, n);

        if (score >= beta) {
            return beta;
        }

        if (score > alpha) {
            best_move = moves[i].move;
            alpha = score;

#ifdef PRINT_SEARCH_INFO
            cerr << "Score for move " << move_to_notation(moves[i].move) << ": " << win_prob(score) << "\n";
        } else {
            cerr << "Score for move " << move_to_notation(moves[i].move) << ": --\n";
#endif
        }
    }

    (*move_out) = best_move;

    return alpha;
}
