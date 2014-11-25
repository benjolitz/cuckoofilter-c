typedef struct single_table_t {
    const size_t tags_per_bucket;
    const size_t bytes_per_bucket;
    size_t num_buckets;
    const uint32_t TAGMASK;
    void* buckets;
} single_table_t;