#include "cell.h"
#include "raymath.h"
#include "colors.h"

bool cellClicked(void* raw_data, GameState gameState, Vector2 mousePos) {
    (void) mousePos;
    CellData* data = raw_data;
    if (!data->state) {
        data->state = gameState;
        return true;
    }
    return false;
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
            DrawLineEx(pos1, pos2, CELL_LINE_THICK * size, FOREGROUND_COLOR);
            pos1 = Vector2Add(pos, (Vector2) { size - CELL_PADDING * size,        CELL_PADDING * size });
            pos2 = Vector2Add(pos, (Vector2) {      + CELL_PADDING * size, size - CELL_PADDING * size });
            DrawLineEx(pos1, pos2, CELL_LINE_THICK * size, FOREGROUND_COLOR);
        } break;
        case GAME_STATE_CIRCLE: {
            pos = Vector2AddValue(pos, size/2);
            DrawCircleV(pos, size / 2 - CELL_PADDING * size, FOREGROUND_COLOR);
            DrawCircleV(pos, size / 2 - CELL_PADDING * size - CELL_LINE_THICK * size, BACKGROUND_COLOR);
        } break;
    }
}

Game cellCreate(Arena* arena) {
    CellData* data = arena_alloc(arena, sizeof(*data));
    memset(data, sizeof(*data), 1);
    data->state = GAME_STATE_EMPTY;

    return (Game) {
        .clicked = cellClicked,
        .draw = cellDraw,
        .data = data,
    };
}
