#include "board.h"

#include <cstdlib>


using namespace std;


namespace board {


uint16_t hash_vals[2][64];

void init_hash() {
    srand(1337);

    for (auto i = 0; i < 64; ++i) {
        hash_vals[BLACK][i] = rand();
        hash_vals[WHITE][i] = rand();
    }
}




/* ====== MOVE GENERATION AND MAKE MOVE ====== */

/* Masks to filter out end files. */
const uint64_t notAFile = 0xfefefefefefefefe;
const uint64_t notHFile = 0x7f7f7f7f7f7f7f7f;

/*
 * Fill algorithms:
 * Smears the gen bitmap in the specified direction as long as there is
 * something in the pro bitmap. For east, west, and diagonals, we exclude the
 * first file in that shift direction to avoid wrap-around.
 *
 * Shift algorithms:
 * Shifts the pieces one step in the specified direction, making sure pieces
 * don't wrap around.
 *
 * The result takes each piece in gen, and draws a ray from it in the specified
 * direction as far as pieces in pro go in that direction.
 */
uint64_t soutOccl(uint64_t gen, uint64_t pro) {
    gen |= pro & (gen >> 8);
    pro &=       (pro >> 8);
    gen |= pro & (gen >> 16);
    pro &=       (pro >> 16);
    gen |= pro & (gen >> 32);
    return gen;
}

uint64_t soutOne(uint64_t gen) {
    return gen >> 8;
}

uint64_t nortOccl(uint64_t gen, uint64_t pro) {
    gen |= pro & (gen << 8);
    pro &=       (pro << 8);
    gen |= pro & (gen << 16);
    pro &=       (pro << 16);
    gen |= pro & (gen << 32);
    return gen;
}

uint64_t nortOne(uint64_t gen) {
    return gen << 8;
}

uint64_t eastOccl(uint64_t gen, uint64_t pro) {
    pro &= notAFile;
    gen |= pro & (gen << 1);
    pro &=       (pro << 1);
    gen |= pro & (gen << 2);
    pro &=       (pro << 2);
    gen |= pro & (gen << 4);
    return gen;
}

uint64_t eastOne(uint64_t gen) {
    return (gen & notAFile) << 1;
}

uint64_t westOccl(uint64_t gen, uint64_t pro) {
    pro &= notHFile;
    gen |= pro & (gen >> 1);
    pro &=       (pro >> 1);
    gen |= pro & (gen >> 2);
    pro &=       (pro >> 2);
    gen |= pro & (gen >> 4);
    return gen;
}

uint64_t westOne(uint64_t gen) {
    return (gen & notHFile) >> 1;
}

uint64_t noEaOccl(uint64_t gen, uint64_t pro) {
    pro &= notAFile;
    gen |= pro & (gen << 9);
    pro &=       (pro << 9);
    gen |= pro & (gen << 18);
    pro &=       (pro << 18);
    gen |= pro & (gen << 36);
    return gen;
}

uint64_t noEaOne(uint64_t gen) {
    return (gen & notAFile) << 9;
}

uint64_t soEaOccl(uint64_t gen, uint64_t pro) {
    pro &= notAFile;
    gen |= pro & (gen >> 7);
    pro &=       (pro >> 7);
    gen |= pro & (gen >> 14);
    pro &=       (pro >> 14);
    gen |= pro & (gen >> 28);
    return gen;
}

uint64_t soEaOne(uint64_t gen) {
    return (gen & notAFile) >> 7;
}

uint64_t noWeOccl(uint64_t gen, uint64_t pro) {
    pro &= notHFile;
    gen |= pro & (gen << 7);
    pro &=       (pro << 7);
    gen |= pro & (gen << 14);
    pro &=       (pro << 14);
    gen |= pro & (gen << 28);
    return gen;
}

uint64_t noWeOne(uint64_t gen) {
    return (gen & notHFile) << 7;
}

uint64_t soWeOccl(uint64_t gen, uint64_t pro) {
    pro &= notHFile;
    gen |= pro & (gen >> 9);
    pro &=       (pro >> 9);
    gen |= pro & (gen >> 18);
    pro &=       (pro >> 18);
    gen |= pro & (gen >> 36);
    return gen;
}

uint64_t soWeOne(uint64_t gen) {
    return (gen & notHFile) >> 9;
}


/*
 * Move generation:
 * Use own pieces as generator, opponents pieces as propogator. After rays have
 * been calculated, they must be &-ed with pro to exclude the generator piece.
 * Then shift one more step in the ray direction (making sure not to wrap
 * around), and & with empty squares to get playable squares.
 *
 * Returns a long representing squares that can be played in.
 */
uint64_t get_moves(Board b, bool c) {
    uint64_t gen, pro, empty, tmp, moves;

    if (c == BLACK) {
        gen = b.b;
        pro = b.w;
    } else {
        gen = b.w;
        pro = b.b;
    }

    moves = 0L;
    empty = ~(gen | pro);

    tmp = soutOccl(gen, pro) & pro;
    moves |= (tmp >> 8) & empty;

    tmp = nortOccl(gen, pro) & pro;
    moves |= (tmp << 8) & empty;

    tmp = eastOccl(gen, pro) & pro;
    moves |= (tmp << 1) & notAFile & empty;

    tmp = westOccl(gen, pro) & pro;
    moves |= (tmp >> 1) & notHFile & empty;

    tmp = noEaOccl(gen, pro) & pro;
    moves |= (tmp << 9) & notAFile & empty;

    tmp = soEaOccl(gen, pro) & pro;
    moves |= (tmp >> 7) & notAFile & empty;

    tmp = noWeOccl(gen, pro) & pro;
    moves |= (tmp << 7) & notHFile & empty;

    tmp = soWeOccl(gen, pro) & pro;
    moves |= (tmp >> 9) & notHFile & empty;

    return moves;
}


/**
 * Get frontier pieces:
 * Gives the number of pieces of color c that are adjacent to empty squares.
 * Takes the pieces of color c, and shifts in every direction, &-ing with empty
 * pieces.
 */
int get_frontier(Board b, bool c) {
    uint64_t own, opp, empty, frontier;

    if (c == BLACK) {
        own = b.b;
        opp = b.w;
    } else {
        own = b.w;
        opp = b.b;
    }

    empty = ~(own | opp);
    frontier = 0L;

    frontier |= (own >> 8) & empty;
    frontier |= (own << 8) & empty;
    frontier |= (own << 1) & notAFile & empty;
    frontier |= (own >> 1) & notHFile & empty;
    frontier |= (own << 9) & notAFile & empty;
    frontier |= (own >> 7) & notAFile & empty;
    frontier |= (own << 7) & notHFile & empty;
    frontier |= (own >> 9) & notHFile & empty;

    return popcount(frontier);
}


int get_stable(Board b, bool c) {
    const uint64_t top    = 0xff00000000000000;
    const uint64_t bottom = 0x00000000000000ff;
    const uint64_t left   = 0x0101010101010101;
    const uint64_t right  = 0x8080808080808080;

    uint64_t own, opp, pcs, vert, horiz, diag1, diag2, stable_own, stable_opp;

    if (c == BLACK) {
        own = b.b;
        opp = b.w;
    } else {
        own = b.w;
        opp = b.b;
    }

    pcs = b.b | b.w;

    vert  = nortOccl(bottom & pcs, pcs) & soutOccl(top & pcs, pcs);
    horiz = eastOccl(left & pcs, pcs) & westOccl(right & pcs, pcs);
    diag1 = noEaOccl((bottom | left) & pcs, pcs) & soWeOccl((top | right) & pcs, pcs);
    diag2 = noWeOccl((bottom | right) & pcs, pcs) & soEaOccl((top | left) & pcs, pcs);

    stable_own = (0x8100000000000081 | (vert & horiz & diag1 & diag2)) & own;
    stable_opp = (0x8100000000000081 | (vert & horiz & diag1 & diag2)) & opp;

    /* Expand the stable areas. */
    for (int ii = 0; ii < 8; ++ii) {
        stable_own |= own & (
            (nortOne(stable_own) | soutOne(stable_own) | vert) &
            (eastOne(stable_own) | westOne(stable_own) | horiz) &
            (noEaOne(stable_own) | soWeOne(stable_own) | diag1) &
            (noWeOne(stable_own) | soEaOne(stable_own) | diag2)
        );
        stable_opp |= opp & (
            (nortOne(stable_opp) | soutOne(stable_opp) | vert) &
            (eastOne(stable_opp) | westOne(stable_opp) | horiz) &
            (noEaOne(stable_opp) | soWeOne(stable_opp) | diag1) &
            (noWeOne(stable_opp) | soEaOne(stable_opp) | diag2)
        );
    }

    return popcount(stable_own) - popcount(stable_opp);
}


/*
 * Make-move: Makes a move for color c in position pos, and updates the board's
 * hash. Give -1 as pos for pass.  Gen is a one-hot long representing the added
 * piece. Filling from gen along opponent pieces and &-ing with rays in the
 * opposite direction from existing pieces gives only lines of opponent pieces
 * that have the new piece on one side and an existing own piece on the other
 * side.
 */
Board do_move(Board b, int pos, bool c) {
    uint64_t gen, own, pro, diff;

    /* -1 for pass. */
    if (pos == -1) {
        return b;
    }

    gen = 1L << pos;

    if (c == BLACK) {
        own = b.b;
        pro = b.w;

        b.b |= gen;
    } else {
        own = b.w;
        pro = b.b;

        b.w |= gen;
    }

    diff = 0L;
    diff |= soutOccl(gen, pro) & nortOccl(own, pro);
    diff |= nortOccl(gen, pro) & soutOccl(own, pro);
    diff |= eastOccl(gen, pro) & westOccl(own, pro);
    diff |= westOccl(gen, pro) & eastOccl(own, pro);
    diff |= noEaOccl(gen, pro) & soWeOccl(own, pro);
    diff |= soEaOccl(gen, pro) & noWeOccl(own, pro);
    diff |= noWeOccl(gen, pro) & soEaOccl(own, pro);
    diff |= soWeOccl(gen, pro) & noEaOccl(own, pro);

    return Board{b.b ^ diff, b.w ^ diff, b.hash ^ hash_vals[c][pos]};
}


Board add_piece(Board b, int pos, bool c) {
    if (c == BLACK) {
        b.b |= (1L << pos);
    } else {
        b.w |= (1L << pos);
    }

    b.hash ^= hash_vals[c][pos];

    return b;
}




string to_str(Board b) {
    string ret = "  a b c d e f g h\n";

    for (auto i = 0; i < 8; ++i) {
        ret += to_string(i + 1) + " ";
        for (auto j = 0; j < 8; ++j) {
            int pos = (i * 8) + j;

            bool black = (b.b >> pos) & 1L;
            bool white = (b.w >> pos) & 1L;

            if (white) {
                ret += "O ";
            } else if (black) {
                ret += "# ";
            } else {
                ret += "_ ";
            }
        }
        ret += "\n";
    }

    return ret;
}


string to_str(const uint64_t mask) {
    string ret = "  a b c d e f g h\n";

    for (auto i = 0; i < 64; ++i) {
        if (i % 8 == 0) ret += to_string((i / 8) + 1) + " "; // number for row
        ret += ((mask >> i) & 1L) ? "X " : "_ ";
        if (i % 8 == 7)  ret += "\n"; // newline to end row
    }
    ret += "\n";

    return ret;
}


int popcount(uint64_t x) {
    const uint64_t m1  = 0x5555555555555555;
    const uint64_t m2  = 0x3333333333333333;
    const uint64_t m4  = 0x0f0f0f0f0f0f0f0f;
    const uint64_t h01 = 0x0101010101010101;

    x -= (x >> 1) & m1;
    x = (x & m2) + ((x >> 2) & m2);
    x = (x + (x >> 4)) & m4;
    return (x * h01) >> 56;
}


}
