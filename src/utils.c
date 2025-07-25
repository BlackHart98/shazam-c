#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>

#include "utils.h"


string init_string(size_t init_size){
    string vec;
    vec.len = 0;
    vec.max = init_size;
    vec.str = NULL;
    return vec;
}

void deinit_string(string* vec){
    if (vec->str != NULL){
        free(vec->str);
        vec->str = NULL;
    }
    vec->len = 0;
}


// side-effect: Table doubling
int append_string(string* dst, const char* src, const size_t* len){
    assert((dst->len < dst->max)&&"Illegal string object, capacity(max) should be greater than len.");
    size_t src_len;
    if (len == NULL){
        src_len = strlen(src);
    } else {
        src_len = *len;
    }
    size_t expeted_len = dst->len + src_len + 1;

    if (dst->max < expeted_len){
        dst->max = expeted_len * 2;
    }
    if (dst->str == NULL){
        dst->str = (char*) malloc(dst->max);
        if (dst->str == NULL) return 1;
    } else{
        char *temp_ = (char*) realloc(dst->str, dst->max);
        if (temp_ == NULL) return 1;
        dst->str = temp_;
    }
    memcpy(&(dst->str[dst->len]), src, src_len);
    dst->len += src_len;
    dst->str[dst->len] = 0;
    return 0;
}


// side-effect: Table doubling
int append_char(string* dst, const char src_char){
    assert((dst->len < dst->max)&&"Illegal string object, capacity(max) should be greater than len.");
    if (src_char == '\0') return 0;
    size_t expeted_len = dst->len + 1 + 1; // +1 for char, +1 for '\0'

    if (dst->str == NULL) {
        dst->max = expeted_len * 2;
        dst->str = (char*) malloc(dst->max);
        if (dst->str == NULL) return 1;
    } else if (dst->max < expeted_len) {
        dst->max = expeted_len * 2;
        char* temp = realloc(dst->str, dst->max);
        if (temp == NULL) return 1;
        dst->str = temp;
    }
    dst->str[dst->len] = src_char;
    dst->str[dst->len + 1] = '\0';
    dst->len += 1;
    return 0;
}
