#include "cpu.h"

#include "alphabeta.h"
#include "endgame.h"
#include "hashtable.h"
#include "pattern_eval.h"

#include <iostream>
#include <fmt/core.h>
#include <climits>
#include <time.h>
#include <math.h>


const int ASP_WINDOW = 125;


SearchResult CPU::next_move(board::Board b, int ms_left) {
    int empties = 64 - board::popcount(b.own | b.opp);

    if (print_search_info) {
        cerr << "\n=======================|   WONKY_KONG   |=======================\n";
        cerr << board::to_str(b) << endl;
        fmt::print(stderr, "{} empties, allocating ", empties);
    }

    double time_budget;
    bool try_endgame;
    if (ms_left == -1) {
        time_budget = max_time;
        try_endgame = est_eg_time(empties) < time_budget;
        if (print_search_info) fmt::print(stderr, "{:.1f}s\n", time_budget);
    } else {
        double time_left = (double)ms_left / 1000;

        // Start assuming we'll solve endgame.
        int eg_empties = empties;
        double eg_time = est_eg_time(eg_empties);
        double midgame_time = 0;
        int midgame_moves = 0;

        // If solving would take too long, adjust for midgame searches.
        try_endgame = eg_time < time_left && eg_time < max_time;
        if (try_endgame) {
            time_budget = max(min(time_left * 0.9, max_time), 0.001);
        } else {
            do {
                eg_empties -= 2;
                eg_time = est_eg_time(eg_empties);
                midgame_moves++;
                midgame_time = time_left - eg_time;
            } while ((midgame_time / midgame_moves) < eg_time / 3 && eg_empties >= 0);

            time_budget = max(min(midgame_time / midgame_moves, max_time), 0.001);
        }

        if (print_search_info) fmt::print(stderr, "{:.1f}s / {:.1f}s left\n", time_budget, time_left);
        if (!try_endgame) fmt::print(stderr, "saving {:.1f}s for endgame at {} empties\n", eg_time, eg_empties);
    }

    SearchResult result = search(b, empties, time_budget, try_endgame);

    total_nodes += result.nodes;
    total_time += result.time_spent;

    if (print_search_info) {
        double nps = (double)result.nodes / result.time_spent;
        fmt::print(stderr, "{:.2e} nodes in {:.3f}s @ {:.2e} node/s\n\n", (double)result.nodes, result.time_spent, nps);
    }

    return result;
}


SearchResult CPU::search(board::Board b, int empties, double time_budget, bool try_endgame) {
    long nodes = 0L;
    clock_t start = clock();

    // Endagme search
    if (try_endgame) {
        // Try WLD search first
        SearchNode wld_result = endgame_search(b, empties, time_budget * 0.75, &nodes, true);

        if (wld_result.type != NodeType::TIMEOUT) {
            // If draw (meaning score is exact), return immediately
            if (wld_result.score == 0) return {wld_result, nodes, get_time_since(start)};

            // Otherwise, try full search if we have time
            if (get_time_since(start) * 10.0 < time_budget) {
                SearchNode full_result = endgame_search(b, empties, time_budget - get_time_since(start), &nodes, false);

                if (full_result.type != NodeType::TIMEOUT) {
                    return {full_result, nodes, get_time_since(start)};
                }
            }

            // If not enough time for full search, take a win if we have one.
            if (wld_result.score > 0) return {wld_result, nodes, get_time_since(start)};
        }
    }

    // Midgame search
    SearchNode mid_result = midgame_search(b, empties, time_budget - get_time_since(start), &nodes, true); 

    // If search returns a guaranteed win/loss, check with no forward pruning.
    if (mid_result.score == INT_MAX || mid_result.score == -INT_MAX) {
        if (print_search_info) fmt::print("re-search without forward pruning\n");
        mid_result = midgame_search(b, empties, time_budget, &nodes, false);
    }

    return {mid_result, nodes, get_time_since(start)};
}


