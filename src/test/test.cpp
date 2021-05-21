#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>

#include "../board/board.h"
#include "../search/hashtable.h"


int main() {
    board::Board b = board::from_str("---------------------------XO------OOO--------------------------");

    HashTable ht;

    cout << board::to_str(b) << "\n";

    size_t h = ht.hash(b);
    cout << "\n" << h << " hash\n";

    return 0;
}
