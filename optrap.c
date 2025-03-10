//
//  optrap.c
//  opemu
//
//  Created by Meowthra on 2019/5/23.
//  Copyright © 2019 Meowthra. All rights reserved.
//  Made in Taiwan.

#include "optrap.h"

#include "aes.h"
#include "avx.h"
#include "vgather.h"
#include "fma.h"
#include "f16c.h"
#include "bmi.h"
#include "vsse.h"
#include "vsse2.h"
#include "vsse3.h"
#include "vssse3.h"
#include "vsse41.h"
#include "vsse42.h"

int opemu_utrap_2(struct pt_regs *regs) {

    int bytes_skip = 0;

#ifdef __x86_64__
    if (is_saved_state64(regs)) {
        uint64_t addr = 0;
        addr = regs->ip;
        uint8_t code_buffer[15];
        memset(code_buffer, 0, sizeof(code_buffer));
        for(size_t i = 0; i < sizeof(code_buffer); ++i)
        {
            unsigned char opcode;
            if (user_mode(regs)) {
                if (get_user(opcode, (unsigned char __user *) (((uint8_t *)addr) + i)))
                    break;
            } else {
                if (get_kernel_nofault(opcode, (((uint8_t *)addr) + i)))
                    break;
            }
            code_buffer[i] = opcode;
        }

        //Enable REX Opcode Emulation
        bytes_skip = rex_ins(code_buffer, regs);
        
        //Enable VEX Opcode Emulation
        if (bytes_skip == 0) {
            bytes_skip = vex_ins(code_buffer, regs);
        }

        regs->ip += bytes_skip;

        if (!bytes_skip) {
            printk("OPEMU: invalid user opcode (64-bit): %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n", code_buffer[0], code_buffer[1], code_buffer[2], code_buffer[3], code_buffer[4], code_buffer[5], code_buffer[6], code_buffer[7], code_buffer[8], code_buffer[9]);
            return 0;
        }

    }
#endif

    if (is_saved_state32(regs)) {
        uint64_t addr = 0;
        addr = regs->ip;
        uint8_t code_buffer[15];
        memset(code_buffer, 0, sizeof(code_buffer));
        for(size_t i = 0; i < sizeof(code_buffer); ++i)
        {
            unsigned char opcode;
            if (user_mode(regs)) {
                if (get_user(opcode, (unsigned char __user *) (((uint8_t *)addr) + i)))
                    break;
            } else {
                if (get_kernel_nofault(opcode, (((uint8_t *)addr) + i)))
                    break;
            }
            code_buffer[i] = opcode;
        }

        //Enable REX Opcode Emulation
        bytes_skip = rex_ins(code_buffer, regs);
        
        //Enable VEX Opcode Emulation
        if (bytes_skip == 0) {
            bytes_skip = vex_ins(code_buffer, regs);
        }

        regs->ip += bytes_skip;

        if (!bytes_skip) {
            printk("OPEMU: invalid user opcode (32-bit): %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n", code_buffer[0], code_buffer[1], code_buffer[2], code_buffer[3], code_buffer[4], code_buffer[5], code_buffer[6], code_buffer[7], code_buffer[8], code_buffer[9]);
            return 0;
        }
    }

    return 1;
}


