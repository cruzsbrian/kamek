#include "board/board.h"
#include "game/game.h"
#include "game/cpu.h"
#include "util.h"

#include <iostream>
#include <string.h>
#include <cctype>


bool read_cs2_move(int *move, int *ms_left) {
    int row, col;

    cin >> row >> col >> *ms_left;

    if (cin.eof()) {
        return false;
    }

    if (row == -1 && col == -1) *move = -1;
    else *move = row * 8 + col;

    return true;
}

void print_cs2_move(int move) {
    int row, col;

    if (move == -1) {
        cout << "pass" << "\n";
        return;
    }

    row = move / 8;
    col = move % 8;

    cout << row << " " << col << "\n";
}


int main(int argc, char *argv[]) {
    if (argc != 2) {
        cout << "Missing bot color arg\n";
        exit(1);
    }

    bool bot_color;
    if (strcmp(argv[1], "black") == 0) {
        bot_color = BLACK;
    } else {
        bot_color = WHITE;
    }

    board::init_hash();

    board::Board b;
    b = board::add_piece(b, 27, WHITE);
    b = board::add_piece(b, 28, BLACK);
    b = board::add_piece(b, 35, BLACK);
    b = board::add_piece(b, 36, WHITE);

    CPU cpu{11, 22};

    cout << "Init done.\n";

    int ms_left;
    int opp_move;

    // The first move sent to us is nothing if we're black
    read_cs2_move(&opp_move, &ms_left);

    if (bot_color != BLACK) {
        b = board::do_move(b, opp_move);
    }

    while (true) {
        cerr << board::to_grid(b, bot_color) << "\n";

        int bot_move = cpu.next_move(b, ms_left);

        print_cs2_move(bot_move);

        b = board::do_move(b, bot_move);

        if (!read_cs2_move(&opp_move, &ms_left)) break;
        b = board::do_move(b, opp_move);
    }

    return 0;
}