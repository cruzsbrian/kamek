#include "simple.h"


#define W_MOBILITY 0
#define W_FRONTIER 1
#define W_STABLE   2
#define W_EDGE     3
#define W_CORNER   4
#define W_PIECES   5

const uint64_t m_corner   = 0x8100000000000081;
const uint64_t m_c_square = 0x4281000000004281;
const uint64_t m_edge     = 0x3c7e818181817e3c;
const uint64_t m_other    = 0x003c7e7e7e7e3c00;


int SimpleEval::score(board::Board b, bool c) const {
    uint64_t own, opp;
    if (c == BLACK) {
        own = b.b;
        opp = b.w;
    } else {
        own = b.w;
        opp = b.b;
    }

    int pieces = board::popcount(own & m_other) - board::popcount(opp & m_other);
    int edges = board::popcount(own & m_edge) - board::popcount(opp & m_edge);
    int corners = board::popcount(own & m_corner) - board::popcount(opp & m_corner);

    int mobility = board::popcount(board::get_moves(b, c)) - board::popcount(board::get_moves(b, !c));
    int frontier = board::get_frontier(b, c) - board::get_frontier(b, !c);
    int stable = board::get_stable(b, c) - board::get_stable(b, !c) - corners;

    int score = (mobility * params.p[W_MOBILITY])
                + (frontier * params.p[W_FRONTIER])
                + (stable   * params.p[W_STABLE])
                + (edges    * params.p[W_EDGE])
                + (corners  * params.p[W_CORNER])
                + (pieces   * params.p[W_PIECES]);
    return score;
}
