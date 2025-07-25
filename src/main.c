#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <curl/curl.h>   

#include "ffmpeg.h"
#include "utils.h"
#include "base64.h"

#define DEFAULT_AUDIO_SOURCE                    ":1"                        // this is because I use mac lol
#define DEFAULT_MEDIA_FORMAT                    "avfoundation"              // this is because I use mac lol
#define DEFAULT_RECORDING_TIME                  "5"                         // seconds
#define API_KEY_VAULT                           ".env"                      // API vault
#define BUFFER_SIZE                             1024
#define PAYLOAD_BUFFER_SIZE                     4096
#define DEFAULT_AUDIO_FILE                      "./tmp/recording.dat"
#define DEFAULT_AUDIO_B64_FILE                  "./tmp/recording_b64.dat"
#define DEFAULT_AUDIO_FILE_CONVERTED            "./tmp/converted.dat"


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
    "     -s AUDIO_SOURCE       ffmpeg audio input source, (default: \":1\").\n"
    "     -t RECORDING_TIME     Length of recording time, in seconds, (default: 5).\n"
    "     -i INPUT_FORMAT\n";

void shazam_from_audio_source();                            // shazam music from the audio source
void shazam_from_file();                                    // shazam music from file - this will be worked on in the future
int _has_arg_value(int, int);
int curl_request(string*, const string*, const char*);          // return non-zero if it fails
size_t write_chunk(void *data, size_t size, size_t nmemb, void *result);


int fetch_api_key(string*, const char*);            // return non-zero if it fails


int main(int argc, char *argv[]){
    int idx = 1;
    string api_key = init_string(100);
    // char *input_format = NULL;
    char *audio_source = DEFAULT_AUDIO_SOURCE;
    char *recording_time_str = DEFAULT_RECORDING_TIME;

    // parsing the arguments (brittle)
    while (idx < argc){
        if (memcmp(argv[idx], "-h", 2) == 0){
            printf("%s", USAGE);
            return 0;
        } else if (memcmp(argv[idx], "-a", 2) == 0){
            try_or_return(_has_arg_value(idx + 1, argc), 1, 1);
            if (append_string(&api_key, argv[idx + 1], NULL) != 0) return 1;
            idx += 1;
        } else if (memcmp(argv[idx], "-s", 2) == 0){
            try_or_return(_has_arg_value(idx + 1, argc), 1, 1);
            audio_source = argv[idx + 1];
            idx += 1;
        } else if (memcmp(argv[idx], "-t", 2) == 0){
            try_or_return(_has_arg_value(idx + 1, argc), 1, 1);
            recording_time_str = argv[idx + 1];
            idx += 1;
        } else if (memcmp(argv[idx], "-i", 2) == 0){
            try_or_return(_has_arg_value(idx + 1, argc), 1, 1);
            // input_format = argv[idx + 1];
            idx += 1;
        } else {
            break;
        }
        idx++;
    }
    string file_name = init_string(1);
    if (idx < argc) {
        append_string(&file_name, argv[idx], NULL);
        // convert file
        struct stat buffer;
        try_or_return_msg(stat(file_name.str, &buffer), -1, 1, "Could not find the file.");
        convert_audio_to_dat(file_name.str, DEFAULT_AUDIO_FILE_CONVERTED, recording_time_str);
    } else {
        try_or_return(
            ffmpeg_record_audio_from_source(
                DEFAULT_MEDIA_FORMAT, 
                audio_source, 
                DEFAULT_AUDIO_FILE,
                atoi(recording_time_str)), 
            1, 
            1);
    }

    // just make it very simple first, at least I will be learning C along with
    if (api_key.str == NULL){
        if (fetch_api_key(&api_key, API_KEY_VAULT) != 0) return 1;
    }
    // printf("finally here is your api key: %s\n", api_key.str);
    char *audio_b64;
    if (file_name.len == 0){
        audio_b64 = parse_dat_file(DEFAULT_AUDIO_FILE);
    } else {
        audio_b64 = parse_dat_file(DEFAULT_AUDIO_FILE_CONVERTED);
    }

    printf(":::::::::::::::::Audio Base 64:::::::::::::::::::::::::\n");
    printf("%s\n", audio_b64);
    try_or_return_msg(audio_b64, NULL, 1, "Could not generate base 64");

    string json_response = init_string(100);
    curl_request(&json_response, &api_key, audio_b64);


    if (audio_b64 != NULL) free(audio_b64);
    deinit_string(&json_response);
    deinit_string(&api_key);
    deinit_string(&file_name);
    return 0;
}

int fetch_api_key(string* api_key, const char* api_key_vault){
    FILE *fptr = fopen(api_key_vault, "r"); 
    try_or_return_msg(fptr, NULL, 1, "Internal error unable to read API key vault."); 
    char buffer[BUFFER_SIZE];  
    while (fgets(buffer, BUFFER_SIZE, fptr) != NULL) {
        append_string(api_key, buffer, NULL);
    }
    fclose(fptr); 
    return 0;
}

// will revisit this soon
int _has_arg_value(int next_idx, int argc){
    if (next_idx > (argc - 1)){
        printf("%s", USAGE);
        return 1;
    }
    return 0;
}

// curl utility - this won't be used at all lol! I just wanted to put this here 
// so I don't need to do a look up
int curl_request(string *json_response, const string *api_key, const char *audio_base_64){
    CURLcode ret; 
    CURL *curl = curl_easy_init();
    if (curl == NULL){
        fprintf(stderr, "Could not initialize curl.\n");
        return 1;
    }

    curl_easy_setopt(curl, CURLOPT_URL, "https://shazam.p.rapidapi.com/songs/v2/detect");

    string response = init_string(1000);
    struct curl_slist *headers = NULL;
    string key_header = init_string(24);
    append_string(&key_header, "x-rapidapi-key: ", NULL);
    append_string(&key_header, api_key->str, NULL);

    printf("%s\n", key_header.str);

    headers = curl_slist_append(headers, "content-type: text/plain");
    headers = curl_slist_append(headers, "x-rapidapi-host: shazam.p.rapidapi.com");
    headers = curl_slist_append(headers, key_header.str);

    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, audio_base_64);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, strlen(audio_base_64));
    
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_chunk);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&response);

    ret = curl_easy_perform(curl);
    if (ret != CURLE_OK){
        fprintf(stderr, "Something went wrong: %d", ret); // I will replace all of my error with this
        curl_easy_cleanup(curl);
        return 1;
    }
    printf("help meeeeeeee: %s\n", response.str);

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    if (response.str != NULL) free(response.str);
    free(key_header.str);
    return 0;
}


size_t write_chunk(void *data, size_t size, size_t nmemb, void *result){
    size_t actual_size = size * nmemb;
    string *response = (string *) result;
    try_or_return(append_string(response, data, &actual_size), 1, CURL_WRITEFUNC_ERROR);
    return actual_size;
}