/** Runs the REX emulator. returns the number of bytes consumed. **/
int rex_ins(uint8_t *instruction, struct pt_regs *regs)
{
    uint8_t *bytep = instruction;
    uint8_t ins_size = 0;
    uint8_t modbyte = 0; //Calculate byte 0 - modrm long
    uint16_t reg_size = 64;
    uint8_t operand_size = 0;
    uint8_t leading_opcode = 0;
    uint8_t simd_prefix = 0; //NP
    uint8_t addrs32 = 0; //Address-size override prefix (Legacy Prefixes 0x67)

    uint8_t high_reg = 0;
    uint8_t high_index = 0;
    uint8_t high_base = 0;
 
    uint8_t REX_W = 0; //REX.W
    uint8_t REX_R = 0; //REX.R
    uint8_t REX_X = 0; //REX.X
    uint8_t REX_B = 0; //REX.B

    
    // Legacy Prefixes (SIMD prefix)
    if (*bytep == 0x66) {
        reg_size = 128;
        simd_prefix = 1;
        bytep++;
        ins_size++;
        modbyte++;
    } else if (*bytep == 0xF3) {
        reg_size = 128;
        simd_prefix = 2;
        bytep++;
        ins_size++;
        modbyte++;
    } else if (*bytep == 0xF2) {
        reg_size = 128;
        simd_prefix = 3;
        bytep++;
        ins_size++;
        modbyte++;
    } else if (*bytep == 0x67) {
        addrs32 = 1;
        bytep++;
        ins_size++;
        modbyte++;
    } else if ( (*bytep == 0x26) || (*bytep == 0x2E) || (*bytep == 0x36) || (*bytep == 0x3E) ||  (*bytep == 0x64) || (*bytep == 0x65) ) {
        bytep++;
        ins_size++;
        modbyte++;
    }
    
    //REX Prefix
    if ((*bytep & 0xF0) == 0x40) {
        uint8_t *REX_BYTE = &bytep[0];
        REX_W = (*REX_BYTE >> 3) & 1;         //REX.W
        REX_R = (*REX_BYTE >> 2) & 1;         //REX.R
        REX_X = (*REX_BYTE >> 1) & 1;         //REX.X
        REX_B = *REX_BYTE & 1;                //REX.B

        bytep++;
        ins_size++;
        modbyte++;
    }
    
    if (REX_W == 1)
        operand_size = 64;
    else
        operand_size = 32;

    if (REX_R == 1) {
        if (reg_size == 128) {
            high_reg = 1;
        }
    }
    
    if (REX_X == 1) high_index = 1;
    if (REX_B == 1) high_base = 1;

    // leading opcode
    if (bytep[0] == 0x0F && bytep[1] == 0x3A) {
        leading_opcode = 3; //0F3A
        bytep+=2;
        ins_size+=2;
        modbyte+=2;
    } else if (bytep[0] == 0x0F && bytep[1] == 0x38) {
        leading_opcode = 2; //0F38
        bytep+=2;
        ins_size+=2;
        modbyte+=2;
    } else if (bytep[0] == 0x0F) {
        leading_opcode = 1; //0F
        bytep++;
        ins_size++;
        modbyte++;
    }

    /* opcode */
    uint8_t opcode = bytep[0];
    uint8_t *modrm = &bytep[1];
    
    bytep++;
    ins_size++;
    modbyte++;

    // AES Instruction set
    ins_size = aes_instruction(regs, opcode, modrm, high_reg, high_index, high_base, reg_size, operand_size, leading_opcode, simd_prefix, bytep, ins_size, modbyte);
    

    return ins_size;
}


