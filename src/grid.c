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

void gridCalculateWinner(GridData* data) {
    bool diagonalWin = true;
    GameState firstDiagonalSquare = data->cells[0][0].getWinner(data->cells[0][0].data);
    if (firstDiagonalSquare == GAME_STATE_EMPTY)
        diagonalWin = false;
    bool otherDiagonalWin = true;
    GameState firstOtherDiagonalSquare = data->cells[0][GRID_SIZE].getWinner(data->cells[0][0].data);
    if (firstOtherDiagonalSquare == GAME_STATE_EMPTY)
        otherDiagonalWin = false;
    for (int i = 0; i < GRID_SIZE; ++i) {
        bool horizontalWin = true;
        GameState firstHorizontalSquare = data->cells[i][0].getWinner(data->cells[i][0].data);
        if (firstHorizontalSquare == GAME_STATE_EMPTY)
            horizontalWin = false;
        for (int x = 0; x < GRID_SIZE; ++x) {
            if (firstHorizontalSquare != data->cells[i][x].getWinner(data->cells[i][x].data)) {
                horizontalWin = false;
                break;
            }
        }
        if (horizontalWin) {
            data->winner = firstHorizontalSquare;
            return;
        }

        bool verticalWin = true;
        GameState firstVerticalSquare = data->cells[0][i].getWinner(data->cells[0][i].data);
        if (firstVerticalSquare == GAME_STATE_EMPTY)
            verticalWin = false;
        for (int y = 0; y < GRID_SIZE; ++y) {
            if (firstVerticalSquare != data->cells[y][i].getWinner(data->cells[y][i].data)) {
                verticalWin = false;
                break;
            }
        }
        if (verticalWin) {
            data->winner = firstVerticalSquare;
            return;
        }

        if (firstDiagonalSquare != data->cells[i][i].getWinner(data->cells[i][i].data))
            diagonalWin = false;
        
        if (firstOtherDiagonalSquare != data->cells[i][GRID_SIZE-i-1].getWinner(data->cells[i][GRID_SIZE-i-1].data))
            otherDiagonalWin = false;
    }
    if (diagonalWin) {
        data->winner = firstDiagonalSquare;
    } else if (otherDiagonalWin) {
        data->winner = firstOtherDiagonalSquare;
    }
}

GameState gridGetWinner(void* raw_data) {
    GridData* data = raw_data;
    return data->winner;
}

GameClickResult gridClicked(void* raw_data, GameState gameState, Vector2 mousePos) {
    GridData* data = raw_data;
    if (data->winner)
        goto fail;
    for (int y = 0; y < GRID_SIZE; ++y) {
        for (int x = 0; x < GRID_SIZE; ++x) {
            Game cell = data->cells[y][x];
            Rectangle cellBounds = gridCalculateCellBounds(y, x, data->bounds);
            if (mousePos.x >= cellBounds.x && mousePos.x < cellBounds.x + cellBounds.width &&
                mousePos.y >= cellBounds.y && mousePos.y < cellBounds.y + cellBounds.height
            ) {
                if (!data->allowedCells[y][x]) {
                    data->allowShowTimer = 2.0;
                    continue;
                }
                GameClickResult cellResult = cell.clicked(cell.data, gameState, mousePos);
                if (!cellResult.success)
                    goto fail;
                
                GameClickResult result = {0};
                result.success = cellResult.success;

                result.squareWon.active = true;
                result.squareWon.square.x = x;
                result.squareWon.square.y = y;

                gridCalculateWinner(data);
                result.winner = data->winner;
                if (result.winner) {
                    // We don't need to mess with data->allowedCells anymore, because this grid is won already
                    return result;
                }

                if (cellResult.squareWon.active) {
                    Vector2 pos = cellResult.squareWon.square;
                    Game cellThatHasWon = data->cells[(int)pos.y][(int)pos.x];
                    data->allowEverything = cellThatHasWon.getWinner(cellThatHasWon.data) != GAME_STATE_EMPTY;
                    for (int j = 0; j < GRID_SIZE; ++j) {
                        for (int i = 0; i < GRID_SIZE; ++i) {
                            data->allowedCells[j][i] = data->allowEverything || (i == (int)pos.x && j == (int)pos.y);
                        }
                    }
                }

                return result;
            }
        }
    }
fail:
    return (GameClickResult){0};
}

void gridDraw(void* raw_data, Rectangle bounds, Color foregroundColor) {
    GridData* data = raw_data;
    data->bounds = bounds;

    assert(bounds.width == bounds.height);
    Vector2 pos = (Vector2) { bounds.x, bounds.y };
    float size = bounds.width;

    float lineLen = size - GRID_PADDING * 2 * size;
    float cellSize = (lineLen - (GRID_SIZE  - 1) * GRID_LINE_THICK * size) / GRID_SIZE;

    Color color = foregroundColor;
    if (data->winner) {
        color.a /= 2;
    }
    // Draw the vertical lines
    for (int vline = 1; vline < GRID_SIZE; ++vline) {
        Vector2 linePos1 = Vector2Add(pos, (Vector2) { GRID_PADDING * size + vline * (cellSize + GRID_LINE_THICK * size) - GRID_LINE_THICK * size / 2, GRID_PADDING * size });
        Vector2 linePos2 = Vector2Add(linePos1, (Vector2) { 0, lineLen });
        DrawLineEx(linePos1, linePos2, GRID_LINE_THICK * size, color);
    }
    // Draw the horizontal lines
    for (int hline = 1; hline < GRID_SIZE; ++hline) {
        Vector2 linePos1 = Vector2Add(pos, (Vector2) { GRID_PADDING * size, GRID_PADDING * size + hline * (cellSize + GRID_LINE_THICK * size) - GRID_LINE_THICK * size / 2 });
        Vector2 linePos2 = Vector2Add(linePos1, (Vector2) { lineLen, 0 });
        DrawLineEx(linePos1, linePos2, GRID_LINE_THICK * size, color);
    }

    // Draw the cells
    for (int y = 0; y < GRID_SIZE; ++y) {
        for (int x = 0; x < GRID_SIZE; ++x) {
            Game cell = data->cells[y][x];
            Rectangle cellBounds = gridCalculateCellBounds(y, x, bounds);
            if (!data->allowEverything && data->allowShowTimer > 0.0f && data->allowedCells[y][x]) {
                data->allowShowTimer -= GetFrameTime();
                if ((int)(data->allowShowTimer * 2) % 2 == 1)
                    DrawRectangleLinesEx(cellBounds, GRID_PADDING * cellBounds.width, GREEN);
            }
            cell.draw(cell.data, cellBounds, color);
        }
    }

    if (data->winner) {
        cellDrawState(data->winner, pos, size, foregroundColor);
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
            data->allowedCells[y][x] = true;
            // data->cells[y][x].clicked(data->cells[y][x].data, (y+x)%2 + 1, Vector2Zero());
        }
    }
    data->allowEverything = true;
    data->winner = GAME_STATE_EMPTY;

    return (Game) {
        .getWinner = gridGetWinner,
        .clicked = gridClicked,
        .draw = gridDraw,
        .data = data,
    };
}
