#include <board.h>
#include <bitboard.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <ctype.h>

static inline const char *skip_spaces(const char *p) {
    while (*p == ' ') p++;
    return p;
}

static inline U64 *fen_piece_bb(Board *b, unsigned char c) {
    switch (c) {
        case 'P': return &b->bitboards[WP];
        case 'N': return &b->bitboards[WN];
        case 'B': return &b->bitboards[WB];
        case 'R': return &b->bitboards[WR];
        case 'Q': return &b->bitboards[WQ];
        case 'K': return &b->bitboards[WK];
        case 'p': return &b->bitboards[BP];
        case 'n': return &b->bitboards[BN];
        case 'b': return &b->bitboards[BB];
        case 'r': return &b->bitboards[BR];
        case 'q': return &b->bitboards[BQ];
        case 'k': return &b->bitboards[BK];
        default:  return NULL;
    }
}

static int parse_ep_square(const char *p, U8 *out_sq) {
    if (p[0] == '-') { 
        *out_sq = SQ_NONE; 
        return 1; 
    }

    char f = p[0], r = p[1];
    if (f < 'a' || f > 'h') return -1;
    if (r < '1' || r > '8') return -1;

    int file = f - 'a';
    int rank = r - '1';
    *out_sq = (U8)INDEX(file, rank);
    return 2;
}

static int parse_int(const char **p, unsigned int *out) {
    const char *s = *p;
    if (!isdigit((unsigned char)*s)) return -1;

    int v = 0;
    while (isdigit((unsigned char)*s)) {
        v = v * 10u + (unsigned int)(*s - '0');
        s++;
    }

    *out = v;
    *p = s;
    return 0;
}

int load_fen(Board *board, const char *fen) {
    if (!board || !fen) return -1;
    clear_board(board);

    int file = 0, rank = 7;

    /* Piece Placement */ {
        while (*fen && *fen != ' ') {
            char c = *fen++;

            if (c == '/') { 
                rank--; 
                file = 0; 
                continue; 
            }

            if (c >= '1' && c <= '8') { 
                file += (c - '0'); 
                continue; 
            }

            U64 *bb = fen_piece_bb(board, c);
            if (!bb) return -1;

            U8 sq = INDEX(file, rank);
            SET_BIT(*bb, sq);

            file++;
        }
    }

    fen = skip_spaces(fen);

    /* Active Color */ {
        if (*fen != 'w' && *fen != 'b') return -1;
        board->active_color = (*fen == 'w') ? WHITE : BLACK;

        fen++;
    }

    fen = skip_spaces(fen);

    /* Castling Rights */ {
        while (*fen && *fen != ' ') {
            char c = *fen++;
            switch (c) {
                case 'K': board->castling_rights |= WKCA;  break;
                case 'Q': board->castling_rights |= WQCA; break;
                case 'k': board->castling_rights |= BKCA;  break;
                case 'q': board->castling_rights |= BQCA; break;
            }
        }
    }

    fen = skip_spaces(fen);

    /* En Passant Square */ {
        int n = parse_ep_square(fen, &board->en_passant_target);
        if (n < 0) return -1;
        fen += n;
    }

    fen = skip_spaces(fen);

    /* Halfmove Clock */ {
        unsigned int hm = 0;
        if (parse_int(&fen, &hm) != 0) return -1;
        if (hm > 255u) return -1;
        board->halfmove_clock = (U8)hm;
    }

    fen = skip_spaces(fen);

    /* Fullmove Number */ {
        unsigned int fm = 0;
        if (parse_int(&fen, &fm) != 0) return -1;
        if (fm > 65535u) return -1;
        board->fullmove_number = (U16)fm;
    }

    update_occupied(board);

    return 0;
}

void clear_board(Board *board) {
    if (!board) return;

    memset(board, 0, sizeof(*board));

    board->active_color = WHITE;
    board->en_passant_target = SQ_NONE;
    board->fullmove_number = 1;
}

static inline void print_u8_dec(int v) {
    /* v is 1..8 here */
    putchar('0' + v);
}

void print_board(const Board *board) {
    if (!board) return;

    for (int r = 7; r >= 0; r--) {
        print_u8_dec(r + 1);
        fputs("   ", stdout);

        for (int f = 0; f < 8; f++) {
            U8 sq = (U8)INDEX(f, r);
            if (sq >= 64) { fputs("? ", stdout); continue; }

            U8 piece = 255;
            for (U8 p = 0; p < EMPTY; p++) {
                if (GET_BIT(board->bitboards[p], sq)) { piece = p; break; }
            }

            if (piece == 255) {
                fputs(". ", stdout);
                continue;
            }

            static const char pc[12] = {'P','N','B','R','Q','K','p','n','b','r','q','k'};
            char c = (piece < 12) ? pc[piece] : '?';

            putchar(c);
            putchar(' ');
        }
        putchar('\n');
    }

    putchar('\n');
    fputs("    a b c d e f g h\n", stdout);
    fflush(stdout);
}
