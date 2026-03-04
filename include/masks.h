/**
 * masks.h
 */

#pragma once

#include <types.h>

extern const U64 pawn_masks[2][64];
extern const U64 knight_masks[64];
extern const U64 bishop_masks[64];
extern const U64 rook_masks[64];
extern const U64 king_masks[64];

extern const U64 bishop_magic_masks[64][512];
extern const U64 rook_magic_masks[64][4096];
