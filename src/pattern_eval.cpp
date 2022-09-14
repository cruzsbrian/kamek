#include "pattern_eval.h"

#include <fstream>
#include <iostream>
#include <immintrin.h>


namespace eval {


int16_t weights[total_instances];
int16_t weight_intercept;


inline int score_pat(uint64_t own, uint64_t opp, size_t mask_idx) {
    uint64_t mask = masks[mask_idx];
    uint64_t own_bits = pext(own, mask);
    uint64_t opp_bits = pext(opp, mask);

    // Determine index into weights array from pattern and instance
    uint16_t instance =  2 * ternary_ones[own_bits] + ternary_ones[opp_bits];
    uint16_t weight_idx = pattern_start[mask_idx] + instance;

    return weights[weight_idx];
}

int score(board::Board b) {
    int score = -weight_intercept;

    // Diagonal-aligned masks
    for (size_t mask_idx = 0; mask_idx < 4; mask_idx++) {
        score += score_pat(b.own, b.opp, mask_idx);
        score += score_pat(flip_adiag(b.own), flip_adiag(b.opp), mask_idx);
        score += score_pat(flip_vert(b.own), flip_vert(b.opp), mask_idx);
        score += score_pat(flip_horiz(b.own), flip_horiz(b.opp), mask_idx);
    }

    // Long diagonal
    score += score_pat(b.own, b.opp, DIAG_8);
    score += score_pat(flip_vert(b.own), flip_vert(b.opp), DIAG_8);

    // Edge-aligned masks
    for (size_t mask_idx = 5; mask_idx < 9; mask_idx++) {
        score += score_pat(b.own, b.opp, mask_idx);
        score += score_pat(flip_vert(b.own), flip_vert(b.opp), mask_idx);
        score += score_pat(flip_diag(b.own), flip_diag(b.opp), mask_idx);
        score += score_pat(flip_adiag(b.own), flip_adiag(b.opp), mask_idx);
    }

    return score;
}


void load_weights(string filename) {
    ifstream weights_file(filename);

    if (!weights_file.is_open()) {
        cerr << "Could not open weights file" << endl;
        exit(1);
    }

    try {
        weights_file >> weight_intercept;
    } catch (...) {
        cerr << "\nError loading weights\n";
        exit(1);
    }

    for (int i = 0; i < total_instances; i++) {
        try {
            weights_file >> weights[i];
        } catch (...) {
            cerr << "\nError loading weights\n";
            exit(1);
        }
    }

    weights_file.close();
}


// Utility functions for different flips of masks
uint64_t flip_vert(uint64_t x) {
    return __builtin_bswap64(x);
}

uint64_t flip_horiz(uint64_t x) {
    const uint64_t k1 = 0x5555555555555555;
    const uint64_t k2 = 0x3333333333333333;
    const uint64_t k4 = 0x0f0f0f0f0f0f0f0f;
    x = ((x >> 1) & k1) +  2*(x & k1);
    x = ((x >> 2) & k2) +  4*(x & k2);
    x = ((x >> 4) & k4) + 16*(x & k4);
    return x;
}

uint64_t flip_diag(uint64_t x) {
    uint64_t t;
    const uint64_t k1 = 0x5500550055005500;
    const uint64_t k2 = 0x3333000033330000;
    const uint64_t k4 = 0x0f0f0f0f00000000;
    t  = k4 & (x ^ (x << 28));
    x ^=       t ^ (t >> 28) ;
    t  = k2 & (x ^ (x << 14));
    x ^=       t ^ (t >> 14) ;
    t  = k1 & (x ^ (x <<  7));
    x ^=       t ^ (t >>  7) ;
    return x;
}

uint64_t flip_adiag(uint64_t x) {
    uint64_t t;
    const uint64_t k1 = 0xaa00aa00aa00aa00;
    const uint64_t k2 = 0xcccc0000cccc0000;
    const uint64_t k4 = 0xf0f0f0f00f0f0f0f;
    t  =       x ^ (x << 36) ;
    x ^= k4 & (t ^ (x >> 36));
    t  = k2 & (x ^ (x << 18));
    x ^=       t ^ (t >> 18) ;
    t  = k1 & (x ^ (x <<  9));
    x ^=       t ^ (t >>  9) ;
    return x;
}


inline uint64_t pext(uint64_t src, uint64_t mask) {
    return _pext_u64(src, mask);
}


};
