#include "board/board.h"
#include "game/game.h"
#include "util.h"

#include "eval/simple.h"
#include "search/negamax.h"

#include <iostream>
#include <string.h>
#include <cctype>


bool read_cs2_move(int *move, int *ms_left) {
    int x, y;

    cin >> x >> y >> *ms_left;

    if (cin.eof()) {
        return false;
    }

    if (x == -1 && y == -1) *move = -1;
    else *move = y * 8 + x;

    return true;
}

void print_cs2_move(int move) {
    int x, y;

    if (move == -1) {
        x = -1;
        y = -1;
    } else {
        x = move % 8;
        y = move / 8;
    }

    cout << x << " " << y << "\n";
}


int main(int argc, char *argv[]) {
    if (argc != 2) {
        cout << "Missing bot color arg\n";
        exit(1);
    }

    bool bot_color;
    if (strcmp(argv[1], "Black") == 0) {
        bot_color = BLACK;
    } else {
        bot_color = WHITE;
    }

    board::Board b;
    b = board::add_piece(b, 27, WHITE);
    b = board::add_piece(b, 28, BLACK);
    b = board::add_piece(b, 35, BLACK);
    b = board::add_piece(b, 36, WHITE);

    /* SimpleEval ev{{306, -124, 743, 100, 548, -158}}; */
    SimpleEval ev{{158, -81, 725, 145, 648}};
    NegamaxSearch cpu{&ev, 8};

    cout << "Init done.\n";

    int ms_left;
    int opp_move;
    while (read_cs2_move(&opp_move, &ms_left)) {
        b = board::do_move(b, opp_move, !bot_color);

        int bot_move = cpu.next_move(b, bot_color, ms_left);
        print_cs2_move(bot_move);

        b = board::do_move(b, bot_move, bot_color);
    }

    return 0;
}
