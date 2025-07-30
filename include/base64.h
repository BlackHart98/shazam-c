#ifndef BASE64_H_
#define BASE64_H_

#include<stdlib.h>


char* encode64(const unsigned char*, size_t);
char* decode64(const char*, size_t);

#endif /* BASE64_H_ */