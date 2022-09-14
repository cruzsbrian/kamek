#include "book.h"

#include <iostream>
#include <fstream>
#include <queue>
#include <time.h>
#include <fmt/core.h>

#include "common.h"
#include "cpu.h"


namespace book {

vector<BookEntry> book;

void load_book(const string &filename) {
    ifstream book_file(filename);

    if (!book_file.is_open()) {
        cerr << "Could not open book at " << filename << endl;
        return;
    }

    string board_str, move_str;
    while (book_file >> board_str >> move_str) {
        board::Board pos = board::from_str(board_str);
        int best_move = notation_to_move(move_str);

        book.push_back({pos, best_move});
    }
}

int search(board::Board b) {
    for (auto entry : book) {
        if (entry.b == b) return entry.best_move;
    }

    return MOVE_NULL;
}

void build_book(const string &filename, queue<board::Board> &pos_queue, int depth, int plies) {
    CPU cpu{depth, 600, 0, false};   // use given depth, up to 10 minutes
    CPU test_cpu{10, 1, 0, false};  // depth 10, up to 1 second
    load_book(filename);

    queue<board::Board> next_queue;

    int plies_done = 0;
    while (plies_done < plies) {
        cerr << "Starting ply " << plies_done + 1 << endl;

        while (pos_queue.size() != 0) {
            board::Board curr_pos = pos_queue.front();
            pos_queue.pop();

            // Find best move for this position, add to book.
            cerr << board::to_str(curr_pos);
            SearchResult sr = cpu.next_move(curr_pos, -1);

            // Add to book if it wasn't found in the book.
            if (sr.nodes != 0) book.push_back({curr_pos, sr.node.best_move});
            fmt::print(stderr, " {} {:.3f}s\n", move_to_notation(sr.node.best_move), sr.time_spent);

            // Add all opponent's moves to queue.
            board::Board next_pos = board::do_move(curr_pos, sr.node.best_move);
            uint64_t move_mask = board::get_moves(next_pos);
            while (move_mask != 0ULL) {
                int m = __builtin_ctzll(move_mask);
                move_mask &= move_mask - 1;

                // See if a reasonable opponent would make this move
                SearchResult test_search = test_cpu.next_move(board::do_move(next_pos, m), -1);
                if (win_prob(test_search.node.score) < 0.55) next_queue.push(board::do_move(next_pos, m));
            }
        }

        cerr << "Writing to " << filename << endl;
        write_book(filename);

        plies_done++;

        // Copy positions from next_queue into current queue
        while (next_queue.size() != 0) {
            pos_queue.push(next_queue.front());
            next_queue.pop();
        }
    }
}

void write_book(const string &filename) {
    ofstream book_file(filename);

    if (!book_file.is_open()) {
        cerr << "Could not open book at " << filename << endl;
        exit(1);
    }

    for (auto entry : book) {
        try {
            book_file << board::to_str(entry.b) << " "
                      << move_to_notation(entry.best_move) << endl;
        } catch (...) {
            cerr << "Error writing to book" << endl;
            break;
        }
    }

    book_file.close();
}

} // namespace book
