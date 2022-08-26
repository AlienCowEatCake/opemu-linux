//
//  vgather.h
//  opemu
//
//  Created by Meowthra on 2019/5/25.
//  Copyright © 2019 Meowthra. All rights reserved.
//  Made in Taiwan.

#ifndef vgather_h
#define vgather_h

#include "optrap.h"

int vgather_instruction(struct pt_regs *regs,
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
/**  AVX Gather instructions implementation  **/
/**********************************************/
// Integer Values
static inline void vpgatherdq128(XMM vsrc, XMM vindex, XMM *res, struct pt_regs *regs, uint8_t *modrm, uint8_t high_base, uint8_t ins_size) {
    int i;
    int cs;
    uint64_t mask_bit = 0;
    uint64_t data_addr = 0;
    XMM MASK;
    XMM vaddr;

    for (i = 0; i < 2; ++i) {
        mask_bit = vsrc.u64[i];
        cs = mask_bit >> 63 & 1;
        if (cs == 1) {
            MASK.u64[i] = 0xffffffffffffffff;
        } else {
            MASK.u64[i] = 0x0000000000000000;
        }
    }
    
    for (i = 0; i < 2; ++i) {
        vaddr.a32[0] = vindex.a32[i];
        vaddr.a32[1] = 0xffffffff;
        data_addr = vmaddrs(regs, modrm, high_base, vaddr, ins_size);

        mask_bit = MASK.u64[i];
        cs = mask_bit & 1;
        if (cs == 1) {
            res->u64[i] = *(uint64_t*)&data_addr;
        } else {
            res->u64[i] = 0;
        }
    }
}

static inline void vpgatherdq256(YMM vsrc, XMM vindex, YMM *res, struct pt_regs *regs, uint8_t *modrm, uint8_t high_base, uint8_t ins_size) {
    int i;
    int cs;
    uint64_t mask_bit = 0;
    uint64_t data_addr = 0;
    YMM MASK;
    XMM vaddr;
    
    for (i = 0; i < 4; ++i) {
        mask_bit = vsrc.u64[i];
        cs = mask_bit >> 63 & 1;
        if (cs == 1) {
            MASK.u64[i] = 0xffffffffffffffff;
        } else {
            MASK.u64[i] = 0x0000000000000000;
        }
    }
    
    for (i = 0; i < 4; ++i) {
        vaddr.a32[0] = vindex.a32[i];
        vaddr.a32[1] = 0xffffffff;
        data_addr = vmaddrs(regs, modrm, high_base, vaddr, ins_size);
        
        mask_bit = MASK.u64[i];
        cs = mask_bit & 1;
        if (cs == 1) {
            res->u64[i] = *(uint64_t*)&data_addr;
        } else {
            res->u64[i] = 0;
        }
    }
}

static inline void vpgatherdd128(XMM vsrc, XMM vindex, XMM *res, struct pt_regs *regs, uint8_t *modrm, uint8_t high_base, uint8_t ins_size) {
    int i;
    int cs;
    uint32_t mask_bit = 0;
    uint64_t data_addr = 0;
    XMM MASK;
    XMM vaddr;
    
    for (i = 0; i < 4; ++i) {
        mask_bit = vsrc.u32[i];
        cs = mask_bit >> 31 & 1;
        if (cs == 1) {
            MASK.u32[i] = 0xffffffff;
        } else {
            MASK.u32[i] = 0x00000000;
        }
    }
    
    for (i = 0; i < 4; ++i) {
        vaddr.a32[0] = vindex.a32[i];
        vaddr.a32[1] = 0xffffffff;
        data_addr = vmaddrs(regs, modrm, high_base, vaddr, ins_size);
        
        mask_bit = MASK.u32[i];
        cs = mask_bit & 1;
        if (cs == 1) {
            res->u32[i] = *(uint32_t*)&data_addr;
        } else {
            res->u32[i] = 0;
        }
    }
}

static inline void vpgatherdd256(YMM vsrc, YMM vindex, YMM *res, struct pt_regs *regs, uint8_t *modrm, uint8_t high_base, uint8_t ins_size) {
    int i;
    int cs;
    uint32_t mask_bit = 0;
    uint64_t data_addr = 0;
    YMM MASK;
    XMM vaddr;
    
    for (i = 0; i < 8; ++i) {
        mask_bit = vsrc.u32[i];
        cs = mask_bit >> 31 & 1;
        if (cs == 1) {
            MASK.u32[i] = 0xffffffff;
        } else {
            MASK.u32[i] = 0x00000000;
        }
    }
    
    for (i = 0; i < 8; ++i) {
        vaddr.a32[0] = vindex.a32[i];
        vaddr.a32[1] = 0xffffffff;
        data_addr = vmaddrs(regs, modrm, high_base, vaddr, ins_size);
        
        mask_bit = MASK.u32[i];
        cs = mask_bit & 1;
        if (cs == 1) {
            res->u32[i] = *(uint32_t*)&data_addr;
        } else {
            res->u32[i] = 0;
        }
    }
}

static inline void vpgatherqq128(XMM vsrc, XMM vindex, XMM *res, struct pt_regs *regs, uint8_t *modrm, uint8_t high_base, uint8_t ins_size) {
    int i;
    int cs;
    uint64_t mask_bit = 0;
    uint64_t data_addr = 0;
    XMM MASK;
    XMM vaddr;
    
    for (i = 0; i < 2; ++i) {
        mask_bit = vsrc.u64[i];
        cs = mask_bit >> 63 & 1;
        if (cs == 1) {
            MASK.u64[i] = 0xffffffffffffffff;
        } else {
            MASK.u64[i] = 0x0000000000000000;
        }
    }
    
    for (i = 0; i < 2; ++i) {
        vaddr.a64[0] = vindex.a64[i];
        data_addr = vmaddrs(regs, modrm, high_base, vaddr, ins_size);
        
        mask_bit = MASK.u64[i];
        cs = mask_bit & 1;
        if (cs == 1) {
            res->u64[i] = *(uint64_t*)&data_addr;
        } else {
            res->u64[i] = 0;
        }
    }
}

static inline void vpgatherqq256(YMM vsrc, YMM vindex, YMM *res, struct pt_regs *regs, uint8_t *modrm, uint8_t high_base, uint8_t ins_size) {
    int i;
    int cs;
    uint64_t mask_bit = 0;
    uint64_t data_addr = 0;
    YMM MASK;
    XMM vaddr;
    
    for (i = 0; i < 4; ++i) {
        mask_bit = vsrc.u64[i];
        cs = mask_bit >> 63 & 1;
        if (cs == 1) {
            MASK.u64[i] = 0xffffffffffffffff;
        } else {
            MASK.u64[i] = 0x0000000000000000;
        }
    }
    
    for (i = 0; i < 4; ++i) {
        vaddr.a64[0] = vindex.a64[i];
        data_addr = vmaddrs(regs, modrm, high_base, vaddr, ins_size);
        
        mask_bit = MASK.u64[i];
        cs = mask_bit & 1;
        if (cs == 1) {
            res->u64[i] = *(uint64_t*)&data_addr;
        } else {
            res->u64[i] = 0;
        }
    }
}

static inline void vpgatherqd128(XMM vsrc, XMM vindex, XMM *res, struct pt_regs *regs, uint8_t *modrm, uint8_t high_base, uint8_t ins_size) {
    int i;
    int cs;
    uint32_t mask_bit = 0;
    uint64_t data_addr = 0;
    XMM MASK;
    XMM vaddr;
    
    for (i = 0; i < 4; ++i) {
        mask_bit = vsrc.u32[i];
        cs = mask_bit >> 31 & 1;
        if (cs == 1) {
            MASK.u32[i] = 0xffffffff;
        } else {
            MASK.u32[i] = 0x00000000;
        }
    }
    
    for (i = 0; i < 2; ++i) {
        vaddr.a64[0] = vindex.a64[i];
        data_addr = vmaddrs(regs, modrm, high_base, vaddr, ins_size);
        
        mask_bit = MASK.u32[i];
        cs = mask_bit & 1;
        if (cs == 1) {
            res->u32[i] = *(uint32_t*)&data_addr;
        } else {
            res->u32[i] = 0;
        }
    }
}

static inline void vpgatherqd256(XMM vsrc, YMM vindex, XMM *res, struct pt_regs *regs, uint8_t *modrm, uint8_t high_base, uint8_t ins_size) {
    int i;
    int cs;
    uint32_t mask_bit = 0;
    uint64_t data_addr = 0;
    XMM MASK;
    XMM vaddr;
    
    for (i = 0; i < 8; ++i) {
        mask_bit = vsrc.u32[i];
        cs = mask_bit >> 31 & 1;
        if (cs == 1) {
            MASK.u32[i] = 0xffffffff;
        } else {
            MASK.u32[i] = 0x00000000;
        }
    }
    
    for (i = 0; i < 4; ++i) {
        vaddr.a64[0] = vindex.a64[i];
        data_addr = vmaddrs(regs, modrm, high_base, vaddr, ins_size);
        
        mask_bit = MASK.u32[i];
        cs = mask_bit & 1;
        if (cs == 1) {
            res->u32[i] = *(uint32_t*)&data_addr;
        } else {
            res->u32[i] = 0;
        }
    }
}

// Float Values
static inline void vgatherdpd128(XMM vsrc, XMM vindex, XMM *res, struct pt_regs *regs, uint8_t *modrm, uint8_t high_base, uint8_t ins_size) {
    int i;
    int cs;
    uint64_t mask_bit = 0;
    uint64_t data_addr = 0;
    XMM MASK;
    XMM vaddr;
    XMM tmp;
    for (i = 0; i < 2; ++i) {
        mask_bit = vsrc.u64[i];
        cs = mask_bit >> 63 & 1;
        if (cs == 1) {
            MASK.u64[i] = 0xffffffffffffffff;
        } else {
            MASK.u64[i] = 0x0000000000000000;
        }
    }
    
    for (i = 0; i < 2; ++i) {
        vaddr.a32[0] = vindex.a32[i];
        vaddr.a32[1] = 0xffffffff;
        data_addr = vmaddrs(regs, modrm, high_base, vaddr, ins_size);
        
        mask_bit = MASK.u64[i];
        cs = mask_bit & 1;
        if (cs == 1) {
            tmp.u64[i] = *(uint64_t*)&data_addr;
        } else {
            tmp.u64[i] = 0;
        }
    }
    res->fa64[0] = tmp.fa64[0];
    res->fa64[1] = tmp.fa64[1];
}

static inline void vgatherdpd256(YMM vsrc, XMM vindex, YMM *res, struct pt_regs *regs, uint8_t *modrm, uint8_t high_base, uint8_t ins_size) {
    int i;
    int cs;
    uint64_t mask_bit = 0;
    uint64_t data_addr = 0;
    YMM MASK;
    XMM vaddr;
    YMM tmp;
    for (i = 0; i < 4; ++i) {
        mask_bit = vsrc.u64[i];
        cs = mask_bit >> 63 & 1;
        if (cs == 1) {
            MASK.u64[i] = 0xffffffffffffffff;
        } else {
            MASK.u64[i] = 0x0000000000000000;
        }
    }
    
    for (i = 0; i < 4; ++i) {
        vaddr.a32[0] = vindex.a32[i];
        vaddr.a32[1] = 0xffffffff;
        data_addr = vmaddrs(regs, modrm, high_base, vaddr, ins_size);
        
        mask_bit = MASK.u64[i];
        cs = mask_bit & 1;
        if (cs == 1) {
            tmp.u64[i] = *(uint64_t*)&data_addr;
        } else {
            tmp.u64[i] = 0;
        }
    }
    res->fa64[0] = tmp.fa64[0];
    res->fa64[1] = tmp.fa64[1];
    res->fa64[2] = tmp.fa64[2];
    res->fa64[3] = tmp.fa64[3];
}

static inline void vgatherdps128(XMM vsrc, XMM vindex, XMM *res, struct pt_regs *regs, uint8_t *modrm, uint8_t high_base, uint8_t ins_size) {
    int i;
    int cs;
    uint64_t mask_bit = 0;
    uint64_t data_addr = 0;
    XMM MASK;
    XMM vaddr;
    XMM tmp;
    for (i = 0; i < 4; ++i) {
        mask_bit = vsrc.u32[i];
        cs = mask_bit >> 31 & 1;
        if (cs == 1) {
            MASK.u32[i] = 0xffffffff;
        } else {
            MASK.u32[i] = 0x00000000;
        }
    }
    
    for (i = 0; i < 4; ++i) {
        vaddr.a32[0] = vindex.a32[i];
        vaddr.a32[1] = 0xffffffff;
        data_addr = vmaddrs(regs, modrm, high_base, vaddr, ins_size);
        
        mask_bit = MASK.u32[i];
        cs = mask_bit & 1;
        if (cs == 1) {
            tmp.u32[i] = *(uint32_t*)&data_addr;
        } else {
            tmp.u32[i] = 0;
        }
    }
    res->fa32[0] = tmp.fa32[0];
    res->fa32[1] = tmp.fa32[1];
    res->fa32[2] = tmp.fa32[3];
    res->fa32[3] = tmp.fa32[3];
}

static inline void vgatherdps256(YMM vsrc, YMM vindex, YMM *res, struct pt_regs *regs, uint8_t *modrm, uint8_t high_base, uint8_t ins_size) {
    int i;
    int cs;
    uint64_t mask_bit = 0;
    uint64_t data_addr = 0;
    YMM MASK;
    XMM vaddr;
    YMM tmp;
    for (i = 0; i < 8; ++i) {
        mask_bit = vsrc.u32[i];
        cs = mask_bit >> 31 & 1;
        if (cs == 1) {
            MASK.u32[i] = 0xffffffff;
        } else {
            MASK.u32[i] = 0x00000000;
        }
    }
    
    for (i = 0; i < 8; ++i) {
        vaddr.a32[0] = vindex.a32[i];
        vaddr.a32[1] = 0xffffffff;
        data_addr = vmaddrs(regs, modrm, high_base, vaddr, ins_size);
        
        mask_bit = MASK.u32[i];
        cs = mask_bit & 1;
        if (cs == 1) {
            tmp.u32[i] = *(uint32_t*)&data_addr;
        } else {
            tmp.u32[i] = 0;
        }
    }
    res->fa32[0] = tmp.fa32[0];
    res->fa32[1] = tmp.fa32[1];
    res->fa32[2] = tmp.fa32[3];
    res->fa32[3] = tmp.fa32[3];
    res->fa32[4] = tmp.fa32[4];
    res->fa32[5] = tmp.fa32[5];
    res->fa32[6] = tmp.fa32[6];
    res->fa32[7] = tmp.fa32[7];
}

static inline void vgatherqpd128(XMM vsrc, XMM vindex, XMM *res, struct pt_regs *regs, uint8_t *modrm, uint8_t high_base, uint8_t ins_size) {
    int i;
    int cs;
    uint64_t mask_bit = 0;
    uint64_t data_addr = 0;
    XMM MASK;
    XMM vaddr;
    XMM tmp;
    for (i = 0; i < 2; ++i) {
        mask_bit = vsrc.u64[i];
        cs = mask_bit >> 63 & 1;
        if (cs == 1) {
            MASK.u64[i] = 0xffffffffffffffff;
        } else {
            MASK.u64[i] = 0x0000000000000000;
        }
    }
    
    for (i = 0; i < 2; ++i) {
        vaddr.a64[0] = vindex.a64[i];
        data_addr = vmaddrs(regs, modrm, high_base, vaddr, ins_size);
        
        mask_bit = MASK.u64[i];
        cs = mask_bit & 1;
        if (cs == 1) {
            tmp.u64[i] = *(uint64_t*)&data_addr;
        } else {
            tmp.u64[i] = 0;
        }
    }
    res->fa64[0] = tmp.fa64[0];
    res->fa64[1] = tmp.fa64[1];
}

static inline void vgatherqpd256(YMM vsrc, YMM vindex, YMM *res, struct pt_regs *regs, uint8_t *modrm, uint8_t high_base, uint8_t ins_size) {
    int i;
    int cs;
    uint64_t mask_bit = 0;
    uint64_t data_addr = 0;
    YMM MASK;
    XMM vaddr;
    YMM tmp;
    for (i = 0; i < 4; ++i) {
        mask_bit = vsrc.u64[i];
        cs = mask_bit >> 63 & 1;
        if (cs == 1) {
            MASK.u64[i] = 0xffffffffffffffff;
        } else {
            MASK.u64[i] = 0x0000000000000000;
        }
    }
    
    for (i = 0; i < 4; ++i) {
        vaddr.a64[0] = vindex.a64[i];
        data_addr = vmaddrs(regs, modrm, high_base, vaddr, ins_size);
        
        mask_bit = MASK.u64[i];
        cs = mask_bit & 1;
        if (cs == 1) {
            tmp.u64[i] = *(uint64_t*)&data_addr;
        } else {
            tmp.u64[i] = 0;
        }
    }
    res->fa64[0] = tmp.fa64[0];
    res->fa64[1] = tmp.fa64[1];
    res->fa64[2] = tmp.fa64[2];
    res->fa64[3] = tmp.fa64[3];
}

static inline void vgatherqps128(XMM vsrc, XMM vindex, XMM *res, struct pt_regs *regs, uint8_t *modrm, uint8_t high_base, uint8_t ins_size) {
    int i;
    int cs;
    uint64_t mask_bit = 0;
    uint64_t data_addr = 0;
    XMM MASK;
    XMM vaddr;
    XMM tmp;
    for (i = 0; i < 4; ++i) {
        mask_bit = vsrc.u32[i];
        cs = mask_bit >> 31 & 1;
        if (cs == 1) {
            MASK.u32[i] = 0xffffffff;
        } else {
            MASK.u32[i] = 0x00000000;
        }
    }
    
    for (i = 0; i < 2; ++i) {
        vaddr.a64[0] = vindex.a64[i];
        data_addr = vmaddrs(regs, modrm, high_base, vaddr, ins_size);
        
        mask_bit = MASK.u32[i];
        cs = mask_bit & 1;
        if (cs == 1) {
            tmp.u32[i] = *(uint32_t*)&data_addr;
        } else {
            tmp.u32[i] = 0;
        }
    }
    res->fa32[0] = tmp.fa32[0];
    res->fa32[1] = tmp.fa32[1];
}

static inline void vgatherqps256(XMM vsrc, YMM vindex, XMM *res, struct pt_regs *regs, uint8_t *modrm, uint8_t high_base, uint8_t ins_size) {
    int i;
    int cs;
    uint64_t mask_bit = 0;
    uint64_t data_addr = 0;
    XMM MASK;
    XMM vaddr;
    XMM tmp;
    for (i = 0; i < 8; ++i) {
        mask_bit = vsrc.u32[i];
        cs = mask_bit >> 31 & 1;
        if (cs == 1) {
            MASK.u32[i] = 0xffffffff;
        } else {
            MASK.u32[i] = 0x00000000;
        }
    }
    
    for (i = 0; i < 4; ++i) {
        vaddr.a64[0] = vindex.a64[i];
        data_addr = vmaddrs(regs, modrm, high_base, vaddr, ins_size);
        
        mask_bit = MASK.u32[i];
        cs = mask_bit & 1;
        if (cs == 1) {
            tmp.u32[i] = *(uint32_t*)&data_addr;
        } else {
            tmp.u32[i] = 0;
        }
    }
    res->fa32[0] = tmp.fa32[0];
    res->fa32[1] = tmp.fa32[1];
    res->fa32[2] = tmp.fa32[3];
    res->fa32[3] = tmp.fa32[3];
}

#endif /* vgather_h */
