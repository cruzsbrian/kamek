#include "alphabeta.h"

#include <climits>
#include <iostream>
#include <algorithm>

#include <fmt/core.h>

#include "pattern_eval.h"


const int DEEP_CUTOFF = 8;
const int MED_CUTOFF = 5;

const int SORT_DEPTH_REDUCTION = 5;

const int STATIC_EVAL_MARGIN_MEDIUM = 400;
const int STATIC_EVAL_MARGIN_SHALLOW = 300;
const int PROBCUT_MARGIN = 100;


SearchNode ab_deep(board::Board b, int alpha, int beta, int depth, bool passed, SearchInfo &si) {
    si.nodes++;

    // Check hashtable to avoid re-search.
    SearchNode *table_entry = si.ht->get(b);
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
    if (get_time_since(si.start) >= si.time_limit) {
        return {depth, NodeType::TIMEOUT, 0, MOVE_NULL};
    }

    // Probcut
    if (si.forward_prune) {
        int prob_depth = max(depth - 2, 0);

        if (alpha != -INT_MAX) {
            int bound_low = alpha - PROBCUT_MARGIN;
            SearchNode prob_low = ab_deep(b, bound_low, bound_low + 1, prob_depth, passed, si);
            if (prob_low.type == NodeType::LOW) return {depth, NodeType::LOW, prob_low.score, MOVE_NULL};
        }

        if (beta != INT_MAX) {
            int bound_high = beta + PROBCUT_MARGIN;
            SearchNode prob_high = ab_deep(b, bound_high - 1, bound_high, prob_depth, passed, si);
            if (prob_high.type == NodeType::HIGH) return {depth, NodeType::HIGH, prob_high.score, MOVE_NULL};
        }
    }

    int sort_depth = max(0, depth - SORT_DEPTH_REDUCTION);
    vector<ScoredMove> moves = get_sorted_moves(b, sort_depth, si);

    if (moves.size() == 0) {
        if (passed) { // Game is over: solved node
            int score = INT_MAX * sgn(board::popcount(b.own) - board::popcount(b.opp));
            si.ht->set(b, {depth, NodeType::PV, score, -1});
            return {depth, NodeType::PV, score, -1};
        } else {
            SearchNode result = 
                ab_deep(board::do_move(b, MOVE_PASS), -beta, -alpha, depth, true, si);
            if (result.type == NodeType::TIMEOUT) { // propagate timeouts back up
                return {depth, NodeType::TIMEOUT, 0, MOVE_NULL};
            }

            int score = -result.score;
            if (score > alpha) {
                si.ht->set(b, {depth, NodeType::PV, score, -1});
                return {depth, NodeType::PV, score, -1};
            } else {
                si.ht->set(b, {depth, NodeType::LOW, alpha, -1});
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
            score = -ab_medium(m.after, -beta, -best_score, depth - 1, false, si);
        } else {
            SearchNode result = ab_deep(m.after, -beta, -best_score, depth - 1, false, si);
            if (result.type == NodeType::TIMEOUT) { // propagate timeouts back up
                return {depth, NodeType::TIMEOUT, 0, MOVE_NULL};
            }

            score = -result.score;
        }

        if (score >= beta) {
            si.ht->set(b, {depth, NodeType::HIGH, score, m.move});
            return {depth, NodeType::HIGH, score, m.move};
        }
        if (score > best_score) {
            best_score = score;
            best_move = m.move;
        }
    }

    if (best_score > alpha) {
        si.ht->set(b, {depth, NodeType::PV, best_score, best_move});
        return {depth, NodeType::PV, best_score, best_move};
    } else {
        si.ht->set(b, {depth, NodeType::LOW, alpha, best_move});
        return {depth, NodeType::LOW, alpha, best_move};
    }
}


int ab_medium(board::Board b, int alpha, int beta, int depth, bool passed, SearchInfo &si) {
    si.nodes++;

    if (depth == 0) {
        return eval::score(b);
    }

    // Probcut
    if (si.forward_prune) {
        int prob_depth = max(0, depth - 2);

        if (alpha != -INT_MAX) {
            int bound_low = alpha - PROBCUT_MARGIN;
            int prob_low = ab(b, bound_low, bound_low + 1, prob_depth, passed, si);
            if (prob_low <= bound_low) return prob_low;
        }

        if (beta != INT_MAX) {
            int bound_high = beta + PROBCUT_MARGIN;
            int prob_high = ab(b, bound_high - 1, bound_high, prob_depth, passed, si);
            if (prob_high >= bound_high) return prob_high;
        }
    }

    int sort_depth = max(0, depth - SORT_DEPTH_REDUCTION);
    vector<ScoredMove> moves = get_sorted_moves(b, sort_depth, si);

    if (moves.size() == 0) {
        if (passed) return INT_MAX * sgn(board::popcount(b.own) - board::popcount(b.opp));
        return -ab_medium(board::do_move(b, MOVE_PASS), -beta, -alpha, depth, true, si);
    }

    for (auto m : moves) {
        int score;
        if (depth <= MED_CUTOFF) {
            score = -ab(m.after, -beta, -alpha, depth - 1, false, si);
        } else {
            score = -ab_medium(m.after, -beta, -alpha, depth - 1, false, si);
        }

        if (score >= beta) return score;
        if (score > alpha) alpha = score;
    }

    return alpha;
}


int ab(board::Board b, int alpha, int beta, int depth, bool passed, SearchInfo &si) {
    si.nodes++;

    if (depth == 0) {
        return eval::score(b);
    }

    // Static eval pruning
    if (si.forward_prune) {
        int static_score = eval::score(b);
        if (static_score - STATIC_EVAL_MARGIN_SHALLOW > beta) {
            return static_score;
        } else if (static_score + STATIC_EVAL_MARGIN_SHALLOW < alpha) {
            return static_score;
        }
    }

    uint64_t move_mask = board::get_moves(b);

    if (move_mask == 0ULL) {
        if (passed) return INT_MAX * sgn(board::popcount(b.own) - board::popcount(b.opp));
        return -ab(board::do_move(b, MOVE_PASS), -beta, -alpha, depth, true, si);
    }

    while (move_mask != 0ULL) {
        int m = __builtin_ctzll(move_mask);
        move_mask &= move_mask - 1;

        int score = -ab(board::do_move(b, m), -beta, -alpha, depth - 1, false, si);

        if (score >= beta) return score;
        if (score > alpha) alpha = score;
    }

    return alpha;
}


vector<ScoredMove> get_sorted_moves(board::Board b, int depth, SearchInfo &si) {
    vector<ScoredMove> ret;

    uint64_t move_mask = board::get_moves(b);

    while (move_mask != 0ULL) {
        int m = __builtin_ctzll(move_mask);
        move_mask &= move_mask - 1;

        board::Board after = board::do_move(b, m);

        // Check hashtable for stored score. Only take exact scores.
        // If none found, search to depth given to get score.
        int score;
        SearchNode *table_entry = si.ht->get(after);
        if (table_entry && table_entry->type == NodeType::PV) {
            score = table_entry->score;
        } else {
            if (depth >= DEEP_CUTOFF) {
                score = ab_deep(after, -INT_MAX, INT_MAX, depth, false, si).score;
            } else if (depth >= MED_CUTOFF) {
                score = ab_medium(after, -INT_MAX, INT_MAX, depth, false, si);
            } else {
                score = ab(after, -INT_MAX, INT_MAX, depth, false, si);
            }
        }

        ret.push_back(ScoredMove{m, score, after});
    }

    std::sort(ret.begin(), ret.end());
    return ret;
}
