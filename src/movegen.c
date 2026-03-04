#include <movegen.h>
#include <bitboard.h>
#include <magic.h>
#include <masks.h>

#include <stdlib.h>
#include <string.h>

static inline void add_move(Board *board, U32 move, U32 moves[], U8 *count) {
    if (!board || !count) return;

    U64 undo = make_move(board, move);
    board->active_color ^= 1;

    if (!is_check(board)) {
        if (moves)
            moves[(*count)] = move;
        (*count)++;
    }

    board->active_color ^= 1;
    undo_move(board, undo);
}

void generate_pawn_moves(Board *board, U32 moves[], U8 *count) {
    if (!board || !count) return;

    const U8  color = board->active_color;
    const U16 piece = PIECE(PAWN, color);
    const U64 enemy = board->occupied[color ^ 1];
    const U64 empty = ~board->all_occupied;

    U64 pawns = board->bitboards[piece];

    const int push = (color == WHITE) ? 8 : -8;
    const U64 rank2 = (color == WHITE) ? RANK_2 : RANK_7;
    const U64 promo_rank = (color == WHITE) ? RANK_8 : RANK_1;

    while (pawns) {
        const U8 from = __builtin_ctzll(pawns);
        pawns &= pawns - 1;

        U64 caps = pawn_masks[color][from] & enemy;

        // Pawn Attacks
        while (caps) {
            const U8 to = __builtin_ctzll(caps);
            caps &= caps - 1;

            const U64 to_bb = BB(to);

            if (to_bb & promo_rank) {
                add_move(board, MOVE(piece, to, from, CAPTURE | PROMOTION_QUEEN), moves, count);
                add_move(board, MOVE(piece, to, from, CAPTURE | PROMOTION_ROOK), moves, count);
                add_move(board, MOVE(piece, to, from, CAPTURE | PROMOTION_BISHOP), moves, count);
                add_move(board, MOVE(piece, to, from, CAPTURE | PROMOTION_KNIGHT), moves, count);
            } else {
                add_move(board, MOVE(piece, to, from, CAPTURE), moves, count);
            }
        }

        // En Passant
        if (board->en_passant_target != SQ_NONE) {
            const U8 ep = board->en_passant_target;
            const U64 ep_bb = BB(ep);

            if (pawn_masks[color][from] & ep_bb) {
                add_move(board, MOVE(piece, ep, from, CAPTURE | EN_PASSANT), moves, count);
            }
        }

        // Pawn Pushes
        const U64 from_bb = BB(from);
        const int to_i = (int)from + push;
        if ((unsigned)to_i < 64) {
            const U8 to = (U8)to_i;
            const U64 to_bb = BB(to);

            if (to_bb & empty) {
                if (to_bb & promo_rank) {
                    add_move(board, MOVE(piece, to, from, QUIET | PROMOTION_QUEEN), moves, count);
                    add_move(board, MOVE(piece, to, from, QUIET | PROMOTION_ROOK), moves, count);
                    add_move(board, MOVE(piece, to, from, QUIET | PROMOTION_BISHOP), moves, count);
                    add_move(board, MOVE(piece, to, from, QUIET | PROMOTION_KNIGHT), moves, count);
                } else {
                    add_move(board, MOVE(piece, to, from, QUIET), moves, count);

                    if (from_bb & rank2) {
                        const int to2_i = (int)from + 2 * push;
                        const U8 to2 = (U8)to2_i;
                        const U64 to2_bb = BB(to2);

                        if (to2_bb & empty) {
                            add_move(board, MOVE(piece, to2, from, QUIET | DOUBLE_PUSH), moves, count);
                        }
                    }
                }
            }
        }
    }
}

void generate_knight_moves(Board *board, U32 moves[], U8 *count) {
    if (!board || !count) return;

    const U8  color = board->active_color;
    const U16 piece = PIECE(KNIGHT, color);
    const U64 own   = board->occupied[color];
    const U64 enemy = board->occupied[color ^ 1];

    U64 knights = board->bitboards[piece];

    while (knights) {
        const U8 from = __builtin_ctzll(knights);
        knights &= knights - 1;

        U64 attacks = knight_masks[from];
        attacks &= ~own;

        while (attacks) {
            const U8 to = __builtin_ctzll(attacks);
            attacks &= attacks - 1;

            U32 flags;

            if (BB(to) & enemy) {
                flags = CAPTURE;
            } else {
                flags = QUIET;
            }

            add_move(board, MOVE(piece, to, from, flags), moves, count);
        }
    }
}

void generate_bishop_moves(Board *board, U32 moves[], U8 *count) {
    if (!board || !count) return;

    const U8  color = board->active_color;
    const U16 piece = PIECE(BISHOP, color);
    const U64 own   = board->occupied[color];
    const U64 enemy = board->occupied[color ^ 1];
    const U64 all   = board->all_occupied;

    U64 bishops = board->bitboards[piece];

    while (bishops) {
        const U8 from = __builtin_ctzll(bishops);
        bishops &= bishops - 1;

        const Magic *magic = &bishop_magics[from];
        U64 attacks = get_magic_bitboard(magic, all);
        attacks &= ~own;

        while (attacks) {
            const U8 to = __builtin_ctzll(attacks);
            attacks &= attacks - 1;

            U32 flags;

            if (BB(to) & enemy) {
                flags = CAPTURE;
            } else {
                flags = QUIET;
            }

            add_move(board, MOVE(piece, to, from, flags), moves, count);
        }
    }
}

