/**
 * magic.h
 */

#pragma once

#include <types.h>

typedef struct {
    const U64 *ptr;
    U64 mask;
    U64 magic;
    U8 shift;
} Magic;

extern const U64 bishop_magic_values[64];
extern const U64 rook_magic_values[64];

extern Magic bishop_magics[64];
extern Magic rook_magics[64];

static inline U32 get_magic_index(const Magic *magic, U64 occupied) {
    return (U32)(((occupied & magic->mask) * magic->magic) >> magic->shift);
}

static inline U64 get_magic_bitboard(const Magic *magic, U64 occupied) {
    return magic->ptr[get_magic_index(magic, occupied)];
}

void initialize_bishop_magics(void);
void initialize_rook_magics(void);
