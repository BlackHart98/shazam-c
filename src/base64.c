#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include"base64.h"
#include"utils.h"


static const char encoding[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

// let me start with this...
char* encode64(const unsigned char *bytes_stream, size_t stream_len){
    if (stream_len == 0) return NULL;

    string result = init_string(100);

    // sliding window of bits
    size_t idx = 0;
    while(idx < stream_len){

        // First 6 bits
        append_char(&result, encoding[(bytes_stream[idx] & 0xFC) >> 2 ]);
        if ((idx + 1) == stream_len){
            append_char(&result, encoding[(bytes_stream[idx] & 0x3) << 4]);
            append_char(&result, '=');
            append_char(&result, '=');
            break;
        }
        
        // Second 6 bits
        append_char(
            &result, 
            encoding[((bytes_stream[idx] & 0x3) << 4) | ((bytes_stream[idx+1] & 0xF0) >> 4)]);
        if ((idx + 2) == stream_len){
            append_char(&result, encoding[((bytes_stream[idx + 1] & 0xF) << 2) ]);
            append_char(&result, '=');
            break;
        }

        // Third 6 bits
        append_char(
            &result, 
            encoding[((bytes_stream[idx + 1] & 0xF) << 2) | ((bytes_stream[idx + 2] & 0xC0) >> 6)]);

        // Fourth 6 bits
        append_char(&result, encoding[bytes_stream[idx + 2] & 0x3F ]);

        idx += 3;
    }

    return result.str;
}



// const char* decode64(const char* bytes_stream, size_t stream_len){
//     return NULL;
// }
