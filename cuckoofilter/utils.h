#include <cuckoofilter/universal.h>
#ifndef UTILS_H
    #define UTILS_H
    #define GET_LENGTH_OF_NUMBER(number_var, base_var, length_var) \
        do { \
            ++length_var; \
            number_var /= base_var; \
        } while (number_var)

    unsigned int int_str_length(unsigned int integer, unsigned int base);
    unsigned int sizet_str_length(size_t size, unsigned int base);
#endif