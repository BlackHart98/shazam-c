#define NOB_IMPLEMENTATION
#define NOB_WARNING_DEPRECATED
#define NOB_FETCH_IMPLEMENTATION
#include "nob_fetch.h"

#define LIBS_DIR "libs/"
#if !defined(_WIN32)
    #define LIB_CURL_PATH "/opt/local/lib/"
#else
    #error "Build failed, could not find libcurl /opt/local/lib/"
#endif
#define SHAZAMC_INCLUDE "shazamc/"
#define BUILD_DIR "build/"
#define CURL_DYLIB "curl"
#define TEST_PATH "tests/"
#define SHAZAMC_DYLIB "shazamc"


int main(int argc, char *argv[]){
    NOB_GO_REBUILD_URSELF_PLUS(argc, argv, "nob.h", "nob_fetch.h");

    nob_mkdir_if_not_exists(BUILD_DIR);
    nob_mkdir_if_not_exists(LIBS_DIR);

    Nob_Cmd cmd = {0};
    Nob_Procs procs = {0};

    // Stage 1: Compile shazamc module into a dynamically linked library
    nob_cc(&cmd);
    nob_cc_flags(&cmd);
    nob_cc_add_include(&cmd, SHAZAMC_INCLUDE);
    nob_cc_sharedlib_output(&cmd, .lib_dir = LIBS_DIR, .lib_name = SHAZAMC_DYLIB);
#if !defined(_WIN32)
    nob_cmd_append(
        &cmd, 
        "-O2",
        "-L"LIB_CURL_PATH,
        "-l"CURL_DYLIB);
#else
    nob_log(NOB_ERROR, "Build failed, unsupported platform");
    return 1;
#endif
    nob_cc_inputs(&cmd, SHAZAMC_INCLUDE"base64.c", SHAZAMC_INCLUDE"ffmpeg.c", SHAZAMC_INCLUDE"utils.c");
    if (!nob_cmd_run(&cmd)) return 1;

    // Stage 2: Compile the test files into executables
    // 1. test.c
    nob_cc(&cmd);
    nob_cc_flags(&cmd);
    nob_cc_add_include(&cmd, SHAZAMC_INCLUDE);
#if !defined(_WIN32)
    nob_cmd_append(
        &cmd,
        "-fsanitize=address",
        "-L"LIBS_DIR,
        "-l"SHAZAMC_DYLIB);
#else
    nob_log(NOB_ERROR, "Build failed, unsupported platform");
    return 1;
#endif  
    nob_cc_output(&cmd, BUILD_DIR"test");
    nob_cc_inputs(&cmd, TEST_PATH"test.c");
    if (!nob_cmd_run(&cmd, .async = &procs)) return 1;

    // 2. test_lazy.c
    nob_cc(&cmd);
    nob_cc_flags(&cmd);
    nob_cc_add_include(&cmd, SHAZAMC_INCLUDE);
#if !defined(_WIN32)
    nob_cmd_append(
        &cmd,
        "-fsanitize=address",
        "-L"LIBS_DIR,
        "-ldl");
    nob_cc_output(&cmd, BUILD_DIR"test_lazy");
#else
    nob_log(NOB_ERROR, "Build failed, unsupported platform");
    return 1;
#endif
    nob_cc_output(&cmd, BUILD_DIR"test_lazy");
    nob_cc_inputs(&cmd, TEST_PATH"test_lazy.c");
    if (!nob_cmd_run(&cmd, .async = &procs)) return 1;

    if (!nob_procs_flush(&procs)) return 1;

    nob_log(NOB_INFO, "Build successfull!");

    return 0;
}