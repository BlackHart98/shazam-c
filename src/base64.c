#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include"base64.h"


// let me start with this...
char* encode64(const char *bytes_stream, size_t stream_len){
    char encoding[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    if (stream_len == 0) return NULL;
    size_t expected_len = (size_t)((stream_len * 8)/6) + 3;
    char *result = (char*) malloc(expected_len + 1); // The 1 is for the null character '\0'

    // partition the stream by 6

    // sliding window of bits
    size_t idx = 0;
    size_t jdx = 0;
    unsigned char residue = 0;
    unsigned char window = 0;

    while(idx < stream_len){
        // First 6 bits
        window = (bytes_stream[idx] & 0xFC) >> 2;
        residue = (bytes_stream[idx] & 0x3) << 4;

        result[jdx] = encoding[window];
        idx++;jdx++;
        if (idx >= stream_len){
            result[jdx] = encoding[residue];
            jdx++;
            result[jdx] = '=';
            jdx++;
            result[jdx] = '=';
            jdx++;
            result[jdx] = '\0';
            break;
        }
        
        // Second 6 bits
        window = (bytes_stream[idx] & 0xF0) >> 4 | residue;
        residue = (bytes_stream[idx] & 0xF) << 2;

        result[jdx] = encoding[window];
        idx++;jdx++;
        if (idx >= stream_len){
            result[jdx] = encoding[residue];
            jdx++;
            result[jdx] = '=';
            jdx++;
            result[jdx] = '\0';
            break;
        }

        // Third 6 bits
        window = ((bytes_stream[idx] & 0xC0) >> 6) | residue;
        result[jdx] = encoding[window];
        jdx++;


        window = (bytes_stream[idx] & 0x3F);
        result[jdx] = encoding[window];
        idx++;jdx++;
    }

    printf("%s was encoded as %s:: old length %lu new length %lu \n", bytes_stream, result, expected_len, strlen(result));
    return result;
}


const char* decode64(const char* bytes_stream, size_t stream_len){
    return NULL;
}
