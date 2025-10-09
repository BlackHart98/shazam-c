#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <curl/curl.h>


#include "shazamc.h"

#if defined(__APPLE__) || defined(__linux__) || defined(FreeBSD)
    #define OS_UNIX_LIKE
    #include <sys/wait.h>
    #include <sys/stat.h>
    #include <sys/types.h>
#elif defined(_WIN32)
    #define OS_WINDOWS
    #include <windows.h>
#else
    #error Unsupported platform!
#endif


#define BUFFER_SIZE                             1024
#define PAYLOAD_BUFFER_SIZE                     4096
#define DEFAULT_AUDIO_FILE                      "./tmp/recording.dat"
#define DEFAULT_AUDIO_B64_FILE                  "./tmp/recording_b64.dat"
#define DEFAULT_AUDIO_FILE_CONVERTED            "./tmp/converted.dat"
#define SHAZAM_API_URL                          "https://shazam.p.rapidapi.com/songs/v2/detect"
#define RAPID_API_HEADER                        "x-rapidapi-host: shazam.p.rapidapi.com"
#define CONTENT_TYPE                            "content-type: text/plain"


string_t shazamc_string_init(size_t init_size){
    assert((0 < init_size)&&"init size should be > 0");
    string_t vec;
    vec.len = 0;
    vec.max = init_size;
    vec.str = NULL;
    return vec;
}

// the literal being passed is cloned into the string object
int shazamc_string_str_init(string_t *string, const char *str_lit){
    size_t str_len = strlen(str_lit);
    size_t init_size = (str_len << 1) + 1;
    char *buffer = (char *)malloc(init_size);

    if (NULL == buffer) return 1; // Out of memory!
    memcpy(buffer, str_lit, str_len + 1);
    string->len = str_len;
    string->max = init_size;
    string->str = buffer;
    return 0;
}


void shazamc_string_deinit(string_t *vec){
    if (NULL != vec->str){
        free(vec->str);
        vec->str = NULL;
    }
    vec->len = 0;
}


// side-effect: Table doubling
int shazamc_append_string(string_t *dst, const char *str_lit){
    if (NULL == str_lit) return 0;
    size_t len = strlen(str_lit);
    size_t expected_len = dst->len + len + 1;
    if (dst->max < expected_len){
        dst->max = expected_len << 1;
        if (NULL == dst->str)
            dst->str = (char *)malloc(dst->max);
        else
            dst->str = (char *)realloc(dst->str, dst->max);
        if (NULL == dst->str) return 1; // Out of memory!
    } else if (NULL == dst->str){
        dst->str = (char *) malloc(dst->max);
        if (NULL == dst->str) return 1; // Out of memory!
        memset(dst->str, 0, dst->max);
    }
    memcpy(&(dst->str[dst->len]), str_lit, len);
    dst->len += len;
    dst->str[dst->len] = 0;
    return 0;
}


// side-effect: Table doubling
int shazamc_append_char(string_t *dst, const char src_char){
    if (src_char == '\0') return 0;
    size_t expected_len = dst->len + 1 + 1; // +1 for char, +1 for '\0'

    if (dst->max < expected_len){
        dst->max = expected_len << 1;
        if (NULL == dst->str)
            dst->str = (char *)malloc(dst->max);
        else
            dst->str = (char *)realloc(dst->str, dst->max);
        if (NULL == dst->str) return 1; // Out of memory!
    } else if (NULL == dst->str){
        dst->str = (char *) malloc(dst->max);
    }
    dst->str[dst->len] = src_char;
    dst->str[dst->len + 1] = '\0';
    dst->len += 1;
    return 0;
}


// curl utility
int shazamc_curl_request(string_t *json_response, const char *api_key, const char *audio_base_64){
    CURLcode ret; 
    CURL *curl = curl_easy_init();
    if (NULL == curl){
        fprintf(stderr, "Could not initialize curl.\n");
        return 1;
    }

    curl_easy_setopt(curl, CURLOPT_URL, SHAZAM_API_URL);

    struct curl_slist *headers = NULL;
    string_t key_header;
    if (shazamc_string_str_init(&key_header, "x-rapidapi-key: ")) return 1;
    if (shazamc_append_string(&key_header, api_key)) return 1;

    headers = curl_slist_append(headers, CONTENT_TYPE);
    headers = curl_slist_append(headers, RAPID_API_HEADER);
    headers = curl_slist_append(headers, key_header.str);

    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, audio_base_64);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, strlen(audio_base_64));
    
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, shazamc_write_chunk);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)json_response);

    ret = curl_easy_perform(curl);
    if (CURLE_OK != ret){
        fprintf(stderr, "Something went wrong\n"); // I will replace all of my error with this
        curl_easy_cleanup(curl);
        return 1;
    }
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    free(key_header.str);
    return 0;
}


size_t shazamc_write_chunk(void *data, size_t size, size_t nmemb, void *result){
    size_t actual_size = size * nmemb;
    string_t *response = (string_t *) result;
    try_or_return(shazamc_append_string(response, data), 1, CURL_WRITEFUNC_ERROR);
    return actual_size;
}


char* shazamc_from_file(char *file_name, char *api_key, char *recording_time){
    assert(NULL != api_key&&"No API key provided!");
#if defined(OS_UNIX_LIKE)
    struct stat buffer;
    try_or_return_msg(stat(file_name, &buffer), -1, NULL, "Could not find the file.");
#else
    #error Unsupported platform!
#endif
    shazamc_ffmpeg_convert_audio_to_dat(file_name, DEFAULT_AUDIO_FILE_CONVERTED, recording_time);
    char *audio_b64;
    audio_b64 = shazamc_ffmpeg_parse_dat_file(DEFAULT_AUDIO_FILE_CONVERTED);
    try_or_return_msg(audio_b64, NULL, NULL, "Could not generate base 64");
    string_t json_response = shazamc_string_init(KB(1));
    shazamc_curl_request(&json_response, api_key, audio_b64);
    if (NULL != audio_b64) free(audio_b64);
    return json_response.str;
}


char* shazamc_from_audio(char *api_key, char *recording_time){
    try_or_return(shazamc_ffmpeg_record_audio_from_source(DEFAULT_AUDIO_FILE, atoi(recording_time)), 1, NULL);
    char *audio_b64;
    audio_b64 = shazamc_ffmpeg_parse_dat_file(DEFAULT_AUDIO_FILE);
    try_or_return_msg(audio_b64, NULL, NULL, "Could not generate base 64");
    string_t json_response = shazamc_string_init(KB(1));
    shazamc_curl_request(&json_response, api_key, audio_b64);
    if (NULL != audio_b64) free(audio_b64);
    return json_response.str;
}