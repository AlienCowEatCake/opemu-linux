//
//  vsse42.h
//  opemu
//
//  Created by Meowthra on 2019/5/26.
//  Copyright © 2019 Meowthra. All rights reserved.
//  Made in Taiwan.

#ifndef vsse42_h
#define vsse42_h

#include "optrap.h"
#include "pcmpstr.h"

int vsse42_instruction(struct pt_regs *regs,
                       uint8_t vexreg,
                       uint8_t opcode,
                       uint8_t *modrm,
                       uint8_t high_reg,
                       uint8_t high_index,
                       uint8_t high_base,
                       uint16_t reg_size,
                       uint8_t operand_size,
                       uint8_t leading_opcode,
                       uint8_t simd_prefix,
                       uint8_t *bytep,
                       uint8_t ins_size,
                       uint8_t modbyte);

/**********************************************/
/**  VSSE4.2  instructions implementation    **/
/**********************************************/
static inline void vpcmpgtq_128(XMM src, XMM vsrc, XMM *res) {
    int i;
    for (i = 0; i < 2; ++i) {
        if (vsrc.a64[i] > src.a64[i])
            res->u64[i] = 0xFFFFFFFFFFFFFFFF;
        else
            res->u64[i] = 0;
    }
}
static inline void vpcmpgtq_256(YMM src, YMM vsrc, YMM *res) {
    int i;
    for (i = 0; i < 4; ++i) {
        if (vsrc.a64[i] > src.a64[i])
            res->u64[i] = 0xFFFFFFFFFFFFFFFF;
        else
            res->u64[i] = 0;
    }
}

/**********************************************/
/** SSE4.2 / VSSE4.2 Compare Strings         **/
/**********************************************/
static inline void pcmpestrm(XMM src, XMM dst, uint8_t imm, struct pt_regs *regs) {
    M64 RAX, RDX;

    int la; //src length
    int lb; //dst length

    RAX.a64 = regs->ax;
    RDX.a64 = regs->dx;
    la = RDX.u16[0];
    lb = RAX.u16[0];

    int flagx = 0;
    
    __int128_t a = src.a128;
    __int128_t b = dst.a128;
    __int128_t mask;
    mask = cmp_em(&a, la, &b, lb, imm, &flagx);
    
    regs->flags &= ~ 0b100011010101;
    regs->flags |= flagx;

    //load mask to xmm0
    XMM res;
    res.a128 = mask;
    asm __volatile__ ("movdqu %0, %%xmm0" :: "m" (res));
}

static inline void pcmpestri(XMM src, XMM dst, uint8_t imm, struct pt_regs *regs) {
    M64 RAX, RDX;
    
    int la; //src length
    int lb; //dst length
    
    RAX.a64 = regs->ax;
    RDX.a64 = regs->dx;
    la = RDX.u16[0];
    lb = RAX.u16[0];

    int index = 0;
    int flagx = 0;
    __int128_t a = src.a128;
    __int128_t b = dst.a128;
    index = cmp_ei(&a, la, &b, lb, imm, &flagx);

    regs->cx = index;
    regs->flags &= ~ 0b100011010101;
    regs->flags |= flagx;
}

static inline void pcmpistrm(XMM src, XMM dst, uint8_t imm, struct pt_regs *regs) {
    int flagx = 0;
    __int128_t a = src.a128;
    __int128_t b = dst.a128;
    __int128_t mask;
    mask = cmp_im(&a, &b, imm, &flagx);

    regs->flags &= ~ 0b100011010101;
    regs->flags |= flagx;

    //load mask to xmm0
    XMM res;
    res.a128 = mask;
    asm __volatile__ ("movdqu %0, %%xmm0" :: "m" (res));
}

static inline void pcmpistri(XMM src, XMM dst, uint8_t imm, struct pt_regs *regs) {
    int index = 0;
    int flagx = 0;
    __int128_t a = src.a128;
    __int128_t b = dst.a128;
    index = cmp_ii(&a, &b, imm, &flagx);

    regs->cx = index;
    regs->flags &= ~ 0b100011010101;
    regs->flags |= flagx;
}

#endif /* vsse42_h */
