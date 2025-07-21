#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"ffmpeg.h"

#define DEFAULT_AUDIO_SOURCE            "avfoundation" // this is because I use mac lol
#define DEFAULT_RECORDING_TIME          5 // seconds
#define API_KEY_VAULT                   ".env" // API vault
#define BUFFER_SIZE                     1024
#define PAYLOAD_BUFFER_SIZE             4096

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

void shazam_from_audio_source(); // shazam music from the audio source
void shazam_from_file(); // shazam music from file - this will be worked on in the future
int _has_arg_value(int, int);
int curl_request(char*, const char*, const char*); // return non-zero if it fails



// dynamic string
typedef struct _string{
    size_t len;
    size_t max;
    char *str;
} string;
int append_string(string*, const char*); // return non-zero if it fails
void init_string(string*, size_t); // return non-zero if it fails
void deinit_string(string*); // return non-zero if it fails
int fetch_api_key(string*, const char*, size_t); // return non-zero if it fails


int main(int argc, char *argv[]){
    if (argc < 2){
        printf("%s", USAGE);
        return 1;
    }
    int idx = 1;
    // string api_key;
    string* api_key = (string *) malloc(sizeof(string));
    init_string(api_key, 20);
    char *input_format = NULL;
    char *audio_source = DEFAULT_AUDIO_SOURCE;
    float recording_time = DEFAULT_RECORDING_TIME;

    // parsing the arguments (brittle)
    if (memcmp(argv[idx], "-h", 2) == 0){
        printf("%s", USAGE);
        return 0;
    }
    while (idx <= (argc - 2)){
        if (memcmp(argv[idx], "-h", 2) == 0){
            printf("%s", USAGE);
            return 0;
        } else if (memcmp(argv[idx], "-a", 2) == 0){
            if(_has_arg_value(idx + 1, argc) != 0) return 1;
            if (append_string(api_key, argv[idx + 1]) != 0) return 1;
            idx += 1;
        } else if (memcmp(argv[idx], "-s", 2) == 0){
            if(_has_arg_value(idx + 1, argc) != 0) return 1;
            audio_source = argv[idx + 1];
            idx += 1;
        } else if (memcmp(argv[idx], "-t", 2) == 0){
            if(_has_arg_value(idx + 1, argc) != 0) return 1;
            recording_time = atof(argv[idx + 1]);
            idx += 1;
        } else if (memcmp(argv[idx], "-i", 2) == 0){
            if(_has_arg_value(idx + 1, argc) != 0) return 1;
            input_format = argv[idx + 1];
            idx += 1;
        }
        idx++;
    }

    if (idx >= argc){
        printf("No file provided\n");
        printf("%s", USAGE);
        return 1;
    }

    char *file_name = argv[idx];
    // just make it very simple first...
    if (api_key->str == NULL){
        if (fetch_api_key(api_key, API_KEY_VAULT, BUFFER_SIZE) != 0) return 1;
    }
    printf("finally here is your api key: %s\n", api_key->str);

    char request[PAYLOAD_BUFFER_SIZE];
    curl_request(request, api_key->str, "some_b64");

    printf("curl request: %s\n", request);
    deinit_string(api_key);
    return 0;
}


// side-effect: Table doubling
int append_string(string* dst, const char* src){
    size_t src_len = strlen(src);
    size_t expeted_len = dst->len + src_len + 1;
    printf("the size of %s is %lu\n", src, src_len);

    if (dst->max < expeted_len){
        dst->max = expeted_len * 2;
    }
    if (dst->str == NULL || dst->len == 0) {
        dst->str = (char*) malloc(dst->max);
        if (dst->str == NULL) return 1;
        dst->len = 0;
        dst->str[0] = '\0';

    } else{
        char *temp_ = (char*) realloc(dst->str, dst->max);
        if (temp_ == NULL) return 1;
        if (dst->str != NULL) free(dst->str);
        dst->str=temp_;
        dst->str[dst->len] = '\0';
    }
    strcat(dst->str, src);
    dst->len += src_len;
    return 0;
}


int fetch_api_key(string* api_key, const char* api_key_vault, size_t buffer_size){
    FILE *fptr = fopen(api_key_vault, "r");
    if (fptr == NULL) {
        printf("Internal error unable to read API key vault.\n");
        return 1;
    }       
    char buffer[buffer_size];  
    while (fgets(buffer, buffer_size, fptr) != NULL) {
        append_string(api_key, buffer);
    }
    fclose(fptr); 
    return 0;
}


void init_string(string* vec, size_t init_size){
    vec->len = 0;
    vec->max = init_size;
    vec->str = NULL;
}

void deinit_string(string* vec){
    if (vec->str != NULL){
        free(vec->str);
        vec->str = NULL;
    }
    if (vec != NULL){
        free(vec);
        vec = NULL;
    }
}

// will revisit this soon
int _has_arg_value(int next_idx, int argc){
    if (next_idx > (argc - 1)){
        printf("%s", USAGE);
        return 1;
    }
    return 0;
}

// recording utility


// curl utility
int curl_request(char* request, const char *api_key, const char* audio_base_64){
    char buffer[BUFFER_SIZE];
    int result = snprintf(
        buffer,
        BUFFER_SIZE,
        "curl --silent -X POST"
        "   'https://shazam.p.rapidapi.com/songs/v2/detect'"
        "   --header 'content-type: text/plain'"
        "   --header 'x-rapidapi-host: shazam.p.rapidapi.com'"
        "   --header \"x-rapidapi-key: %s\""
        "   --data %s",
        api_key, audio_base_64);
    if (result == -1) return -1;
    memcpy(request, buffer, strlen(buffer));
    return 0;
}