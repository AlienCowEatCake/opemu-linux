//
//  optrap.h
//  opemu
//
//  Created by Meowthra on 2019/5/23.
//  Copyright © 2019 Meowthra. All rights reserved.
//  Made in Taiwan.

#ifndef optrap_h
#define optrap_h

#include <linux/ptrace.h>
#include <linux/kernel.h>

//SaturateToSignedByte
#define STSB(x) ((x > 127)? 127 : ((x < -128)? -128 : x) )
//SaturateToSignedWord
#define STSW(x) ((x > 32767)? 32767 : ((x < -32768)? -32768 : x) )
//SaturateToUnsignedByte
#define STUB(x) ((x > 255)? 255 : ((x < 0)? 0 : x) )
//SaturateToUnsignedWord
#define STUW(x) ((x > 65535)? 65535 : ((x < 0)? 0 : x) )

//Absolute value
#define ABS(x) ((x < 0)? -x : x )

struct __int256_t {
    int64_t word[4];
};
typedef struct __int256_t __int256_t;

struct __uint256_t {
    uint64_t word[4];
};
typedef struct __uint256_t __uint256_t;

union __attribute__((__packed__)) YMM_u {
    int8_t a8[32];
    int16_t a16[16];
    int32_t a32[8];
    int64_t a64[4];
    __int128_t a128[2];
    __int256_t a256;
    
    uint8_t u8[32];
    uint16_t u16[16];
    uint32_t u32[8];
    uint64_t u64[4];
    __uint128_t u128[2];
    __uint256_t u256;
    double fa64[4];
    float fa32[8];
};

typedef union YMM_u YMM;

union __attribute__((__packed__)) XMM_u {
    int8_t a8[16];
    int16_t a16[8];
    int32_t a32[4];
    int64_t a64[2];
    __int128_t a128;
    uint8_t u8[16];
    uint16_t u16[8];
    uint32_t u32[4];
    uint64_t u64[2];
    __uint128_t u128;
    double fa64[2];
    float fa32[4];
};
typedef union XMM_u XMM;

union __attribute__((__packed__)) MM_u {
    int8_t a8[8];
    int16_t a16[4];
    int32_t a32[2];
    int64_t a64;
    uint8_t u8[8];
    uint16_t u16[4];
    uint32_t u32[2];
    uint64_t u64;
};
typedef union MM_u MM;


union __attribute__((__packed__)) M64_u {
    int8_t a8[8];
    int16_t a16[4];
    int32_t a32[2];
    int64_t a64;
    uint8_t u8[8];
    uint16_t u16[4];
    uint32_t u32[2];
    uint64_t u64;
};
typedef union M64_u M64;

union __attribute__((__packed__)) M32_u {
    int8_t a8[4];
    int16_t a16[2];
    int32_t a32;
    uint8_t u8[4];
    uint16_t u16[2];
    uint32_t u32;
};
typedef union M32_u M32;

/**************************
 * Virtual YMM Register
 *************************/
static YMM VYMM0;
static YMM VYMM1;
static YMM VYMM2;
static YMM VYMM3;
static YMM VYMM4;
static YMM VYMM5;
static YMM VYMM6;
static YMM VYMM7;
static YMM VYMM8;
static YMM VYMM9;
static YMM VYMM10;
static YMM VYMM11;
static YMM VYMM12;
static YMM VYMM13;
static YMM VYMM14;
static YMM VYMM15;

int opemu_utrap_2(struct pt_regs *regs);

int rex_ins(uint8_t *instruction, struct pt_regs *regs);
int vex_ins(uint8_t *instruction, struct pt_regs *regs);

int get_consumed(uint8_t *ModRM);

void get_x64regs(uint8_t *modrm,
                 uint8_t high_reg,
                 uint8_t high_index,
                 uint8_t high_base,
                 void *src,
                 void *vsrc,
                 void *dst,
                 uint8_t vexreg,
                 struct pt_regs *regs,
                 uint8_t modbyte,
                 uint64_t *rmaddrs);

