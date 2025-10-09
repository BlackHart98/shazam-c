#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>
#include "shazamc.h"

int main(void){
    char *recording_time = "5";
    char *api_key = getenv("SHAZAM_API_KEY");
    char *file_name = "examples/Frank_Ocean_Nights.mp3";

    char *result = shazamc_from_file(file_name, api_key, recording_time);
    if (NULL != result)
        fprintf(stdout, "Here is the JSON result:\n%s\n=============================\n", result);
    else
       fprintf(stderr, "Could not get JSON result\n");
    return 0;
}