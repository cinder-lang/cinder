#define NOB_IMPLEMENTATION
#define NOB_WARN_DEPRECATED
#define NOB_EXPERIMENTAL_DELETE_OLD
#define NOB_UNSTRIP_PREFIX
#define NOB_TEMP_CAPACITY  (64 * 1024 * 1024)
#include "nob.h"
#undef NOB_IMPLEMENTATION

#include <stdbool.h>
#include <stddefer.h>

#include <libgen.h>

typedef struct {
    const char **items;
    size_t count;
    size_t capacity;
} CStrings;

static struct {
    const char* llvm_root_dir_path;
    const char* llvm_bin_dir_path;
} _bLLVMConfig = {};


static CStrings
_bQueryLLVMConfig(Nob_Cmd* cmd) {
    CStrings result = {};

    if (!nob_cmd_run(cmd, .stdout_path = "llvm-config-out.txt")) {
        return result;
    }
    defer remove("llvm-config-out.txt");

    Nob_String_Builder contents = {};
    if (!nob_read_entire_file("llvm-config-out.txt", &contents)) {
        return result;
    }

    for (size_t i = 0; i < contents.count; i++) {
        if (contents.items[i] == '\n') {
            contents.items[i] = ' ';
        }
    }

    Nob_String_View sv = nob_sb_to_sv(contents);
    while (sv = nob_sv_trim_left(sv), sv.count > 0) {
        Nob_String_View item = nob_sv_chop_by_delim(&sv, ' ');
        item = nob_sv_trim_right(item);
        const char* item_str = nob_temp_strndup(item.data, item.count);
        nob_da_append(&result, item_str);
    }

    return result;
}

static void
_bAppendLLVMCFlags(Nob_Cmd* cmd) {
    Nob_Cmd config_cmd = {};
    nob_cmd_append(&config_cmd, nob_temp_sprintf("%s/llvm-config", _bLLVMConfig.llvm_bin_dir_path));
    nob_cmd_append(&config_cmd, "--cflags");
    CStrings cflags = _bQueryLLVMConfig(&config_cmd);
    defer nob_da_free(cflags);
    nob_da_append_many(cmd, cflags.items, cflags.count);
}

static void
_bAppendLLVMLdFlags(Nob_Cmd* cmd) {
    Nob_Cmd config_cmd = {};
    nob_cmd_append(&config_cmd, nob_temp_sprintf("%s/llvm-config", _bLLVMConfig.llvm_bin_dir_path));
    nob_cmd_append(&config_cmd, "--ldflags");
    nob_cmd_append(&config_cmd, "--libs");
    nob_cmd_append(&config_cmd, "--system-libs");
    CStrings cflags = _bQueryLLVMConfig(&config_cmd);
    defer nob_da_free(cflags);
    nob_da_append_many(cmd, cflags.items, cflags.count);
    nob_da_append(cmd, "-lstdc++");
}


typedef struct {
    const char* ext;
    Nob_File_Paths* out_file_paths;
} bReadDirRecursiveExtArgs;

static bool
_bReadDirRecursiveExtWalkFunc(Nob_Walk_Entry entry) {
    bReadDirRecursiveExtArgs* args = entry.data;

    const char* ext = args->ext;
    Nob_File_Paths* out_file_paths = args->out_file_paths;

    size_t ext_len = strlen(ext);

    if (entry.type == NOB_FILE_REGULAR) {
        size_t path_len = strlen(entry.path);
        if (path_len < ext_len) {
            return true;
        }

        if (0 != strncmp(entry.path + path_len - ext_len, ext, ext_len)) {
            return true;
        }

        nob_da_append(out_file_paths, nob_temp_strdup(entry.path));
    }

    return true;
}

static bool
_bReadDirRecursiveExt(
    const char* dir_path,
    const char* ext,
    Nob_File_Paths* out_file_paths
) {
    bReadDirRecursiveExtArgs args = {
        .ext = ext,
        .out_file_paths = out_file_paths,
    };
    return nob_walk_dir(dir_path, _bReadDirRecursiveExtWalkFunc, .data = &args);
}


static bool
_bCompileObject(
    const char* target,
    const char* source,
    Nob_File_Paths headers
) {
    Nob_Cmd cmd = {};
    nob_cmd_append(&cmd, "clang");
    nob_cmd_append(&cmd, "-o", target);
    nob_cmd_append(&cmd, "-c", source);
    nob_cmd_append(&cmd, "-Iinclude");
    _bAppendLLVMCFlags(&cmd);
    nob_cmd_append(&cmd, "-std=c2y");
    nob_cmd_append(&cmd, "-fdefer-ts");
    nob_cmd_append(&cmd, "-Wall");
    nob_cmd_append(&cmd, "-Werror");
    nob_cmd_append(&cmd, "-pedantic-errors");
    nob_cmd_append(&cmd, "-fsanitize=address,undefined,integer");
    nob_cmd_append(&cmd, "-ggdb");
    if (!nob_cmd_run(&cmd)) {
        return false;
    }

    return true;
}


static bool
_bFindLLVM() {
    _bLLVMConfig.llvm_root_dir_path = "/usr/local/llvm-23.0.0";
    _bLLVMConfig.llvm_bin_dir_path = "/usr/local/llvm-23.0.0/bin";

    return true;
}


