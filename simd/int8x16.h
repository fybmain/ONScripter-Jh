/* -*- C++ -*-
*
*  int8x16.h
*
*  Copyright (C) 2015 jh10001 <jh10001@live.cn>
*
*  This program is free software; you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation; either version 2 of the License, or
*  (at your option) any later version.
*
*  This program is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with this program; if not, write to the Free Software
*  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
#pragma once

#ifndef __SIMD_H__
#error "This file must be included through simd.h"
#endif

namespace simd {
  class uint8x16 {
#ifdef USE_SIMD_X86_SSE2
    __m128i v_;
#elif USE_SIMD_ARM_NEON
    uint8x16_t v_;
#endif
  public:
    uint8x16() = default;
    uint8x16(const uint8x16&) = default;
    uint8x16 &operator=(const uint8x16&) = default;
#ifdef USE_SIMD_X86_SSE2
    uint8x16(__m128i v) : v_(v) {};
    operator __m128i() const { return v_; }
#elif USE_SIMD_ARM_NEON
    uint8x16(uint8x16_t v) : v_(v) {};
    operator uint8x16_t() const { return v_; };
#endif
    static uint8x16 set(uint8_t rm1, uint8_t rm2, uint8_t rm3, uint8_t rm4) {
#ifdef USE_SIMD_X86_SSE2
      return _mm_set_epi8(rm4, rm3, rm2, rm1, rm4, rm3, rm2, rm1, rm4, rm3, rm2, rm1, rm4, rm3, rm2, rm1);
#elif USE_SIMD_ARM_NEON
      union {
        uint32_t v32[1];
        uint8_t v[4];
      };
      v[0] = rm1; v[1] = rm2; v[2] = rm3; v[3] = rm4;
      return reinterpret_cast<uint8x16_t>(vld1q_dup_u32(v32));
#endif
    };
  };

  //Arithmetic
  static uint8x16 operator+(uint8x16 a, uint8x16 b);

  static uint8x16 operator+=(uint8x16 &a, uint8x16 b);

  static uint8x16 adds(uint8x16 a, uint8x16 b);

  //Load
  static uint8x16 load_u(const void* m);

  //Logical
  static uint8x16 operator|(uint8x16 a, uint8x16 b);

  static uint8x16 operator|=(uint8x16 &a, uint8x16 b);

  //Set
  static void setzero(uint8x16 &a);

  //Store
  static void store_u(void* m, uint8x16 a);

  //Swizzle
  class uint16x8;
  static uint16x8 widen_hi(uint8x16 a, uint8x16 b);

  static uint16x8 widen_lo(uint8x16 a, uint8x16 b);
}