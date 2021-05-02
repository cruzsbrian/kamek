#include "simple_eval.h"


namespace eval {


int score(board::Board b) {
    int ret = 0;

    int empties = board::popcount(~(b.own | b.opp));
    if (empties == 0) {
        return (board::popcount(b.own) - board::popcount(b.opp)) * 50;
    }

    int game_phase = (empties - 1) / 6;

    ret += corners(b)           * weights[game_phase][W_CORNER];
    ret += edge_a(b)            * weights[game_phase][W_EDGE_A];
    ret += edge_b(b)            * weights[game_phase][W_EDGE_B];
    ret += c_sq_corner_empty(b) * weights[game_phase][W_C_SQ_CORNER_EMPTY];
    ret += c_sq_corner_same(b)  * weights[game_phase][W_C_SQ_CORNER_SAME];
    ret += c_sq_corner_opp(b)   * weights[game_phase][W_C_SQ_CORNER_OPP];
    ret += x_sq_corner_empty(b) * weights[game_phase][W_X_SQ_CORNER_EMPTY];
    ret += x_sq_corner_same(b)  * weights[game_phase][W_X_SQ_CORNER_SAME];
    ret += x_sq_corner_opp(b)   * weights[game_phase][W_X_SQ_CORNER_OPP];
    ret += mobility(b)          * weights[game_phase][W_MOBILITY];
    ret += frontier(b)          * weights[game_phase][W_FRONTIER];
    ret += weights[game_phase][W_CONST];

    return ret;
}


int corners(board::Board b) {
    return board::popcount(m_corner & b.own) - board::popcount(m_corner & b.opp);
}

int edge_a(board::Board b) {
    return board::popcount(m_edge_a & b.own) - board::popcount(m_edge_a & b.opp);
}

int edge_b(board::Board b) {
    return board::popcount(m_edge_b & b.own) - board::popcount(m_edge_b & b.opp);
}

int c_sq_corner_empty(board::Board b) {
    int ret = 0;

    uint64_t empty = ~(b.own | b.opp);
    if (m_corner_ul & empty) ret += board::popcount(m_c_sq_ul & b.own) - board::popcount(m_c_sq_ul & b.opp);
    if (m_corner_ur & empty) ret += board::popcount(m_c_sq_ur & b.own) - board::popcount(m_c_sq_ur & b.opp);
    if (m_corner_ll & empty) ret += board::popcount(m_c_sq_ll & b.own) - board::popcount(m_c_sq_ll & b.opp);
    if (m_corner_lr & empty) ret += board::popcount(m_c_sq_lr & b.own) - board::popcount(m_c_sq_lr & b.opp);

    return ret;
}

int c_sq_corner_same(board::Board b) {
    int ret = 0;

    if (m_corner_ul & b.own) ret += board::popcount(m_c_sq_ul & b.own);
    if (m_corner_ur & b.own) ret += board::popcount(m_c_sq_ur & b.own);
    if (m_corner_ll & b.own) ret += board::popcount(m_c_sq_ll & b.own);
    if (m_corner_lr & b.own) ret += board::popcount(m_c_sq_lr & b.own);

    if (m_corner_ul & b.opp) ret -= board::popcount(m_c_sq_ul & b.opp);
    if (m_corner_ur & b.opp) ret -= board::popcount(m_c_sq_ur & b.opp);
    if (m_corner_ll & b.opp) ret -= board::popcount(m_c_sq_ll & b.opp);
    if (m_corner_lr & b.opp) ret -= board::popcount(m_c_sq_lr & b.opp);

    return ret;
}

int c_sq_corner_opp(board::Board b) {
    int ret = 0;

    if (m_corner_ul & b.opp) ret += board::popcount(m_c_sq_ul & b.own);
    if (m_corner_ur & b.opp) ret += board::popcount(m_c_sq_ur & b.own);
    if (m_corner_ll & b.opp) ret += board::popcount(m_c_sq_ll & b.own);
    if (m_corner_lr & b.opp) ret += board::popcount(m_c_sq_lr & b.own);

    if (m_corner_ul & b.own) ret -= board::popcount(m_c_sq_ul & b.opp);
    if (m_corner_ur & b.own) ret -= board::popcount(m_c_sq_ur & b.opp);
    if (m_corner_ll & b.own) ret -= board::popcount(m_c_sq_ll & b.opp);
    if (m_corner_lr & b.own) ret -= board::popcount(m_c_sq_lr & b.opp);

    return ret;
}

int x_sq_corner_empty(board::Board b) {
    int ret = 0;

    uint64_t empty = ~(b.own | b.opp);

    if (m_corner_ul & empty) {
        if (m_x_sq_ul & b.own) ret++;
        if (m_x_sq_ul & b.opp) ret--;
    }

    if (m_corner_ur & empty) {
        if (m_x_sq_ur & b.own) ret++;
        if (m_x_sq_ur & b.opp) ret--;
    }

    if (m_corner_ll & empty) {
        if (m_x_sq_ll & b.own) ret++;
        if (m_x_sq_ll & b.opp) ret--;
    }

    if (m_corner_lr & empty) {
        if (m_x_sq_lr & b.own) ret++;
        if (m_x_sq_lr & b.opp) ret--;
    }

    return ret;
}

int x_sq_corner_same(board::Board b) {
    int ret = 0;

    if ((m_corner_ul & b.own) && (m_x_sq_ur & b.own)) ret++;
    if ((m_corner_ur & b.own) && (m_x_sq_ur & b.own)) ret++;
    if ((m_corner_ll & b.own) && (m_x_sq_ur & b.own)) ret++;
    if ((m_corner_lr & b.own) && (m_x_sq_ur & b.own)) ret++;

    if ((m_corner_ul & b.opp) && (m_x_sq_ur & b.opp)) ret--;
    if ((m_corner_ur & b.opp) && (m_x_sq_ur & b.opp)) ret--;
    if ((m_corner_ll & b.opp) && (m_x_sq_ur & b.opp)) ret--;
    if ((m_corner_lr & b.opp) && (m_x_sq_ur & b.opp)) ret--;

    return ret;
}

int x_sq_corner_opp(board::Board b) {
    int ret = 0;

    if ((m_corner_ul & b.opp) && (m_x_sq_ur & b.own)) ret++;
    if ((m_corner_ur & b.opp) && (m_x_sq_ur & b.own)) ret++;
    if ((m_corner_ll & b.opp) && (m_x_sq_ur & b.own)) ret++;
    if ((m_corner_lr & b.opp) && (m_x_sq_ur & b.own)) ret++;

    if ((m_corner_ul & b.own) && (m_x_sq_ur & b.opp)) ret--;
    if ((m_corner_ur & b.own) && (m_x_sq_ur & b.opp)) ret--;
    if ((m_corner_ll & b.own) && (m_x_sq_ur & b.opp)) ret--;
    if ((m_corner_lr & b.own) && (m_x_sq_ur & b.opp)) ret--;

    return ret;
}

int mobility(board::Board b) {
    return board::popcount(board::get_moves(b)) - board::popcount(board::get_moves(board::Board{b.opp, b.own}));
}

int frontier(board::Board b) {
    return board::popcount(board::get_frontier(b)) - board::popcount(board::get_frontier(board::Board{b.opp, b.own}));
}



}
