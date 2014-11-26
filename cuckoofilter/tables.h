#include <stdlib.h>
#include <stdbool.h>
#include <cuckoofilter/maybe.h>
#include <cuckoofilter/packed_table.h>
#include <cuckoofilter/single_table.h>

typedef enum CuckooTableTypes_t {
    CuckooTableSingle = 0,
    CuckooTablePacked
} CuckooTableTypes_t;

/*
** Table definitions and methods
*/


typedef size_t (*SizeInBytes_p)(const TableType_t*);
typedef size_t (*SizeInTags_p)(const TableType_t*);
typedef bool (*FindTagInBuckets_p)(
    const TableType_t*, const size_t, const size_t, const uint32_t);
typedef bool (*FindTagInBucket_p)(
    const TableType_t*, const size_t, const uint32_t);
typedef uint32_t (*ReadTag_p)(
    const TableType_t*, const size_t, const size_t);
typedef char* (*InfoTable_p)(const TableType_t*, unsigned int*);
typedef size_t (*NumTagsInBucket_p)(const TableType_t*, const size_t);


typedef void (*CleanupTags_m)(const TableType_t*);
typedef void (*WriteTag_m)(
    TableType_t*, const size_t, const size_t, const uint32_t);
typedef bool (*DeleteTagFromBucket_m)(
    TableType_t*, const size_t, const uint32_t);
typedef bool (*InsertTagToBucket_m)(
    TableType_t*, const size_t, const uint32_t,
    const bool, const uint32_t*);


struct TableType_t {
    const size_t bits_per_tag;
    void* __rawTable__;
    CuckooTableTypes_t table_type;
    SizeInBytes_p* getSizeInBytes;
    SizeInTags_p* getSizeInTags;
    FindTagInBuckets_p* findTagInBuckets;
    FindTagInBucket_p* findTagInBucket;
    ReadTag_p* readTag;
    InfoTable_p* info;
    NumTagsInBucket_p* getNumTagsInBucket;
    CleanupTags_m* cleanupTags;
    WriteTag_m* writeTag;
    DeleteTagFromBucket_m* deleteTagFromBucket;
    InsertTagToBucket_m* insertTagToBucket;
};


