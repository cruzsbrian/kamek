#include "cpu.h"

#include "../search/alphabeta.h"
#include "../search/endgame.h"
#include "../search/hashtable.h"
#include "../eval/pattern_eval.h"

#include <iostream>
#include <fmt/core.h>
#include <climits>
#include <time.h>
#include <math.h>


const int ASP_WINDOW = 125;


SearchResult CPU::next_move(board::Board b, int ms_left) {
    HashTable ht;

    int empties = 64 - board::popcount(b.own | b.opp);

    int moves_left = (empties - 20) / 2; // endgame solver after 20 is fast
    float time_left = (float)ms_left / 1000;

    float time_budget;
    if (ms_left == -1) { // default if game is not being timed
        time_budget = max_time;
    } else if (empties > endgame_depth) { // midgame search
        // Save 30s for endgame solver, and never over max_time or under 0.01s.
        time_budget = max(min((time_left - 30.f) / moves_left, max_time), 0.01f);
    } else { // endgame search
        // Budget half of time remaining for search.
        time_budget = min(time_left / 2.f, max_time);
    }

    #ifdef PRINT_SEARCH_INFO
    cerr << "\n=======================|   WONKY_KONG   |=======================\n";
    cerr << board::to_str(b) << "\n";
    fmt::print(stderr, "{} empties, allocating {:.1f}s / {:.1f}s left\n",
               empties, time_budget, time_left);
    #endif

    float total_time = 0.;

    if (empties <= endgame_depth) {
        long nodes = 0L;
        float time_spent;
        clock_t start = clock();

        bool wld = empties > 20;
        float time_limit = time_budget / 2.0;

        SearchNode result;
        if (wld) {
            #ifdef PRINT_SEARCH_INFO
            fmt::print(stderr, "endgame 100%W \t");
            #endif
            result = endgame::eg_deep(b, -1, 1, empties, false, &nodes, start, time_limit);
        } else {
            #ifdef PRINT_SEARCH_INFO
            fmt::print(stderr, "endgame 100%  \t");
            #endif
            result = endgame::eg_deep(b, -INT_MAX, INT_MAX, empties, false, &nodes, start, time_limit);
        }

        time_spent = get_time_since(start);

        if (result.type == NodeType::TIMEOUT) {
            #ifdef PRINT_SEARCH_INFO
            fmt::print(stderr, "TIMEOUT  {:.3f}s\n", time_spent);
            #endif
        } else if (result.best_move != endgame::MOVE_LOSE) { // accept draws
            #ifdef PRINT_SEARCH_INFO
            fmt::print(stderr, "{} ", move_to_notation(result.best_move), result.score);
            if (wld) {
                if (result.score > 0) fmt::print(stderr, "win\n");
                else if (result.score < 0) fmt::print(stderr, "loss\n");
                else fmt::print(stderr, "draw\n");
            } else {
                fmt::print(stderr, "score {}\n", result.score);
            }

            float nps = (float)nodes / time_spent;
            fmt::print(stderr, "{:.2e} nodes in {:.3f}s @ {:.2e} node/s\n\n",
                       (float)nodes, time_spent, nps);
            #endif

            return {result.best_move, wld ? DEPTH_100W : DEPTH_100, result.score, nodes, time_spent};
        } else {
            #ifdef PRINT_SEARCH_INFO
            fmt::print(stderr, "no move found\n");
            #endif
        }


        total_time = time_spent;
    }



    int alpha = -INT_MAX;
    int beta = INT_MAX;

    long total_nodes = 0L;
    float time_spent;
    float branch_factor = 4.0;

    int depth = 1;

    SearchNode result;
    while (total_time + time_spent * branch_factor < time_budget &&
           depth <= max_depth && depth <= empties) {
        clock_t start = clock();
        long nodes = 0L;

        // Try search in current window
        #ifdef PRINT_SEARCH_INFO
        fmt::print(stderr, "depth {:2} ({:.2f}, {:.2f})   ",
                   depth, win_prob(alpha), win_prob(beta));
        #endif
        float time_limit = time_budget - total_time;
        SearchNode new_result = ab_deep(b, alpha, beta, depth, ht, false, &nodes, start, time_limit);

        time_spent = get_time_since(start);
        total_time += time_spent;
        total_nodes += nodes;

        if (new_result.type == TIMEOUT) {
            #ifdef PRINT_SEARCH_INFO
            fmt::print(stderr, "TIMEOUT  {:.3f}s\n", time_spent);
            #endif
            break;
        }

        if (new_result.score >= beta) { // Fail-high: increase beta
            beta = beta + ASP_WINDOW * 2;

            #ifdef PRINT_SEARCH_INFO
            fmt::print(stderr, "-- HIGH  {:.3f}s\n", time_spent);
            #endif

        } else if (new_result.score <= alpha) { // Fail-low: decrease alpha
            alpha = alpha - ASP_WINDOW * 2;

            #ifdef PRINT_SEARCH_INFO
            fmt::print(stderr, "-- LOW   {:.3f}s\n", time_spent);
            #endif

        } else { // alpha < score < beta: success
            result = new_result;

            #ifdef PRINT_SEARCH_INFO
            fmt::print(stderr, "{} {:.3f} {:.3f}s\n",
                       move_to_notation(result.best_move), win_prob(result.score), time_spent);
            #endif

            branch_factor = pow((float)nodes, 1 / (float)depth);

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
    }

    #ifdef PRINT_SEARCH_INFO
    float nps = (float)total_nodes / total_time;
    fmt::print(stderr, "{:.2e} nodes in {:.3f}s @ {:.2e} node/s\n",
               (float)total_nodes, total_time, nps);
    fmt::print(stderr, "Average branching factor: {:.3f}\n", branch_factor);
    #endif

    // Follow PV through stored nodes to get predicted sequence of moves
    vector<int> pv_moves;
    SearchNode *pv = &result;
    do {
        pv_moves.push_back(pv->best_move);
        b = board::do_move(b, pv->best_move);
    } while ((pv = ht.get(b)) && pv->type == NodeType::PV);

    #ifdef PRINT_SEARCH_INFO
    fmt::print(stderr, "PV: ");
    for (auto m : pv_moves) fmt::print(stderr, "{} -> ", move_to_notation(m));
    fmt::print(stderr, "...\n\n");
    #endif

    return {result.best_move, result.depth, result.score, total_nodes, total_time, pv_moves};
}
