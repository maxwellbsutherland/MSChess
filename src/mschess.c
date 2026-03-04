#include <mschess.h>
#include <move.h>
#include <magic.h>
#include <perft.h>
#include <movegen.h>

#include <stdlib.h>

int MSChess_init(MSChess *msc) {
    if (!msc) return -1;

    if (!msc->board)
        msc->board = malloc(sizeof(*msc->board));
    if (!msc->board) return -1;

    initialize_bishop_magics();
    initialize_rook_magics();

    return 0;
}

int MSChess_free(MSChess *msc) {
    if (!msc) return -1;

    if (msc->board)
        free(msc->board);

    free(msc);
    
    return 0;
}

int MSChess_load_fen(MSChess *msc, const char *fen) {
    if (!msc || !msc->board || !fen) return -1;
    return load_fen(msc->board, fen);
}

int MSChess_set_white_eval(MSChess *msc, Eval eval, void (*init)(void)) {
    if (!msc || !msc->board || !eval) return -1;

    if (init) init();
    msc->w_eval = eval;
    
    return 0;
}

int MSChess_set_black_eval(MSChess *msc, Eval eval, void (*init)(void)) {
    if (!msc || !msc->board || !eval) return -1;

    if (init) init();
    msc->b_eval = eval;

    return 0;
}

int MSChess_search(MSChess *msc, int depth, U32 *move) {
    if (!msc || !msc->board || !move || depth <= 0) return -1;

    Eval eval = (msc->board->active_color == WHITE) 
        ? msc->w_eval 
        : msc->b_eval;

    (*move) = search(msc->board, eval, depth);

    return 0;
}

int MSChess_move(MSChess *msc, U32 move) {
    if (!msc) return -1;
    return make_move(msc->board, move) ? -1 : 0;
}

int MSChess_print_board(const MSChess *msc) {
    if (!msc || !msc->board) return -1;
    
    print_board(msc->board);
    return 0;
}

int MSChess_perft(MSChess *msc, int depth, U64 *nodes) {
    if (!msc || !msc->board || !nodes || depth <= 0) return -1;

    (*nodes) = perft(msc->board, depth);
    
    return 0;
}

int MSChess_state(MSChess *msc) {
    if (!msc || !msc->board) return -1;

    U8 c = generate_legal_moves(msc->board, NULL);

    if (is_checkmate(msc->board, c))
        return WHITE_WIN + !msc->board->active_color;

    if (is_stalemate(msc->board, c))
        return DRAW;

    if (msc->board->halfmove_clock >= 50)
        return DRAW;

    return ONGOING;
}