/** Runs the VEX emulator. returns the number of bytes consumed. **/
int vex_ins(uint8_t *instruction, struct pt_regs *regs)
{
    uint8_t *bytep = instruction;
    uint8_t ins_size = 0;
    uint8_t modbyte = 0; //Calculate byte 0 - modrm long
    //const char* opcodename;
    uint8_t addrs32 = 0; //Address-size override prefix (Legacy Prefixes 0x67)

    uint16_t reg_size = 0;
    uint8_t operand_size = 0;
    uint8_t leading_opcode = 0;
    uint8_t simd_prefix = 0;
    //uint8_t op_type = 0;

    uint8_t high_reg = 0;
    uint8_t high_index = 0;
    uint8_t high_base = 0;
    uint8_t vexreg = 0;

    uint8_t VEX_R = 0; //VEX.R
    uint8_t VEX_X = 0; //VEX.X
    uint8_t VEX_B = 0; //VEX.B
    uint8_t VEX_M = 0; //VEX.mmmmm
    uint8_t VEX_W = 0; //VEX.W
    uint8_t VEX_V = 0; //VEX.vvvv
    uint8_t VEX_L = 0; //VEX.L
    uint8_t VEX_P = 0; //VEX.pp
    
    // Legacy Prefixes
    if (*bytep == 0x67) {
        addrs32 = 1;
        bytep++;
        ins_size++;
        modbyte++;
    }
    
    // VEX Prefixes
    if(*bytep == 0xC4) {
        uint8_t *VEX_BYTE1 = &bytep[1];
        uint8_t *VEX_BYTE2 = &bytep[2];
        VEX_R = *VEX_BYTE1 >> 0x7;         //VEX.R
        VEX_X = (*VEX_BYTE1 >> 0x6) & 0x1; //VEX.X
        VEX_B = (*VEX_BYTE1 >> 5) & 0x1;   //VEX.B
        VEX_M = *VEX_BYTE1 & 0x1F;         //VEX.mmmmm
        VEX_W = *VEX_BYTE2 >> 7;           //VEX.W
        VEX_V = (*VEX_BYTE2 >> 3) & 0xF;   //VEX.vvvv
        VEX_L = (*VEX_BYTE2 >> 2) & 0x1;   //VEX.L
        VEX_P = *VEX_BYTE2 & 0x3;          //VEX.pp
        
        bytep+=3;
        ins_size+=3;
        modbyte+=3;
    } else {
        if(*bytep == 0xC5) {
            uint8_t *VEX_BYTE1 = &bytep[1];
            VEX_R = *VEX_BYTE1 >> 0x7;        //VEX.R
            VEX_V = (*VEX_BYTE1 >> 3) & 0xF;  //VEX.vvvv
            VEX_L = (*VEX_BYTE1 >> 2) & 0x1;  //VEX.L
            VEX_P = *VEX_BYTE1 & 0x3;         //VEX.pp
            
            VEX_X = 1;
            VEX_B = 1;
            //VEX_W = 1;

            bytep+=2;
            ins_size+=2;
            modbyte+=2;
        } else {
            return 0;
        }
    }
    
    if (VEX_R == 0) high_reg = 1;
    if (VEX_X == 0) high_index = 1;
    if (VEX_B == 0) high_base = 1;

    //L0=128bit XMM / L1=256bit YMM
    if (VEX_L == 1)
        reg_size = 256;
    else
        reg_size = 128;

    if (VEX_W == 1)
        operand_size = 64;
    else
        operand_size = 32;

    if (VEX_M == 1)
        leading_opcode = 1; //0F
    else if (VEX_M == 2)
        leading_opcode = 2; //0F38
    else if (VEX_M == 3)
        leading_opcode = 3; //0F3A
    else
        leading_opcode = 1; //0F

    if (VEX_P == 1)
        simd_prefix = 1; //66
    else if (VEX_P == 2)
        simd_prefix = 2; //F3
    else if (VEX_P == 3)
        simd_prefix = 3; //F2
    else
        simd_prefix = 0; //None

    /* VEX.vvvv Register */
    if (VEX_V == 15) vexreg = 0;
    else if (VEX_V == 14) vexreg = 1;
    else if (VEX_V == 13) vexreg = 2;
    else if (VEX_V == 12) vexreg = 3;
    else if (VEX_V == 11) vexreg = 4;
    else if (VEX_V == 10) vexreg = 5;
    else if (VEX_V == 9) vexreg = 6;
    else if (VEX_V == 8) vexreg = 7;
    else if (VEX_V == 7) vexreg = 8;
    else if (VEX_V == 6) vexreg = 9;
    else if (VEX_V == 5) vexreg = 10;
    else if (VEX_V == 4) vexreg = 11;
    else if (VEX_V == 3) vexreg = 12;
    else if (VEX_V == 2) vexreg = 13;
    else if (VEX_V == 1) vexreg = 14;
    else if (VEX_V == 0) vexreg = 15;

    /* opcode */
    uint8_t opcode = bytep[0];
    uint8_t *modrm = &bytep[1];
    //uint8_t imm;

    bytep++;
    ins_size++;
    modbyte++;

    //uint8_t modreg = (*modrm >> 3) & 0x7;

    // VAES Instruction set
    uint8_t ins_size_2 = vaes_instruction(regs, vexreg, opcode, modrm, high_reg, high_index, high_base, reg_size, operand_size, leading_opcode, simd_prefix, bytep, ins_size, modbyte);

    // AVX / AVX2 Instruction set
    if (ins_size_2 == 0) {
        ins_size_2 = avx_instruction(regs, vexreg, opcode, modrm, high_reg, high_index, high_base, reg_size, operand_size, leading_opcode, simd_prefix, bytep, ins_size, modbyte);
    }

    // AVX Gather Instruction set
    if (ins_size_2 == 0) {
        ins_size_2 = vgather_instruction(regs, vexreg, opcode, modrm, high_reg, high_index, high_base, reg_size, operand_size, leading_opcode, simd_prefix, bytep, ins_size, modbyte);
    }

    // FMA Instruction set
    if (ins_size_2 == 0) {
        ins_size_2 = fma_instruction(regs, vexreg, opcode, modrm, high_reg, high_index, high_base, reg_size, operand_size, leading_opcode, simd_prefix, bytep, ins_size, modbyte);
    }

    // F16C Instruction set
    if (ins_size_2 == 0) {
        ins_size_2 = f16c_instruction(regs, vexreg, opcode, modrm, high_reg, high_index, high_base, reg_size, leading_opcode, simd_prefix, bytep, ins_size, modbyte);
    }

    // BMI1/2 Instruction set
    if (ins_size_2 == 0) {
    	ins_size_2 = bmi_instruction(regs, vexreg, opcode, modrm, high_reg, high_index, high_base, operand_size, leading_opcode, simd_prefix, bytep, ins_size, modbyte);
    }
    
    // VSSE Instruction set
    if (ins_size_2 == 0) {
        ins_size_2 = vsse_instruction(regs, vexreg, opcode, modrm, high_reg, high_index, high_base, reg_size, operand_size, leading_opcode, simd_prefix, bytep, ins_size, modbyte);
    }

    // VSSE2 Instruction set
    if (ins_size_2 == 0) {
        ins_size_2 = vsse2_instruction(regs, vexreg, opcode, modrm, high_reg, high_index, high_base, reg_size, operand_size, leading_opcode, simd_prefix, bytep, ins_size, modbyte);
    }

    // VSSE3 Instruction set
    if (ins_size_2 == 0) {
        ins_size_2 = vsse3_instruction(regs, vexreg, opcode, modrm, high_reg, high_index, high_base, reg_size, operand_size, leading_opcode, simd_prefix, bytep, ins_size, modbyte);
    }

    // VSSSE3 Instruction set
    if (ins_size_2 == 0) {
        ins_size_2 = vssse3_instruction(regs, vexreg, opcode, modrm, high_reg, high_index, high_base, reg_size, operand_size, leading_opcode, simd_prefix, bytep, ins_size, modbyte);
    }

    // VSSE4.1 Instruction set
    if (ins_size_2 == 0) {
        ins_size_2 = vsse41_instruction(regs, vexreg, opcode, modrm, high_reg, high_index, high_base, reg_size, operand_size, leading_opcode, simd_prefix, bytep, ins_size, modbyte);
    }

    // VSSE4.2 Instruction set
    if (ins_size_2 == 0) {
        ins_size_2 = vsse42_instruction(regs, vexreg, opcode, modrm, high_reg, high_index, high_base, reg_size, operand_size, leading_opcode, simd_prefix, bytep, ins_size, modbyte);
    }

    return ins_size_2;
}

