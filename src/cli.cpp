#include <iostream>
#include <cstring>

#include "game/game.h"
#include "game/human_player.h"
#include "game/cpu.h"


#define BLACK false
#define WHITE true


int main(int argc, char *argv[]) {
    if (argc != 2) {
        cerr << "usage: " << argv[0] << " COLOR\n";
        exit(1);
    }

    bool bot_color;
    if (strcmp(argv[1], "black") == 0 || strcmp(argv[1], "b") == 0) {
        bot_color = BLACK;
    } else if (strcmp(argv[1], "white") == 0 || strcmp(argv[1], "w") == 0) {
        bot_color = WHITE;
    } else {
        cerr << "use [black, white, b, w] to specify color\n";
        exit(1);
    }

    HumanPlayer human;
    CPU cpu{13,22};

    if (bot_color == BLACK) {
        Game game{&cpu, &human};
        game.play(true);
    } else {
        Game game{&human, &cpu};
        game.play(true);
    }

    return 0;
}
