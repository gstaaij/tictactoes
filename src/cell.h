#pragma once

#include "arena/arena.h"
#include "game.h"

#define CELL_PADDING 0.1
#define CELL_LINE_THICK 0.05

typedef struct {
    GameState state;
} CellData;

Game cellCreate(Arena* arena);
