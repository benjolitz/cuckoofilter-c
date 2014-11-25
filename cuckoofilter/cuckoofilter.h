#include <stdlib.h>
#include <stdbool.h>
#include <cuckoofilter/cuckoo_tables.h>
#define MAX_CUCKOO_KICK_COUNT_FAIL 500

// function typedef styles:
// _p suffix means "pure"
// _m suffix means "mutates state on an input"


typedef enum CuckooFilterStatus {
    CuckooFilterStatus_Ok = 0,
    CuckooFilterStatus_NotFound,
    CuckooFilterStatus_NotEnoughSpace,
    CuckooFilterStatus_NotSupported
} CuckooFilterStatus_t;


typedef struct VictimCache {
    size_t index;
    uint32_t tag;
    bool used;
} VictimCache_t;

//Leave incomplete until function pointer typedefs are done.
typedef struct CuckooFilter_t CuckooFilter_t;

//primary data structures

/*
** The actual filter, paired with it's methods:
*/


/***
**** PURE functions
***/

/*
These do not depend on the CuckooFilter_t instance
*/
typedef size_t (*IndexHash_p)(uint32_t);
typedef uint32_t (*TagHash_p)(uint32_t);
typedef size_t (*AltIndex_p)(
    const size_t index, const uint32_t tag);
typedef double (*LoadFactor_p)(void);
typedef double (*BitsPerItem_p)(void);
/*
These do. Their first argument ALWAYS is CuckooFilter_t
*/

typedef CuckooFilterStatus_t (*Contains_p)(
    const CuckooFilter_t, const void*, const size_t);
typedef void (*InfoCuckoo_p)(void);
/**
*** MUTABLE functions
**/

/*
These do not depend on the CuckooFilter_t instance
*/
typedef void (*GenerateIndexTagHash_m)(
    const void*, const size_t, size_t*, uint32_t*);

/*
These do. Their first argument ALWAYS is CuckooFilter_t
*/

typedef CuckooFilterStatus_t (*AddItem_m)(
    CuckooFilter_t*, const void*, const size_t);
typedef CuckooFilterStatus_t (*DeleteItem_m)(
    CuckooFilter_t*, const void*, const size_t);



struct CuckooFilter_t {
    const size_t bits_per_item;
    const TableType_t* table;
    size_t num_items;
    VictimCache_t victim;
    //function pointers

    //private-ish ones
    const IndexHash_p __getIndexHash__;
    const TagHash_p __getTagHash__;
    const AltIndex_p __generateAltIndex__;
    const LoadFactor_p __getLoadFactor__;
    const BitsPerItem_p __getBitsPerItem__;

    const GenerateIndexTagHash_m __generateIndexTagHash__;
    //public-ish ones

    const AddItem_m AddItem;
    const Contains_p ContainsItem;
    const DeleteItem_m DeleteItem;
    const InfoCuckoo_p Info;
};