/*********************************************************/
int get_consumed(uint8_t *ModRM) {
    uint8_t mod = *ModRM >> 6; // ModRM.mod
    uint8_t num_src = *ModRM & 0x7; // ModRM.r/m (SRC1:register or memory)
    int consumed = 1; //modrm byte +1

    if (mod == 0) {
        if (num_src == 4) {
            //sib
            uint8_t *SIB = (uint8_t *)&ModRM[1];
            uint8_t base = *SIB & 0x7;
            if (base == 5) {
                consumed += 5;
            } else {
                consumed++;
            }
        } else {
            if (num_src == 5) {
                consumed += 4;
            }
        }
    }

    if (mod == 1) {
        if (num_src == 4) {
            consumed+= 2;
        } else {
            consumed++;
        }
    }

    if (mod == 2) {
        if (num_src == 4) {
            consumed += 5;
        } else {
            consumed += 4;
        }
    }

    return consumed;
}

/*********************************************************
 *** Get the register or memory address value.         ***
 *********************************************************/
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
                 uint64_t *rmaddrs
                 )
{
    uint8_t mod = *modrm >> 6; // ModRM.mod
    uint8_t num_dst = (*modrm >> 3) & 0x7; // ModRM.reg (DEST)
    uint8_t num_src = *modrm & 0x7; // ModRM.r/m (SRC1:register or memory)
    uint8_t bytelen = 0;

#ifdef __x86_64__
    if (is_saved_state64(regs)) {
        
        if (high_reg) num_dst += 8;
        if (high_base) num_src += 8;
        
        _store_m64(vexreg, (M64*)vsrc, regs);
        _store_m64(num_dst, (M64*)dst, regs);
        
        if(mod == 3) //mod field = 11b
        {
            _store_m64(num_src, (M64*)src, regs);
            
        } else {
            // Get the Mod.R/M memory address value.
            uint64_t maddr = 0;
            maddr = addressing64(regs, modrm, mod, num_src, high_index, high_base, modbyte, bytelen);
            *rmaddrs = maddr;            
            copy_from_any_memory((char*) &((M64*)src)->u64, (char*) maddr, 8, regs);
        }

    }
#endif
    if (is_saved_state32(regs)) {
        _store_m32(vexreg, (M32*)vsrc, regs);
        _store_m32(num_dst, (M32*)dst, regs);
        
        if(mod == 3) //mod field = 11b
        {
            _store_m32(num_src, (M32*)src, regs);
            
        } else {
            uint32_t maddr = 0;
            // Get the Mod.R/M memory address value.
            maddr = addressing32(regs, modrm, mod, num_src, high_index, high_base, modbyte, bytelen);
            *rmaddrs = maddr;
            copy_from_any_memory((char*) &((M32*)src)->u32, (char*) maddr, 4, regs);
        }
    }
}

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
                 )
{
    
    /*** ModRM Is First Addressing Modes ***/
    /*** SIB Is Second Addressing Modes ***/
    uint8_t mod = *modrm >> 6; // ModRM.mod
    uint8_t num_dst = (*modrm >> 3) & 0x7; // ModRM.reg (DEST)
    uint8_t num_src = *modrm & 0x7; // ModRM.r/m (SRC1:register or memory)
    uint8_t bytelen = 0;
    
    if (high_reg) num_dst += 8;
    if (high_base) num_src += 8;
    
    if (reg_size == 128)
        _store_xmm(num_dst, (XMM*)dst);
    else
        _store_mmx(num_dst, (MM*)dst);

    if(mod == 3) //mod field = 11b
    {
        if (rm_size == 128)
            _store_xmm(num_src, (XMM*)src);
        else
            _store_mmx(num_src, (MM*)src);

    } else {
        // Get the Mod.R/M memory address value.
#ifdef __x86_64__
        if (is_saved_state64(regs)) {
            uint64_t maddr = 0;
            maddr = addressing64(regs, modrm, mod, num_src, high_index, high_base, modbyte, bytelen);
            *rmaddrs = maddr;
            if (rm_size == 128) {
                copy_from_any_memory((char*) &((XMM*)src)->u128, (char*) maddr, 16, regs);
            } else {
                copy_from_any_memory((char*) &((MM*)src)->u64, (char*) maddr, 8, regs);
            }
        }
#endif
        if (is_saved_state32(regs)) {
            uint32_t maddr = 0;
            maddr = addressing32(regs, modrm, mod, num_src, high_index, high_base, modbyte, bytelen);
            *rmaddrs = maddr;
            if (rm_size == 128) {
                copy_from_any_memory((char*) &((XMM*)src)->u128, (char*) maddr, 16, regs);
            } else {
                copy_from_any_memory((char*) &((MM*)src)->u64, (char*) maddr, 8, regs);
            }
        }
    }
}

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
                 uint64_t *rmaddrs
                 )
{
    
    /*** ModRM Is First Addressing Modes ***/
    /*** SIB Is Second Addressing Modes ***/
    uint8_t mod = *modrm >> 6; // ModRM.mod
    uint8_t num_dst = (*modrm >> 3) & 0x7; // ModRM.reg (DEST)
    uint8_t num_src = *modrm & 0x7; // ModRM.r/m (SRC1:register or memory)
    uint8_t bytelen = 0;
    
    if (high_reg) num_dst += 8;
    if (high_base) num_src += 8;

    if( reg_size == 256)
        _store_ymm(vexreg, (YMM*)vsrc);
    else {
        if (reg_size == 128)
            _store_xmm(vexreg, (XMM*)vsrc);
        else {
            _store_mmx(vexreg, (MM*)vsrc);
        }
    }
    
    if (reg_size == 256)
        _store_ymm(num_dst, (YMM*)dst);
    else {
        if (reg_size == 128)
            _store_xmm(num_dst, (XMM*)dst);
        else {
            _store_mmx(num_dst, (MM*)dst);
        }
    }

    if(mod == 3) //mod field = 11b
    {
        if (rm_size == 256)
            _store_ymm(num_src, (YMM*)src);
        else {
            if(rm_size == 128)
                _store_xmm(num_src, (XMM*)src);
            else {
                _store_mmx(num_src, (MM*)src);
            }
        }
    } else {
        // Get the Mod.R/M memory address value.
#ifdef __x86_64__
        if (is_saved_state64(regs)) {
            uint64_t maddr = 0;
            maddr = addressing64(regs, modrm, mod, num_src, high_index, high_base, modbyte, bytelen);
            *rmaddrs = maddr;
            if(rm_size == 256) {
                copy_from_any_memory((char*) &((YMM*)src)->u256, (char*) maddr, 32, regs);
            } else if(rm_size == 128) {
                copy_from_any_memory((char*) &((XMM*)src)->u128, (char*) maddr, 16, regs);
            } else if(rm_size == 64) {
                copy_from_any_memory((char*) &((MM*)src)->u64, (char*) maddr, 8, regs);
            } else if(rm_size == 32) {
                copy_from_any_memory((char*) &((MM*)src)->u32, (char*) maddr, 4, regs);
            }
        }
#endif
        if (is_saved_state32(regs)) {
            uint32_t maddr = 0;
            maddr = addressing32(regs, modrm, mod, num_src, high_index, high_base, modbyte, bytelen);
            *rmaddrs = maddr;
            if(rm_size == 256) {
                copy_from_any_memory((char*) &((YMM*)src)->u256, (char*) maddr, 32, regs);
            } else if(rm_size == 128) {
                copy_from_any_memory((char*) &((XMM*)src)->u128, (char*) maddr, 16, regs);
            } else if(rm_size == 64) {
                copy_from_any_memory((char*) &((MM*)src)->u64, (char*) maddr, 8, regs);
            } else if(rm_size == 32) {
                copy_from_any_memory((char*) &((MM*)src)->u32, (char*) maddr, 4, regs);
            }
        }
        
    }
}

