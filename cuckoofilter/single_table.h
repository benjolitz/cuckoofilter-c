#ifndef SINGLE_TABLE_H
    #define SINGLE_TABLE_H

    #define TAGS_PER_BUCKET 4
    //method defs
    typedef void (*ST_Clean_up_tags_m)(single_table_t*);
    typedef size_t (*ST_Size_in_bytes_p)(const single_table_t*);
    typedef size_t (*ST_Size_in_tags_p)(const single_table_t*);
    typedef char* (*ST_Info_m)(const single_table_t*, unsigned int*);
    typedef uint32_t (*ST_Read_tag_p)(
        const single_table_t*, const size_t, const size_t);
    typedef void (*ST_Write_tag_m)(
        const single_table_t*, const size_t, const size_t, const uint32_t);
    typedef bool (*ST_Find_tag_in_buckets_p)(
        const single_table_t*, const size_t, const size_t, const uint32_t);
    typedef bool (*ST_Find_tag_in_bucket_p)(
        const single_table_t*, const size_t, const uint32_t);
    typedef bool (*ST_Delete_tag_from_bucket_m)(
        const single_table_t*, const size_t, const uint32_t);
    typedef bool (*ST_Insert_tag_to_bucket_m)(
        const single_table_t*, const size_t, const uint32_t, const bool, uint32_t*);
    typedef size_t (*ST_Num_tags_in_bucket_p)(const single_table_t*, const size_t);
    // Impl defs

    void __ST_cleanUpTagsImpl(single_table_t* table)
        __attribute__((nonnull));
    size_t __ST_sizeInBytesImpl(const single_table_t* table)
        __attribute__((nonnull));
    size_t __ST_sizeInTagsImpl(const single_table_t* table)
        __attribute__((nonnull));
    char* __ST_getInfoImpl(const single_table_t* table, unsigned int* length)
        __attribute__((nonnull(1)));
    uint32_t __ST_readTagImpl(const single_table_t* table, const size_t index, 
                              const size_t offset)
        __attribute__((nonnull));
    void __ST_writeTagImpl(
            const single_table_t*, const size_t index, const size_t offset,
            const uint32_t tag)
        __attribute__((nonnull));

    bool __ST_findTagInBucketsImpl(
        const single_table_t* table, const size_t index_hash,
        const size_t alt_index_hash, const uint32_t tag_hash)
        __attribute__((nonnull));

    bool __ST_findTagInBucketImpl(
            const single_table_t* table, const size_t, const uint32_t)
        __attribute__((nonnull));
    bool __ST_deleteTagFromBucketImpl(
            const single_table_t* table, const size_t, const uint32_t)
        __attribute__((nonnull));
    bool __ST_insertTagToBucketImpl(
            const single_table_t* table, const size_t, const uint32_t,
            const bool, uint32_t*)
        __attribute__((nonnull));
    size_t __ST_getNumTagsInBucketImpl(
            const single_table_t* table, const size_t)
        __attribute__((nonnull));


    struct single_table_t {
        const size_t                       bits_per_tag;
        const size_t                       tags_per_bucket;
        const size_t                       bytes_per_bucket;
        const uint32_t                     TAGMASK;
        const size_t                       num_buckets;
        //bucket == char[bytes_per_bucket]
        const void*                        buckets;
        //methods
        const ST_Clean_up_tags_m           cleanUpTags;
        const ST_Size_in_bytes_p           sizeInBytes;
        const ST_Size_in_tags_p            sizeInTags;
        const ST_Info_m                    getInfo;
        const ST_Read_tag_p                readTag;
        const ST_Write_tag_m               writeTag;
        //bucket centric
        const ST_Find_tag_in_buckets_p     findTagInBuckets;
        const ST_Find_tag_in_bucket_p      findTagInBucket;
        const ST_Delete_tag_from_bucket_m  deleteTagFromBucket;
        const ST_Insert_tag_to_bucket_m    insertTagToBucket;
        const ST_Num_tags_in_bucket_p      getNumTagsInBucket;
    };

    MAYBE_PTR initialize_single_table(
            const size_t num_buckets, const size_t bits_per_tag);
    void free_single_table(single_table_t* table)
        __attribute__((nonnull));
#endif