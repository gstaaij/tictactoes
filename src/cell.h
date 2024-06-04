#pragma once

#include "arena/arena.h"
#include "game.h"

#define CELL_PADDING 0.1
#define CELL_LINE_THICK 0.05

typedef struct {
    GameState state;
    int isHighlighted;
} CellData;

Game cellCreate(Arena* arena);

void cellDrawState(GameState state, Vector2 pos, float size, Color foregroundColor);