uint64_t addressing64(
                  struct pt_regs *regs,
                  uint8_t *modrm,
                  uint8_t mod,
                  uint8_t num_src,
                  uint8_t high_index,
                  uint8_t high_base,
                  uint8_t modbyte,
                  uint8_t bytelen
                  )
{
    uint64_t address = 0;
    uint64_t reg_sel[16];
    reg_sel[0] = regs->ax;
    reg_sel[1] = regs->cx;
    reg_sel[2] = regs->dx;
    reg_sel[3] = regs->bx;
    reg_sel[4] = regs->sp;
    reg_sel[5] = regs->bp;
    reg_sel[6] = regs->si;
    reg_sel[7] = regs->di;
    reg_sel[8] = regs->r8;
    reg_sel[9] = regs->r9;
    reg_sel[10] = regs->r10;
    reg_sel[11] = regs->r11;
    reg_sel[12] = regs->r12;
    reg_sel[13] = regs->r13;
    reg_sel[14] = regs->r14;
    reg_sel[15] = regs->r15;

    // MOD 0
    if (mod == 0) {
        //SIB START
        if ((num_src == 4) || (num_src == 12)) { //SIB
            uint8_t *sib = (uint8_t *)&modrm[1]; //Second Addressing Modes
            uint8_t scale = *sib >> 6; //SIB Scale field
            uint8_t base = *sib & 0x7; //SIB Base
            uint8_t index = (*sib >> 3) & 0x7; //SIB Index
            uint8_t factor = 0; //Scaling factor
            if (scale == 0) factor = 1;
            else if (scale == 1) factor = 2;
            else if (scale == 2) factor = 4;
            else if (scale == 3) factor = 8;
            if (high_index) index += 8;
            if (high_base) base += 8;
            
            if (index == 4) { //INDEX4
                if ((base == 5) || (base == 13)) {
                    //[DISP32] MODRM + 5
                    bytelen = modbyte + 5;
                    address = regs->ip + bytelen + *((int32_t*)&modrm[2]);
                    //consumed += 5;
                } else {
                    //[BASE] MODRM + 1
                    address = reg_sel[base];
                    //consumed++;
                }
            } else { //Not INDEX4
                if ((base == 5) || (base == 13)) {
                    //[DISP32 + (INDEX * FACTOR)] MODRM + 5
                    bytelen = modbyte + 5;
                    address = (regs->ip + bytelen + *((int32_t*)&modrm[2])) + (reg_sel[index] * factor);
                    //consumed += 5;
                } else {
                    //[BASE + (INDEX * FACTOR)] MODRM + 1
                    address = reg_sel[base] + (reg_sel[index] * factor);
                    //consumed++;
                }
            }
        }
        //SIB END
        else {
            if ((num_src == 5) || (num_src == 13)) {
                //[DISP32] MODRM + 4
                bytelen = modbyte + 4;
                address = regs->ip + bytelen + *((int32_t*)&modrm[1]);
                //consumed += 4;
            } else {
                //[SRC]
                address = reg_sel[num_src];
            }
        }
    }
    
    // MOD 1
    if (mod == 1) {
        //SIB START
        if ((num_src == 4) || (num_src == 12)) { //SIB
            uint8_t *sib = (uint8_t *)&modrm[1]; //Second Addressing Modes
            uint8_t scale = *sib >> 6; //SIB Scale field
            uint8_t base = *sib & 0x7; //SIB Base
            uint8_t index = (*sib >> 3) & 0x7; //SIB Index
            uint8_t factor = 0; //Scaling factor
            if (scale == 0) factor = 1;
            else if (scale == 1) factor = 2;
            else if (scale == 2) factor = 4;
            else if (scale == 3) factor = 8;
            if (high_index) index += 8;
            if (high_base) base += 8;
            
            if (index == 4) { //INDEX4
                //[BASE + DISP8] MODRM + 2
                address = reg_sel[base] + *((int8_t*)&modrm[2]);
                //consumed+= 2;
            } else { //Not INDEX4
                //[BASE + (INDEX * FACTOR) + DISP8] MODRM + 2
                address = reg_sel[base] + (reg_sel[index] * factor) + *((int8_t*)&modrm[2]);
                //consumed+= 2;
            }
        }
        //SIB END
        else {
            //[SRC + DISP8] MODRM + 1
            address = reg_sel[num_src] + *((int8_t*)&modrm[1]);
            //consumed++;
        }
    }
    
    // MOD 2
    if (mod == 2) {
        //SIB START
        if ((num_src == 4) || (num_src == 12)) { //SIB
            uint8_t *sib = (uint8_t *)&modrm[1]; //Second Addressing Modes
            uint8_t scale = *sib >> 6; //SIB Scale field
            uint8_t base = *sib & 0x7; //SIB Base
            uint8_t index = (*sib >> 3) & 0x7; //SIB Index
            uint8_t factor = 0; //Scaling factor
            if (scale == 0) factor = 1;
            else if (scale == 1) factor = 2;
            else if (scale == 2) factor = 4;
            else if (scale == 3) factor = 8;
            if (high_index) index += 8;
            if (high_base) base += 8;
            
            if (index == 4) { //INDEX4
                //[BASE + DISP32] MODRM + 5
                bytelen = modbyte + 5;
                address = reg_sel[base] + (regs->ip + bytelen + *((int32_t*)&modrm[2]));
                //consumed += 5;
            } else { //Not INDEX4
                //[BASE + (INDEX * FACTOR) + DISP32] MODRM + 5
                bytelen = modbyte + 5;
                address = reg_sel[base] + (reg_sel[index] * factor) + (regs->ip + bytelen + *((int32_t*)&modrm[2]));
                //consumed += 5;
            }
        }
        //SIB END
        else {
            //[SRC + DISP32] MODRM + 4
            bytelen = modbyte + 4;
            address = reg_sel[num_src] + (regs->ip + bytelen + *((int32_t*)&modrm[1]));
            //consumed += 4;
        }
    }
    // 64-bit address
    return address;
}

