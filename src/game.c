#include "game.h"

GameClickResult gameDummyClicked(void* data, GameState currentGameState, Vector2 mousePos) {
    (void) data;
    (void) currentGameState;
    (void) mousePos;
    return (GameClickResult){0};
}
