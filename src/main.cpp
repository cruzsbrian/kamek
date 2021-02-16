#include "board/board.h"

#include <iostream>

int main() {
    Board::init_hash();

    Board b;

    b.add_piece(27, Color::WHITE);
    b.add_piece(28, Color::BLACK);
    b.add_piece(35, Color::BLACK);
    b.add_piece(36, Color::WHITE);

    cout << "Board:\n" << b.to_str() << "\n";

    uint64_t black_moves = b.get_moves(Color::BLACK);
    cout << "Moves for black:\n" << mask_to_str(black_moves) << "\n";

    b.do_move(023, Color::BLACK);

    cout << "Board:\n" << b.to_str() << "\n";

    uint64_t white_moves = b.get_moves(Color::WHITE);
    cout << "Moves for white:\n" << mask_to_str(white_moves) << "\n";

    return 0;
}
