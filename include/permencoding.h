#define PERM_NENTS 3876


typedef void (*Perm_unpack_m)(uint16_t, uint8_t[4]);
typedef uint16_t (*Perm_pack_p)(const uint8_t[4]);
typedef void (*Perm_decode_m)(const uint16_t, uint8_t[4]);
typedef uint16_t (*Perm_encode_p)(const uint8_t[4]);
typedef void (*Perm_gen_tables_m)(int, int, uint8_t[4], uint16_t);

typedef struct PermEncoding_t {
    const Perm_unpack_m* __unpack__;
    const Perm_pack_p* __pack__;
    uint16_t dec_table[PERM_NENTS];
    uint16_t enc_table[1 << 16];
    const Perm_decode_m* decode;
    const Perm_encode_p* encode;
    const Perm_gen_tables_m gen_tables;
} PermEncoding_t;