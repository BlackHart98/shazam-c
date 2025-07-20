#include<stdio.h>
#include<stdlib.h>
#include<string.h>

const char USAGE[] = 
    "Usage: shazam [OPTION]... [FILE]\n"
    "query the Shazam music recognition API.\n\n"
    "Get free API access at: https://rapidapi.com/apidojo/api/shazam/\n\n"
    "The API key can be read from file:\n"
    "$ echo \"api-key\" > \"%s\"\n"
    "If no FILE is provided, a recording is made using the AUDIO_SOURCE.\n\n"
    "Usage:\n"
    "     -h                    Show this message and exit.\n"
    "     -a API_KEY            API token.\n"
    "     -s AUDIO_SOURCE       ffmpeg audio input source, (default: \"default\").\n"
    "     -t RECORDING_TIME     Length of recording time, in seconds, (default: 5).\n"
    "     -i INPUT_FORMAT\n";

void shazam_from_audio_source();
void shazam_from_file();

int main(int argc, char* argv[]){
    if (argc < 2){
        printf("%s", USAGE);
        return 1;
    }
    int idx = 1;
    char* API_KEY;

    // parsing the arguments
    while (idx <= (argc - 1)){
        if (memcmp(argv[idx], "-h", 2) == 0){
            printf("got here.... %s \n", argv[idx]);
            printf("%s", USAGE);
            return 0;
        } else if (memcmp(argv[idx], "-a", 2) == 0){
            API_KEY = argv[idx + 1];
            printf("my api key = %s\n", API_KEY);
            idx += 1;
        } else if (memcmp(argv[idx], "-s", 2) == 0){

        } else {
            printf("%s", USAGE);
            return 1;
        }
        idx++;
    }

    char* file_name = argv[argc - 1];



    return 0;
}