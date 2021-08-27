#include "cpu.h"

#include "../search/alphabeta.h"
#include "../search/endgame.h"
#include "../eval/pattern_eval.h"
#include "../util.h"

#include <iostream>
#include <fmt/core.h>
#include <climits>
#include <time.h>
#include <math.h>


const int ASP_WINDOW = 125;


int CPU::next_move(board::Board b, int ms_left) {
    int empties = 64 - board::popcount(b.own | b.opp);

    int moves_left = (empties - 20) / 2; // endgame solver after 20 is fast
    float time_left = (float)ms_left / 1000;

    float time_budget;
    if (ms_left == -1) { // default if game is not being timed
        time_budget = max_time;
    } else if (empties > endgame_depth) { // midgame search
        // Save 30s for endgame solver, and never over max_time.
        time_budget = min((time_left - 30.f) / moves_left, max_time);
    } else { // endgame search
        // Budget half of time remaining for search.
        time_budget = time_left / 2.0;
    }

    #ifdef PRINT_SEARCH_INFO
    cerr << "\n=======================|   WONKY_KONG   |=======================\n";
    cerr << board::to_str(b) << "\n";
    fmt::print(stderr, "{} empties, allocating {:.1f}s / {:.1f}s left\n",
               empties, time_budget, time_left);
    #endif


    if (empties <= endgame_depth) {
        #ifdef PRINT_SEARCH_INFO
        cerr << "Solving endgame with " << empties << " empties\n";
        #endif

        endgame::EndgameStats stats;

        int best_move = endgame::best_move(b, stats, -1, 1);

        #ifdef PRINT_SEARCH_INFO
        float nps = (float)stats.nodes / stats.time_spent;
        fmt::print(stderr, "{:.2e} nodes in {:.3f}s @ {:.2e} node/s\n",
                   (float)stats.nodes, stats.time_spent, nps);
        #endif

        if (best_move != endgame::MOVE_LOSE) { // accept draws
            #ifdef PRINT_SEARCH_INFO
            fmt::print(stderr, "Playing move {}\n", move_to_notation(best_move));
            #endif

            return best_move;
        }
    }



    int alpha = -INT_MAX;
    int beta = INT_MAX;

    long total_nodes = 0L;
    float total_time = 0.;
    float time_spent;
    float branch_factor = 4.0;

    int depth = 1;

    SearchNode result;
    while (total_time + time_spent * branch_factor < time_budget && depth <= max_depth) {
        if (depth >= empties) { // If reaching full depth, do a full window endgame search
            #ifdef PRINT_SEARCH_INFO
            cerr << "Solving endgame with " << empties << " empties\n";
            #endif

            endgame::EndgameStats stats;
            int best_move = endgame::best_move(b, stats, -INT_MAX, INT_MAX);

            #ifdef PRINT_SEARCH_INFO
            float nps = (float)stats.nodes / stats.time_spent;
            fmt::print(stderr, "{:.2e} nodes in {:.3f}s @ {:.2e} node/s\n",
                       (float)stats.nodes, stats.time_spent, nps);
            fmt::print(stderr, "Playing move {}\n", move_to_notation(best_move));
            #endif

            return best_move;
        }

        // Loop until search succeeds.
        while (true) {
            clock_t start = clock();
            long nodes = 0L;

            // Try search in current window
            #ifdef PRINT_SEARCH_INFO
            fmt::print(stderr, "depth {:2} ({:.2f}, {:.2f}) \t",
                       depth, win_prob(alpha), win_prob(beta));
            #endif
            result = ab_deep(b, alpha, beta, depth, ht, false, &nodes);

            time_spent = (float)(clock() - start) / CLOCKS_PER_SEC;
            total_time += time_spent;
            total_nodes += nodes;

            if (result.score >= beta) { // Fail-high: increase beta
                beta = beta + ASP_WINDOW * 2;

                #ifdef PRINT_SEARCH_INFO
                fmt::print(stderr, "-- HIGH  {:.3f}s\n", time_spent);
                #endif

            } else if (result.score <= alpha) { // Fail-low: decrease alpha
                alpha = alpha - ASP_WINDOW * 2;

                #ifdef PRINT_SEARCH_INFO
                fmt::print(stderr, "-- LOW   {:.3f}s\n", time_spent);
                #endif

            } else { // alpha < score < beta: success
                #ifdef PRINT_SEARCH_INFO
                fmt::print(stderr, "{} {:.3f} {:.3f}s\n",
                           move_to_notation(result.best_move), win_prob(result.score), time_spent);
                #endif

                branch_factor = pow((float)nodes, 1 / (float)depth);
                break;
            }
        }

        // Set aspiration window around result for next search.
        alpha = result.score - ASP_WINDOW;
        beta = result.score + ASP_WINDOW;

        // Increment by 1 when we are nearing the last few plys or the next
        // depth is last, otherwise increment by 2.
        if (total_time + time_spent * pow(branch_factor, 4) > time_budget || depth + 1 == max_depth) {
            depth += 1;
        } else {
            depth += 2;
        }
    }

    #ifdef PRINT_SEARCH_INFO
    float nps = (float)total_nodes / total_time;
    fmt::print(stderr, "{:.2e} nodes in {:.3f}s @ {:.2e} node/s\n",
               (float)total_nodes, total_time, nps);

    fmt::print(stderr, "Average branching factor: {:.3f}\n", branch_factor);

    // Follow PV through stored nodes to print predicted sequence of moves
    SearchNode *pv = &result;
    fmt::print(stderr, "PV: ");
    do {
        fmt::print(stderr, "{} -> ", move_to_notation(pv->best_move));
        b = board::do_move(b, pv->best_move);
    } while (pv->best_move != -1 && (pv = ht.get(b)) && pv->type == NodeType::PV); // fail-high nodes are not given best moves
    fmt::print(stderr, "...\n\n");
    #endif

    return result.best_move;
}
