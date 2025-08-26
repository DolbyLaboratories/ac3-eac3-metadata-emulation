/****************************************************************************
 *
 *
 * Copyright (c) 2011-2013 Dolby International AB.
 * All rights reserved.

 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE GRANTED
 * BY THIS LICENSE. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
 * CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING,
 * BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * @file
 * @brief Useful integer operations that are not provided by C itself, with function
 *        names that match the rest of the intrinsics, reducing surprise.
 *
 *        Applies the functions min, max and abs to the data types
 *        int, int32_t, int16_t and unsigned int in the expected fashion.
 */

#ifndef DLB_STD_UTIL_INTOPS_H
#define DLB_STD_UTIL_INTOPS_H

#include "dlb_compiler.h"

static DLB_PURE_FN inline unsigned int
DLB_UmaxUU(unsigned int a, unsigned int b) {
	return a > b ? a : b; }

static DLB_PURE_FN inline unsigned int
DLB_UminUU(unsigned int a, unsigned int b) {
	return a < b ? a : b; }

static DLB_PURE_FN inline unsigned int
DLB_UclipUUU(unsigned int a, unsigned int lo, unsigned int hi) {
    return DLB_UminUU(DLB_UmaxUU(lo, a), hi); }

static DLB_PURE_FN inline int
DLB_ImaxII(int a, int b) {
	return a > b ? a : b; }

static DLB_PURE_FN inline int
DLB_IminII(int a, int b) {
	return a < b ? a : b; }

static DLB_PURE_FN inline int
DLB_IabsI(int a) {
	return DLB_ImaxII(a, -a); }

static DLB_PURE_FN inline int
DLB_IclipIII(int a, int lo, int hi) {
    return DLB_IminII(DLB_ImaxII(lo, a), hi); }

static DLB_PURE_FN inline int32_t
DLB_32max3232(int32_t a, int32_t b) {
	return a > b ? a : b; }

static DLB_PURE_FN inline int32_t
DLB_32min3232(int32_t a, int32_t b) {
	return a < b ? a : b; }

static DLB_PURE_FN inline int32_t
DLB_32abs32(int32_t a) {
	return DLB_32max3232(a, -a); }

static DLB_PURE_FN inline int32_t
DLB_32clip323232(int32_t a, int32_t lo, int32_t hi) {
    return DLB_32min3232(DLB_32max3232(lo, a), hi); }

static DLB_PURE_FN inline int16_t
DLB_16max1616(int a, int b) {
	return (int16_t)(a > b ? a : b); }

static DLB_PURE_FN inline int16_t
DLB_16min1616(int a, int b) {
	return (int16_t)(a < b ? a : b); }

static DLB_PURE_FN inline int16_t
DLB_16abs16(int a) {
	return DLB_16max1616(a, -a); }

static DLB_PURE_FN inline int16_t
DLB_16clip161616(int a, int lo, int hi) {
    return DLB_16min1616(DLB_16max1616(lo, a), hi); }

static DLB_PURE_FN inline unsigned int
DLB_Uones32(uint32_t a) {
    a = a - ((a >> 1u) & 0x55555555);
    a = (((a >> 2u) & 0x33333333) + (a & 0x33333333));
    a = (((a >> 4u) + a) & 0x0f0f0f0f);
    a += (a >> 8u);
    a += (a >> 16u);
    return a & 0x3f; }

static DLB_PURE_FN inline uint32_t
_dlb_32smear32(uint32_t a) {
    a |= a >> 1;
    a |= a >> 2;
    a |= a >> 4;
    a |= a >> 8;
    a |= a >> 16;
    return a; }

#ifndef DLB_HAVE_UCLZ32
static DLB_PURE_FN inline unsigned int
DLB_Uclz32(uint32_t a) {
    return 32 - DLB_Uones32(_dlb_32smear32(a)); }
#define DLB_HAVE_UCLZ32
#endif /* !defined(DLB_HAVE_UCLZ32) */

static DLB_PURE_FN inline unsigned int
DLB_Uclz64(uint64_t a) {
    uint32_t h = (uint32_t)(a >> 32u);
    uint32_t l = (uint32_t)(a & (~0u));
    return h ? DLB_Uclz32(h) : 32 + DLB_Uclz32(l); }

#ifndef DLB_HAVE_UBITREVU
/**
 * A bit-reverse word function.  Some processors actually have an instruction
 * for this, which would of course be used instead.
 * This implementation implicitly assumes that unsigned int is 32-bits.  A
 * platform with a different-sized int would want to supply a different implementation.
 */
static DLB_PURE_FN inline unsigned int
DLB_UbitrevU(unsigned int i) {
    i = (i >> 16) | (i << 16);
    i = ((i >> 8) & 0xff00ff) | ((i & 0xff00ff) << 8);
    i = ((i >> 4) & 0xf0f0f0f) | ((i & 0xf0f0f0f) << 4);
    i = ((i >> 2) & 0x33333333) | ((i & 0x33333333) << 2);
    i = ((i >> 1) & 0x55555555) | ((i & 0x55555555) << 1);
    return i; }

#define DLB_HAVE_UBITREVU
#endif /* DLB_HAVE_UBITREVU */

#endif /* DLB_STD_UTIL_INTOPS_H */
