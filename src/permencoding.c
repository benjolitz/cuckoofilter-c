#include <cuckoofilter/cuckoofilter.h>
#include <stdint.h>
/*
** The reference implementation looks like this:
** out[0] = (in & 0x000f);
** out[2] = ((in>>4) & 0x000f);
** out[1] = ((in>>8) & 0x000f);
** out[3] = ((in>>12) & 0x000f);
**
** I find this confusing. Notice order of operations for out:
** which is "set index 0, 2, 1, 3."
**
*/

void __PermUnpackImpl(uint16_t source, uint8_t destination[4]) {
    destination[0] = (source & 0x00F);
    destination[1] = ((source>>8) & 0x00F);
    destination[2] = ((source>>4) & 0x00F);
    destination[3] = ((source>>12) & 0x00F);
}

uint16_t __PermPackImpl(const uint8_t source[4]) {
    uint16_t first_part = *((uint16_t*)source) & 0x0F0F;
    uint16_t second_part = (*(uint16_t*)(source+2)) << 4;
    return first_part | second_part;
}

// void __PermDecodeImpl(PermEncoding_t*, const uint16_t, uint8_t[4]);
void __PermDecodeImpl(
        PermEncoding_t* table, const uint16_t index,
        uint8_t lowbits[4]) {
    uint16_t source = table->dec_table[index];
    table->__unpack__(source, lowbits);
    #ifdef NOISY
        printf(
            "PermDecode method.\nIndex = %x\nsource=%x\nResults:\n",
            index, source);
        for (unsigned int i=0; i <4; i++) {
            printf("lowbits[%d] = %x\n", i, lowbits[i]);
        }
    #endif
}

// uint16_t __PermEncodeImpl(PermEncoding_t*, const uint8_t[4]);
uint16_t __PermEncodeImpl(
        PermEncoding_t* table, const uint8_t lowbits[4]) {
    uint16_t packed = table->__pack__(lowbits);
    #ifdef NOISY
        printf("PermEncode method.\n");
        for (unsigned int index = 0; index < 4; index++) {
            printf("lowbits[%d] = %x\n", index, lowbits[index]);
        }
        printf(
            "pack(lowbits) => %x\nenc_table[%x] = %x\n",
            packed, packed, table->enc_table[packed]);
    #endif
    return table->enc_table[packed];
}
// void __PermGenTablesImpl(PermEncoding_t*, int, int, uint8_t[4], uint16_t*);
void __PermGenTablesImpl(
        PermEncoding_t* table, int start, int k, uint8_t destination[4],
        uint16_t* index) {
    for (int i = start; i < 16; i++) {
        destination[k] = i;
        // For every k < 4, call genTables on each successive index (k) for
        // destination
        if ( (k + 1) < 4) {
            table->genTables(i, k+1, destination, index);
            continue;
        }
        // set the dec_table element (via *index) to the uint16
        // formed by the packed destination.
        uint16_t packed_dest = table->__pack__(destination);
        table->dec_table[*index] = packed_dest;
        table->enc_table[packed_dest] = *index;
        #ifdef NOISY
            printf(
                "enc_table[%04x] = %04x\t %x %x %x %x\n",
                packed_dest, *index,
                destination[0], destination[1], destination[2],
                destination[3]);
        #endif
        *index = *index + 1;
    }
}
// PermEncoding_t* initializePermEncoding(void);