int
main(int argc, char** argv) {
    Nob_Cmd cmd = {};

    if (!_bFindLLVM()) {
        return 1;
    }

    const char* public_dir_path = "include";
    const char* private_dir_path = "lib";
    const char* entries_dir_path = "src";

    Nob_File_Paths public_headers = {};
    Nob_File_Paths library_headers = {};
    Nob_File_Paths library_sources = {};

    if (!nob_mkdir_if_not_exists("out/lib")) {
        return 1;
    }

    if (!nob_mkdir_if_not_exists("out/src")) {
        return 1;
    }

    if (!_bReadDirRecursiveExt(public_dir_path, ".h", &public_headers)) {
        return 1;
    }

    nob_da_append_many(&library_headers, public_headers.items, public_headers.count);
    if (!_bReadDirRecursiveExt(private_dir_path, ".h", &library_headers)) {
        return 1;
    }

    if (!_bReadDirRecursiveExt(private_dir_path, ".c", &library_sources)) {
        return 1;
    }

    Nob_File_Paths library_object_file_paths = {};
    for (size_t i = 0; i < library_sources.count; i++) {
        const char* source_file_path = library_sources.items[i];

        char* source_file_path2 = nob_temp_strdup(library_sources.items[i]);
        const char* source_name = basename(source_file_path2);
        const char* module_name = dirname(source_file_path2 + strlen(private_dir_path) + 1);
        if (!nob_mkdir_if_not_exists(nob_temp_sprintf("out/lib/%s", module_name))) {
            return 1;
        }

        const char* object_name = nob_temp_sprintf("%.*s.o", (int) strlen(source_name) - 2, source_name);
        const char* object_file_path = nob_temp_sprintf("out/lib/%s/%s", module_name, object_name);
        if (!_bCompileObject(object_file_path, source_file_path, library_headers)) {
            return 1;
        }

        nob_da_append(&library_object_file_paths, object_file_path);
    }

#if defined(_WIN32)
    const char* library_file_path = "out/libcinder.lib";
    nob_cmd_append(&cmd, "llvm-lib", "/NOLOGO");
    nob_cmd_append(&cmd, "/OUT", library_file_path);
#else
    const char* library_file_path = "out/libcinder.a";
    nob_cmd_append(&cmd, "ar", "rcs");
    nob_cmd_append(&cmd, library_file_path);
#endif
    nob_da_append_many(&cmd, library_object_file_paths.items, library_object_file_paths.count);
    if (!nob_cmd_run(&cmd)) {
        return 1;
    }

    Nob_File_Paths source_dir_names = {};
    if (!nob_read_entire_dir(entries_dir_path, &source_dir_names)) {
        return 1;
    }

    if (!nob_mkdir_if_not_exists("out/bin")) {
        return 1;
    }

    for (size_t i = 0; i < source_dir_names.count; i++) {
        const char* source_dir_name = source_dir_names.items[i];
        if (*source_dir_name == '.') continue;
        const char* source_dir_path = nob_temp_sprintf("%s/%s", entries_dir_path, source_dir_name);
        const char* output_dir_path = nob_temp_sprintf("out/src/%s", source_dir_name);
        if (!nob_mkdir_if_not_exists(output_dir_path)) {
            return 1;
        }

        Nob_File_Paths entry_source_file_paths = {};
        if (!_bReadDirRecursiveExt(source_dir_path, ".c", &entry_source_file_paths)) {
            return 1;
        }

        if (entry_source_file_paths.count == 0) {
            continue;
        }

        Nob_File_Paths entry_header_file_paths = {};
        if (!_bReadDirRecursiveExt(source_dir_path, ".c", &entry_header_file_paths)) {
            return 1;
        }

        nob_da_append_many(&entry_header_file_paths, public_headers.items, public_headers.count);

        Nob_File_Paths entry_object_file_paths = {};
        for (size_t i = 0; i < entry_source_file_paths.count; i++) {
            const char* source_file_path = entry_source_file_paths.items[i];
            char* source_file_path2 = nob_temp_strdup(source_file_path);
            const char* source_name = basename(source_file_path2);

            const char* object_name = nob_temp_sprintf("%.*s.o", (int) strlen(source_name) - 2, source_name);
            const char* object_file_path = nob_temp_sprintf("out/src/%s/%s", source_dir_name, object_name);
            if (!_bCompileObject(object_file_path, source_file_path, library_headers)) {
                return 1;
            }

            nob_da_append(&entry_object_file_paths, object_file_path);
        }

        nob_cmd_append(&cmd, "clang");
        nob_cmd_append(&cmd, "-o", nob_temp_sprintf("out/bin/%s", source_dir_name));
        nob_da_append_many(&cmd, entry_object_file_paths.items, entry_object_file_paths.count);
        nob_cmd_append(&cmd, "-fsanitize=address,undefined,integer");
        _bAppendLLVMLdFlags(&cmd);
        nob_cmd_append(&cmd, library_file_path);
        if (!nob_cmd_run(&cmd)) {
            return 1;
        }
    }

    return 0;
}
