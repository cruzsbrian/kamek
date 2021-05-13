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

// Perft7 + WTHOR + Logistello -> Linear regression
/* const int weights[10][12] = { */
/*     {-3, 10, 21, -10, 182, 62, 42, 40, 30, 170, 15, -17}, */
/*     {89, -2, 7, -35, 206, 38, -70, 23, 29, 98, 16, -24}, */
/*     {149, -6, -2, -50, 232, 20, -141, 5, 36, 78, 23, -37}, */
/*     {238, -3, -13, -51, 233, 21, -203, -13, 47, 69, 1, -41}, */
/*     {315, 4, -19, -58, 210, 15, -273, 33, 43, 65, -1, -37}, */
/*     {380, 15, -21, -70, 193, 23, -343, 47, -36, 63, -2, -23}, */
/*     {495, 30, -19, -89, 192, 29, -436, -25, -155, 55, -6, -3}, */
/*     {609, 42, -18, -114, 113, -101, -541, 112, -145, 37, -8, 9}, */
/*     {557, 36, -17, -145, 97, -80, -591, -442, 0, 21, 2, 15}, */
/*     {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0} */
/* }; */

// Perft7 + WTHOR + Logistello -> Logistic regression
const int weights[10][12] = {
    {31, 12, 27, 2, 203, 124, 68, 82, 17, 207, 9, 0},
    {116, -5, 10, -30, 233, 84, -53, 63, 1, 106, 10, 0},
    {165, -8, 2, -43, 269, 57, -113, 29, 4, 71, 29, 0},
    {258, -2, -6, -41, 272, 50, -158, -9, -1, 54, 3, 0},
    {323, 4, -8, -44, 264, -7, -222, 14, -1, 43, 0, 0},
    {404, 14, -8, -55, 269, -55, -294, 2, -16, 39, -2, 0},
    {503, 28, -5, -85, 260, 103, -425, -120, -119, 35, -6, 0},
    {644, 46, 0, -114, 269, -231, -524, 19, -368, 24, -19, 0},
    {683, 51, 5, -168, 129, -173, -574, -488, 0, 14, 0, 0},
    {0, -56, -234, 0, 0, 0, 176, 0, 0, -3, 74, 0}
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
