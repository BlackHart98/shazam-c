#ifndef BASE64_H_

#include<stdlib.h>


char* encode64(const unsigned char*, size_t);
// void encode64(const char *bytes_stream, const size_t in_len, char *out);
char* decode64(const char*, size_t);

#endif /* BASE64_H_ */