void generate_rook_moves(Board *board, U32 moves[], U8 *count) {
    if (!board || !count) return;

    const U8  color = board->active_color;
    const U16 piece = PIECE(ROOK, color);
    const U64 own   = board->occupied[color];
    const U64 enemy = board->occupied[color ^ 1];
    const U64 all   = board->all_occupied;

    U64 rooks = board->bitboards[piece];

    while (rooks) {
        const U8 from = __builtin_ctzll(rooks);
        rooks &= rooks - 1;

        const Magic *magic = &rook_magics[from];
        U64 attacks = get_magic_bitboard(magic, all);
        attacks &= ~own;

        while (attacks) {
            const U8 to = __builtin_ctzll(attacks);
            attacks &= attacks - 1;

            U32 flags;

            if (BB(to) & enemy) {
                flags = CAPTURE;
            } else {
                flags = QUIET;
            }

            add_move(board, MOVE(piece, to, from, flags), moves, count);
        }
    }
}

void generate_queen_moves(Board *board, U32 moves[], U8 *count) {
    if (!board || !count) return;

    const U8  color = board->active_color;
    const U16 piece = PIECE(QUEEN, color);
    const U64 own   = board->occupied[color];
    const U64 enemy = board->occupied[color ^ 1];
    const U64 all   = board->all_occupied;

    U64 queens = board->bitboards[piece];

    while (queens) {
        const U8 from = __builtin_ctzll(queens);
        queens &= queens - 1;

        const Magic *bishop_magic = &bishop_magics[from];
        const Magic *rook_magic   = &rook_magics[from];

        const U64 bishop_attacks = get_magic_bitboard(bishop_magic, all);
        const U64 rook_attacks = get_magic_bitboard(rook_magic, all);

        U64 attacks = bishop_attacks | rook_attacks;
        attacks &= ~own;

        while (attacks) {
            const U8 to = __builtin_ctzll(attacks);
            attacks &= attacks - 1;

            U32 flags;

            if (BB(to) & enemy) {
                flags = CAPTURE;
            } else {
                flags = QUIET;
            }

            add_move(board, MOVE(piece, to, from, flags), moves, count);
        }
    }
}

void generate_king_moves(Board *board, U32 moves[], U8 *count) {
    if (!board || !count) return;

    const U8  color = board->active_color;
    const U16 piece = PIECE(KING, color);
    const U64 own   = board->occupied[color];
    const U64 enemy = board->occupied[color ^ 1];

    U64 kings = board->bitboards[piece];

    while (kings) {
        const U8 from = __builtin_ctzll(kings);
        kings &= kings - 1;

        // King Attacks
        U64 attacks = king_masks[from] & ~own;
        while (attacks) {
            const U8 to = __builtin_ctzll(attacks);
            attacks &= attacks - 1;

            U32 flags = (BB(to) & enemy) ? CAPTURE : QUIET;
            add_move(board, MOVE(piece, to, from, flags), moves, count);
        }

        // Castling
        const U8 rights = board->castling_rights;
        if (color == WHITE) {
            if (from == e1 && !is_square_attacked(board, e1)) {
                if ((rights & 0x1) &&
                    !(board->all_occupied & (BB(f1) | BB(g1))) &&
                    !is_square_attacked(board, f1) &&
                    !is_square_attacked(board, g1)) {

                    add_move(board, MOVE(piece, g1, e1, QUIET | CASTLE_KING), moves, count);
                }

                if ((rights & 0x2) &&
                    !(board->all_occupied & (BB(d1) | BB(c1) | BB(b1))) &&
                    !is_square_attacked(board, d1) &&
                    !is_square_attacked(board, c1)) {

                    add_move(board, MOVE(piece, c1, e1, QUIET | CASTLE_QUEEN), moves, count);
                }
            }
        } else {
            if (from == e8 && !is_square_attacked(board, e8)) {
                if ((rights & 0x4) &&
                    !(board->all_occupied & (BB(f8) | BB(g8))) &&
                    !is_square_attacked(board, f8) &&
                    !is_square_attacked(board, g8)) {

                    add_move(board, MOVE(piece, g8, e8, QUIET | CASTLE_KING), moves, count);
                }

                if ((rights & 0x8) &&
                    !(board->all_occupied & (BB(d8) | BB(c8) | BB(b8))) &&
                    !is_square_attacked(board, d8) &&
                    !is_square_attacked(board, c8)) {

                   add_move(board, MOVE(piece, c8, e8, QUIET | CASTLE_QUEEN), moves, count);
                }
            }
        }
    }
}

U8 generate_legal_moves(Board *board, U32 moves[]) {
    if (!board) return 0;
    U8 count = 0;

    generate_pawn_moves(board, moves, &count);
    generate_knight_moves(board, moves, &count);
    generate_bishop_moves(board, moves, &count);
    generate_rook_moves(board, moves, &count);
    generate_queen_moves(board, moves, &count);
    generate_king_moves(board, moves, &count);

    return count;
}
