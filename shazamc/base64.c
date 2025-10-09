#include "shazamc.h"
#include <stdio.h>

char shazamc_base64_to_byte(char base64_char);

static const char encoding[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

// let me start with this...
char* shazamc_base64_encode(const unsigned char *bytes_stream, size_t stream_len){
    if (0 == stream_len || NULL == bytes_stream) return NULL;

    string_t result = shazamc_string_init(100);

    // sliding window of bits
    size_t idx = 0;
    while(idx < stream_len){

        // First 6 bits
        shazamc_append_char(&result, encoding[(bytes_stream[idx] & 0xFC) >> 2 ]);
        if ((idx + 1) == stream_len){
            shazamc_append_char(&result, encoding[(bytes_stream[idx] & 0x3) << 4]);
            shazamc_append_char(&result, '=');
            shazamc_append_char(&result, '=');
            break;
        }
        
        // Second 6 bits
        shazamc_append_char(
            &result, 
            encoding[((bytes_stream[idx] & 0x3) << 4) | ((bytes_stream[idx+1] & 0xF0) >> 4)]);
        if ((idx + 2) == stream_len){
            shazamc_append_char(&result, encoding[((bytes_stream[idx + 1] & 0xF) << 2) ]);
            shazamc_append_char(&result, '=');
            break;
        }

        // Third 6 bits
        shazamc_append_char(
            &result, 
            encoding[((bytes_stream[idx + 1] & 0xF) << 2) | ((bytes_stream[idx + 2] & 0xC0) >> 6)]);

        // Fourth 6 bits
        shazamc_append_char(&result, encoding[bytes_stream[idx + 2] & 0x3F]);

        idx += 3;
    }

    return result.str;
}


inline char shazamc_base64_to_byte(char base64_char){
    switch (base64_char){
        case 'A':
        case 'B':
        case 'C':
        case 'D':
        case 'E':
        case 'F':
        case 'G':
        case 'H':
        case 'I':
        case 'J':
        case 'K':
        case 'L':
        case 'M':
        case 'N':
        case 'O':
        case 'P':
        case 'Q':
        case 'R':
        case 'S':
        case 'T':
        case 'U':
        case 'V':
        case 'W':
        case 'X':
        case 'Y':
        case 'Z':
            return base64_char - 'A';
        case 'a':
        case 'b':
        case 'c':
        case 'd':
        case 'e':
        case 'f':
        case 'g':
        case 'h':
        case 'i':
        case 'j':
        case 'k':
        case 'l':
        case 'm':
        case 'n':
        case 'o':
        case 'p':
        case 'q':
        case 'r':
        case 's':
        case 't':
        case 'u':
        case 'v':
        case 'w':
        case 'x':
        case 'y':
        case 'z':
            return 26 + (base64_char - 'a');
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            return 52 + (base64_char - '0');
        case '+':
            return 63;
        case '/':
            return 64;
        default: 
            return -1;
    }
}


// let me start with this...
char* shazamc_base64_encode_no_padding(const unsigned char *bytes_stream, size_t stream_len){
    if (0 == stream_len || NULL == bytes_stream) return NULL;

    string_t result = shazamc_string_init(100);

    // sliding window of bits
    size_t idx = 0;
    while(idx < stream_len){

        // First 6 bits
        shazamc_append_char(&result, encoding[(bytes_stream[idx] & 0xFC) >> 2 ]);
        if ((idx + 1) == stream_len){
            shazamc_append_char(&result, encoding[(bytes_stream[idx] & 0x3) << 4]);
            break;
        }
        
        // Second 6 bits
        shazamc_append_char(
            &result, 
            encoding[((bytes_stream[idx] & 0x3) << 4) | ((bytes_stream[idx+1] & 0xF0) >> 4)]);
        if ((idx + 2) == stream_len){
            shazamc_append_char(&result, encoding[((bytes_stream[idx + 1] & 0xF) << 2) ]);
            break;
        }

        // Third 6 bits
        shazamc_append_char(
            &result, 
            encoding[((bytes_stream[idx + 1] & 0xF) << 2) | ((bytes_stream[idx + 2] & 0xC0) >> 6)]);

        // Fourth 6 bits
        shazamc_append_char(&result, encoding[bytes_stream[idx + 2] & 0x3F]);

        idx += 3;
    }

    return result.str;
}


// base64 decoder
char* shazamc_base64_decode(const char* base64_stream, size_t stream_len){
    if (2 > stream_len || NULL == base64_stream) return NULL;
    string_t result = shazamc_string_init(100);

    size_t idx = 0;
    while (idx < stream_len){
        // First byte
        shazamc_append_char(
            &result
            , ((shazamc_base64_to_byte(base64_stream[idx]) << 2) & 0xFC) 
            | ((shazamc_base64_to_byte(base64_stream[idx + 1]) & 0x30) >> 4));
        
        // Second byte
        char six_bit_suffix = (shazamc_base64_to_byte(base64_stream[idx + 1]) & 0x0F) << 4;
        if (idx + 2 == stream_len || '=' == base64_stream[idx + 2]){
            shazamc_append_char(&result, six_bit_suffix);
            break;
        }

        shazamc_append_char(
            &result
            , six_bit_suffix 
            | ((shazamc_base64_to_byte(base64_stream[idx + 2])) >> 2));

        // Third byte
        six_bit_suffix = (shazamc_base64_to_byte(base64_stream[idx + 2]) << 6) & 0xC0;
        if (idx + 3 == stream_len || '=' == base64_stream[idx + 3]){
            shazamc_append_char(&result, six_bit_suffix);
            break;
        }

        shazamc_append_char(
            &result
            , six_bit_suffix 
            | (shazamc_base64_to_byte(base64_stream[idx + 3])));

        idx += 4;
        // break;
    }
    return result.str;
}


char* shazamc_base64_decode_no_padding(const char* base64_stream, size_t stream_len){
    if (2 > stream_len || NULL == base64_stream) return NULL;
    string_t result = shazamc_string_init(100);

    size_t idx = 0;
    while (idx < stream_len){
        // First byte
        shazamc_append_char(
            &result
            , ((shazamc_base64_to_byte(base64_stream[idx]) << 2) & 0xFC) 
            | ((shazamc_base64_to_byte(base64_stream[idx + 1]) & 0x30) >> 4));
        
        // Second byte
        char six_bit_suffix = (shazamc_base64_to_byte(base64_stream[idx + 1]) & 0x0F) << 4;
        if (idx + 2 == stream_len){
            shazamc_append_char(&result, six_bit_suffix);
            break;
        }

        shazamc_append_char(
            &result
            , six_bit_suffix 
            | ((shazamc_base64_to_byte(base64_stream[idx + 2])) >> 2));

        // Third byte
        six_bit_suffix = (shazamc_base64_to_byte(base64_stream[idx + 2]) << 6) & 0xC0;
        if (idx + 3 == stream_len){
            shazamc_append_char(&result, six_bit_suffix);
            break;
        }

        shazamc_append_char(
            &result
            , six_bit_suffix 
            | (shazamc_base64_to_byte(base64_stream[idx + 3])));

        idx += 4;
        // break;
    }
    return result.str;
}