#include "alphabeta.h"

#include <climits>
#include <iostream>
#include <algorithm>

#include "../eval/pattern_eval.h"


const int DEEP_CUTOFF = 8;
const int MED_CUTOFF = 5;

const float SIGMA = 200.;
const float T = 0.39;


SearchNode ab_deep(board::Board b, int alpha, int beta, int depth, HashTable &ht, bool passed, long *n) {
    (*n)++;

    if (depth == 0) {
        int score = eval::score(b);
        if (score > beta) return {depth, NodeType::HIGH, beta, -1};
        if (score < alpha) return {depth, NodeType::LOW, alpha, -1};
        return {depth, NodeType::PV, score, -1};
    }

    int sort_depth = max(0, depth - DEEP_CUTOFF);
    vector<ScoredMove> moves = get_sorted_moves(b, alpha, beta, sort_depth, ht);

    if (moves.size() == 0) {
        if (passed) { // Game is over: solved node
            int score = board::popcount(b.own) - board::popcount(b.opp);
            ht.set(b, {depth, NodeType::PV, score, -1});
            return {depth, NodeType::PV, score, -1};
        } else {
            int score = -ab_deep(board::Board{b.opp, b.own}, -beta, -alpha, depth, ht, true, n).score;
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
    int best_move = -1;
    int best_score = alpha;
    for (auto m : moves) {
        // Get score
        int score;

        if (depth <= DEEP_CUTOFF) {
            score = -ab_medium(m.after, -beta, -best_score, depth - 1, ht, false, n);
        } else {
            score = -ab_deep(m.after, -beta, -best_score, depth - 1, ht, false, n).score;
        }

        if (score >= beta) {
            ht.set(b, {depth, NodeType::HIGH, beta, -1});
            return {depth, NodeType::HIGH, beta, -1};
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

    if (depth == 0) {
        return eval::score(b);
    }

    int sort_depth = max(0, depth - MED_CUTOFF);
    vector<ScoredMove> moves = get_sorted_moves(b, alpha, beta, sort_depth, ht);

    if (moves.size() == 0) {
        if (passed) return (board::popcount(b.own) - board::popcount(b.opp)) * 50;
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

    if (depth == 0) {
        return eval::score(b);
    }

    uint64_t move_mask = board::get_moves(b);

    if (move_mask == 0ULL) {
        if (passed) return (board::popcount(b.own) - board::popcount(b.opp)) * 50;
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


vector<ScoredMove> get_sorted_moves(board::Board b, int alpha, int beta, int depth, HashTable &ht) {
    vector<ScoredMove> ret;

    uint64_t move_mask = board::get_moves(b);

    long nodes;
    while (move_mask != 0ULL) {
        int m = __builtin_ctzll(move_mask);
        move_mask &= move_mask - 1;

        board::Board after = board::do_move(b, m);

        int score;
        SearchNode *table_entry = ht.get(after);
        if (table_entry) {
            score = table_entry->score;
        } else {
            score = ab(after, -beta, -alpha, depth, false, &nodes);
        }

        ret.push_back(ScoredMove{m, score, after});
    }

    std::sort(ret.begin(), ret.end());
    return ret;
}
