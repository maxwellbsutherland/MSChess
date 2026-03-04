#include <perft.h>
#include <movegen.h>
#include <move.h>

U64 perft(Board *board, int depth) {
    if (depth == 0)
        return 1ULL;

    U32 moves[MAX_MOVES];
    U8 c = generate_legal_moves(board, moves);

    U64 nodes = 0ULL;
    for (int i = 0; i < c; i++) {
        U64 undo = make_move(board, moves[i]);
        nodes += perft(board, depth - 1);
        undo_move(board, undo);
    }

    return nodes;
}