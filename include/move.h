/**
 * move.h
 */

#pragma once

#include <types.h>
#include <board.h>

#define QUIET            0x000U
#define CAPTURE          0x001U
#define EN_PASSANT       0x002U
#define DOUBLE_PUSH      0x004U
#define PROMOTION_QUEEN  0x008U
#define PROMOTION_ROOK   0x010U
#define PROMOTION_BISHOP 0x020U
#define PROMOTION_KNIGHT 0x040U
#define CASTLE_KING      0x080U
#define CASTLE_QUEEN     0x100U

#define MOVE_TO(m)    ((U8)(((m)  >> 0)  & 0x03FU))
#define MOVE_FROM(m)  ((U8)(((m)  >> 6)  & 0x03FU))
#define MOVE_PIECE(m) ((U8)(((m)  >> 12) & 0x00FU))
#define MOVE_FLAGS(m) ((U32)(((m) >> 16) & 0x3FFU))

#define UNDO_CAPTURED(u) ((U8)(((u)  >> 0)  & 0x0FU))
#define UNDO_RIGHTS(u)   ((U8)(((u)  >> 4)  & 0x0FU))
#define UNDO_EP(u)       ((U8)(((u)  >> 8)  & 0x3FU))
#define UNDO_HALFMOVE(u) ((U8)(((u)  >> 14) & 0xFFU))
#define UNDO_MOVE(u)     ((U32)(((u) >> 22) & 0xFFFFFFFFu))

#define MOVE(piece, to, from, flags) \
    ((U32)( \
        ((U32)((to)    & 0x03FU) << 0)  | \
        ((U32)((from)  & 0x03FU) << 6)  | \
        ((U32)((piece) & 0x00FU) << 12) | \
        ((U32)((flags) & 0x3FFU) << 16)))

#define UNDO(move, captured, rights, ep, halfmove) \
    ((U64)( \
        ((U64)((captured) & 0x0FU)       << 0)  | \
        ((U64)((rights)   & 0x0FU)       << 4)  | \
        ((U64)((ep)       & 0x3FU)       << 8)  | \
        ((U64)((halfmove) & 0xFFU)       << 14) | \
        ((U64)((move)     & 0xFFFFFFFFu) << 22)))

U64 make_move(Board *board, U32 move);
void undo_move(Board *board, U64 undo);

U8 is_square_attacked(const Board *board, U8 sq);
