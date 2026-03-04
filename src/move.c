#include <move.h>
#include <bitboard.h>
#include <masks.h>
#include <magic.h>

U64 make_move(Board *board, const U32 move) {
    if (!board) return 0;

    const U8  to    = MOVE_TO(move);
    const U8  from  = MOVE_FROM(move);
    const U8  piece = MOVE_PIECE(move);
    const U32 flags = MOVE_FLAGS(move);
    const U8  color = board->active_color;

    POP_BIT(board->bitboards[piece], from);
    SET_BIT(board->bitboards[piece], to);

    // Captures
    U8 captured = EMPTY;
    if (flags & EN_PASSANT) {
        const U8 cap_sq    = (color == WHITE) ? (to - 8) : (to + 8);
        const U8 cap_piece = (color == WHITE) ? BP : WP;
        captured = cap_piece;
        POP_BIT(board->bitboards[cap_piece], cap_sq);
    } else if (flags & CAPTURE) {
        const U8 start = (color == WHITE) ? BP : WP;
        const U8 end   = (color == WHITE) ? BK : WK;
        for (U8 enemy = start; enemy <= end; enemy++) {
            if (GET_BIT(board->bitboards[enemy], to)) {
                captured = enemy;
                POP_BIT(board->bitboards[enemy], to);
                break;
            }
        }
    }

    // Set Undo
    U64 undo = UNDO(
        move, 
        captured, 
        board->castling_rights, 
        board->en_passant_target, 
        board->halfmove_clock
    );

    // En Passant
    board->en_passant_target = SQ_NONE;
    if (flags & DOUBLE_PUSH) {
        board->en_passant_target = (color == WHITE) 
            ? (to - 8) 
            : (to + 8);
    }

    // Castling
    if (flags & (CASTLE_KING | CASTLE_QUEEN)) {
        const U8 rook = (color == WHITE) ? WR : BR;
        if (flags & CASTLE_KING) {
            const U8 r_from = (color == WHITE) ? h1 : h8;
            const U8 r_to   = (color == WHITE) ? f1 : f8;
            POP_BIT(board->bitboards[rook], r_from);
            SET_BIT(board->bitboards[rook], r_to);
        } else {
            const U8 r_from = (color == WHITE) ? a1 : a8;
            const U8 r_to   = (color == WHITE) ? d1 : d8;
            POP_BIT(board->bitboards[rook], r_from);
            SET_BIT(board->bitboards[rook], r_to);
        }
    }

    // Promotion
    if (flags & (PROMOTION_QUEEN | PROMOTION_ROOK | PROMOTION_BISHOP | PROMOTION_KNIGHT)) {
        POP_BIT(board->bitboards[piece], to);
        U8 promo = piece;
        if (flags & PROMOTION_QUEEN)      promo = (color == WHITE) ? WQ : BQ;
        else if (flags & PROMOTION_ROOK)  promo = (color == WHITE) ? WR : BR;
        else if (flags & PROMOTION_BISHOP)promo = (color == WHITE) ? WB : BB;
        else                              promo = (color == WHITE) ? WN : BN;
        SET_BIT(board->bitboards[promo], to);
    }

    // Castling Rights
    if (TYPE(piece) == KING) {
        board->castling_rights &= (color == WHITE) 
            ? (U8)~(WKCA | WQCA) 
            : (U8)~(BKCA | BQCA);
    }

    if (TYPE(piece) == ROOK) {
        if (from == h1) board->castling_rights &= (U8)~WKCA;
        else if (from == a1) board->castling_rights &= (U8)~WQCA;
        else if (from == h8) board->castling_rights &= (U8)~BKCA;
        else if (from == a8) board->castling_rights &= (U8)~BQCA;
    }

    if ((flags & CAPTURE) && captured != EMPTY && TYPE(captured) == ROOK) {
        if (to == h1) board->castling_rights &= (U8)~WKCA;
        else if (to == a1) board->castling_rights &= (U8)~WQCA;
        else if (to == h8) board->castling_rights &= (U8)~BKCA;
        else if (to == a8) board->castling_rights &= (U8)~BQCA;
    }

    if ((flags & (CAPTURE | EN_PASSANT)) || TYPE(piece) == PAWN) board->halfmove_clock = 0;
    else board->halfmove_clock++;

    board->fullmove_number += color;
    board->active_color ^= 1;

    update_occupied(board);
    return undo;
}