uint32_t addressing32(
                  struct pt_regs *regs,
                  uint8_t *modrm,
                  uint8_t mod,
                  uint8_t num_src,
                  uint8_t high_index,
                  uint8_t high_base,
                  uint8_t modbyte,
                  uint8_t bytelen
                  )
{
    uint32_t address = 0;
    uint32_t reg_sel[8];
    reg_sel[0] = regs->ax;
    reg_sel[1] = regs->cx;
    reg_sel[2] = regs->dx;
    reg_sel[3] = regs->bx;
    reg_sel[4] = regs->sp;
    reg_sel[5] = regs->bp;
    reg_sel[6] = regs->si;
    reg_sel[7] = regs->di;

    // MOD 0
    if (mod == 0) {
        //SIB START
        if (num_src == 4) { //SIB
            uint8_t *sib = (uint8_t *)&modrm[1]; //Second Addressing Modes
            uint8_t scale = *sib >> 6; //SIB Scale field
            uint8_t base = *sib & 0x7; //SIB Base
            uint8_t index = (*sib >> 3) & 0x7; //SIB Index
            uint8_t factor = 0; //Scaling factor
            if (scale == 0) factor = 1;
            else if (scale == 1) factor = 2;
            else if (scale == 2) factor = 4;
            else if (scale == 3) factor = 8;
            
            if (index == 4) { //INDEX4
                if (base == 5) {
                    //[DISP32] MODRM + 5
                    address = *((uint32_t*)&modrm[2]);
                    //consumed += 5;
                } else {
                    //[BASE] MODRM + 1
                    address = reg_sel[base];
                    //consumed++;
                }
            } else { //Not INDEX4
                if (base == 5) {
                    //[DISP32 + (INDEX * FACTOR)] MODRM + 5
                    address = *((int32_t*)&modrm[2]) + (reg_sel[index] * factor);
                    //consumed += 5;
                } else {
                    //[BASE + (INDEX * FACTOR)] MODRM + 1
                    address = reg_sel[base] + (reg_sel[index] * factor);
                    //consumed++;
                }
            }
        }
        //SIB END
        else {
            if (num_src == 5) {
                //[DISP32] MODRM + 4
                address = *((uint32_t*)&modrm[1]);
                //consumed += 4;
            } else {
                //[SRC]
                address = reg_sel[num_src];
            }
        }
    }
    
    // MOD 1
    if (mod == 1) {
        //SIB START
        if (num_src == 4) { //SIB
            uint8_t *sib = (uint8_t *)&modrm[1]; //Second Addressing Modes
            uint8_t scale = *sib >> 6; //SIB Scale field
            uint8_t base = *sib & 0x7; //SIB Base
            uint8_t index = (*sib >> 3) & 0x7; //SIB Index
            uint8_t factor = 0; //Scaling factor
            if (scale == 0) factor = 1;
            else if (scale == 1) factor = 2;
            else if (scale == 2) factor = 4;
            else if (scale == 3) factor = 8;
            
            if (index == 4) { //INDEX4
                //[BASE + DISP8] MODRM + 2
                address = reg_sel[base] + *((int8_t*)&modrm[2]);
                //consumed+= 2;
            } else { //Not INDEX4
                //[BASE + (INDEX * FACTOR) + DISP8] MODRM + 2
                address = reg_sel[base] + (reg_sel[index] * factor) + *((int8_t*)&modrm[2]);
                //consumed+= 2;
            }
        }
        //SIB END
        else {
            //[SRC + DISP8] MODRM + 1
            address = reg_sel[num_src] + *((int8_t*)&modrm[1]);
            //consumed++;
        }
    }
    
    // MOD 2
    if (mod == 2) {
        //SIB START
        if (num_src == 4) { //SIB
            uint8_t *sib = (uint8_t *)&modrm[1]; //Second Addressing Modes
            uint8_t scale = *sib >> 6; //SIB Scale field
            uint8_t base = *sib & 0x7; //SIB Base
            uint8_t index = (*sib >> 3) & 0x7; //SIB Index
            uint8_t factor = 0; //Scaling factor
            if (scale == 0) factor = 1;
            else if (scale == 1) factor = 2;
            else if (scale == 2) factor = 4;
            else if (scale == 3) factor = 8;
            
            if (index == 4) { //INDEX4
                //[BASE + DISP32] MODRM + 5
                address = reg_sel[base] + *((int32_t*)&modrm[2]);
                //consumed += 5;
            } else { //Not INDEX4
                //[BASE + (INDEX * FACTOR) + DISP32] MODRM + 5
                address = reg_sel[base] + (reg_sel[index] * factor) + *((int32_t*)&modrm[2]);
                //consumed += 5;
            }
        }
        //SIB END
        else {
            //[SRC + DISP32] MODRM + 4
            address = reg_sel[num_src] + *((int32_t*)&modrm[1]);
            //consumed += 4;
        }
    }
    // 32-bit address
    return address;
}

