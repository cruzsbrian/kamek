#include "cpu.h"

#include "../search/alphabeta.h"
#include "../search/endgame.h"
#include "../eval/pattern_eval.h"
#include "../util.h"

#include <iostream>
#include <climits>
#include <time.h>


const int ASP_DEPTH_DELTA = 2;
const int ASP_WINDOW = 75;


CPU::CPU(int search_depth, int endgame_depth) {
    this->search_depth = search_depth;
    this->endgame_depth = endgame_depth;
}


int CPU::next_move(board::Board b, int ms_left) {
    int empties = 64 - board::popcount(b.own | b.opp);

    #ifdef PRINT_SEARCH_INFO
    cerr << "\n=======================|   WONKY_KONG   |=======================\n";
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

    #ifdef PRINT_SEARCH_INFO
    cerr << "Running depth " << search_depth << " search\n";
    #endif

    long nodes = 0L;
    clock_t start = clock();

    // Lower depth search to set aspiration window
    int asp_depth = max(search_depth - ASP_DEPTH_DELTA, 0);
    int asp_score = ab_deep(b, -INT_MAX, INT_MAX, asp_depth, false, &nodes).score;
    int alpha = asp_score - ASP_WINDOW;
    int beta = asp_score + ASP_WINDOW;

    SearchNode result;
    int best_move;

    // Loop until search succeeds
    while (true) {
        // Try search in current window
        #ifdef PRINT_SEARCH_INFO
        cerr << "Trying aspiration search in (" << win_prob(alpha) << ", " << win_prob(beta) << ")\n";
        #endif
        result = ab_deep(b, alpha, beta, search_depth, false, &nodes);
        /* result.score = aspiration_search(b, &best_move, alpha, beta, &nodes); */
        /* result.best_move = best_move; */

        if (result.score >= beta) { // Fail-high: increase beta
            beta = beta + ASP_WINDOW * 2;
        } else if (result.score <= alpha) { // Fail-low: decrease alpha
            alpha = alpha - ASP_WINDOW * 2;
        } else { // alpha < score < beta: success
            break;
        }
    }

    #ifdef PRINT_SEARCH_INFO
    cerr << "Best move: " << move_to_notation(result.best_move) << " score " << win_prob(result.score) << "\n";
    clock_t end = clock();
    float time_spent = (float)(end - start) / CLOCKS_PER_SEC;
    float nps = (float)nodes / time_spent;
    cerr << nodes << " nodes in " << time_spent << "s @ " << nps << " node/s\n";
    #endif

    return result.best_move;
}
