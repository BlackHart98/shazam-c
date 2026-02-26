#define try_or_return(                  \
    function_call,                      \
    failure_result,                     \
    failure_return)                     if(function_call == failure_result) return failure_return
#define try_or_return_msg(              \
    function_call,                      \
    failure_result,                     \
    failure_return,                     \
    message)                            if (function_call == failure_result) {\
                                            fprintf(stderr, "%s\n", message);\
                                            return failure_return;\
                                        }
#define KB(size_)                       (size_ * 1024)


#ifndef SHAZAMC_H
#define SHAZAMC_H

#include <stdlib.h>
#define WSA_IMPLEMENTATION
#include "why_so_arena.h"

// BASE64 functions
extern char* shazamc_base64_encode(const unsigned char *bytes_stream, size_t stream_len);
extern char* shazamc_base64_encode_no_padding(const unsigned char *bytes_stream, size_t stream_len);
extern char* shazamc_base64_decode(const char* base64_stream, size_t stream_len);
extern char* shazamc_base64_decode_no_padding(const char* base64_stream, size_t stream_len);


// FFMPEG functions
extern int shazamc_ffmpeg_record_audio_from_source(const char *audio_file_target, int recording_time);
extern int shazamc_ffmpeg_convert_audio_to_dat(const char *audio_file_source, const char *audio_target_dat, const char *recording_time);
extern char* shazamc_ffmpeg_parse_dat_file(const char*);
static inline void shazamc_ffmpeg_start_recording(const char *audio_file_target);



// @todo: Replace with slice
// UTILS
typedef struct string_t{
    size_t len;
    size_t max;
    char *str;
} string_t;

extern int shazamc_append_string(string_t *str, const char *str_lit);                 // return non-zero if it fails
extern int shazamc_string_str_init(string_t *string, const char *str_lit); 
extern string_t shazamc_string_init(size_t size_);                              // return non-zero if it fails
extern void shazamc_string_deinit(string_t *str);                             // return non-zero if it fails
extern int shazamc_append_char(string_t *str, const char single_char);                    // return non-zero if it fails

extern int shazamc_curl_request(string_t *json_response, const char *api_key, const char *audio_base_64);
extern size_t shazamc_write_chunk(void *data, size_t size, size_t nmemb, void *result);



// Core functions
extern char* shazamc_from_file(char *file_name, char *api_key, char *recording_time);
extern char* shazamc_from_audio(char *api_key, char *recording_time);


// // This makes is a misc to make it possible shorten the names of the ShazamC extern API functions
// #ifdef STRIP_SHAZAMC_PREFIX
// #define base64_encode shazamc_base64_encode
// #define base64_encode_no_padding shazamc_base64_encode_no_padding
// #define base64_decode shazamc_base64_decode
// #define base64_decode_no_padding shazamc_base64_decode_no_padding


// #define shazamc_ffmpeg_record_audio_from_source ffmpeg_record_audio_from_source
// #define shazamc_ffmpeg_convert_audio_to_dat ffmpeg_convert_audio_to_dat
// #define shazamc_ffmpeg_parse_dat_file ffmpeg_parse_dat_file

// #define shazamc_append_string append_string
// #define shazamc_string_str_init string_str_init
// #define shazamc_string_init string_init
// #define shazamc_string_deinit string_deinit
// #define shazamc_append_char append_char
// #endif

#endif /* SHAZAMC_H */