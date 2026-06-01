#include <cinder/Driver/Driver.h>

#include <stddefer.h>


int
main(int argc, char** argv) {
    CndDriverCreateArgs driver_create_args = {
        .kind = CND_DRIVER_HIGH_LEVEL,
    };
    auto driver = cndDriverCreate(&driver_create_args, nullptr);
    defer cndDriverDestroy(driver);

    auto cli_parse_result = cndDriverParseCLIArguments(driver, (const char**) argv, argc);
    if (cli_parse_result != CND_SUCCESS) {
        return (int) cli_parse_result;
    }

    return (int) cndDriverRun(driver);
}
