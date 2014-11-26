//I hate macros but I really want to avoid an edge case of
// "I didn't set the has_error part and  I CRASHED!"

#define SET_ERROR(MAYBE_STRUCT, ERROR_TYPE) \
    do { \
        MAYBE_STRUCT.has_error=true; \
        MAYBE_STRUCT.result.error = ERROR_TYPE; \
    } while (0)
//All error types:
typedef enum ERROR_TYPE {
    //malloc explosions
    GENERAL_MALLOC_FAILURE,
    SINGLE_TABLE_MALLOC_FAILURE,
    SINGLE_TABLE_BUCKETS_MALLOC_FAILURE,
    PERM_ENCODING_MALLOC_FAILURE,
    //bad funargs
    ILLEGAL_BITS_PER_TAG_DEFINITION,
    ILLEGAL_NUM_BUCKET_DEFINITION,
} ERROR_TYPE;

// main MAYBE types:
typedef enum MAYBE_TYPE {
    VOID_PTR,
    SINGLE_TABLE_PTR,
    PERM_ENCODING_PTR,
} MAYBE_TYPE;

typedef struct MAYBE_PTR {
    MAYBE_TYPE type;
    bool has_error;
    union result {
        ERROR_TYPE error;
        void* void_ptr;
        single_table_t* single_table_ptr;
        PermEncoding_t* perm_encoding_ptr;
    }
} MAYBE;
