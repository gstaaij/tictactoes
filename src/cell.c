#include "cell.h"
#include "raymath.h"
#include "colors.h"

GameState cellGetWinner(void* raw_data) {
    CellData* data = raw_data;
    return data->state;
}

GameClickResult cellClicked(void* raw_data, GameState gameState, Vector2 mousePos) {
    (void) mousePos;
    CellData* data = raw_data;
    if (!data->state) {
        data->state = gameState;
        GameClickResult result = {0};
        result.success = true;
        result.winner = data->state;
        return result;
    }
    return (GameClickResult){0};
}

void cellDraw(void* raw_data, Rectangle bounds, Color foregroundColor) {
    CellData* data = raw_data;

    assert(bounds.width == bounds.height);
    Vector2 pos = (Vector2) { bounds.x, bounds.y };
    float size = bounds.width;

    cellDrawState(data->state, pos, size, foregroundColor);
}

void cellDrawState(GameState state, Vector2 pos, float size, Color foregroundColor) {
    switch (state) {
        case GAME_STATE_EMPTY: {} break;
        case GAME_STATE_CROSS: {
            Vector2 pos1 = Vector2AddValue(pos,        CELL_PADDING * size);
            Vector2 pos2 = Vector2AddValue(pos, size - CELL_PADDING * size);
            DrawLineEx(pos1, pos2, CELL_LINE_THICK * size, foregroundColor);
            pos1 = Vector2Add(pos, (Vector2) { size - CELL_PADDING * size,        CELL_PADDING * size });
            pos2 = Vector2Add(pos, (Vector2) {      + CELL_PADDING * size, size - CELL_PADDING * size });
            DrawLineEx(pos1, pos2, CELL_LINE_THICK * size, foregroundColor);
        } break;
        case GAME_STATE_CIRCLE: {
            pos = Vector2AddValue(pos, size/2);
            DrawRing(pos, size / 2 - CELL_PADDING * size - CELL_LINE_THICK * size, size / 2 - CELL_PADDING * size, 0.0f, 360.0f, 72, foregroundColor);
        } break;
    }
}

Game cellCreate(Arena* arena) {
    CellData* data = arena_alloc(arena, sizeof(*data));
    memset(data, sizeof(*data), 1);
    data->state = GAME_STATE_EMPTY;

    return (Game) {
        .getWinner = cellGetWinner,
        .clicked = cellClicked,
        .draw = cellDraw,
        .data = data,
    };
}
