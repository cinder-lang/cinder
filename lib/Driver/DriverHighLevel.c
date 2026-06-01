#include "../Support/String.h"

#include "Driver.h"

#include <stddefer.h>
#include <string.h>


static void
_cndShowHelp(CndDriverRef driver) {
    fprintf(stdout, "Usage: %.*s [options...] <files...>\n", CND_STRING_FORMAT(driver->invoked_as));
    fprintf(stdout, "Options:\n");
    fprintf(stdout, "    -###                Print trace information, but don't run any commands.\n");
    fprintf(stdout, "    -h, --help          Show this help message, then exit.\n");
    fprintf(stdout, "    -V, --version       Show the compiler version, then exit.\n");
    fprintf(stdout, "    -v, --verbose       Enable verbose output.\n");
    fprintf(stdout, "    --list-stds         List valid language standard names.\n");
    fprintf(stdout, "    -std=<arg>          Set the active language standard.\n");
    fprintf(stdout, "                        Depending on the name specified, this option will\n");
    fprintf(stdout, "                        update the standard only for the associated language.\n");
    fprintf(stdout, "    -cstd=<arg>         Set the active C language standard.\n");
    fprintf(stdout, "    -cndstd=<arg>       Set the active Cinder language standard.\n");
}


static void
_cndShowVersion(CndDriverRef driver) {
    fprintf(stdout, "Cinder compiler\n");
}


static void
_cndListStandards(CndDriverRef driver) {
    static const char* _standard_short_names[] = {
#define STD(Ident, Short, Lang, Desc, Flags, Ver)  "" Short "",
#define STD_ALIAS(Ident, Short)  "" Short "",
#define STD_DEPR(Ident, Short)  "" Short "",
#include <cinder/FrontEnd/XLanguageStandards.h>
    };

    for (size_t i = 0; i < countof(_standard_short_names); i++) {
        fprintf(stdout, "%s\n", _standard_short_names[i]);
    }
}


static inline CndResult
_cndHandleLanguageStandardCLIArgument(
    CndDriverRef driver,
    CndLanguage required_language,
    CndConstString alias_name
) {
    if (alias_name.data == nullptr || alias_name.count == 0) {
        _cndDriverIssueError(driver, "missing language standard argument");
        return CND_ERROR_CLI_OPTION_MISSING_ARGUMENT;
    }

    auto std = cndGetLanguageStandardFromShortName(alias_name);
    if (std == nullptr) {
        _cndDriverIssueErrorF(driver, "unknown language standard '%.*s'", CND_STRING_FORMAT(alias_name));
        return CND_ERROR_CLI_OPTION_UNKNOWN_LANGUAGE_STANDARD;
    }

    if (required_language != CND_LANG_UNSPECIFIED && std->language != required_language) {
        _cndDriverIssueErrorF(driver, "language standard '%.*s' is not a valid %.*s language standard.", CND_STRING_FORMAT(alias_name), CND_STRING_FORMAT(cndLanguageToString(required_language)));
        return CND_ERROR_CLI_OPTION_INVALID_LANGUAGE_STANDARD;
    }

    auto language = required_language;
    if (language == CND_LANG_UNSPECIFIED) {
        language = std->language;
    }

    switch (language) {
        case CND_LANG_UNSPECIFIED: unreachable(); break;
        case CND_LANG_CINDER: driver->cndstd = std; break;
        case CND_LANG_C: driver->cstd = std; break;
    }

    return CND_SUCCESS;
}

static CndResult
_cndConcatFirstTwoGivenArgumentsAndCallArgumentParser(
    CndDriverRef driver,
    CndDriverKind new_kind,
    const char** old_arguments,
    int32_t old_count,
    CndResult (*parser)(CndDriverRef driver, const char** arguments, int32_t count)
) {
    driver->kind = new_kind;

    const char** new_arguments = cndAllocateAligned(driver->allocator, (size_t) (old_count - 1) * sizeof(*new_arguments), alignof_e(*new_arguments));
    defer cndDeallocateAligned(driver->allocator, new_arguments, (size_t) (old_count - 1) * sizeof(*new_arguments), alignof_e(*new_arguments));

    size_t invoked_as_length = strlen(old_arguments[0]) + strlen(old_arguments[1]) + 1;
    char* invoked_as = cndAllocate(driver->allocator, invoked_as_length + 1);
    defer cndDeallocate(driver->allocator, invoked_as, invoked_as_length + 1);

    (void) sprintf(invoked_as, "%s %s", old_arguments[0], old_arguments[1]);
    new_arguments[0] = invoked_as;

    for (int32_t i = 1; i < old_count - 1; i++) {
        new_arguments[i] = old_arguments[i + 1];
    }

    return parser(driver, new_arguments, old_count - 1);
}

