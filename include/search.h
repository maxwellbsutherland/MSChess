#pragma once

#include <board.h>

#define INF 1000000

typedef int (*Eval)(const Board *board);

U32 search(Board *board, Eval eval, int depth);
