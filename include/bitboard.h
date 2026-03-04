/**
 * bitboard.h
 */

#pragma once

#include <types.h>

#define RANK_1 0x00000000000000FFULL
#define RANK_2 0x000000000000FF00ULL
#define RANK_3 0x0000000000FF0000ULL
#define RANK_4 0x00000000FF000000ULL
#define RANK_5 0x000000FF00000000ULL
#define RANK_6 0x0000FF0000000000ULL
#define RANK_7 0x00FF000000000000ULL
#define RANK_8 0xFF00000000000000ULL

#define FILE_A 0x0101010101010101ULL
#define FILE_B 0x0202020202020202ULL
#define FILE_C 0x0404040404040404ULL
#define FILE_D 0x0808080808080808ULL
#define FILE_E 0x1010101010101010ULL
#define FILE_F 0x2020202020202020ULL
#define FILE_G 0x4040404040404040ULL
#define FILE_H 0x8080808080808080ULL

#define NOT_RANK_1 ~RANK_1
#define NOT_RANK_2 ~RANK_2
#define NOT_RANK_3 ~RANK_3
#define NOT_RANK_4 ~RANK_4
#define NOT_RANK_5 ~RANK_5
#define NOT_RANK_6 ~RANK_6
#define NOT_RANK_7 ~RANK_7
#define NOT_RANK_8 ~RANK_8

#define NOT_FILE_A ~FILE_A
#define NOT_FILE_B ~FILE_B
#define NOT_FILE_C ~FILE_C
#define NOT_FILE_D ~FILE_D
#define NOT_FILE_E ~FILE_E
#define NOT_FILE_F ~FILE_F
#define NOT_FILE_G ~FILE_G
#define NOT_FILE_H ~FILE_H

#define BB(sq) (1ULL << (sq))

#define SET_BIT(bb, sq) ((bb) |= BB(sq))
#define GET_BIT(bb, sq) (1 & ((bb) >> (sq)))
#define POP_BIT(bb, sq) ((bb) &= ~BB(sq))

#define FILE(sq)    ((sq) % 8)
#define RANK(sq)    ((sq) / 8)
#define INDEX(f, r) ((r) * 8 + (f))
