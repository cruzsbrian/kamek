#include <string>
#include <cstdint>

using namespace std;


enum Color { BLACK = 0, WHITE = 1 };


class Board {
public:
    static void init_hash();
    static uint16_t hash_vals[2][64];

    uint64_t get_moves(Color c) const;
    int get_frontier(Color c) const;

    void do_move(int pos, Color c);
    void add_piece(int pos, Color c);

    string to_str() const;

private:
    uint64_t b = 0L;
    uint64_t w = 0L;
    uint16_t hash = 0;
};


int popcount(const uint64_t x);
string mask_to_str(uint64_t x);
