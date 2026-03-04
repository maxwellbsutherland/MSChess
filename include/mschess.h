/**
 * mschess.h
 */

#pragma once

#include <board.h>
#include <search.h>

#define ONGOING   0x00
#define WHITE_WIN 0x01
#define BLACK_WIN 0x02
#define DRAW      0x03

#define STANDARD_INITIAL_POSITION \
    ("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1")

typedef struct {
    Board *board;
    Eval w_eval, b_eval;
} MSChess;

int MSChess_init(MSChess *msc);
int MSChess_load_fen(MSChess *msc, const char *fen);
int MSChess_free(MSChess *msc);

int MSChess_set_white_eval(MSChess *msc, Eval eval, void (*init)(void));
int MSChess_set_black_eval(MSChess *msc, Eval eval, void (*init)(void));

int MSChess_search(MSChess *msc, int depth, U32 *move);
int MSChess_move(MSChess *msc, U32 move);

int MSChess_print_board(const MSChess *msc);

int MSChess_perft(MSChess *msc, int depth, U64 *nodes);

int MSChess_state(MSChess *msc);
