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

void gameDummyClicked(void* data, GameState currentGameState, Vector2 mousePos) {
    (void) data;
    (void) currentGameState;
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
    memset(data, sizeof(*data), 1);
    data->state = GAME_STATE_EMPTY;

    return (Game) {
        .clicked = cellClicked,
        .draw = cellDraw,
        .data = data,
    };
}

// --------------------

#define GRID_SIZE 3
#define GRID_PADDING 0.05
#define GRID_LINE_THICK 0.025

typedef struct {
    Game cells[GRID_SIZE][GRID_SIZE];
    GameState winner;
} GridData;

void gridDraw(void* raw_data, Rectangle bounds) {
    GridData* data = raw_data;

    assert(bounds.width == bounds.height);
    Vector2 pos = (Vector2) { bounds.x, bounds.y };
    float size = bounds.width;

    float lineLen = size - GRID_PADDING * 2 * size;
    float cellSize = (lineLen - (GRID_SIZE  - 1) * LINE_THICK * size) / GRID_SIZE;

    // Draw the vertical lines
    for (int vline = 1; vline < GRID_SIZE; ++vline) {
        Vector2 linePos1 = Vector2Add(pos, (Vector2) { GRID_PADDING * size + vline * (cellSize + LINE_THICK * size) - LINE_THICK * size / 2, GRID_PADDING * size });
        Vector2 linePos2 = Vector2Add(linePos1, (Vector2) { 0, lineLen });
        DrawLineEx(linePos1, linePos2, LINE_THICK * size, BLACK);
    }
    // Draw the horizontal lines
    for (int hline = 1; hline < GRID_SIZE; ++hline) {
        Vector2 linePos1 = Vector2Add(pos, (Vector2) { GRID_PADDING * size, GRID_PADDING * size + hline * (cellSize + LINE_THICK * size) - LINE_THICK * size / 2 });
        Vector2 linePos2 = Vector2Add(linePos1, (Vector2) { lineLen, 0 });
        DrawLineEx(linePos1, linePos2, LINE_THICK * size, BLACK);
    }

    // Draw the cells
    for (int y = 0; y < GRID_SIZE; ++y) {
        for (int x = 0; x < GRID_SIZE; ++x) {
            Game cell = data->cells[y][x];
            Rectangle cellBounds = {
                .x = bounds.x + GRID_PADDING * size + x * (cellSize + LINE_THICK * size),
                .y = bounds.y + GRID_PADDING * size + y * (cellSize + LINE_THICK * size),
                .width  = cellSize,
                .height = cellSize,
            };
            cell.draw(cell.data, cellBounds);
        }
    }
}

Game gridCreate() {
    GridData* data = arena_alloc(&arena, sizeof(*data));
    memset(data, sizeof(*data), 1);
    for (int y = 0; y < GRID_SIZE; ++y) {
        for (int x = 0; x < GRID_SIZE; ++x) {
            data->cells[y][x] = cellCreate();
            data->cells[y][x].clicked(data->cells[y][x].data, (y+x)%2 + 1, Vector2Zero());
        }
    }
    data->winner = GAME_STATE_EMPTY;

    return (Game) {
        .clicked = gameDummyClicked,
        .draw = gridDraw,
        .data = data,
    };
}

// --------------------

int main() {
    InitWindow(1280, 720, "TicTacToe Ultimate");
    SetWindowState(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
    SetWindowMinSize(640, 480);

    Game game = gridCreate();

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
    arena_free(&arena);

    return 0;
}
