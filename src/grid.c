#include "grid.h"
#include "raymath.h"
#include "cell.h"
#include "colors.h"

Rectangle gridCalculateCellBounds(int y, int x, Rectangle gridBounds) {
    assert(gridBounds.width == gridBounds.height);
    float size = gridBounds.width;
    float lineLen = size - GRID_PADDING * 2 * size;
    float cellSize = (lineLen - (GRID_SIZE  - 1) * GRID_LINE_THICK * size) / GRID_SIZE;
    return (Rectangle) {
        .x = gridBounds.x + GRID_PADDING * size + x * (cellSize + GRID_LINE_THICK * size),
        .y = gridBounds.y + GRID_PADDING * size + y * (cellSize + GRID_LINE_THICK * size),
        .width  = cellSize,
        .height = cellSize,
    };
}

bool gridClicked(void* raw_data, GameState gameState, Vector2 mousePos) {
    GridData* data = raw_data;
    for (int y = 0; y < GRID_SIZE; ++y) {
        for (int x = 0; x < GRID_SIZE; ++x) {
            Game cell = data->cells[y][x];
            Rectangle cellBounds = gridCalculateCellBounds(y, x, data->bounds);
            if (mousePos.x >= cellBounds.x && mousePos.x < cellBounds.x + cellBounds.width &&
                mousePos.y >= cellBounds.y && mousePos.y < cellBounds.y + cellBounds.height
            ) {
                return cell.clicked(cell.data, gameState, mousePos);
            }
        }
    }
    return false;
}

void gridDraw(void* raw_data, Rectangle bounds) {
    GridData* data = raw_data;
    data->bounds = bounds;

    assert(bounds.width == bounds.height);
    Vector2 pos = (Vector2) { bounds.x, bounds.y };
    float size = bounds.width;

    float lineLen = size - GRID_PADDING * 2 * size;
    float cellSize = (lineLen - (GRID_SIZE  - 1) * GRID_LINE_THICK * size) / GRID_SIZE;

    // Draw the vertical lines
    for (int vline = 1; vline < GRID_SIZE; ++vline) {
        Vector2 linePos1 = Vector2Add(pos, (Vector2) { GRID_PADDING * size + vline * (cellSize + GRID_LINE_THICK * size) - GRID_LINE_THICK * size / 2, GRID_PADDING * size });
        Vector2 linePos2 = Vector2Add(linePos1, (Vector2) { 0, lineLen });
        DrawLineEx(linePos1, linePos2, GRID_LINE_THICK * size, FOREGROUND_COLOR);
    }
    // Draw the horizontal lines
    for (int hline = 1; hline < GRID_SIZE; ++hline) {
        Vector2 linePos1 = Vector2Add(pos, (Vector2) { GRID_PADDING * size, GRID_PADDING * size + hline * (cellSize + GRID_LINE_THICK * size) - GRID_LINE_THICK * size / 2 });
        Vector2 linePos2 = Vector2Add(linePos1, (Vector2) { lineLen, 0 });
        DrawLineEx(linePos1, linePos2, GRID_LINE_THICK * size, FOREGROUND_COLOR);
    }

    // Draw the cells
    for (int y = 0; y < GRID_SIZE; ++y) {
        for (int x = 0; x < GRID_SIZE; ++x) {
            Game cell = data->cells[y][x];
            Rectangle cellBounds = gridCalculateCellBounds(y, x, bounds);
            cell.draw(cell.data, cellBounds);
        }
    }
}

Game gridCreate(Arena* arena, int layers) {
    GridData* data = arena_alloc(arena, sizeof(*data));
    memset(data, sizeof(*data), 1);
    for (int y = 0; y < GRID_SIZE; ++y) {
        for (int x = 0; x < GRID_SIZE; ++x) {
            if (layers <= 1)
                data->cells[y][x] = cellCreate(arena);
            else
                data->cells[y][x] = gridCreate(arena, layers - 1);
            // data->cells[y][x].clicked(data->cells[y][x].data, (y+x)%2 + 1, Vector2Zero());
        }
    }
    data->winner = GAME_STATE_EMPTY;

    return (Game) {
        .clicked = gridClicked,
        .draw = gridDraw,
        .data = data,
    };
}
