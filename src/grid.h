#pragma once

#include "raylib.h"
#include "arena/arena.h"
#include "game.h"

#define GRID_SIZE 3
#define GRID_PADDING 0.05
#define GRID_LINE_THICK 0.025

typedef struct {
    Game cells[GRID_SIZE][GRID_SIZE];
    GameState winner;
    Rectangle bounds;
} GridData;

Game gridCreate(Arena* arena, int depth);
