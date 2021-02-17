#include "simple.h"


const uint64_t m_corner   = 0x8100000000000081;
const uint64_t m_c_square = 0x4281000000004281;
const uint64_t m_edge     = 0x3c7e818181817e3c;
const uint64_t m_other    = 0x003c7e7e7e7e3c00;


int SimpleEval::score(board::Board b, bool c) const {
    int mobility = board::popcount(board::get_moves(b, c)) - board::popcount(board::get_moves(b, !c));
    int frontier = board::get_frontier(b, c) - board::get_frontier(b, !c);
    int stable = board::get_stable(b, c) - board::get_stable(b, !c);

    uint64_t own, opp;
    if (c == BLACK) {
        own = b.b;
        opp = b.w;
    } else {
        own = b.w;
        opp = b.b;
    }

    int pieces = board::popcount(own) - board::popcount(opp);
    int edges = board::popcount(own & m_edge) - board::popcount(opp & m_edge);
    int corners = board::popcount(own & m_corner) - board::popcount(opp & m_corner);

    return (mobility * params.w_mobility)
        + (frontier  * params.w_frontier)
        + (stable    * params.w_stable)
        + (edges     * params.w_edge)
        + (corners   * params.w_corner)
        + (pieces    * params.w_pieces);
}
