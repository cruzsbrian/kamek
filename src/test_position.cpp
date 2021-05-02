#include <iostream>
#include <string>
#include <climits>

#include "board/board.h"
#include "eval/simple_eval.h"
#include "search/basic.h"
#include "game/cpu.h"



int main(int argc, char *argv[]) {
    if (argc != 2) {
        cerr << "usage: test_position POSITION_STR\n";
        exit(1);
    }

    /* cout << "Eval masks:\n"; */
    /* cout << "m_corner\n" << board::to_str(eval::m_corner); */
    /* cout << "m_edge_a\n" << board::to_str(eval::m_edge_a); */
    /* cout << "m_edge_b\n" << board::to_str(eval::m_edge_b); */

    /* cout << "m_corner_ul\n" << board::to_str(eval::m_corner_ul); */
    /* cout << "m_corner_ur\n" << board::to_str(eval::m_corner_ur); */
    /* cout << "m_corner_ll\n" << board::to_str(eval::m_corner_ll); */
    /* cout << "m_corner_lr\n" << board::to_str(eval::m_corner_lr); */

    /* cout << "m_x_sq_ul\n" << board::to_str(eval::m_x_sq_ul); */
    /* cout << "m_x_sq_ur\n" << board::to_str(eval::m_x_sq_ur); */
    /* cout << "m_x_sq_ll\n" << board::to_str(eval::m_x_sq_ll); */
    /* cout << "m_x_sq_lr\n" << board::to_str(eval::m_x_sq_lr); */

    /* cout << "m_c_sq_ul\n" << board::to_str(eval::m_c_sq_ul); */
    /* cout << "m_c_sq_ur\n" << board::to_str(eval::m_c_sq_ur); */
    /* cout << "m_c_sq_ll\n" << board::to_str(eval::m_c_sq_ll); */
    /* cout << "m_c_sq_lr\n" << board::to_str(eval::m_c_sq_lr); */

    board::Board b = board::from_str(argv[1]);

    cout << board::to_str(b);

    for (unsigned i = 4; i <= 10; i++) {
        CPU cpu{i};
        long nodes;

        cout << "depth " << i << ": " << ab_ff(b, -INT_MAX, INT_MAX, i, false, &nodes) << "\n";
    }

    cout << eval::corners(b) << " "
        << eval::edge_a(b) << " "
        << eval::edge_b(b) << " "
        << eval::c_sq_corner_empty(b) << " "
        << eval::c_sq_corner_same(b) << " "
        << eval::c_sq_corner_opp(b) << " "
        << eval::x_sq_corner_empty(b) << " "
        << eval::x_sq_corner_same(b) << " "
        << eval::x_sq_corner_opp(b) << " "
        << eval::mobility(b) << " "
        << eval::frontier(b) << "\n";

    b = board::Board{b.opp, b.own};

    cout << eval::corners(b) << " "
        << eval::edge_a(b) << " "
        << eval::edge_b(b) << " "
        << eval::c_sq_corner_empty(b) << " "
        << eval::c_sq_corner_same(b) << " "
        << eval::c_sq_corner_opp(b) << " "
        << eval::x_sq_corner_empty(b) << " "
        << eval::x_sq_corner_same(b) << " "
        << eval::x_sq_corner_opp(b) << " "
        << eval::mobility(b) << " "
        << eval::frontier(b) << "\n";

    return 0;
}
