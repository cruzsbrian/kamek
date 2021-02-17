#include "board/board.h"
#include "eval/simple.h"
#include "search/negamax.h"
#include "util.h"

#include <iostream>
#include <string>


#define GAME_OVER(move) \
        if (cpu_move == -1) { \
            if (prev_pass) break; \
            else prev_pass = true; \
        } else { \
            prev_pass = false; \
        }


using namespace std;


int main() {
    board::init_hash();

    board::Board b;

    b = board::add_piece(b, 27, WHITE);
    b = board::add_piece(b, 28, BLACK);
    b = board::add_piece(b, 35, BLACK);
    b = board::add_piece(b, 36, WHITE);

    NegamaxSearch s{ SimpleEval{}, 7 };

    bool prev_pass = false;
    while (true) {
        cout << board::to_str(b) << "\n";

        int cpu_move = s.next_move(b, BLACK, 0);
        cout << "\nCPU move: " << move_to_notation(cpu_move) << "\n";

        GAME_OVER(cpu_move);

        b = board::do_move(b, cpu_move, BLACK);


        cout << board::to_str(b) << "\n";

        uint64_t white_moves = board::get_moves(b, WHITE);
        /* cout << "Moves for white:\n" << board::to_str(white_moves); */

        int white_move;
        if (white_moves == 0ULL) {
            cout << "\nWhite must pass\n";
            white_move = -1;
        } else {
            string input;
            cout << "\nEnter move: ";
            cin >> input;
            white_move = notation_to_move(input);
        }

        GAME_OVER(white_move);

        b = board::do_move(b, white_move, WHITE);
    }

    cout << "Game over\n";

    return 0;
}
