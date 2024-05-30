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

#define SCROLL_ZOOM_SPEED 7.5f
#define KEY_ZOOM_SPEED 1.5f
#define KEY_MOVE_SPEED 750.0f

static Arena arena = {0};

int main() {
    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_MSAA_4X_HINT);
    InitWindow(1280, 720, "TicTacToes");
    SetWindowState(FLAG_WINDOW_RESIZABLE);
    SetWindowMinSize(640, 480);

    // Initialize the game
    Game game = gridCreate(&arena, 2);
    GameState state = GAME_STATE_CROSS;
    Vector2 cameraPos = {0};
    float scaleExp = 0.0f;

    while (!WindowShouldClose()) {
        BeginDrawing();
            ClearBackground(BACKGROUND_COLOR);

            float screenWidth = GetScreenWidth();
            float screenHeight = GetScreenHeight();
            float size = fminf(screenWidth, screenHeight);

            Rectangle rect = { -size/2, -size/2, size, size };

            Camera2D camera = {
                .offset = { screenWidth/2, screenHeight/2 },
                .target = cameraPos,
                .zoom = powf(2, scaleExp),
                .rotation = 0.0f,
            };

            BeginMode2D(camera);
                game.draw(game.data, rect, FOREGROUND_COLOR);
            EndMode2D();

            Vector2 offset = Vector2Transform(Vector2Zero(), GetCameraMatrix2D(camera));

            float offsetFromBorder = 5.0f;
            float currentPlayerSize = size / 10;
            if (currentPlayerSize > fabsf(screenWidth - screenHeight)) {
                currentPlayerSize = GRID_PADDING * size - offsetFromBorder;
            }
            float x = offsetFromBorder;
            float yw = screenWidth - offsetFromBorder - currentPlayerSize;
            float yh = screenHeight - offsetFromBorder - currentPlayerSize;
            // Top left
            DrawRectangleV(      (Vector2) { x,  x  }, (Vector2) {currentPlayerSize, currentPlayerSize}, BACKGROUND_COLOR);
            cellDrawState(state, (Vector2) { x,  x  }, currentPlayerSize, FOREGROUND_COLOR);
            // Bottom left
            DrawRectangleV(      (Vector2) { x,  yh }, (Vector2) {currentPlayerSize, currentPlayerSize}, BACKGROUND_COLOR);
            cellDrawState(state, (Vector2) { x,  yh }, currentPlayerSize, FOREGROUND_COLOR);
            // Top right
            DrawRectangleV(      (Vector2) { yw, x  }, (Vector2) {currentPlayerSize, currentPlayerSize}, BACKGROUND_COLOR);
            cellDrawState(state, (Vector2) { yw, x  }, currentPlayerSize, FOREGROUND_COLOR);
            // Bottom right
            DrawRectangleV(      (Vector2) { yw, yh }, (Vector2) {currentPlayerSize, currentPlayerSize}, BACKGROUND_COLOR);
            cellDrawState(state, (Vector2) { yw, yh }, currentPlayerSize, FOREGROUND_COLOR);

            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                Vector2 mousePos = Vector2Scale(Vector2Subtract(GetMousePosition(), offset), 1 / camera.zoom);

                if (mousePos.x >= rect.x && mousePos.x < rect.x + rect.width &&
                    mousePos.y >= rect.y && mousePos.y < rect.y + rect.height &&
                    game.clicked(game.data, state, mousePos).success
                ) {
                    state = -state;
                }
            }

            if (IsMouseButtonDown(MOUSE_BUTTON_MIDDLE) || IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
                Vector2 mouseMove = GetMouseDelta();
                cameraPos = Vector2Subtract(cameraPos, Vector2Scale(mouseMove, 1.0f / camera.zoom));
            }

            scaleExp += GetMouseWheelMove() * SCROLL_ZOOM_SPEED * GetFrameTime();

            if (IsKeyDown(KEY_MINUS)) {
                scaleExp -= KEY_ZOOM_SPEED * GetFrameTime();
            }
            if (IsKeyDown(KEY_EQUAL)) {
                scaleExp += KEY_ZOOM_SPEED * GetFrameTime();
            }

            int leftright = IsKeyDown(KEY_RIGHT) - IsKeyDown(KEY_LEFT);
            int updown = IsKeyDown(KEY_DOWN) - IsKeyDown(KEY_UP);
            cameraPos.x += leftright / camera.zoom * KEY_MOVE_SPEED * GetFrameTime();
            cameraPos.y += updown / camera.zoom * KEY_MOVE_SPEED * GetFrameTime();

            if (IsKeyPressed(KEY_R)) {
                arena_reset(&arena);
                game = gridCreate(&arena, 2);
                state = GAME_STATE_CROSS;
                cameraPos = (Vector2){0};
                scaleExp = 0.0f;
            }

            DrawFPS(10, currentPlayerSize + 10);
        EndDrawing();
    }

    CloseWindow();
    arena_free(&arena);

    return 0;
}
