#pragma once

#include "raylib.h"

typedef enum {
    GAME_STATE_CROSS  = -1,
    GAME_STATE_EMPTY  =  0,
    GAME_STATE_CIRCLE =  1,
} GameState;

typedef struct {
    bool (*clicked)(void* data, GameState currentGameState, Vector2 mousePos);
    void (*draw)(void* data, Rectangle bounds);
    void* data;
} Game;

void gameDummyClicked(void* data, GameState currentGameState, Vector2 mousePos);
