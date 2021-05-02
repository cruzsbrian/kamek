#include "game/game.h"
#include "game/human_player.h"
#include "game/cpu.h"


int main() {
    HumanPlayer human;
    CPU cpu{7,22};

    Game game{&cpu, &human};
    game.play(true);

    return 0;
}