SearchNode CPU::midgame_search(board::Board b, int empties, double time_limit, long *nodes, bool forward_prune) {
    HashTable ht;

    int alpha = -INT_MAX;
    int beta = INT_MAX;

    double time_spent = 0.0;
    double last_time = 0.0;
    double branch_factor = 3.0;

    int depth = 1;

    SearchNode result;
    while (time_spent + last_time * branch_factor < time_limit &&
           depth <= max_depth && depth <= empties) {
        
        // Try search in current window
        if (print_search_info) {
            fmt::print(stderr, "depth {:2} ({:.2f}, {:.2f})   ",
                depth, win_prob(alpha), win_prob(beta));
        }

        SearchInfo si(&ht, time_limit - time_spent, forward_prune);
        SearchNode new_result = ab_deep(b, alpha, beta, depth, false, si);

        last_time = get_time_since(si.start);
        time_spent += last_time;
        (*nodes) += si.nodes;

        if (new_result.type == TIMEOUT) {
            if (print_search_info) {
                fmt::print(stderr, "TIMEOUT  {:.3f}s\n", time_spent);
            }
            break;
        }

        // If search returned a guaranteed win/loss, don't try to search deeper.
        if (new_result.score == INT_MAX || new_result.score == -INT_MAX) {
            result = new_result;
            
            if (print_search_info) {
                if (result.score == INT_MAX) fmt::print(stderr, "win      {:.3f}s\n", time_spent);
                else fmt::print(stderr, "loss     {:.3f}s\n", time_spent);
            }

            break;
        }

        if (new_result.score >= beta && new_result.score != INT_MAX) { // Fail-high: increase beta
            beta = beta + ASP_WINDOW * 2;

            if (print_search_info) {
                fmt::print(stderr, "-- HIGH  {:.3f}s\n", time_spent);
            }

        } else if (new_result.score <= alpha && new_result.score != -INT_MAX) { // Fail-low: decrease alpha
            alpha = alpha - ASP_WINDOW * 2;

            if (print_search_info) {
                fmt::print(stderr, "-- LOW   {:.3f}s\n", time_spent);
            }

        } else { // alpha < score < beta: success
            result = new_result;

            if (print_search_info) {
                fmt::print(stderr, "{} {:.3f} {:.3f}s\n",
                        move_to_notation(result.best_move), win_prob(result.score), time_spent);
            }

            branch_factor = pow((float)si.nodes, 1 / (float)depth);

            // Set aspiration window around result for next search.
            alpha = result.score - ASP_WINDOW;
            beta = result.score + ASP_WINDOW;

            // Increment by 1 when we are nearing the last few plys or the next
            // depth is last, otherwise increment by 2.
            if (time_spent + last_time * pow(branch_factor, 4) > time_limit || depth + 1 == max_depth) {
                depth += 1;
            } else {
                depth += 2;
            }
        }
    }

    return result;
}


SearchNode CPU::endgame_search(board::Board b, int empties, double time_limit, long *nodes, bool wld) {
    clock_t start = clock();

    SearchNode result;
    if (wld) {
        fmt::print(stderr, "endgame 100%W \t");
        result = endgame::eg_deep(b, -1, 1, empties, false, nodes, start, time_limit);
    } else {
        fmt::print(stderr, "endgame 100%  \t");
        result = endgame::eg_deep(b, -INT_MAX, INT_MAX, empties, false, nodes, start, time_limit);
    }

    double time_spent = get_time_since(start);

    if (print_search_info) {
        if (result.type == NodeType::TIMEOUT) {
            fmt::print(stderr, "TIMEOUT  {:.3f}s\n", time_spent);
        } else {
            fmt::print(stderr, "{} ", move_to_notation(result.best_move));
            if (wld) {
                if (result.score > 0) fmt::print(stderr, "win\n");
                else if (result.score < 0) fmt::print(stderr, "loss\n");
                else fmt::print(stderr, "draw\n");
            } else {
                fmt::print(stderr, "score {}\n", result.score);
            }
        }
    }

    return result;
}


/**
 * Estimates time for a WLD endgame search. Based on FFO test suite positions.
 */
double CPU::est_eg_time(int empties) {
    double nodes = 2.62e-3 * exp(1.13 * (double)empties);
    return nodes / (avg_nps() * 3);   // endgame is faster than midgame search
}

/**
 * Estimates the number of empties that can be WLD solved in a given amount of time. Rounded down.
 */
int CPU::est_eg_empties(double time) {
    double nodes = time * avg_nps() * 3;
    return (int) (0.885 * log(381.7 * nodes));
}

double CPU::avg_nps() {
    if (total_time == 0) return 1e6; // arbitrary estimate for when there's no data
    return (double)total_nodes / total_time;
}