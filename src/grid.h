#pragma once

#include "raylib.h"
#include "arena/arena.h"
#include "game.h"

#define GRID_SIZE 3
#define GRID_PADDING 0.05
#define GRID_LINE_THICK 0.025

typedef struct {
    Game cells[GRID_SIZE][GRID_SIZE];
    bool allowEverything;
    bool allowedCells[GRID_SIZE][GRID_SIZE];
    float allowShowTimer;
    GameState winner;
    Rectangle bounds;
    int layer;
} GridData;

Game gridCreate(Arena* arena, int layers);
