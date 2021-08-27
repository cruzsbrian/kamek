#include "cpu.h"

#include "../search/alphabeta.h"
#include "../search/endgame.h"
#include "../eval/pattern_eval.h"
#include "../util.h"

#include <iostream>
#include <fmt/core.h>
#include <climits>
#include <time.h>


const int ASP_WINDOW = 75;
const float TIME_BUDGET = 10.;


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

        if (best_move != endgame::MOVE_LOSE) { // accept draws
            return best_move;
        }
    }


    long nodes = 0L;
    clock_t start = clock();

    int alpha = -INT_MAX;
    int beta = INT_MAX;
    float time_spent = 0.;
    int depth = 1;

    SearchNode result;
    while (time_spent * 4 < TIME_BUDGET && depth <= max_depth) {
        // Loop until search succeeds
        while (true) {
            // Try search in current window
            #ifdef PRINT_SEARCH_INFO
            fmt::print(stderr, "depth {:2} ({:.2f}, {:.2f}) \t",
                       depth, win_prob(alpha), win_prob(beta));
            #endif
            result = ab_deep(b, alpha, beta, depth, ht, false, &nodes);

            if (result.score >= beta) { // Fail-high: increase beta
                beta = beta + ASP_WINDOW * 2;
            } else if (result.score <= alpha) { // Fail-low: decrease alpha
                alpha = alpha - ASP_WINDOW * 2;
            } else { // alpha < score < beta: success
                break;
            }

            cerr << "\n";
        }

        alpha = result.score - ASP_WINDOW;
        beta = result.score + ASP_WINDOW;

        if (depth >= max_depth / 2) {
            depth += 1;
        } else {
            depth += 2;
        }

        time_spent = (float)(clock() - start) / CLOCKS_PER_SEC;

        #ifdef PRINT_SEARCH_INFO
        fmt::print(stderr, "{} {:.3f} {:.3f}s\n",
                   move_to_notation(result.best_move), win_prob(result.score), time_spent);
        #endif
    }

    #ifdef PRINT_SEARCH_INFO
    float nps = (float)nodes / time_spent;
    fmt::print(stderr, "{:.2e} nodes in {:.3f}s @ {:.2e} node/s\n",
               (float)nodes, time_spent, nps);

    // Follow PV through stored nodes to print predicted sequence of moves
    SearchNode *pv = &result;
    fmt::print(stderr, "PV: ");
    do {
        fmt::print(stderr, "{} -> ", move_to_notation(pv->best_move));
        b = board::do_move(b, pv->best_move);
    } while (pv-> best_move != -1 && (pv = ht.get(b)) && pv->type == NodeType::PV); // fail-high nodes are not given best moves
    fmt::print(stderr, "...\n\n");
    #endif

    return result.best_move;
}
