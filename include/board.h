/**
 * board.h
 */

#pragma once

#include <types.h>

#define PIECE(t, c) ((t) + ((c) * 6u))
#define COLOR(x)    (((unsigned)(x) + 2u) >> 3)
#define TYPE(x)     ((unsigned)(x) - (COLOR(x) * 6u))

typedef struct {
    U8 pieces[64];
    U64 bitboards[12];
    U64 occupied[2];
    U64 all_occupied;

    U8 active_color;
    U8 castling_rights;
    U8 en_passant_target;

    U8 halfmove_clock;
    U16 fullmove_number;
} Board;

static inline void update_occupied(Board *board) {
    U64 w_occupied = board->bitboards[WP] 
                   | board->bitboards[WN]
                   | board->bitboards[WB]
                   | board->bitboards[WR]
                   | board->bitboards[WQ]
                   | board->bitboards[WK];

    U64 b_occupied = board->bitboards[BP] 
                   | board->bitboards[BN]
                   | board->bitboards[BB]
                   | board->bitboards[BR]
                   | board->bitboards[BQ]
                   | board->bitboards[BK];

    board->occupied[WHITE] = w_occupied;
    board->occupied[BLACK] = b_occupied;
    board->all_occupied = w_occupied | b_occupied;
}

int load_fen(Board *board, const char *fen);

void clear_board(Board *board);
void print_board(const Board *board);