void get_rexregs(uint8_t *modrm,
                 uint8_t high_reg,
                 uint8_t high_index,
                 uint8_t high_base,
                 void *src,
                 void *dst,
                 struct pt_regs *regs,
                 uint16_t reg_size,
                 uint16_t rm_size,
                 uint8_t modbyte,
                 uint64_t *rmaddrs
                 );

void get_vexregs(uint8_t *modrm,
                 uint8_t high_reg,
                 uint8_t high_index,
                 uint8_t high_base,
                 void *src,
                 void *vsrc,
                 void *dst,
                 uint8_t vexreg,
                 struct pt_regs *regs,
                 uint16_t reg_size,
                 uint16_t rm_size,
                 uint8_t modbyte,
                 uint64_t *rmaddrs);

uint64_t addressing64(
                  struct pt_regs *regs,
                  uint8_t *modrm,
                  uint8_t mod,
                  uint8_t num_src,
                  uint8_t high_index,
                  uint8_t high_base,
                  uint8_t modbyte,
                  uint8_t bytelen);

uint32_t addressing32(
                  struct pt_regs *regs,
                  uint8_t *modrm,
                  uint8_t mod,
                  uint8_t num_src,
                  uint8_t high_index,
                  uint8_t high_base,
                  uint8_t modbyte,
                  uint8_t bytelen);

uint64_t vmaddrs(struct pt_regs *regs,
                 uint8_t *modrm,
                 uint8_t high_base,
                 XMM vaddr,
                 uint8_t ins_size
                 );

#ifdef __x86_64__
static inline int
is_saved_state64(struct pt_regs *regs)
{
    uint16_t REG_CS = regs->cs;
    if ((REG_CS & 0x10) == 0x10)
        return 1;
    else
        return 0;
}
#endif

static inline int
is_saved_state32(struct pt_regs *regs)
{
    uint16_t REG_CS = regs->cs;
#ifdef __x86_64__
    if ((REG_CS & 0x10) == 0x0)
#else
    if ((REG_CS & 0x10) == 0x10)
#endif
        return 1;
    else
        return 0;
}