void undo_move(Board *board, U64 undo) {
    if (!board) return;

    const U32 move     = UNDO_MOVE(undo);
    const U8  captured = UNDO_CAPTURED(undo);
    const U8  rights   = UNDO_RIGHTS(undo);
    const U8  ep       = UNDO_EP(undo);
    const U8  hmc      = UNDO_HALFMOVE(undo);

    const U8  to       = MOVE_TO(move);
    const U8  from     = MOVE_FROM(move);
    const U8  piece    = MOVE_PIECE(move);
    const U32 flags    = MOVE_FLAGS(move);

    board->active_color ^= 1;
    const U8 color = board->active_color;

    board->castling_rights   = rights;
    board->en_passant_target = ep;
    board->halfmove_clock    = hmc;
    board->fullmove_number  -= color;

    // Castling
    if (flags & (CASTLE_KING | CASTLE_QUEEN)) {
        const U8 rook = (color == WHITE) ? WR : BR;
        if (flags & CASTLE_KING) {
            const U8 r_from = (color == WHITE) ? h1 : h8;
            const U8 r_to   = (color == WHITE) ? f1 : f8;
            POP_BIT(board->bitboards[rook], r_to);
            SET_BIT(board->bitboards[rook], r_from);
        } else {
            const U8 r_from = (color == WHITE) ? a1 : a8;
            const U8 r_to   = (color == WHITE) ? d1 : d8;
            POP_BIT(board->bitboards[rook], r_to);
            SET_BIT(board->bitboards[rook], r_from);
        }
    }

    // Promotion
    if (flags & (PROMOTION_QUEEN | PROMOTION_ROOK | PROMOTION_BISHOP | PROMOTION_KNIGHT)) {
        U8 promo;
        if (flags & PROMOTION_QUEEN)       promo = (color == WHITE) ? WQ : BQ;
        else if (flags & PROMOTION_ROOK)   promo = (color == WHITE) ? WR : BR;
        else if (flags & PROMOTION_BISHOP) promo = (color == WHITE) ? WB : BB;
        else                               promo = (color == WHITE) ? WN : BN;
        POP_BIT(board->bitboards[promo], to);
    }

    POP_BIT(board->bitboards[piece], to);
    SET_BIT(board->bitboards[piece], from);

    // Captures
    if (captured != EMPTY) {
        if (flags & EN_PASSANT) {
            const U8 cap_sq = (color == WHITE) 
                ? (to - 8) 
                : (to + 8);
            SET_BIT(board->bitboards[captured], cap_sq);
        } else if (flags & CAPTURE) {
            SET_BIT(board->bitboards[captured], to);
        }
    }

    update_occupied(board);
}

U8 is_square_attacked(const Board *board, U8 sq) {
    if (!board) return 0;

    const U8 attacker = board->active_color ^ 1;
    const U64 occupied = board->all_occupied;

    // Pawn
    if (pawn_masks[attacker ^ 1][sq] & board->bitboards[PIECE(PAWN, attacker)])
        return 1;

    // Knight
    if (knight_masks[sq] & board->bitboards[PIECE(KNIGHT, attacker)])
        return 1;

    // King
    if (king_masks[sq] & board->bitboards[PIECE(KING, attacker)])
        return 1;
    
    // Bishop & Queen
    const Magic *bmagic = &bishop_magics[sq];
    U64 battacks = get_magic_bitboard(bmagic, occupied);

    if (battacks & (
        board->bitboards[PIECE(BISHOP, attacker)] |
        board->bitboards[PIECE(QUEEN, attacker)]
    )) return 1;

    // Rook & Queen
    const Magic *rmagic = &rook_magics[sq];
    U64 rattacks = get_magic_bitboard(rmagic, occupied);

    if (rattacks & (
        board->bitboards[PIECE(ROOK, attacker)] |
        board->bitboards[PIECE(QUEEN, attacker)]
    ))return 1;

    return 0;
}
