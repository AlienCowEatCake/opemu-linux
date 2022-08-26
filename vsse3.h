//
//  vsse3.h
//  opemu
//
//  Created by Meowthra on 2019/5/26.
//  Copyright © 2019 Meowthra. All rights reserved.
//  Made in Taiwan.

#ifndef vsse3_h
#define vsse3_h

#include "optrap.h"

int vsse3_instruction(struct pt_regs *regs,
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
/**  VSSE3  instructions implementation       **/
/**********************************************/
static inline void vmovsldup_128(XMM src, XMM *res) {
    res->fa32[0] = src.fa32[0];
    res->fa32[1] = src.fa32[0];
    res->fa32[2] = src.fa32[2];
    res->fa32[3] = src.fa32[2];
}
static inline void vmovsldup_256(YMM src, YMM *res) {
    res->fa32[0] = src.fa32[0];
    res->fa32[1] = src.fa32[0];
    res->fa32[2] = src.fa32[2];
    res->fa32[3] = src.fa32[2];
    res->fa32[4] = src.fa32[4];
    res->fa32[5] = src.fa32[4];
    res->fa32[6] = src.fa32[6];
    res->fa32[7] = src.fa32[6];
}

static inline void vmovddup_128(XMM src, XMM *res) {
    res->fa64[0] = src.fa64[0];
    res->fa64[1] = src.fa64[0];
}
static inline void vmovddup_256(YMM src, YMM *res) {
    res->fa64[0] = src.fa64[0];
    res->fa64[1] = src.fa64[0];
    res->fa64[2] = src.fa64[2];
    res->fa64[3] = src.fa64[2];
}

static inline void vmovshdup_128(XMM src, XMM *res) {
    res->fa32[0] = src.fa32[1];
    res->fa32[1] = src.fa32[1];
    res->fa32[2] = src.fa32[3];
    res->fa32[3] = src.fa32[3];
}
static inline void vmovshdup_256(YMM src, YMM *res) {
    res->fa32[0] = src.fa32[1];
    res->fa32[1] = src.fa32[1];
    res->fa32[2] = src.fa32[3];
    res->fa32[3] = src.fa32[3];
    res->fa32[4] = src.fa32[5];
    res->fa32[5] = src.fa32[5];
    res->fa32[6] = src.fa32[7];
    res->fa32[7] = src.fa32[7];
}

static inline void vlddqu_128(XMM src, XMM *res) {
    res->u128 = src.u128;
}
static inline void vlddqu_256(YMM src, YMM *res) {
    res->u256 = src.u256;
}

static inline void vhaddpd_128(XMM src, XMM vsrc, XMM *res) {
    res->fa64[0] = vsrc.fa64[0] + vsrc.fa64[1];
    res->fa64[1] = src.fa64[0] + src.fa64[1];
}
static inline void vhaddpd_256(YMM src, YMM vsrc, YMM *res) {
    res->fa64[0] = vsrc.fa64[0] + vsrc.fa64[1];
    res->fa64[1] = src.fa64[0] + src.fa64[1];
    res->fa64[2] = vsrc.fa64[2] + vsrc.fa64[3];
    res->fa64[3] = src.fa64[2] + src.fa64[3];
}

static inline void vhaddps_128(XMM src, XMM vsrc, XMM *res) {
    res->fa32[0] = vsrc.fa32[0] + vsrc.fa32[1];
    res->fa32[1] = vsrc.fa32[2] + vsrc.fa32[3];
    res->fa32[2] = src.fa32[0] + src.fa32[1];
    res->fa32[3] = src.fa32[2] + src.fa32[3];
}
static inline void vhaddps_256(YMM src, YMM vsrc, YMM *res) {
    res->fa32[0] = vsrc.fa32[0] + vsrc.fa32[1];
    res->fa32[1] = vsrc.fa32[2] + vsrc.fa32[3];
    res->fa32[2] = src.fa32[0] + src.fa32[1];
    res->fa32[3] = src.fa32[2] + src.fa32[3];
    res->fa32[4] = vsrc.fa32[4] + vsrc.fa32[5];
    res->fa32[5] = vsrc.fa32[6] + vsrc.fa32[7];
    res->fa32[6] = src.fa32[4] + src.fa32[5];
    res->fa32[7] = src.fa32[6] + src.fa32[7];
}

static inline void vhsubpd_128(XMM src, XMM vsrc, XMM *res) {
    res->fa64[0] = vsrc.fa64[0] - vsrc.fa64[1];
    res->fa64[1] = src.fa64[0] - src.fa64[1];
}
static inline void vhsubpd_256(YMM src, YMM vsrc, YMM *res) {
    res->fa64[0] = vsrc.fa64[0] - vsrc.fa64[1];
    res->fa64[1] = src.fa64[0] - src.fa64[1];
    res->fa64[2] = vsrc.fa64[2] - vsrc.fa64[3];
    res->fa64[3] = src.fa64[2] - src.fa64[3];
}

static inline void vhsubps_128(XMM src, XMM vsrc, XMM *res) {
    res->fa32[0] = vsrc.fa32[0] - vsrc.fa32[1];
    res->fa32[1] = vsrc.fa32[2] - vsrc.fa32[3];
    res->fa32[2] = src.fa32[0] - src.fa32[1];
    res->fa32[3] = src.fa32[2] - src.fa32[3];
}
static inline void vhsubps_256(YMM src, YMM vsrc, YMM *res) {
    res->fa32[0] = vsrc.fa32[0] - vsrc.fa32[1];
    res->fa32[1] = vsrc.fa32[2] - vsrc.fa32[3];
    res->fa32[2] = src.fa32[0] - src.fa32[1];
    res->fa32[3] = src.fa32[2] - src.fa32[3];
    res->fa32[4] = vsrc.fa32[4] - vsrc.fa32[5];
    res->fa32[5] = vsrc.fa32[6] - vsrc.fa32[7];
    res->fa32[6] = src.fa32[4] - src.fa32[5];
    res->fa32[7] = src.fa32[6] - src.fa32[7];
}

static inline void vaddsubpd_128(XMM src, XMM vsrc, XMM *res) {
    res->fa64[0] = vsrc.fa64[0] - src.fa64[0];
    res->fa64[1] = vsrc.fa64[1] + src.fa64[1];
}
static inline void vaddsubpd_256(YMM src, YMM vsrc, YMM *res) {
    res->fa64[0] = vsrc.fa64[0] - src.fa64[0];
    res->fa64[1] = vsrc.fa64[1] + src.fa64[1];
    res->fa64[2] = vsrc.fa64[2] - src.fa64[2];
    res->fa64[3] = vsrc.fa64[3] + src.fa64[3];
}

static inline void vaddsubps_128(XMM src, XMM vsrc, XMM *res) {
    res->fa32[0] = vsrc.fa32[0] - src.fa32[0];
    res->fa32[1] = vsrc.fa32[1] + src.fa32[1];
    res->fa32[2] = vsrc.fa32[2] - src.fa32[2];
    res->fa32[3] = vsrc.fa32[3] + src.fa32[3];
}
static inline void vaddsubps_256(YMM src, YMM vsrc, YMM *res) {
    res->fa32[0] = vsrc.fa32[0] - src.fa32[0];
    res->fa32[1] = vsrc.fa32[1] + src.fa32[1];
    res->fa32[2] = vsrc.fa32[2] - src.fa32[2];
    res->fa32[3] = vsrc.fa32[3] + src.fa32[3];
    res->fa32[4] = vsrc.fa32[4] - src.fa32[4];
    res->fa32[5] = vsrc.fa32[5] + src.fa32[5];
    res->fa32[6] = vsrc.fa32[6] - src.fa32[6];
    res->fa32[7] = vsrc.fa32[7] + src.fa32[7];
}

#endif /* vsse3_h */
