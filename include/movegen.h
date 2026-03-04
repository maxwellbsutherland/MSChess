/**
 * movegen.h
 */

#pragma once

#include <types.h>
#include <board.h>
#include <move.h>

#define MAX_MOVES 218u

U8 generate_legal_moves(Board *board, U32 moves[]);

static inline U8 is_check(Board *board) {
    U8 sq = (U8)__builtin_ctzll(board->bitboards[PIECE(KING, board->active_color)]);
    return is_square_attacked(board, sq);
}

static inline U8 is_checkmate(Board *board, U8 count) {
    if (count != 0)
        return 0;

    return is_check(board);
}

static inline U8 is_stalemate(Board *board, U8 count) {
    if (count != 0)
        return 0;

    return !is_check(board);
}

void generate_pawn_moves(Board *board, U32 moves[], U8 *count);
void generate_knight_moves(Board *board, U32 moves[], U8 *count);
void generate_bishop_moves(Board *board, U32 moves[], U8 *count);
void generate_rook_moves(Board *board, U32 moves[], U8 *count);
void generate_queen_moves(Board *board, U32 moves[], U8 *count);
void generate_king_moves(Board *board, U32 moves[], U8 *count);