/*********************************************************
 *** AVX 2.0 Gather Instruction Addressing             ***
 *********************************************************/
uint64_t vmaddrs(struct pt_regs *regs,
                 uint8_t *modrm,
                 uint8_t high_base,
                 XMM vaddr,
                 uint8_t ins_size
                 )
{
    uint8_t mod = *modrm >> 6; // ModRM.mod
    uint64_t address = 0;
    int64_t vindex = 0;
    
    uint8_t *sib = (uint8_t *)&modrm[1];
    uint8_t scale = *sib >> 6;
    uint8_t base = *sib & 0x7;
    uint8_t factor = 0; //Scaling factor
    if (scale == 0) factor = 1;
    else if (scale == 1) factor = 2;
    else if (scale == 2) factor = 4;
    else if (scale == 3) factor = 8;
    
    if (high_base) base += 8;
    
    uint64_t reg_sel[16];
    reg_sel[0] = regs->ax;
    reg_sel[1] = regs->cx;
    reg_sel[2] = regs->dx;
    reg_sel[3] = regs->bx;
    reg_sel[4] = regs->sp;
    reg_sel[5] = regs->bp;
    reg_sel[6] = regs->si;
    reg_sel[7] = regs->di;
    reg_sel[8] = regs->r8;
    reg_sel[9] = regs->r9;
    reg_sel[10] = regs->r10;
    reg_sel[11] = regs->r11;
    reg_sel[12] = regs->r12;
    reg_sel[13] = regs->r13;
    reg_sel[14] = regs->r14;
    reg_sel[15] = regs->r15;
    
    vindex = vaddr.a64[0];
    
    if (mod == 0) {
        if ((base == 5) || (base == 13)) {
            address = (regs->ip + ins_size + *((int32_t*)&modrm[2])) + (vindex * factor);
        } else {
            address = reg_sel[base] + (vindex * factor);
        }
    }
    if (mod == 1) {
        address = reg_sel[base] + (vindex * factor) + *((int8_t*)&modrm[2]);
    }
    if (mod == 2) {
        address = reg_sel[base] + (vindex * factor) + (regs->ip + ins_size + *((int32_t*)&modrm[2]));
    }
    
    return address;
}
