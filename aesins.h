//
//  aesins.h
//  opemu
//
//  Created by Meowthra on 2019/5/24.
//  Copyright © 2019 Meowthra. All rights reserved.
//  Made in Taiwan.

#ifndef aesins_h
#define aesins_h

#include "optrap.h"

unsigned char GF28_Multi(unsigned char a, unsigned char b);
void MixColumns(XMM *block);
void InvMixColumns(XMM *block);

void SubBytes(XMM *block);
void InvSubBytes(XMM *block);

void ShiftRows(XMM *block);
void InvShiftRows(XMM *block);

uint32_t SubWord(uint32_t X);
uint32_t RotWord(uint32_t X);

#endif /* aesins_h */
