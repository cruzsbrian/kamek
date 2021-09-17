#include "alphabeta.h"

#include <climits>
#include <iostream>
#include <algorithm>

#include <fmt/core.h>

#include "pattern_eval.h"


const int DEEP_CUTOFF = 8;
const int MED_CUTOFF = 5;

const int SORT_DEPTH_REDUCTION = 3;

const int STATIC_EVAL_MARGIN_MEDIUM = 300;
const int STATIC_EVAL_MARGIN_SHALLOW = 150;


SearchNode ab_deep(board::Board b, int alpha, int beta, int depth, HashTable &ht, bool passed, long *n, clock_t start, float time_limit) {
    (*n)++;

    // Check hashtable to avoid re-search.
    SearchNode *table_entry = ht.get(b);
    if (table_entry && table_entry->depth >= depth) {
        // If score is exact, return it.
        if (table_entry->type == NodeType::PV) return *table_entry;

        // If score is lower bound, check for beta cutoff.
        if (table_entry->type == NodeType::HIGH && table_entry->score >= beta)
            return {depth, NodeType::HIGH, table_entry->score, table_entry->best_move};

        // If score is upper bound, check for alpha cutoff.
        if (table_entry->type == NodeType::LOW && table_entry->score <= alpha)
            return {depth, NodeType::LOW, table_entry->score, table_entry->best_move};
    }

    if (depth == 0) {
        int score = eval::score(b);
        if (score > beta) return {depth, NodeType::HIGH, beta, -1};
        if (score < alpha) return {depth, NodeType::LOW, alpha, -1};
        return {depth, NodeType::PV, score, MOVE_NULL};
    }

    // Check for timeout.
    if (get_time_since(start) >= time_limit) {
        return {depth, NodeType::TIMEOUT, 0, MOVE_NULL};
    }

    int sort_depth = max(0, depth - SORT_DEPTH_REDUCTION);
    vector<ScoredMove> moves = get_sorted_moves(b, sort_depth, ht, n);

    if (moves.size() == 0) {
        if (passed) { // Game is over: solved node
            int score = INT_MAX * sgn(board::popcount(b.own) - board::popcount(b.opp));
            ht.set(b, {depth, NodeType::PV, score, -1});
            return {depth, NodeType::PV, score, -1};
        } else {
            SearchNode result = ab_deep(board::Board{b.opp, b.own}, -beta, -alpha, depth, ht, true, n, start, time_limit);
            if (result.type == NodeType::TIMEOUT) { // propagate timeouts back up
                return {depth, NodeType::TIMEOUT, 0, MOVE_NULL};
            }

            int score = -result.score;
            if (score > alpha) {
                ht.set(b, {depth, NodeType::PV, score, -1});
                return {depth, NodeType::PV, score, -1};
            } else {
                ht.set(b, {depth, NodeType::LOW, alpha, -1});
                return {depth, NodeType::LOW, alpha, -1};
            }
        }
    }

    // Search moves with alphabeta algorithm
    int best_move = MOVE_NULL;
    int best_score = alpha;
    for (auto m : moves) {
        // Get score
        int score;

        if (depth <= DEEP_CUTOFF) {
            score = -ab_medium(m.after, -beta, -best_score, depth - 1, ht, false, n);
        } else {
            SearchNode result = ab_deep(m.after, -beta, -best_score, depth - 1, ht, false, n, start, time_limit);
            if (result.type == NodeType::TIMEOUT) { // propagate timeouts back up
                return {depth, NodeType::TIMEOUT, 0, MOVE_NULL};
            }

            score = -result.score;
        }

        if (score >= beta) {
            ht.set(b, {depth, NodeType::HIGH, beta, m.move});
            return {depth, NodeType::HIGH, beta, m.move};
        }
        if (score > best_score) {
            best_score = score;
            best_move = m.move;
        }
    }

    if (best_score > alpha) {
        ht.set(b, {depth, NodeType::PV, best_score, best_move});
        return {depth, NodeType::PV, best_score, best_move};
    } else {
        ht.set(b, {depth, NodeType::LOW, alpha, best_move});
        return {depth, NodeType::LOW, alpha, best_move};
    }
}


int ab_medium(board::Board b, int alpha, int beta, int depth, HashTable &ht, bool passed, long *n) {
    (*n)++;

    // Static eval pruning
    int static_score = eval::score(b);
    if (static_score >= beta + STATIC_EVAL_MARGIN_MEDIUM) {
        return static_score;
    } else if (static_score <= alpha - STATIC_EVAL_MARGIN_MEDIUM) {
        return static_score;
    }

    if (depth == 0) {
        return eval::score(b);
    }

    int sort_depth = max(0, depth - SORT_DEPTH_REDUCTION);
    vector<ScoredMove> moves = get_sorted_moves(b, sort_depth, ht, n);

    if (moves.size() == 0) {
        if (passed) return INT_MAX * sgn(board::popcount(b.own) - board::popcount(b.opp));
        return -ab_medium(board::Board{b.opp, b.own}, -beta, -alpha, depth, ht, true, n);
    }

    for (auto m : moves) {
        int score;
        if (depth <= MED_CUTOFF) {
            score = -ab(m.after, -beta, -alpha, depth - 1, false, n);
        } else {
            score = -ab_medium(m.after, -beta, -alpha, depth - 1, ht, false, n);
        }

        if (score >= beta) return beta;
        if (score > alpha) alpha = score;
    }

    return alpha;
}


int ab(board::Board b, int alpha, int beta, int depth, bool passed, long *n) {
    (*n)++;

    // Static eval pruning
    int static_score = eval::score(b);
    if (static_score > beta + STATIC_EVAL_MARGIN_SHALLOW) {
        return static_score;
    } else if (static_score < alpha - STATIC_EVAL_MARGIN_SHALLOW) {
        return static_score;
    }

    if (depth == 0) {
        return eval::score(b);
    }

    uint64_t move_mask = board::get_moves(b);

    if (move_mask == 0ULL) {
        if (passed) return INT_MAX * sgn(board::popcount(b.own) - board::popcount(b.opp));
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


vector<ScoredMove> get_sorted_moves(board::Board b, int depth, HashTable &ht, long *n) {
    vector<ScoredMove> ret;

    uint64_t move_mask = board::get_moves(b);

    long nodes = 0L;
    clock_t start = clock();
    while (move_mask != 0ULL) {
        int m = __builtin_ctzll(move_mask);
        move_mask &= move_mask - 1;

        board::Board after = board::do_move(b, m);

        // Check hashtable for stored score. Only take exact scores.
        // If none found, search to depth given to get score.
        int score;
        SearchNode *table_entry = ht.get(after);
        if (table_entry && table_entry->type == NodeType::PV) {
            score = table_entry->score;
        } else {
            if (depth >= DEEP_CUTOFF) {
                score = ab_deep(after, -INT_MAX, INT_MAX, depth, ht, false, &nodes, start, 10.0).score;
            } else if (depth >= MED_CUTOFF) {
                score = ab_medium(after, -INT_MAX, INT_MAX, depth, ht, false, &nodes);
            } else {
                score = ab(after, -INT_MAX, INT_MAX, depth, false, &nodes);
            }
        }

        ret.push_back(ScoredMove{m, score, after});
    }

    (*n) += nodes;

    std::sort(ret.begin(), ret.end());
    return ret;
}
