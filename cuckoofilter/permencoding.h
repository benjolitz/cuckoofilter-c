#define PERM_NENTS 3876

typedef void (*Perm_unpack_m)(uint16_t, uint8_t[4]);
typedef uint16_t (*Perm_pack_p)(const uint8_t[4]);
typedef void (*Perm_decode_m)(const PermEncoding_t *, const uint16_t, uint8_t[4]);
typedef uint16_t (*Perm_encode_p)(Pconst ermEncoding_t *, const uint8_t[4]);
typedef void (*Perm_gen_tables_m)(PermEncoding_t *, int, int, uint8_t[4], uint16_t*);

struct PermEncoding_t {
    const Perm_unpack_m __unpack__;
    const Perm_pack_p __pack__;
    const Perm_decode_m decode;
    const Perm_encode_p encode;
    const Perm_gen_tables_m genTables;
    uint16_t dec_table[PERM_NENTS];
    uint16_t enc_table[1 << 16];
};

void __PermUnpackImpl(uint16_t, uint8_t[4]);
uint16_t __PermPackImpl(const uint8_t[4])  __attribute__ ((pure));

void __PermDecodeImpl(const PermEncoding_t*, const uint16_t, uint8_t[4])
    __attribute__((nonnull));

uint16_t __PermEncodeImpl(const PermEncoding_t*, const uint8_t[4])
    __attribute__((nonnull));
void __PermGenTablesImpl(PermEncoding_t*, int, int, uint8_t[4], uint16_t*)
    __attribute__((nonnull));

// Our constructor:
MAYBE_PTR initializePermEncoding(void);
