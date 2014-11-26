#include <cuckoofilter/universal.h>

#ifndef SINGLE_TABLE_H
#include <cuckoofilter/single_table.h>
#endif
#ifndef UTILS_h
#include <cuckoofilter/utils.h>
#endif

void __ST_cleanUpTagsImpl(single_table_t* table) {
    memset(table->buckets, 0, table->bytes_per_bucket, table->num_buckets);
}

size_t __ST_sizeInBytesImpl(const single_table_t* table) {
    return table->bytes_per_bucket * table->num_buckets;
}

size_t __ST_sizeInTagsImpl(const single_table_t* table) {
    return table->tags_per_bucket * num_buckets;
}
// This is stack allocated, so the str array will die
// when it isn't passed around.
char* __ST_getInfoImpl(const single_table_t* table, unsigned int* length) {
    size_t slots = table->sizeInTags(table);
    unsigned int buf_length = (
        94 +
        sizet_str_length(table->bits_per_tag) +
        sizet_str_length(table->tags_per_bucket) + 
        sizet_str_length(table->num_buckets) +
        sizet_str_length(slots)) + 1; //1 is null term.
    // length should avoid the null terminator, allowing
    // a developer to suture str(length) into another str.
    // As such, it is optional (NULL-safe).
    if (NULL != length){
        *length = buf_length-1;
    }
    char buf[buf_length];
    memset(&buf, '\0', buf_length);
    snprintf(buf, buf_length-1,
        "SingeHashtable with tag size %d bits\n"
        "\t\tAssociativity: %d\n"
        "\t\tTotal # of rows: %d\n"
        "\t\tTotal # of slots: %d\n",
            table->bits_per_tag,
            table->tags_per_bucket,
            table->num_buckets,
            slots);
    return buf;
}

uint32_t __ST_readTagImpl(
        const single_table_t* table, const size_t index, const size_t offset) {
    uint32_t tag;
    size_t bits_per_tag = table->bits_per_tag;
    const *char bucket[table->bytes_per_bucket] = *(
        (table->buckets + (table->bytes_per_bucket*index)))
    switch(bits_per_tag) {
        case 2:
            tag = *((uint8_t*) bucket) >> (offset * 2);
            break;
        case 4:
            bucket += (offset >> 1);
            tag = *((uint8_t *) bucket) >> ((offset & 1) << 2);
            break;
        case 8:
            bucket += offset;
            tag = *((uint8_t *) bucket);
            break;
        case 12:
            bucket += offset + (offset >> 1);
            tag = *((uint16_t *) bucket) >> ((offset & 1) << 2);
            break;
        case 16:
            bucket += (offset << 1);
            tag = *((uint16_t*) bucket);
            break;
        case 32:
            tag = ((uint32_t*) bucket)[offset];
            break;
        default:
            #ifdef NOISY
                printf("__ST_readTagImpl did NOTHING!\n");
            #endif
            break;
    }
    return tag & table->TAGMASK;
}

void __ST_writeTagImpl(
        single_table_t*, const size_t, const size_t, const uint32_t) {

}

bool __ST_findTagInBucketsImpl(
        single_table_t*, const size_t, const size_t, const uint32_t) {

}

bool __ST_findTagInBucketImpl(
        single_table_t*, const size_t, const uint32_t) {

}

bool __ST_deleteTagFromBucketImpl(
        single_table_t*, const size_t, const uint32_t) {

}

bool __ST_insertTagToBucketImpl(
        single_table_t*, const size_t, const uint32_t, const bool, uint32_t*) {

}

size_t __ST_getNumTagsInBucketImpl(
        single_table_t*, const size_t) {

}


/*
Promise: This function will free any memory alocated if an error!
*/
MAYBE_PTR initialize_single_table(
        const size_t num_buckets, const size_t bits_per_tag) {
    MAYBE_PTR maybe;
    maybe.has_error = false;
    maybe.type = SINGLE_TABLE_PTR;

    if (bits_per_tag == 0) {
        SET_ERROR(maybe, ILLEGAL_BITS_PER_TAG_DEFINITION)
        goto done;
    }

    if (num_buckets == 0) {
        SET_ERROR(maybe, ILLEGAL_NUM_BUCKET_DEFINITION);
        goto done;
    }

    single_table_t* table = malloc(sizeof(single_table_t));
    if (NULL == table) {
        SET_ERROR(maybe, SINGLE_TABLE_MALLOC_FAILURE);
        goto done;
    }
    size_t bytes_per_bucket;
    *((size_t*)(&table->bytes_per_bucket)) = bytes_per_bucket = (
        bits_per_tag * TAGS_PER_BUCKET + 7) >> 3;

    //Allocate num_buckets Buckets where each bucket size == bytes_per_bucket
    void* buckets = malloc(bytes_per_bucket*num_buckets);

    if (NULL == buckets) {
        SET_ERROR(maybe, SINGLE_TABLE_BUCKETS_MALLOC_FAILURE);
        // de-alloc the table cause we are NOT using it.
        free(table);
        goto done;
    }
    *((void**)&table->buckets) = buckets;

    *((size_t*)(&table->bits_per_tag)) = bits_per_tag;
    *((size_t*)(&table->tags_per_bucket)) = TAGS_PER_BUCKET;
    *((size_t*)(&table->TAGMASK)) = (1ULL << bits_per_tag) - 1;
    *((size_t*)(&table->num_buckets)) = num_buckets;
    // Assign methods
    *((ST_Clean_up_tags_m*)(&table->cleanUpTags)) = __ST_cleanUpTagsImpl;
    *((ST_Size_in_bytes_p*)(&table->sizeInBytes)) = __ST_sizeInBytesImpl;
    *((ST_Size_in_tags_p*)(&table->sizeInTags)) = __ST_sizeInTagsImpl;
    *((ST_Info_m*)(&table->getInfo)) = __ST_getInfoImpl;
    *((ST_Read_tag_p*)(&table->readTag)) = __ST_readTagImpl;
    *((ST_Write_tag_m*)(&table->writeTag)) = __ST_writeTagImpl;
    *((ST_Find_tag_in_buckets_p*)(&table->findTagInBuckets)) = __ST_findTagInBucketsImpl;
    *((ST_Find_tag_in_bucket_p*)(&table->findTagInBucket)) = __ST_findTagInBucketImpl;
    *((ST_Delete_tag_from_bucket_m*)(&table->deleteTagFromBucket)) = __ST_deleteTagFromBucketImpl;
    *((ST_Insert_tag_to_bucket_m*)(&table->insertTagToBucket)) = __ST_insertTagToBucketImpl;
    *((ST_Num_tags_in_bucket_p*)(&table->getNumTagsInBucket)) = __ST_getNumTagsInBucketImpl;


    maybe.result.single_table_ptr = table;

    done:
    return maybe;
}

void free_single_table(single_table_t* table) {
    free(table->buckets);
    free(table);
}