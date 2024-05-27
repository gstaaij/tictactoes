#include <assert.h>
#include <math.h>

#include "raylib.h"
#include "raymath.h"
#define ARENA_IMPLEMENTATION
#include "arena/arena.h"

#define BACKGROUND_COLOR RAYWHITE

static Arena arena = {0};

// --------------------

typedef enum {
    GAME_STATE_EMPTY = 0,
    GAME_STATE_CROSS,
    GAME_STATE_CIRCLE,
} GameState;

typedef struct {
    void (*clicked)(void* data, GameState currentGameState, Vector2 mousePos);
    void (*draw)(void* data, Rectangle bounds);
    void* data;
} Game;

void gameDummyClicked(void* data, Vector2 mousePos) {
    (void) data;
    (void) mousePos;
}

// --------------------

typedef struct {
    GameState state;
} CellData;

#define CELL_PADDING 0.1
#define LINE_THICK 0.05

void cellClicked(void* raw_data, GameState gameState, Vector2 mousePos) {
    (void) mousePos;
    CellData* data = raw_data;
    if (!data->state)
        data->state = gameState;
}

void cellDraw(void* raw_data, Rectangle bounds) {
    CellData* data = raw_data;

    assert(bounds.width == bounds.height);
    Vector2 pos = (Vector2) { bounds.x, bounds.y };
    float size = bounds.width;

    switch (data->state) {
        case GAME_STATE_EMPTY: {} break;
        case GAME_STATE_CROSS: {
            Vector2 pos1 = Vector2AddValue(pos,        CELL_PADDING * size);
            Vector2 pos2 = Vector2AddValue(pos, size - CELL_PADDING * size);
            DrawLineEx(pos1, pos2, LINE_THICK * size, BLACK);
            pos1 = Vector2Add(pos, (Vector2) { size - CELL_PADDING * size,        CELL_PADDING * size });
            pos2 = Vector2Add(pos, (Vector2) {      + CELL_PADDING * size, size - CELL_PADDING * size });
            DrawLineEx(pos1, pos2, LINE_THICK * size, BLACK);
        } break;
        case GAME_STATE_CIRCLE: {
            pos = Vector2AddValue(pos, size/2);
            DrawCircleV(pos, size / 2 - CELL_PADDING * size, BLACK);
            DrawCircleV(pos, size / 2 - CELL_PADDING * size - LINE_THICK * size, BACKGROUND_COLOR);
        } break;
    }
}

Game cellCreate() {
    CellData* data = arena_alloc(&arena, sizeof(*data));
    data->state = GAME_STATE_EMPTY;

    return (Game) {
        .clicked = cellClicked,
        .draw = cellDraw,
        .data = data,
    };
}

// --------------------

int main() {
    InitWindow(1280, 720, "TicTacToe Ultimate");
    SetWindowState(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);

    Game game = cellCreate();

    while (!WindowShouldClose()) {
        BeginDrawing();
            ClearBackground(BACKGROUND_COLOR);
            DrawFPS(10, 10);

            float screenWidth = GetScreenWidth();
            float screenHeight = GetScreenHeight();
            float size = fminf(screenWidth, screenHeight);

            Rectangle rect = { screenWidth/2 - size/2, screenHeight/2 - size/2, size, size };

            game.draw(game.data, rect);

            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                Vector2 mousePos = GetMousePosition();

                if (mousePos.x >= rect.x && mousePos.x < rect.x + rect.width &&
                    mousePos.y >= rect.y && mousePos.y < rect.y + rect.height
                ) {
                    game.clicked(game.data, GAME_STATE_CROSS, mousePos);
                }
            }

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
