#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "../eval/pattern_eval.h"
#include "../board/board.h"


int main() {
    board::Board b = board::starting_position();

    cout << board::to_grid(b, BLACK);

    return 0;
}
