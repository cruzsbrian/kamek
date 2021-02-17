#include "board/board.h"

#include <iostream>

int main() {
    board::init_hash();

    board::Board b;

    b = board::add_piece(b, 27, Color::WHITE);
    b = board::add_piece(b, 28, Color::BLACK);
    b = board::add_piece(b, 35, Color::BLACK);
    b = board::add_piece(b, 36, Color::WHITE);

    cout << "Board:\n" << board::to_str(b) << "\n";

    uint64_t black_moves = board::get_moves(b, Color::BLACK);
    cout << "Moves for black:\n" << board::to_str(black_moves) << "\n";

    b = board::do_move(b, 023, Color::BLACK);

    cout << "Board:\n" << board::to_str(b) << "\n";

    uint64_t white_moves = board::get_moves(b, Color::WHITE);
    cout << "Moves for white:\n" << board::to_str(white_moves) << "\n";

    return 0;
}
