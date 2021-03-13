#include "simple.h"


#define W_MOBILITY 0
#define W_FRONTIER 1
#define W_STABLE   2
#define W_EDGE     3
#define W_CORNER   4
#define W_PIECES   5

const uint64_t m_corner   = 0x8100000000000081;
const uint64_t m_c_square = 0x4281000000004281;
const uint64_t m_edge     = 0x3c0081818181003c;
const uint64_t m_other    = 0x003c7e7e7e7e3c00;


int SimpleEval::score(board::Board b) const {
    int pieces = board::popcount(b.own & m_other) - board::popcount(b.opp & m_other);
    int edges = board::popcount(b.own & m_edge) - board::popcount(b.opp & m_edge);
    int corners = board::popcount(b.own & m_corner) - board::popcount(b.opp & m_corner);

    board::Board opp_b{b.opp, b.own};

    int mobility = board::popcount(board::get_moves(b)) - board::popcount(board::get_moves(opp_b));
    int frontier = board::get_frontier(b) - board::get_frontier(opp_b);
    int stable = board::get_stable(b) - corners;

    int score = (mobility * params.p[W_MOBILITY])
                + (frontier * params.p[W_FRONTIER])
                + (stable   * params.p[W_STABLE])
                + (edges    * params.p[W_EDGE])
                + (corners  * params.p[W_CORNER])
                + (pieces   * params.p[W_PIECES]);
    return score;
}
