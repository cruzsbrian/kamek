#include "book.h"

#include <iostream>

#include "common.h"


int main(int argc, char *argv[]) {
    if (argc != 5) {
        cerr << "usage: gen_book color filename time_limit(m) depth" << endl;
        exit(1);
    }

    string color = argv[1];
    string filename = argv[2];
    double time_limit = stod(argv[3]) * 60;
    int depth = stoi(argv[4]);

    board::Board start_pos = board::starting_position();

    queue<board::Board> pos_queue;

    if (color == "b" || color == "black") {
        pos_queue.push(start_pos);
    } else if (color == "w" || color == "white") {
        pos_queue.push(board::do_move(start_pos, notation_to_move("d3")));
        pos_queue.push(board::do_move(start_pos, notation_to_move("c4")));
        pos_queue.push(board::do_move(start_pos, notation_to_move("f5")));
        pos_queue.push(board::do_move(start_pos, notation_to_move("e6")));
    } else {
        cerr << "Unrecognized color " << color << endl;
        exit(1);
    }

    book::build_book(pos_queue, time_limit, depth);

    cerr << "Writing book to " << filename << endl;
    book::write_book(filename);
}