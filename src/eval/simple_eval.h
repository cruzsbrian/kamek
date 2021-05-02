#include "../board/board.h"


namespace eval {


#define W_CORNER             0
#define W_EDGE_A             1
#define W_EDGE_B             2
#define W_C_SQ_CORNER_EMPTY  3
#define W_C_SQ_CORNER_SAME   4
#define W_C_SQ_CORNER_OPP    5
#define W_X_SQ_CORNER_EMPTY  6
#define W_X_SQ_CORNER_SAME   7
#define W_X_SQ_CORNER_OPP    8
#define W_MOBILITY           9
#define W_FRONTIER          10
#define W_CONST             11


const uint64_t m_corner    = 0x8100000000000081;
const uint64_t m_edge_a    = 0x2400810000810024;
const uint64_t m_edge_b    = 0x1800008181000018; 

const uint64_t m_corner_ul = 0x8000000000000000;
const uint64_t m_corner_ur = 0x0100000000000000;
const uint64_t m_corner_ll = 0x0000000000000080;
const uint64_t m_corner_lr = 0x0000000000000001;

const uint64_t m_x_sq_ul   = 0x0040000000000000;
const uint64_t m_x_sq_ur   = 0x0002000000000000;
const uint64_t m_x_sq_ll   = 0x0000000000004000;
const uint64_t m_x_sq_lr   = 0x0000000000000200;

const uint64_t m_c_sq_ul   = 0x4080000000000000;
const uint64_t m_c_sq_ur   = 0x0201000000000000;
const uint64_t m_c_sq_ll   = 0x0000000000008040;
const uint64_t m_c_sq_lr   = 0x0000000000000102;

/* const int weights[10][11] = { */
/*     {-39,  2,  12,  -17,  254, 58,   62,   51,   39,   248, 33 }, */
/*     {80,   -3, -6,  -62,  260, 1,    -74,  13,   44,   169, 17 }, */
/*     {125,  -6, -17, -93,  292, -17,  -144, -8,   35,   146, 21 }, */
/*     {153,  -1, -34, -106, 345, -9,   -183, -10,  -7,   133, -4 }, */
/*     {140,  8,  -50, -116, 422, -11,  -213, -17,  -96,  128, -9 }, */
/*     {212,  26, -55, -133, 476, -149, -213, 158,  -332, 126, -18 }, */
/*     {394,  53, -51, -127, 503, -190, -318, 385,  -612, 111, -11 }, */
/*     {-279, 72, -40, -46,  748, -368, -433, -135, 0,    71,  -5 }, */
/*     {-670, 19, -23, -252, 0,   0,    61,   0,    0,    28,  5 }, */
/*     {-670, 19, -23, -252, 0,   0,    61,   0,    0,    28,  5 }, */
/* }; */

const int weights[10][12] = {
    {-56,  3,  13,  -20,  258, 45,   51,   51,   36,   249, 30,  -52 },
    {62,   -3, -7,  -65,  263, -16,  -85,  12,   40,   171, 16,  -69 },
    {103,  -6, -18, -96,  294, -40,  -160, -11,  29,   148, 17,  -90 },
    {133,  -2, -37, -108, 344, -34,  -199, -15,  -16,  135, -6,  -95 },
    {123,  5,  -53, -117, 419, -37,  -228, -24,  -107, 131, -8,  -85 },
    {200,  24, -59, -134, 472, -168, -227, 151,  -349, 128, -17, -63 },
    {389,  51, -53, -127, 502, -202, -327, 383,  -617, 112, -11, -29 },
    {-279, 73, -39, -46,  745, -362, -432, -135, 0,    71,  -6,  5 },
    {-670, 21, -20, -251, 0,   0,    61,   0,    0,    28,  4,   9 }
};

int score(board::Board b);


int corners(board::Board b);
int edge_a(board::Board b);
int edge_b(board::Board b);
int c_sq_corner_empty(board::Board b);
int c_sq_corner_same(board::Board b);
int c_sq_corner_opp(board::Board b);
int x_sq_corner_empty(board::Board b);
int x_sq_corner_same(board::Board b);
int x_sq_corner_opp(board::Board b);
int mobility(board::Board b);
int frontier(board::Board b);


}
