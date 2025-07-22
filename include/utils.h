#define try_or_return(                  \
    function_call,                      \
    failure_result,                     \
    failure_return)                     if(function_call == failure_result) return failure_return
#define try_or_return_msg(              \
    function_call,                      \
    failure_result,                     \
    failure_return,                     \
    message)                            if (function_call == failure_result) {\
                                            printf("%s\n", message);\
                                            return failure_return;\
                                        }


#ifndef UTILS_H_
#define UTILS_H_



typedef struct _string{
    size_t len;
    size_t max;
    char *str;
} string;

int append_string(string*, const char*);                 // return non-zero if it fails
string init_string(size_t);                              // return non-zero if it fails
void deinit_string(string*);                             // return non-zero if it fails



#endif /* UTILS_H_ */