#include <search.h>
#include <types.h>
#include <movegen.h>
#include <move.h>

static inline int negamax(Board *board, Eval eval, int depth, int alpha, int beta) {
    if (depth == 0) {
        return eval(board);
    }

    U32 moves[MAX_MOVES];
    U8 count = generate_legal_moves(board, moves);

    if (count == 0) {
        return eval(board);
    }

    for (U8 i = 0; i < count; i++) {
        U64 undo = make_move(board, moves[i]);

        int score = -negamax(board, eval, depth - 1, -beta, -alpha);

        undo_move(board, undo);

        if (score >= beta)
            return beta;

        if (score > alpha)
            alpha = score;
    }

    return alpha;
}

U32 search(Board *board, Eval eval, int depth) {
    U32 moves[MAX_MOVES];
    U8 count = generate_legal_moves(board, moves);
    if (count == 0) return 0;

    U32 best_move = moves[0];

    int best  = -INF;
    int alpha = -INF;
    int beta  =  INF;

    for (U8 i = 0; i < count; i++) {
        U64 undo = make_move(board, moves[i]);

        int score = -negamax(board, eval, depth - 1, -beta, -alpha);

        undo_move(board, undo);

        if (score > best) {
            best = score;
            best_move = moves[i];
        }

        if (score > alpha)
            alpha = score;

        if (alpha >= beta)
            break;
    }

    return best_move;
}