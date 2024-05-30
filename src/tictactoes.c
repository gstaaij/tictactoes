#include <assert.h>
#include <math.h>

#include "raylib.h"
#include "raymath.h"
#include "colors.h"
#include "game.h"
#include "grid.h"
#include "cell.h"

// stb-style libraries at the end, because we don't want the IMPLEMENTATION macros to be defined in the other headers
#define ARENA_IMPLEMENTATION
#include "arena/arena.h"

static Arena arena = {0};

int main() {
    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_MSAA_4X_HINT);
    InitWindow(1280, 720, "TicTacToes");
    SetWindowState(FLAG_WINDOW_RESIZABLE);
    SetWindowMinSize(640, 480);

    // Initialize the game
    Game game = gridCreate(&arena, 2);
    GameState state = GAME_STATE_CROSS;

    while (!WindowShouldClose()) {
        BeginDrawing();
            ClearBackground(BACKGROUND_COLOR);
            DrawFPS(10, 10);

            float screenWidth = GetScreenWidth();
            float screenHeight = GetScreenHeight();
            float size = fminf(screenWidth, screenHeight);

            Rectangle rect = { screenWidth/2 - size/2, screenHeight/2 - size/2, size, size };

            game.draw(game.data, rect, FOREGROUND_COLOR);

            float offsetFromBorder = 10.0f;
            float currentPlayerSize = size / 10;
            if (currentPlayerSize > fabsf(screenWidth - screenHeight)) {
                currentPlayerSize = GRID_PADDING * size - offsetFromBorder;
            }
            float x = offsetFromBorder;
            float yw = screenWidth - offsetFromBorder - currentPlayerSize;
            float yh = screenHeight - offsetFromBorder - currentPlayerSize;
            cellDrawState(state, (Vector2) { x,  x  }, currentPlayerSize, FOREGROUND_COLOR);
            cellDrawState(state, (Vector2) { x,  yh }, currentPlayerSize, FOREGROUND_COLOR);
            cellDrawState(state, (Vector2) { yw, x  }, currentPlayerSize, FOREGROUND_COLOR);
            cellDrawState(state, (Vector2) { yw, yh }, currentPlayerSize, FOREGROUND_COLOR);

            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                Vector2 mousePos = GetMousePosition();

                if (mousePos.x >= rect.x && mousePos.x < rect.x + rect.width &&
                    mousePos.y >= rect.y && mousePos.y < rect.y + rect.height &&
                    game.clicked(game.data, state, mousePos).success
                ) {
                    state = -state;
                }
            }

            if (IsKeyPressed(KEY_R)) {
                arena_reset(&arena);
                game = gridCreate(&arena, 2);
                state = GAME_STATE_CROSS;
            }

        EndDrawing();
    }

    CloseWindow();
    arena_free(&arena);

    return 0;
}
