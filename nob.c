#define NOB_IMPLEMENTATION
#define NOB_WARN_DEPRECATED
#define NOB_EXPERIMENTAL_DELETE_OLD
#define NOB_UNSTRIP_PREFIX
#define NOB_TEMP_CAPACITY  (64 * 1024 * 1024)
#include "build/nob.h"
#undef NOB_IMPLEMENTATION


int
main(int argc, char** argv) {
    NOB_GO_REBUILD_URSELF(argc, argv);

    if (!nob_mkdir_if_not_exists("out")) {
        return 1;
    }

    if (!nob_mkdir_if_not_exists("out/build")) {
        return 1;
    }

    Nob_Cmd cmd = {};
    nob_cmd_append(&cmd, "clang");
    nob_cmd_append(&cmd, "-o", "out/build/build");
    nob_cmd_append(&cmd, "build/build.c");
    nob_cmd_append(&cmd, "-std=gnu2y");
    nob_cmd_append(&cmd, "-fdefer-ts");
    nob_cmd_append(&cmd, "-Wno-deprecated-octal-literals");
    if (!nob_cmd_run(&cmd)) {
        return 1;
    }

    nob_cmd_append(&cmd, "out/build/build");
    if (!nob_cmd_run(&cmd)) {
        return 1;
    }

    return 0;
}
