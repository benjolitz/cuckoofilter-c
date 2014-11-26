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
    const char *bucket = ((char *)table->buckets)[table->bytes_per_bucket*index];
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
        const single_table_t*, const size_t index, const size_t offset,
        const uint32_t tag) {
    char *bucket = ((char *)table->buckets)[table->bytes_per_bucket*index];
    uint32_t masked_tag = tag & TAGMASK;

    switch(bits_per_tag) {
        case 2:
            *((uint8_t *) bucket) |= masked_tag << (2*offset);
            break;
        case 4:
            bucket += (offset >> 1);
            if ((offset & 1) == 0) {
                *((uint8_t *) bucket) &= 0xF0;
                *((uint8_t *) bucket) |= masked_tag;
            } else {
                *((uint8_t *) bucket) &= 0x0F;
                *((uint8_t *) bucket) |= (masked_tag << 4);
            }
            break;
        case 8:
            ((uint8_t *) bucket)[offset] =  masked_tag;
            break;
        case 12 :
            bucket += offset + (offset >> 1);
            if ((offset & 1) == 0) {
                ((uint16_t *) bucket)[0] &= 0xF000;
                ((uint16_t *) bucket)[0] |= masked_tag;
            } else {
                ((uint16_t *) bucket)[0] &= 0x000F;
                ((uint16_t *) bucket)[0] |= (masked_tag << 4);
            }
            break;
        case 16:
            ((uint16_t *) bucket)[offset] = masked_tag;
            break;
        case 32:
            ((uint32_t *) bucket)[offset] = masked_tag;
            break;
        default:
            #ifdef NOISY
                printf("__ST_writeTagImpl did nothing!\n");
            #endif
            break;
    }
}
/*
** with descriptive names, this actually makes sense.
** It will see if the IndexHash or AltIndex has ever appears in the
** bitfield bucket. Useful for answering if set contains item.
*/
bool __ST_findTagInBucketsImpl(
        const single_table_t* table, const size_t index_hash,
        const size_t alt_index_hash, const uint32_t tag_hash) {
    const char* index_bucket = (
        (char*) table->buckets)[table->bytes_per_bucket*index_hash];
    const char* alt_index_bucket = (
        (char*) table->buckets)[table->bytes_per_bucket*alt_index_hash];

    uint64_t bitfield1 = *((uint64_t *) index_bucket);
    uint64_t bitfield2 = *((uint64_t *) alt_index_bucket);
    bool result = false;

    if (4 == table->tags_per_bucket) {
        switch(table->bits_per_tag) {
            case 4:
                result = hasvalue4(bitfield1, tag_hash) || hasvalue4(bitfield2, tag_hash);
                break;
            case 8:
                result = hasvalue8(bitfield1, tag_hash) || hasvalue8(bitfield2, tag_hash);
                break;
            case 12:
                result = hasvalue12(bitfield1, tag_hash) || hasvalue12(bitfield2, tag_hash);
                break;
            case 16:
                result = hasvalue16(bitfield1, tag_hash) || hasvalue16(bitfield2, tag_hash);
                break;
        }
    } else {
        for (size_t index = 0; index < (table->tags_per_bucket); index++) {
            if ((table->readTag(table, index_hash, index)) == tag_hash ||
                (table->readTag(table, alt_index_hash, index)) == tag_hash) {
                result = true;
                break;
            }
        }
    }
    return result;
}

bool __ST_findTagInBucketImpl(
        const single_table_t* table, const size_t index_hash,
        const uint32_t tag_hash) {
    bool result = false;
    const char* index_bucket = (
        (char*) table->buckets)[table->bytes_per_bucket*index_hash];
    uint64_t bitfield1 = *((uint64_t *) index_bucket);

    if (4 == table->tags_per_bucket) {
        switch(table->bits_per_tag) {
            case 4:
                result = hasvalue4(bitfield1, tag_hash);
                break;
            case 8:
                result = hasvalue8(bitfield1, tag_hash);
                break;
            case 12:
                result = hasvalue12(bitfield1, tag_hash);
                break;
            case 16:
                result = hasvalue16(bitfield1, tag_hash);
                break;
        }
    } else {
        for (size_t index = 0; index < (table->tags_per_bucket); index++) {
            if (table->readTag(table, index_hash, index) == tag_hash) {
                result = true;
                break;
            }
        }
    }
    return result;
}

bool __ST_deleteTagFromBucketImpl(
        const single_table_t* table, const size_t index_hash,
        const uint32_t tag_hash) {
    bool result = false;
    for (size_t index = 0; index < (table->tags_per_bucket); index++) {
        if (tag_hash == table->readTag(table, index_hash, index)) {
            assert(true == table->findTagInBucket(table, index_hash, tag_hash));
            table->writeTag(table, index_hash, index, 0);
            result = true;
            break;
        }
    }
    return result;
}

bool __ST_insertTagToBucketImpl(
        const single_table_t* table, const size_t index_hash,
        const uint32_t tag_hash, const bool evict_tag_if_no_space,
        uint32_t* old_tag_hash) {

    for (size_t index = 0; index < (table->tags_per_bucket); index++) {
        if (0 == table->readTag(table, index_hash, index)) {
            table->writeTag(table, index_hash, index, tag_hash);
            return true;
        }
    }

    if (evict_tag_if_no_space) {
        size_t random_index = rand() % table->tags_per_bucket;
        *old_tag_hash = table->readTag(table, index_hash, random_index);
        table->writeTag(table, index_hash, random_index, tag_hash);
    }

    return false;
}

size_t __ST_getNumTagsInBucketImpl(
        const single_table_t* table, const size_t index_hash) {
    size_t count = 0;
    for (size_t index=0; index < (table->tags_per_bucket); index++) {
        if (0 != table->readTag(table, index_hash, index)) {
            count++;
        }
    }
    return count;
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