CndResult
_cndDriverParseHighLevelCLIArguments(
    CndDriverRef driver,
    const char** arguments,
    int32_t count
) {
    if (driver == nullptr) return CND_ERROR_INVALID_HANDLE;

    if (count >= 2) {
        if (0 == strcmp(arguments[1], "-cndc1")) {
            return _cndConcatFirstTwoGivenArgumentsAndCallArgumentParser(
                driver, CND_DRIVER_CNDC1,
                arguments, count,
                _cndDriverParseCndC1CLIArguments
            );
        } else if (0 == strcmp(arguments[1], "-cc1")) {
            return _cndConcatFirstTwoGivenArgumentsAndCallArgumentParser(
                driver, CND_DRIVER_CC1,
                arguments, count,
                _cndDriverParseCC1CLIArguments
            );
        } else if (0 == strcmp(arguments[1], "-as1")) {
            return _cndConcatFirstTwoGivenArgumentsAndCallArgumentParser(
                driver, CND_DRIVER_AS1,
                arguments, count,
                _cndDriverParseAs1CLIArguments
            );
        }
    }

    _cndDriverCopyCLIArguments(driver, arguments, count);
    const CndConstString* argv = driver->cli_args.data;
    size_t argc = driver->cli_args.count;

    #define _shift()  (argc == 0 ? (CndConstString){} : (argc--, *argv++))

    // The first CLI argument is always what the tool was invoked as,
    // even if invoked programatically.
    driver->invoked_as = _shift();

    bool bypass_options = false;
    CndDriverInputFileKind current_infile_kind = CND_INFILE_UNSPECIFIED;

    while (argc > 0) {
        auto opt = _shift();
        assert(opt.data != nullptr);

        if (bypass_options) {
            goto handle_positional_value;
        }

        if (cndConstStringEqualsC(opt, "--")) {
            bypass_options = true;
        } else if (cndConstStringEqualsC(opt, "--help") || cndConstStringEqualsC(opt, "-h")) {
            driver->show_help = true;
        } else if (cndConstStringEqualsC(opt, "--version") || cndConstStringEqualsC(opt, "-V")) {
            driver->show_version = true;
        } else if (cndConstStringEqualsC(opt, "--list-stds")) {
            driver->list_standards = true;
        } else if (cndConstStringEqualsC(opt, "--verbose") || cndConstStringEqualsC(opt, "-v")) {
            driver->verbose = true;
        } else if (cndConstStringEqualsC(opt, "-###")) {
            driver->hash3 = true;
        } else if (cndConstStringEqualsC(opt, "-std") || cndConstStringEqualsC(opt, "--std")) {
            auto arg = _shift();
            auto result = _cndHandleLanguageStandardCLIArgument(driver, CND_LANG_UNSPECIFIED, arg);
            if (result != CND_SUCCESS) return result;
        } else if (cndConstStringStartsWithC(opt, "-std=")) {
            auto arg = cndConstStringTrimPrefixC(opt, "-std=");
            auto result = _cndHandleLanguageStandardCLIArgument(driver, CND_LANG_UNSPECIFIED, arg);
            if (result != CND_SUCCESS) return result;
        } else if (cndConstStringStartsWithC(opt, "--std=")) {
            auto arg = cndConstStringTrimPrefixC(opt, "--std=");
            auto result = _cndHandleLanguageStandardCLIArgument(driver, CND_LANG_UNSPECIFIED, arg);
            if (result != CND_SUCCESS) return result;
        } else if (cndConstStringEqualsC(opt, "-cstd") || cndConstStringEqualsC(opt, "--cstd")) {
            auto arg = _shift();
            auto result = _cndHandleLanguageStandardCLIArgument(driver, CND_LANG_C, arg);
            if (result != CND_SUCCESS) return result;
        } else if (cndConstStringStartsWithC(opt, "-cstd=")) {
            auto arg = cndConstStringTrimPrefixC(opt, "-cstd=");
            auto result = _cndHandleLanguageStandardCLIArgument(driver, CND_LANG_C, arg);
            if (result != CND_SUCCESS) return result;
        } else if (cndConstStringStartsWithC(opt, "--cstd=")) {
            auto arg = cndConstStringTrimPrefixC(opt, "--cstd=");
            auto result = _cndHandleLanguageStandardCLIArgument(driver, CND_LANG_C, arg);
            if (result != CND_SUCCESS) return result;
        } else if (cndConstStringEqualsC(opt, "-cndstd") || cndConstStringEqualsC(opt, "--cndstd")) {
            auto arg = _shift();
            auto result = _cndHandleLanguageStandardCLIArgument(driver, CND_LANG_CINDER, arg);
            if (result != CND_SUCCESS) return result;
        } else if (cndConstStringStartsWithC(opt, "-cndstd=")) {
            auto arg = cndConstStringTrimPrefixC(opt, "-cndstd=");
            auto result = _cndHandleLanguageStandardCLIArgument(driver, CND_LANG_CINDER, arg);
            if (result != CND_SUCCESS) return result;
        } else if (cndConstStringStartsWithC(opt, "--cndstd=")) {
            auto arg = cndConstStringTrimPrefixC(opt, "--cndstd=");
            auto result = _cndHandleLanguageStandardCLIArgument(driver, CND_LANG_CINDER, arg);
            if (result != CND_SUCCESS) return result;
        } else {
            if (*opt.data == '-') {
                _cndDriverIssueErrorF(driver, "unknown option '%.*s'", CND_STRING_FORMAT(opt));
                return CND_ERROR_CLI_OPTION_UNKNOWN;
            }

        handle_positional_value:;
            CndDriverInputFileKind infile_kind = current_infile_kind;
            if (infile_kind == CND_INFILE_UNSPECIFIED) {
                CndConstString ext = cndConstStringGetFileExtension(opt);
                if (cndConstStringEqualsC(ext, ".cnd")) {
                    infile_kind = CND_INFILE_CINDER_SOURCE;
                } else if (cndConstStringEqualsC(ext, ".c")) {
                    infile_kind = CND_INFILE_C_SOURCE;
                } else if (cndConstStringEqualsC(ext, ".i")) {
                    infile_kind = CND_INFILE_C_SOURCE_NOPP;
                } else if (cndConstStringEqualsC(ext, ".h")) {
                    infile_kind = CND_INFILE_C_HEADER;
                } else {
                    if (ext.count == 0) {
                        _cndDriverIssueErrorF(driver, "input file '%.*s' with missing extension", CND_STRING_FORMAT(opt));
                    } else {
                        _cndDriverIssueErrorF(driver, "input file '%.*s' with unknown extension '%.*s'", CND_STRING_FORMAT(opt), CND_STRING_FORMAT(ext));
                    }
                    _cndDriverIssueNote(driver, "use '-x' to specify the file kind manually");
                    return CND_ERROR_CLI_UNKNOWN_FILE_EXTENSION;
                }
            }

            assert(infile_kind != CND_INFILE_UNSPECIFIED);
            CndDriverInputFile infile = {
                .kind = infile_kind,
                .path = opt,
            };

            cndDynArrPush(&driver->input_files, infile);
        }
    }

    #undef _shift

    return CND_SUCCESS;
}


CndResult
_cndDriverRunHighLevel(CndDriverRef driver) {
    if (driver == nullptr) return CND_ERROR_INVALID_HANDLE;

    if (driver->show_help) {
        _cndShowHelp(driver);
        return CND_SUCCESS;
    }

    if (driver->show_version) {
        _cndShowVersion(driver);
        return CND_SUCCESS;
    }

    if (driver->list_standards) {
        _cndListStandards(driver);
        return CND_SUCCESS;
    }

    _cndVerboseF(driver, "Cinder Language Standard: %.*s", CND_STRING_FORMAT(driver->cndstd->short_name));
    _cndVerboseF(driver, "C Language Standard:      %.*s", CND_STRING_FORMAT(driver->cstd->short_name));

    return CND_SUCCESS;
}
