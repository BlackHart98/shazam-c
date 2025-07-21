#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define DEFAULT_AUDIO_SOURCE "avfoundation" // this is because I use mac lol
#define DEFAULT_RECORDING_TIME 5 // seconds
#define API_KEY_VAULT ".env"
#define BUFFER_SIZE 4096

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
void _has_arg_value(int, int);

// dynamic length str
typedef struct _string{
    size_t len;
    size_t max;
    char *str;
} string;
void append_string(string*, const char*);
void init_string(string*, size_t);
void destroy_string(string*);
int fetch_api_key(string*, const char*, size_t);


int main(int argc, char *argv[]){
    if (argc < 2){
        printf("%s", USAGE);
        return 1;
    }
    int idx = 1;
    string api_key;
    init_string(&api_key, 20);
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
            _has_arg_value(idx + 1, argc);
            append_string(&api_key, argv[idx + 1]);
            idx += 1;
        } else if (memcmp(argv[idx], "-s", 2) == 0){
            _has_arg_value(idx + 1, argc);
            audio_source = argv[idx + 1];
            idx += 1;
        } else if (memcmp(argv[idx], "-t", 2)){
            _has_arg_value(idx + 1, argc);
            recording_time = atof(argv[idx + 1]);
            idx += 1;
        } else if (memcmp(argv[idx], "-i", 2)){
            _has_arg_value(idx + 1, argc);
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
    if (api_key.str == NULL){
        if (fetch_api_key(&api_key, API_KEY_VAULT, BUFFER_SIZE) != 0) return 1;
    }
    printf("finally here is your api key: %s\n", api_key.str);

    destroy_string(&api_key);
    return 0;
}


// side-effect: Table doubling
void append_string(string* dst, const char* src){
    if (dst->max == (dst->len + strlen(src))) {
        dst->max *= 2;
    } else if (dst->max < (dst->len + strlen(src))){
        dst->max = (dst->len + strlen(src)) * 2;
    }
    if (dst->str == NULL || dst->len == 0) {
        dst->str = (char*) malloc(dst->max);
        if (dst->str == NULL) exit(1);
        dst->len = 0;
        dst->str[0] = '\0';
    } else{
        char *temp_ = (char*) malloc(dst->max);
        if (temp_ == NULL) exit(1);
        memcpy(temp_, dst->str, dst->len);
        free(dst->str);
        dst->str=temp_;
        dst->str[dst->len] = '\0';
    }
    strcat(dst->str, src);
    dst->len += strlen(src);
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

    if (init_size == 0) 
        vec->max = 2;
    else
        vec->max = init_size;

    vec->str = NULL;
}

void destroy_string(string* vec){
    free(vec->str);
    vec->str = NULL;
}

void _has_arg_value(int next_idx, int argc){
    if (next_idx > (argc - 1)){
        printf("%s", USAGE);
        exit(1);
    }
}