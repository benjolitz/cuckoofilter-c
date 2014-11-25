#include "permencoding.h"

typedef struct packed_table_t {
    const size_t dirbits_per_tag;
    const size_t bits_per_bucket;
    const size_t bytes_per_bucket;
    size_t len;
    void* buckets;
    PermEncoding_t perm;
    const uint32_t TAGMASK;
    const uint32_t DIRBITSMASK;
    size_t num_buckets;
} packed_table_t;