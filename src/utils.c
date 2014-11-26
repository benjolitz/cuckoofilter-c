#ifndef UTILS_H
#include <cuckoofilter/utils.h>
#endif

unsigned int int_str_length(unsigned int integer, unsigned int base) {
    unsigned int length = 0;
    GET_LENGTH_OF_NUMBER(integer, base, length);
    return length;
}
unsigned int sizet_str_length(size_t size, unsigned int base) {
    unsigned int length = 0;
    GET_LENGTH_OF_NUMBER(size, base, length);
    return length;
}