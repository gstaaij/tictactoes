#define NOB_IMPLEMENTATION
#include "src/nob.h"

#define COMPILE_FOR_WINDOWS

#define APP_NAME "tictactoes"

static const char* CFILES[] = {
    "src/"APP_NAME".c",
    "src/game.c",
    "src/cell.c",
    "src/grid.c",
};

void cflags(Nob_Cmd* cmd) {
    nob_cmd_append(cmd, "-Wall", "-Wextra", "-ggdb");
    nob_cmd_append(cmd, "-I./raylib/include");
    nob_cmd_append(cmd, "-o", "./build/"APP_NAME);
}

bool buildForWin(Nob_Cmd* cmd) {
    cmd->count = 0;
    nob_cmd_append(cmd, "x86_64-w64-mingw32-gcc");
    cflags(cmd);
    for (size_t i = 0; i < NOB_ARRAY_LEN(CFILES); ++i) {
        nob_cmd_append(cmd, CFILES[i]);
    }
    nob_cmd_append(cmd, "-L./raylib/windows", "-l:libraylib.a");
    nob_cmd_append(cmd, "-lwinmm", "-lgdi32");
    nob_cmd_append(cmd, "-static");
    return nob_cmd_run_sync(*cmd);
}

bool buildForLin(Nob_Cmd* cmd) {
    cmd->count = 0;
    nob_cmd_append(cmd, "gcc");
    cflags(cmd);
    for (size_t i = 0; i < NOB_ARRAY_LEN(CFILES); ++i) {
        nob_cmd_append(cmd, CFILES[i]);
    }
    nob_cmd_append(cmd, "-L./raylib/linux", "-l:libraylib.a");
    nob_cmd_append(cmd, "-lm");
    return nob_cmd_run_sync(*cmd);
}

int main(int argc, char* argv[]) {
    NOB_GO_REBUILD_URSELF(argc, argv);

    nob_mkdir_if_not_exists("./build");

    Nob_Cmd cmd = {0};
#ifdef COMPILE_FOR_WINDOWS
    if (!buildForWin(&cmd)) return 1;
#endif
    if (!buildForLin(&cmd)) return 1;

    return 0;
}
