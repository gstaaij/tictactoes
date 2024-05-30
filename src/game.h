#pragma once

#include <stdbool.h>
#include "raylib.h"

typedef enum {
    GAME_STATE_CROSS  = -1,
    GAME_STATE_EMPTY  =  0,
    GAME_STATE_CIRCLE =  1,
} GameState;

typedef struct {
    bool active;
    bool currentSquare;
    Vector2 square;
} GameSquarePos;

typedef struct {
    bool success;
    GameState winner;
    GameSquarePos squareWon;
} GameClickResult;

typedef struct {
    GameClickResult (*clicked)(void* data, GameState currentGameState, Vector2 mousePos);
    GameState (*getWinner)(void* data);
    void (*draw)(void* data, Rectangle bounds, Color foregroundColor);
    void* data;
} Game;

GameClickResult gameDummyClicked(void* data, GameState currentGameState, Vector2 mousePos);;