#define storedqu_template(n, where)                 \
do {                                \
asm __volatile__ ("movdqu %%xmm" #n ", %0" : "=m" (*(where)));  \
} while (0);

#define loaddqu_template(n, where)                  \
do {                                \
asm __volatile__ ("movdqu %0, %%xmm" #n :: "m" (*(where))); \
} while (0);

#define storeq_template(n, where)                   \
do {                                \
asm __volatile__ ("movq %%mm" #n ", %0" : "=m" (*(where))); \
} while (0);

#define loadq_template(n, where)                    \
do {                                \
asm __volatile__ ("movq %0, %%mm" #n :: "m" (*(where)));    \
} while (0);

/**
 * Store xmm register somewhere in memory
 */
static inline void _store_xmm (uint8_t n, XMM *where)
{
    switch (n) {
        case 0:  storedqu_template(0, where); break;
        case 1:  storedqu_template(1, where); break;
        case 2:  storedqu_template(2, where); break;
        case 3:  storedqu_template(3, where); break;
        case 4:  storedqu_template(4, where); break;
        case 5:  storedqu_template(5, where); break;
        case 6:  storedqu_template(6, where); break;
        case 7:  storedqu_template(7, where); break;
        case 8:  storedqu_template(8, where); break;
        case 9:  storedqu_template(9, where); break;
        case 10: storedqu_template(10, where); break;
        case 11: storedqu_template(11, where); break;
        case 12: storedqu_template(12, where); break;
        case 13: storedqu_template(13, where); break;
        case 14: storedqu_template(14, where); break;
        case 15: storedqu_template(15, where); break;
    }
}

/**
 * Load xmm register from memory
 */
static inline void _load_xmm (uint8_t n, XMM *where)
{
    switch (n) {
        case 0:  loaddqu_template(0, where); break;
        case 1:  loaddqu_template(1, where); break;
        case 2:  loaddqu_template(2, where); break;
        case 3:  loaddqu_template(3, where); break;
        case 4:  loaddqu_template(4, where); break;
        case 5:  loaddqu_template(5, where); break;
        case 6:  loaddqu_template(6, where); break;
        case 7:  loaddqu_template(7, where); break;
        case 8:  loaddqu_template(8, where); break;
        case 9:  loaddqu_template(9, where); break;
        case 10: loaddqu_template(10, where); break;
        case 11: loaddqu_template(11, where); break;
        case 12: loaddqu_template(12, where); break;
        case 13: loaddqu_template(13, where); break;
        case 14: loaddqu_template(14, where); break;
        case 15: loaddqu_template(15, where); break;
    }
}

/**
 * Store mmx register somewhere in memory
 */
static inline void _store_mmx (uint8_t n, MM *where)
{
    switch (n) {
        case 0:  storeq_template(0, where); break;
        case 1:  storeq_template(1, where); break;
        case 2:  storeq_template(2, where); break;
        case 3:  storeq_template(3, where); break;
        case 4:  storeq_template(4, where); break;
        case 5:  storeq_template(5, where); break;
        case 6:  storeq_template(6, where); break;
        case 7:  storeq_template(7, where); break;
    }
}

/**
 * Load mmx register from memory
 */
static inline void _load_mmx (uint8_t n, MM *where)
{
    switch (n) {
        case 0:  loadq_template(0, where); break;
        case 1:  loadq_template(1, where); break;
        case 2:  loadq_template(2, where); break;
        case 3:  loadq_template(3, where); break;
        case 4:  loadq_template(4, where); break;
        case 5:  loadq_template(5, where); break;
        case 6:  loadq_template(6, where); break;
        case 7:  loadq_template(7, where); break;
    }
}

/**
 * Load Memory from XMM/YMM register
 */
static inline void _load_maddr_from_ymm (uint64_t rmaddrs, YMM *where, uint16_t rm_size, struct pt_regs *regs) {
#ifdef __x86_64__
    if (is_saved_state64(regs)) {
        if (rm_size == 256) {
            __uint256_t M256RES = ((YMM*)where)->u256;
            *((__uint256_t*)&rmaddrs) = M256RES;
        } else if (rm_size == 128) {
            __uint128_t M128RES = ((YMM*)where)->u128[0];
            *((__uint128_t*)&rmaddrs) = M128RES;
        } else if (rm_size == 64) {
            uint64_t M64RES = ((YMM*)where)->u64[0];
            *((uint64_t*)&rmaddrs) = M64RES;
        } else if (rm_size == 32) {
            uint32_t M32RES = ((YMM*)where)->u32[0];
            *((uint32_t*)&rmaddrs) = M32RES;
        }
    }
#endif
    if (is_saved_state32(regs)) {
        uint32_t rmaddrs32 = rmaddrs & 0xffffffff;
        if (rm_size == 256) {
            __uint256_t M256RES = ((YMM*)where)->u256;
            *((__uint256_t*)&rmaddrs32) = M256RES;
        } else if (rm_size == 128) {
            __uint128_t M128RES = ((YMM*)where)->u128[0];
            *((__uint128_t*)&rmaddrs32) = M128RES;
        } else if (rm_size == 64) {
            uint64_t M64RES = ((YMM*)where)->u64[0];
            *((uint64_t*)&rmaddrs32) = M64RES;
        } else if (rm_size == 32) {
            uint32_t M32RES = ((YMM*)where)->u32[0];
            *((uint32_t*)&rmaddrs) = M32RES;
        }
    }
}

static inline void _load_maddr_from_xmm (uint64_t rmaddrs, XMM *where, uint16_t rm_size, struct pt_regs *regs) {
#ifdef __x86_64__
    if (is_saved_state64(regs)) {
        if (rm_size == 128) {
            __uint128_t M128RES = ((XMM*)where)->u128;
            *((__uint128_t*)&rmaddrs) = M128RES;
        } else if (rm_size == 64) {
            uint64_t M64RES = ((XMM*)where)->u64[0];
            *((uint64_t*)&rmaddrs) = M64RES;
        } else if (rm_size == 32) {
            uint32_t M32RES = ((XMM*)where)->u32[0];
            *((uint32_t*)&rmaddrs) = M32RES;
        }
    }
#endif
    if (is_saved_state32(regs)) {
        uint32_t rmaddrs32 = rmaddrs & 0xffffffff;
        if (rm_size == 128) {
            __uint128_t M128RES = ((XMM*)where)->u128;
            *((__uint128_t*)&rmaddrs32) = M128RES;
        } else if (rm_size == 64) {
            uint64_t M64RES = ((XMM*)where)->u64[0];
            *((uint64_t*)&rmaddrs32) = M64RES;
        } else if (rm_size == 32) {
            uint32_t M32RES = ((XMM*)where)->u32[0];
            *((uint32_t*)&rmaddrs) = M32RES;
        }
    }
}

/**
 * Store VYMM Register Somewhere in Memory
 */
static inline void _store_ymm (uint8_t n, YMM *where)
{
    YMM vymmres;
    XMM lowymm;
    
    switch (n) {
        case 0:  vymmres = VYMM0; break;
        case 1:  vymmres = VYMM1; break;
        case 2:  vymmres = VYMM2; break;
        case 3:  vymmres = VYMM3; break;
        case 4:  vymmres = VYMM4; break;
        case 5:  vymmres = VYMM5; break;
        case 6:  vymmres = VYMM6; break;
        case 7:  vymmres = VYMM7; break;
        case 8:  vymmres = VYMM8; break;
        case 9:  vymmres = VYMM9; break;
        case 10: vymmres = VYMM10; break;
        case 11: vymmres = VYMM11; break;
        case 12: vymmres = VYMM12; break;
        case 13: vymmres = VYMM13; break;
        case 14: vymmres = VYMM14; break;
        case 15: vymmres = VYMM15; break;
    }
    
    ((YMM*)where)->u256 = vymmres.u256;
    
    _store_xmm(n, &lowymm);
    ((YMM*)where)->u128[0] = lowymm.u128;
}

/**
 * Load VYMM Register From Memory
 */
static inline void _load_ymm (uint8_t n, YMM *where)
{
    YMM vymmres;
    XMM lowymm;

    vymmres.u256 = ((YMM*)where)->u256;
    lowymm.u128 = ((YMM*)where)->u128[0];

    switch (n) {
        case 0:  VYMM0 = vymmres; break;
        case 1:  VYMM1 = vymmres; break;
        case 2:  VYMM2 = vymmres; break;
        case 3:  VYMM3 = vymmres; break;
        case 4:  VYMM4 = vymmres; break;
        case 5:  VYMM5 = vymmres; break;
        case 6:  VYMM6 = vymmres; break;
        case 7:  VYMM7 = vymmres; break;
        case 8:  VYMM8 = vymmres; break;
        case 9:  VYMM9 = vymmres; break;
        case 10: VYMM10 = vymmres; break;
        case 11: VYMM11 = vymmres; break;
        case 12: VYMM12 = vymmres; break;
        case 13: VYMM13 = vymmres; break;
        case 14: VYMM14 = vymmres; break;
        case 15: VYMM15 = vymmres; break;
    }
    
    _load_xmm (n, &lowymm);
}

/**
 * Store x64 register somewhere in memory
 */
static inline void _store_m64 (uint8_t n, M64 *where, struct pt_regs *regs)
{
    uint64_t M64REG = 0;
    switch (n) {
        case 0:  M64REG = regs->ax; break;
        case 1:  M64REG = regs->cx; break;
        case 2:  M64REG = regs->dx; break;
        case 3:  M64REG = regs->bx; break;
        case 4:  M64REG = regs->sp; break;
        case 5:  M64REG = regs->bp; break;
        case 6:  M64REG = regs->si; break;
        case 7:  M64REG = regs->di; break;
        case 8:  M64REG = regs->r8; break;
        case 9:  M64REG = regs->r9; break;
        case 10: M64REG = regs->r10; break;
        case 11: M64REG = regs->r11; break;
        case 12: M64REG = regs->r12; break;
        case 13: M64REG = regs->r13; break;
        case 14: M64REG = regs->r14; break;
        case 15: M64REG = regs->r15; break;
    }
    
    ((M64*)where)->u64 = M64REG;
}

/**
 * Load x64 register from memory
 */
static inline void _load_m64 (uint8_t n, M64 *where, struct pt_regs *regs)
{
    uint64_t M64REG = ((M64*)where)->u64;

    switch (n) {
        case 0:  regs->ax = M64REG; break;
        case 1:  regs->cx = M64REG; break;
        case 2:  regs->dx = M64REG; break;
        case 3:  regs->bx = M64REG; break;
        case 4:  regs->sp = M64REG; break;
        case 5:  regs->bp = M64REG; break;
        case 6:  regs->si = M64REG; break;
        case 7:  regs->di = M64REG; break;
        case 8:  regs->r8 = M64REG; break;
        case 9:  regs->r9 = M64REG; break;
        case 10: regs->r10 = M64REG; break;
        case 11: regs->r11 = M64REG; break;
        case 12: regs->r12 = M64REG; break;
        case 13: regs->r13 = M64REG; break;
        case 14: regs->r14 = M64REG; break;
        case 15: regs->r15 = M64REG; break;
    }
}

/**
 * Store x86 register somewhere in memory
 */
static inline void _store_m32 (uint8_t n, M32 *where, struct pt_regs *regs)
{
    uint32_t M32REG = 0;
    switch (n) {
        case 0:  M32REG = regs->ax; break;
        case 1:  M32REG = regs->cx; break;
        case 2:  M32REG = regs->dx; break;
        case 3:  M32REG = regs->bx; break;
        case 4:  M32REG = regs->sp; break;
        case 5:  M32REG = regs->bp; break;
        case 6:  M32REG = regs->si; break;
        case 7:  M32REG = regs->di; break;
    }
    
    ((M32*)where)->u32 = M32REG;
}

/**
 * Load x86 register from memory
 */
static inline void _load_m32 (uint8_t n, M32 *where, struct pt_regs *regs)
{
    uint32_t M32REG = ((M32*)where)->u32;
    switch (n) {
        case 0:  regs->ax = M32REG; break;
        case 1:  regs->cx = M32REG; break;
        case 2:  regs->dx = M32REG; break;
        case 3:  regs->bx = M32REG; break;
        case 4:  regs->sp = M32REG; break;
        case 5:  regs->bp = M32REG; break;
        case 6:  regs->si = M32REG; break;
        case 7:  regs->di = M32REG; break;
    }
}

/**
 * Copy from user or kernel memory
 */
static inline long copy_from_any_memory_(void *dst, const void *src, size_t size, struct pt_regs *regs, const char *file, unsigned long long line)
{
    if (user_mode(regs)) {
        unsigned long res = copy_from_user(dst, src, size);
        if (unlikely(res)) {
            printk("OPEMU: Failed to copy from user at %p for size %llu with result %lu in %s:%llu\n", src, (unsigned long long)size, res, file, line);
            dump_stack();
        }
        return (long)res;
    } else {
        long res = copy_from_kernel_nofault(dst, src, size);
        if (unlikely(res)) {
            printk("OPEMU: Failed to copy from kernel at %p for size %llu with result %ld in %s:%llu\n", src, (unsigned long long)size, res, file, line);
            dump_stack();
        }
        return res;
    }
}
#define copy_from_any_memory(dst, src, size, regs) \
    copy_from_any_memory_((dst), (src), (size), (regs), (__FILE__), (__LINE__))

#endif /* optrap_h */
