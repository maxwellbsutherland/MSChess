#include <mschess.h>
#include <board.h>
#include <eval.h>

#include <stdio.h>
#include <stdlib.h>

int main(void) {
    MSChess *msc = malloc(sizeof(*msc));
    if (!msc) return -1;

    if (MSChess_init(msc) != 0) {
        printf("Init Failed\n");
        return -1;
    }
    
    if (MSChess_load_fen(msc, STANDARD_INITIAL_POSITION) != 0) {
        printf("Load Fen Failed\n");
        return -1;
    }

    U64 nodes;
    MSChess_perft(msc, 6, &nodes);
    printf("perft: %lli\n", nodes);

    MSChess_set_white_eval(msc, evaluate, initialize_evaluation);
    MSChess_set_black_eval(msc, evaluate, initialize_evaluation);

    int state;
    do {
        U32 move;
        MSChess_search(msc, 4, &move);
        MSChess_move(msc, move);

        MSChess_print_board(msc);
    } while (state = MSChess_state(msc), state == ONGOING);

    printf("state: %i\n", state);

    MSChess_free(msc);

    return 0;
}