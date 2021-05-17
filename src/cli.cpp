#include "game/game.h"
#include "game/human_player.h"
#include "game/cpu.h"


int main() {
    HumanPlayer human;
    CPU cpu{12,22};

    Game game{&human, &cpu};
    game.play(true);

    return 0;
}
