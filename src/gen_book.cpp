#include "book.h"

#include <iostream>

#include "common.h"
#include "pattern_eval.h"


int main(int argc, char *argv[]) {
    if (argc != 4) {
        cerr << "usage: gen_book filename depth plies" << endl;
        exit(1);
    }

    string filename = argv[1];
    int depth = stoi(argv[2]);
    int plies = stoi(argv[3]);

    eval::load_weights("weights.txt");

    board::Board start_pos = board::starting_position();

    queue<board::Board> pos_queue;

    pos_queue.push(start_pos);

    pos_queue.push(board::do_move(start_pos, notation_to_move("d3")));
    pos_queue.push(board::do_move(start_pos, notation_to_move("c4")));
    pos_queue.push(board::do_move(start_pos, notation_to_move("f5")));
    pos_queue.push(board::do_move(start_pos, notation_to_move("e6")));

    book::build_book(filename, pos_queue, depth, plies);
}