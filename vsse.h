//
//  vsse.h
//  opemu
//
//  Created by Meowthra on 2019/5/26.
//  Copyright © 2019 Meowthra. All rights reserved.
//  Made in Taiwan.

#ifndef vsse_h
#define vsse_h

#include "optrap.h"
#include "fpins.h"

int vsse_instruction(struct pt_regs *regs,
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

int maxsf(float SRC1, float SRC2);
int minsf(float SRC1, float SRC2);
int cmpoptsf(float SRC1, float SRC2, uint8_t immvar);
float rcp_sf(float fp32);
float rsqrt_sf(float fp32);

/**********************************************/
/**  VSSE  instructions implementation       **/
/**********************************************/
/************* Move *************/
static inline void vmovups_128(XMM src, XMM *res) {
    int i;
    
    for (i = 0; i < 4; ++i) {
        res->fa32[i] = src.fa32[i];
    }
}
static inline void vmovups_256(YMM src, YMM *res) {
    int i;
    for (i = 0; i < 8; ++i) {
        res->fa32[i] = src.fa32[i];
    }
}

static inline void vmovss_128a(XMM src, XMM vsrc, XMM *res) {
    res->u128 = vsrc.u128;
    res->fa32[0] = src.fa32[0];
}
static inline void vmovss_128b(XMM src, XMM *res) {
    res->u128 = 0;
    res->fa32[0] = src.fa32[0];
}

static inline void vmovhlps(XMM src, XMM vsrc, XMM *res) {
    res->a64[0] = src.a64[1];
    res->a64[1] = vsrc.a64[1];
}
static inline void vmovlps_128a(XMM src, XMM vsrc, XMM *res) {
    res->a64[0] = src.a64[0];
    res->a64[1] = vsrc.a64[1];
}
static inline void vmovlps_128b(XMM src, XMM *res) {
    res->a64[0] = src.a64[0];
}
static inline void vmovlhps(XMM src, XMM vsrc, XMM *res) {
    res->a64[0] = vsrc.a64[0];
    res->a64[1] = src.a64[0];
}
static inline void vmovhps_128a(XMM src, XMM vsrc, XMM *res) {
    res->a64[0] = vsrc.a64[0];
    res->a64[1] = src.a64[0];
}
static inline void vmovhps_128b(XMM src, XMM *res) {
    res->a64[0] = src.a64[0];
}
static inline void vmovntps_128(XMM dst, XMM *res) {
    res->u128 = dst.u128;
}
static inline void vmovntps_256(YMM dst, YMM *res) {
    res->u128[0] = dst.u128[0];
    res->u128[1] = dst.u128[1];
}
static inline void vmovmskps_128(XMM src, XMM *res) {
    int i;
    int xbit;
    uint32_t mask;
    uint32_t dest = 0;
    
    for (i = 0; i < 4; ++i) {
        mask = src.u32[i];
        xbit = (mask >> 31) & 1;
        dest |= (xbit << i);
    }
    res->u32[0] = dest;
}
static inline void vmovmskps_256(YMM src, YMM *res) {
    int i;
    int xbit;
    uint32_t mask;
    uint32_t dest = 0;
    
    for (i = 0; i < 8; ++i) {
        mask = src.u32[i];
        xbit = (mask >> 31) & 1;
        dest |= (xbit << i);
    }
    res->u32[0] = dest;
}
/************* ADD *************/
static inline void vaddps_128(XMM src, XMM vsrc, XMM *res) {
    int i;
    for (i = 0; i < 4; ++i) {
        res->fa32[i] = vsrc.fa32[i] + src.fa32[i];
    }
}
static inline void vaddps_256(YMM src, YMM vsrc, YMM *res) {
    int i;
    for (i = 0; i < 8; ++i) {
        res->fa32[i] = vsrc.fa32[i] + src.fa32[i];
    }
}
static inline void vaddss(XMM src, XMM vsrc, XMM *res) {
    res->u128 = vsrc.u128;
    res->fa32[0] = vsrc.fa32[0] + src.fa32[0];
}

/************* SUB *************/
static inline void vsubps_128(XMM src, XMM vsrc, XMM *res) {
    int i;
    for (i = 0; i < 4; ++i) {
        res->fa32[i] = vsrc.fa32[i] - src.fa32[i];
    }
}
static inline void vsubps_256(YMM src, YMM vsrc, YMM *res) {
    int i;
    for (i = 0; i < 8; ++i) {
        res->fa32[i] = vsrc.fa32[i] - src.fa32[i];
    }
}
static inline void vsubss(XMM src, XMM vsrc, XMM *res) {
    res->u128 = vsrc.u128;
    res->fa32[0] = vsrc.fa32[0] - src.fa32[0];
}
/************* Multiply *************/
static inline void vmulps_128(XMM src, XMM vsrc, XMM *res) {
    int i;
    for (i = 0; i < 4; ++i) {
        res->fa32[i] = vsrc.fa32[i] * src.fa32[i];
    }
}
static inline void vmulps_256(YMM src, YMM vsrc, YMM *res) {
    int i;
    for (i = 0; i < 8; ++i) {
        res->fa32[i] = vsrc.fa32[i] * src.fa32[i];
    }
}
static inline void vmulss(XMM src, XMM vsrc, XMM *res) {
    res->u128 = vsrc.u128;
    res->fa32[0] = vsrc.fa32[0] * src.fa32[0];
}
/************* Divide *************/
static inline void vdivps_128(XMM src, XMM vsrc, XMM *res) {
    int i;
    for (i = 0; i < 4; ++i) {
        res->fa32[i] = vsrc.fa32[i] / src.fa32[i];
    }
}
static inline void vdivps_256(YMM src, YMM vsrc, YMM *res) {
    int i;
    for (i = 0; i < 8; ++i) {
        res->fa32[i] = vsrc.fa32[i] / src.fa32[i];
    }
}
static inline void vdivss(XMM src, XMM vsrc, XMM *res) {
    res->u128 = vsrc.u128;
    res->fa32[0] = vsrc.fa32[0] / src.fa32[0];
}
/************* AND *************/
static inline void vandps_128(XMM src, XMM vsrc, XMM *res) {
    XMM tmp;
    int i;
    
    for (i = 0; i < 4; ++i) {
        tmp.u32[i] = vsrc.u32[i] & src.u32[i];
        res->fa32[i] = tmp.fa32[i];
    }
}
static inline void vandps_256(YMM src, YMM vsrc, YMM *res) {
    YMM tmp;
    int i;
    
    for (i = 0; i < 8; ++i) {
        tmp.u32[i] = vsrc.u32[i] & src.u32[i];
        res->fa32[i] = tmp.fa32[i];
    }
}

static inline void vandnps_128(XMM src, XMM vsrc, XMM *res) {
    XMM tmp;
    int i;
    
    for (i = 0; i < 4; ++i) {
        tmp.u32[i] = (~(vsrc.u32[i])) & src.u32[i];
        res->fa32[i] = tmp.fa32[i];
    }
}
static inline void vandnps_256(YMM src, YMM vsrc, YMM *res) {
    YMM tmp;
    int i;
    
    for (i = 0; i < 8; ++i) {
        tmp.u32[i] = (~(vsrc.u32[i])) & src.u32[i];
        res->fa32[i] = tmp.fa32[i];
    }
}

/************* OR/XOR *************/
static inline void vorps_128(XMM src, XMM vsrc, XMM *res) {
    XMM tmp;
    int i;
    
    for (i = 0; i < 4; ++i) {
        tmp.u32[i] = vsrc.u32[i] | src.u32[i];
        res->fa32[i] = tmp.fa32[i];
    }
}
static inline void vorps_256(YMM src, YMM vsrc, YMM *res) {
    YMM tmp;
    int i;
    
    for (i = 0; i < 8; ++i) {
        tmp.u32[i] = vsrc.u32[i] | src.u32[i];
        res->fa32[i] = tmp.fa32[i];
    }
}
static inline void vxorps_128(XMM src, XMM vsrc, XMM *res) {
    XMM tmp;
    int i;
    
    for (i = 0; i < 4; ++i) {
        tmp.u32[i] = vsrc.u32[i] ^ src.u32[i];
        res->fa32[i] = tmp.fa32[i];
    }
}
static inline void vxorps_256(YMM src, YMM vsrc, YMM *res) {
    YMM tmp;
    int i;
    
    for (i = 0; i < 8; ++i) {
        tmp.u32[i] = vsrc.u32[i] ^ src.u32[i];
        res->fa32[i] = tmp.fa32[i];
    }
}

/************* Converts *************/
static inline void vcvtsi2ss(XMM src, XMM vsrc, XMM *res, uint8_t operand_size) {
    float f = 0;
    res->u128 = vsrc.u128;
    
    if (operand_size == 64) {
        int64_t ix = src.a64[0];
        f = ix;
        res->fa32[0] = f;
    } else {
        int32_t ix = src.a32[0];
        f = ix;
        res->fa32[0] = f;
    }
}
static inline void vcvtss2si(XMM src, XMM *res, int rc, uint8_t operand_size) {
    double df = 0;
    
    if (operand_size == 64) {
        int64_t ix = 0;
        df = round_fp32(src.fa32[0], rc);
        ix= df;
        res->a64[0] = ix;
    } else {
        int32_t ix = 0;
        df = round_fp32(src.fa32[0], rc);
        ix = df;
        res->a32[0] = ix;
        res->a32[1] = 0;
    }
}
/************* MAX/MIN Return *************/
static inline void vminps_128(XMM src, XMM vsrc, XMM *res) {
    int i;
    int MIN = 0;
    float SRC1, SRC2;
    
    for (i = 0; i < 2; ++i) {
        SRC1 = vsrc.fa32[i];
        SRC2 = src.fa32[i];
        MIN = minsf(SRC1, SRC2);
        if (MIN)
            res->fa32[i] = vsrc.fa32[i];
        else
            res->fa32[i] = src.fa32[i];
    }
}
static inline void vminps_256(YMM src, YMM vsrc, YMM *res) {
    int i;
    int MIN = 0;
    float SRC1, SRC2;
    
    for (i = 0; i < 4; ++i) {
        SRC1 = vsrc.fa32[i];
        SRC2 = src.fa32[i];
        MIN = minsf(SRC1, SRC2);
        if (MIN)
            res->fa32[i] = vsrc.fa32[i];
        else
            res->fa32[i] = src.fa32[i];
    }
    
}

static inline void vmaxps_128(XMM src, XMM vsrc, XMM *res) {
    int i;
    int MAX = 0;
    float SRC1, SRC2;
    
    for (i = 0; i < 2; ++i) {
        SRC1 = vsrc.fa32[i];
        SRC2 = src.fa32[i];
        MAX = maxsf(SRC1, SRC2);
        if (MAX)
            res->fa32[i] = vsrc.fa32[i];
        else
            res->fa32[i] = src.fa32[i];
    }
}
static inline void vmaxps_256(YMM src, YMM vsrc, YMM *res) {
    int i;
    int MAX = 0;
    float SRC1, SRC2;
    
    for (i = 0; i < 4; ++i) {
        SRC1 = vsrc.fa32[i];
        SRC2 = src.fa32[i];
        MAX = maxsf(SRC1, SRC2);
        if (MAX)
            res->fa32[i] = vsrc.fa32[i];
        else
            res->fa32[i] = src.fa32[i];
    }
}

static inline void vminss(XMM src, XMM vsrc, XMM *res) {
    int MIN = 0;
    res->a128 = vsrc.a128;
    float SRC1 = vsrc.fa32[0];
    float SRC2 = src.fa32[0];
    MIN = minsf(SRC1, SRC2);
    if (MIN)
        res->fa32[0] = vsrc.fa32[0];
    else
        res->fa32[0] = src.fa32[0];
}
static inline void vmaxss(XMM src, XMM vsrc, XMM *res) {
    int MAX = 0;
    res->a128 = vsrc.a128;
    float SRC1 = vsrc.fa32[0];
    float SRC2 = src.fa32[0];
    MAX = maxsf(SRC1, SRC2);
    if (MAX)
        res->fa32[0] = vsrc.fa32[0];
    else
        res->fa32[0] = src.fa32[0];
}

/************* Compare *************/
static inline void vucomiss(XMM src, XMM dst, struct pt_regs *regs) {
    unsigned long FLAGS;
    FLAGS = regs->flags;
    
    float SRC1 = dst.fa32[0];
    float SRC2 = src.fa32[0];
    
    //UNORDERED
    if ((isNaN_f32(SRC1)) || (isNaN_f32(SRC1))) {
        FLAGS |= 0x00000040; //set ZF = 1
        FLAGS |= 0x00000004; //set PF = 1
        FLAGS |= 0x00000001; //set CF = 1
    }
    //GREATER_THAN
    if (SRC1 > SRC2) {
        FLAGS = (FLAGS | 0x00000040) ^ 0x00000040;  //set ZF = 0
        FLAGS = (FLAGS | 0x00000004) ^ 0x00000004;  //set PF = 0
        FLAGS = (FLAGS | 0x00000001) ^ 0x00000001;  //set CF = 0
    }
    //LESS_THAN
    if (SRC1 < SRC2) {
        FLAGS = (FLAGS | 0x00000040) ^ 0x00000040;  //set ZF = 0
        FLAGS = (FLAGS | 0x00000004) ^ 0x00000004;  //set PF = 0
        FLAGS |= 0x00000001; //set CF = 1
    }
    //EQUAL
    if (SRC1 == SRC2) {
        FLAGS |= 0x00000040; //set ZF = 1
        FLAGS = (FLAGS | 0x00000004) ^ 0x00000004;  //set PF = 0
        FLAGS = (FLAGS | 0x00000001) ^ 0x00000001;  //set CF = 0
    }
    
    FLAGS = (FLAGS | 0x00000800) ^ 0x00000800;  //set OF = 0
    FLAGS = (FLAGS | 0x00000010) ^ 0x00000010;  //set AF = 0
    FLAGS = (FLAGS | 0x00000080) ^ 0x00000080;  //set SF = 0
    
    regs->flags = FLAGS;
}
static inline void vcmpps_128(XMM src, XMM vsrc, XMM *res, uint8_t imm) {
    int i;
    int CMP = 0;
    float SRC1, SRC2;
    uint8_t immvar = imm & 31;
    
    for (i = 0; i < 4; ++i) {
        SRC1 = vsrc.fa32[i];
        SRC2 = src.fa32[i];
        CMP = cmpoptsf(SRC1, SRC2, immvar);
        if (CMP)
            res->u32[i] = 0xFFFFFFFF;
        else
            res->u32[i] = 0;
    }
}
static inline void vcmpps_256(YMM src, YMM vsrc, YMM *res, uint8_t imm) {
    int i;
    int CMP = 0;
    float SRC1, SRC2;
    uint8_t immvar = imm & 31;
    
    for (i = 0; i < 8; ++i) {
        SRC1 = vsrc.fa32[i];
        SRC2 = src.fa32[i];
        CMP = cmpoptsf(SRC1, SRC2, immvar);
        if (CMP)
            res->u32[i] = 0xFFFFFFFF;
        else
            res->u32[i] = 0;
    }
}
static inline void vcmpss(XMM src, XMM vsrc, XMM *res, uint8_t imm) {
    int CMP = 0;
    uint8_t immvar = imm & 31;
    res->u128 = vsrc.u128;
    float SRC1 = vsrc.fa32[0];
    float SRC2 = src.fa32[0];
    CMP = cmpoptsf(SRC1, SRC2, immvar);
    if (CMP)
        res->u32[0] = 0xFFFFFFFF;
    else
        res->u32[0] = 0;
}

/************* Interleave *************/
static inline void vunpcklps_128(XMM src, XMM vsrc, XMM *res) {
    res->fa32[0] = vsrc.fa32[0];
    res->fa32[1] = src.fa32[0];
    res->fa32[2] = vsrc.fa32[1];
    res->fa32[3] = src.fa32[1];
}
static inline void vunpcklps_256(YMM src, YMM vsrc, YMM *res) {
    res->fa32[0] = vsrc.fa32[0];
    res->fa32[1] = src.fa32[0];
    res->fa32[2] = vsrc.fa32[1];
    res->fa32[3] = src.fa32[1];
    res->fa32[4] = vsrc.fa32[4];
    res->fa32[5] = src.fa32[4];
    res->fa32[6] = vsrc.fa32[5];
    res->fa32[7] = src.fa32[5];
}
static inline void vunpckhps_128(XMM src, XMM vsrc, XMM *res) {
    res->fa32[0] = vsrc.fa32[2];
    res->fa32[1] = src.fa32[2];
    res->fa32[2] = vsrc.fa32[3];
    res->fa32[3] = src.fa32[3];
}
static inline void vunpckhps_256(YMM src, YMM vsrc, YMM *res) {
    res->fa32[0] = vsrc.fa32[2];
    res->fa32[1] = src.fa32[2];
    res->fa32[2] = vsrc.fa32[3];
    res->fa32[3] = src.fa32[3];
    res->fa32[4] = vsrc.fa32[6];
    res->fa32[5] = src.fa32[6];
    res->fa32[6] = vsrc.fa32[7];
    res->fa32[7] = src.fa32[7];
}
/************* Select *************/
static inline void vshufps_128(XMM src, XMM vsrc, XMM *res, uint8_t imm) {
    int imm0 = imm & 3;
    int imm1 = (imm >> 2) & 3;
    int imm2 = (imm >> 4) & 3;
    int imm3 = (imm >> 6) & 3;
    
    res->fa32[0] = vsrc.fa32[imm0];
    res->fa32[1] = vsrc.fa32[imm1];
    res->fa32[2] = src.fa32[imm2];
    res->fa32[3] = src.fa32[imm3];
}

static inline void vshufps_256(YMM src, YMM vsrc, YMM *res, uint8_t imm) {
    int imm0 = imm & 3;
    int imm1 = (imm >> 2) & 3;
    int imm2 = (imm >> 4) & 3;
    int imm3 = (imm >> 6) & 3;
    
    res->fa32[0] = vsrc.fa32[imm0];
    res->fa32[1] = vsrc.fa32[imm1];
    res->fa32[2] = src.fa32[imm2];
    res->fa32[3] = src.fa32[imm3];
    res->fa32[4] = vsrc.fa32[imm0];
    res->fa32[5] = vsrc.fa32[imm1];
    res->fa32[6] = src.fa32[imm2];
    res->fa32[7] = src.fa32[imm3];
}

/************* Computes *************/
static inline void vrcpps_128(XMM src, XMM *res) {
    int i;
    float fp32;
    for (i = 0; i < 4; ++i) {
        fp32 = rcp_sf(src.fa32[i]);
        res->fa32[i] = fp32;
    }
}
static inline void vrcpps_256(YMM src, YMM *res) {
    int i;
    float fp32;
    for (i = 0; i < 8; ++i) {
        fp32 = rcp_sf(src.fa32[i]);
        res->fa32[i] = fp32;
    }
}
static inline void vrcpss(XMM src, XMM vsrc,XMM *res) {
    res->u128 = vsrc.u128;
    float fp32 = rcp_sf(src.fa32[0]);
    res->fa32[0] = fp32;
}

static inline void vrsqrtps_128(XMM src, XMM *res) {
    int i;
    float fp32;
    for (i = 0; i < 4; ++i) {
        fp32 = rsqrt_sf(src.fa32[i]);
        res->fa32[i] = fp32;
    }
}
static inline void vrsqrtps_256(YMM src, YMM *res) {
    int i;
    float fp32;
    for (i = 0; i < 8; ++i) {
        fp32 = rsqrt_sf(src.fa32[i]);
        res->fa32[i] = fp32;
    }
}
static inline void vrsqrtss(XMM src, XMM vsrc,XMM *res) {
    res->u128 = vsrc.u128;
    float fp32 = rsqrt_sf(src.fa32[0]);
    res->fa32[0] = fp32;
}

static inline void vsqrtps_128(XMM src, XMM *res) {
    int i;
    float f;
    
    for (i = 0; i < 4; ++i) {
        f = sqrt_sf(src.fa32[i]);
        res->fa32[i] = f;
    }
}

static inline void vsqrtps_256(YMM src, YMM *res) {
    int i;
    float f;
    
    for (i = 0; i < 8; ++i) {
        f = sqrt_sf(src.fa32[i]);
        res->fa32[i] = f;
    }
}

static inline void vsqrtss(XMM src, XMM vsrc, XMM *res) {
    res->u128 = vsrc.u128;
    float f = sqrt_sf(src.fa32[0]);
    res->fa32[0] = f;
}

/************* MXCSR register *************/
static inline void vldmxcsr(XMM src) {
    uint32_t mxcsr = src.u32[0];
    __asm__ __volatile__ ("ldmxcsr %0" :: "m" (mxcsr));
}

static inline void vstmxcsr(XMM *res) {
    uint32_t mxcsr = 0;
    __asm__ __volatile__ ("stmxcsr %0" : "=m" (mxcsr));
    res->u32[0] = mxcsr;
}
#endif /* vsse_h */
