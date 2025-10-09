#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>
#include "shazamc.h"

char*  (*shazamc_from_file_ptr)(char *file_name, char *api_key, char *recording_time);

int main(void){
    char *recording_time = "5";
    char *api_key = getenv("SHAZAM_API_KEY");
    char *file_name = "examples/Frank_Ocean_Nights.mp3";

    void *handle = dlopen("./libs/libshazamc.dylib", RTLD_LAZY);

    if (!handle) return 1;

    shazamc_from_file_ptr = dlsym(handle, "shazamc_from_file");

    char *result = shazamc_from_file_ptr(file_name, api_key, recording_time);
    if (NULL != result)
        fprintf(stdout, "Here is the JSON result:\n%s\n=============================\n", result);
    else
       fprintf(stderr, "Could not get JSON result\n");
    return 0;
}