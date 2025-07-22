#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>

#include"utils.h"


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
int append_string(string* dst, const char* src){
    size_t src_len = strlen(src);
    size_t expeted_len = dst->len + src_len + 1;
    printf("the size of %s is %lu\n", src, src_len);

    if (dst->max < expeted_len){
        dst->max = expeted_len * 2;
    }
    if (dst->str == NULL || dst->len == 0){
        dst->str = (char*) malloc(dst->max);
        if (dst->str == NULL) return 1;
        dst->str[0] = '\0';

    } else{
        char *temp_ = (char*) realloc(dst->str, dst->max);
        if (temp_ == NULL) return 1;
        if (dst->str != NULL) free(dst->str);
        dst->str = temp_;
        dst->str[dst->len] = '\0';
    }
    strcat(dst->str, src);
    dst->len += src_len;
    return 0;
}
