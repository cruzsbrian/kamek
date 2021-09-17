#pragma once

#include <string>

#include "board.h"


namespace eval {


#define N_PATTERNS 9
#define N_ALL_MASKS 34  // 4 for each pattern except diag8 (2)


void pattern_activations(int *ret, board::Board b);
int score(board::Board b);

void load_weights(string filename);

uint64_t flip_vert(uint64_t x);
uint64_t flip_horiz(uint64_t x);
uint64_t flip_diag(uint64_t x);
uint64_t flip_adiag(uint64_t x);

inline uint64_t pext(uint64_t src, uint64_t mask);


#define DIAG_4 0
#define DIAG_5 1
#define DIAG_6 2
#define DIAG_7 3
#define DIAG_8 4
#define HORVERT_1 5
#define HORVERT_2 6
#define HORVERT_3 7
#define HORVERT_4 8

// Base set of patterns
const uint64_t base_masks[N_PATTERNS] = {
    0x1020408000000000, // diag 4
    0x0810204080000000, // diag 5
    0x0408102040800000, // diag 6
    0x0204081020408000, // diag 7
    0x0102040810204080, // diag 8
    0xff00000000000000, // hor/vert 1
    0x00ff000000000000, // hor/vert 2
    0x0000ff0000000000, // hor/vert 3
    0x000000ff00000000, // hor/vert 4
};

// Number of instances each pattern can hold = 3^(n_bits)
const uint16_t n_instances[N_PATTERNS] = { 81, 243, 729, 2187, 6561, 6561, 6561, 6561, 6561 };

const uint16_t pattern_start[N_PATTERNS] = { 0, 81, 324, 1053, 3240, 9801, 16362, 22923, 29484 };
const uint16_t total_instances = 36045; 

// Flipped masks
const uint64_t all_masks[N_ALL_MASKS] = {
    // Diagonal-aligned masks
    base_masks[DIAG_4],
    flip_adiag(base_masks[DIAG_4]),
    flip_vert(base_masks[DIAG_4]),
    flip_horiz(base_masks[DIAG_4]),
    base_masks[DIAG_5],
    flip_adiag(base_masks[DIAG_5]),
    flip_vert(base_masks[DIAG_5]),
    flip_horiz(base_masks[DIAG_5]),
    base_masks[DIAG_6],
    flip_adiag(base_masks[DIAG_6]),
    flip_vert(base_masks[DIAG_6]),
    flip_horiz(base_masks[DIAG_6]),
    base_masks[DIAG_7],
    flip_adiag(base_masks[DIAG_7]),
    flip_vert(base_masks[DIAG_7]),
    flip_horiz(base_masks[DIAG_7]),
    base_masks[DIAG_8],
    flip_vert(base_masks[DIAG_8]),

    // Edge-aligned masks
    base_masks[HORVERT_1],
    flip_vert(base_masks[HORVERT_1]),
    flip_diag(base_masks[HORVERT_1]),
    flip_adiag(base_masks[HORVERT_1]),
    base_masks[HORVERT_2],
    flip_vert(base_masks[HORVERT_2]),
    flip_diag(base_masks[HORVERT_2]),
    flip_adiag(base_masks[HORVERT_2]),
    base_masks[HORVERT_3],
    flip_vert(base_masks[HORVERT_3]),
    flip_diag(base_masks[HORVERT_3]),
    flip_adiag(base_masks[HORVERT_3]),
    base_masks[HORVERT_4],
    flip_vert(base_masks[HORVERT_4]),
    flip_diag(base_masks[HORVERT_4]),
    flip_adiag(base_masks[HORVERT_4])
};

// Maps index of flipped mask to base mask
const uint8_t mask_base[N_ALL_MASKS] = {
    DIAG_4, DIAG_4, DIAG_4, DIAG_4,
    DIAG_5, DIAG_5, DIAG_5, DIAG_5,
    DIAG_6, DIAG_6, DIAG_6, DIAG_6,
    DIAG_7, DIAG_7, DIAG_7, DIAG_7,
    DIAG_8, DIAG_8,
    HORVERT_1, HORVERT_1, HORVERT_1, HORVERT_1,
    HORVERT_2, HORVERT_2, HORVERT_2, HORVERT_2,
    HORVERT_3, HORVERT_3, HORVERT_3, HORVERT_3,
    HORVERT_4, HORVERT_4, HORVERT_4, HORVERT_4
};


// Conversion from two binary masks to ternary indices
const uint16_t ternary_ones[256] = {
    0,    1,    3,    4,    9,    10,   12,   13,   27,   28,   30,   31,   36,   37,   39,   40,
    81,   82,   84,   85,   90,   91,   93,   94,   108,  109,  111,  112,  117,  118,  120,  121,
    243,  244,  246,  247,  252,  253,  255,  256,  270,  271,  273,  274,  279,  280,  282,  283,
    324,  325,  327,  328,  333,  334,  336,  337,  351,  352,  354,  355,  360,  361,  363,  364,
    729,  730,  732,  733,  738,  739,  741,  742,  756,  757,  759,  760,  765,  766,  768,  769,
    810,  811,  813,  814,  819,  820,  822,  823,  837,  838,  840,  841,  846,  847,  849,  850,
    972,  973,  975,  976,  981,  982,  984,  985,  999,  1000, 1002, 1003, 1008, 1009, 1011, 1012,
    1053, 1054, 1056, 1057, 1062, 1063, 1065, 1066, 1080, 1081, 1083, 1084, 1089, 1090, 1092, 1093,
    2187, 2188, 2190, 2191, 2196, 2197, 2199, 2200, 2214, 2215, 2217, 2218, 2223, 2224, 2226, 2227,
    2268, 2269, 2271, 2272, 2277, 2278, 2280, 2281, 2295, 2296, 2298, 2299, 2304, 2305, 2307, 2308,
    2430, 2431, 2433, 2434, 2439, 2440, 2442, 2443, 2457, 2458, 2460, 2461, 2466, 2467, 2469, 2470,
    2511, 2512, 2514, 2515, 2520, 2521, 2523, 2524, 2538, 2539, 2541, 2542, 2547, 2548, 2550, 2551,
    2916, 2917, 2919, 2920, 2925, 2926, 2928, 2929, 2943, 2944, 2946, 2947, 2952, 2953, 2955, 2956,
    2997, 2998, 3000, 3001, 3006, 3007, 3009, 3010, 3024, 3025, 3027, 3028, 3033, 3034, 3036, 3037,
    3159, 3160, 3162, 3163, 3168, 3169, 3171, 3172, 3186, 3187, 3189, 3190, 3195, 3196, 3198, 3199,
    3240, 3241, 3243, 3244, 3249, 3250, 3252, 3253, 3267, 3268, 3270, 3271, 3276, 3277, 3279, 3280
};

const uint16_t ternary_twos[256] = {
    0,    2,    6,    8,    18,   20,   24,   26,   54,   56,   60,   62,   72,   74,   78,   80,
    162,  164,  168,  170,  180,  182,  186,  188,  216,  218,  222,  224,  234,  236,  240,  242,
    486,  488,  492,  494,  504,  506,  510,  512,  540,  542,  546,  548,  558,  560,  564,  566,
    648,  650,  654,  656,  666,  668,  672,  674,  702,  704,  708,  710,  720,  722,  726,  728,
    1458, 1460, 1464, 1466, 1476, 1478, 1482, 1484, 1512, 1514, 1518, 1520, 1530, 1532, 1536, 1538,
    1620, 1622, 1626, 1628, 1638, 1640, 1644, 1646, 1674, 1676, 1680, 1682, 1692, 1694, 1698, 1700,
    1944, 1946, 1950, 1952, 1962, 1964, 1968, 1970, 1998, 2000, 2004, 2006, 2016, 2018, 2022, 2024,
    2106, 2108, 2112, 2114, 2124, 2126, 2130, 2132, 2160, 2162, 2166, 2168, 2178, 2180, 2184, 2186,
    4374, 4376, 4380, 4382, 4392, 4394, 4398, 4400, 4428, 4430, 4434, 4436, 4446, 4448, 4452, 4454,
    4536, 4538, 4542, 4544, 4554, 4556, 4560, 4562, 4590, 4592, 4596, 4598, 4608, 4610, 4614, 4616,
    4860, 4862, 4866, 4868, 4878, 4880, 4884, 4886, 4914, 4916, 4920, 4922, 4932, 4934, 4938, 4940,
    5022, 5024, 5028, 5030, 5040, 5042, 5046, 5048, 5076, 5078, 5082, 5084, 5094, 5096, 5100, 5102,
    5832, 5834, 5838, 5840, 5850, 5852, 5856, 5858, 5886, 5888, 5892, 5894, 5904, 5906, 5910, 5912,
    5994, 5996, 6000, 6002, 6012, 6014, 6018, 6020, 6048, 6050, 6054, 6056, 6066, 6068, 6072, 6074,
    6318, 6320, 6324, 6326, 6336, 6338, 6342, 6344, 6372, 6374, 6378, 6380, 6390, 6392, 6396, 6398,
    6480, 6482, 6486, 6488, 6498, 6500, 6504, 6506, 6534, 6536, 6540, 6542, 6552, 6554, 6558, 6560
};


}
