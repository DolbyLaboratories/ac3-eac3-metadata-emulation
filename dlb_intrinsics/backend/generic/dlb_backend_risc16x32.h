/****************************************************************************
 *
 *
 * Copyright (c) 2010-13 Dolby International AB.
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
 * @addtogroup dlb_intrinsics_core_generic_backends
 * @{
 * @section GENERIC_RISC16X32
 * Abstraction for generic 16/32 bit RISC architecture.
 * This is the only generic fixed-point backend. It works well on processors that support 16-bit
 * and 32-bit arithmetic, and particularly 16X32 multiplication. Suitable for most PCs and
 * integer embedded RISC processors (ARM, MIPS, etc.). Uses 64-bit data types for extended-precision accumulators.
 * The backend can be compiled with different preprocessor directives which influence the backend behavior.
 * The list of defines can be found at @ref risc16x32_behavior_defines "backend description".
 * @}
 */

/****************************************************************************//**
 * @file
 * @brief Abstraction for generic 16/32 bit RISC architecture
 * @{
 * @section risc16x32_behavior_defines Behavior-Altering Preprocessor Directives
 * The following symbols may be defined (perhaps implicitly, by the compiler)
 * to alter behaviour in various ways:
 * 
 * @par __GNUC__
 * __builtin_clz() is used for _dlb_clz, else longhand.
 * @par DLB_SGN_SHIFT31
 * sign is computed as (a >> 31) else as (-(a < 0))
 * @par DLB_ABS_FAST_APPROX
 * _dlb_abs uses the xor-with-sign hack, which is off-by-one for negative values, fails operator tests, but fast.
 * @par DLB_METHOD_SHORT_MULS
 * all multiplies based on 16x16->32, else direct 32x32->64.
 * @par DLB_MPYLL_DROPS_LOWxLOW 
 * if DLB_METHOD_SHORT_MULS: use most significant three 16x16 multiplies, rather than (correct) four.
 * Faster but slightly wrong (fails operator tests).
 * @par DLB_NATIVE_FRAC_BITS
 * implicit fractional point for DLB_I_[LS] family (==15).
 * @par DLB_REALLY_ROUND_LONG
 * if set all rounding is done as specified, otherwise only DLB_SFRACT results are rounded.
 * @par DLB_LMPY_USES_LSB
 * DLB_LmpyLS and friends compute the LSB of the result, otherwise uses DLB_L{trunc,sat}A(DLB_AmpyLS()), which leaves LSB zero.
 * @par DLB_LMAC_USES_LMPY
 * selects underlying DLB_ACCU arithmetic or purely LFRACT alters both LSB of results and location of saturation or wrap-around.
 *
 * @note 
 * With all of the DLB_* symbols undefined behaviour is identical to the ARM5TE low level backend.
 * With the LMPY_USES and LMAC_USES symbols defined behaviour is more like TI backends.
 * @par
 * DLB_ABS_FAST_APPROX, DLB_NATIVE_FRAC_BITS and DLB_REALLY_ROUND_LONG also apply to
 * the DLB_BACKEND_ARM* backends.
 * @par
 * DLB_NATIVE_FRAC_BITS and DLB_REALLY_ROUND_LONG also apply to the TI C64 backend.
 * @}
 * @internal
 * @{
 */
 

/******************************************************************************
processor abstraction naming macros
******************************************************************************/
/** A short descriptive name for the processor abstraction.
 * It should be a valid C identifier and be suitable for printing as a string
 * for diagnostics.
 */
#define DLB_INTRINSICS_NAME risc16x32

/******************************************************************************
processor-specific base types
******************************************************************************/
typedef int16_t DLB_SFRACT;
typedef int32_t DLB_LFRACT;
typedef int32_t DLB_ACCU;
typedef int64_t DLB_XACCU;
typedef int64_t DLB_NACCU;

#define DLB_SFRACT_MANTBITS 15
#define DLB_LFRACT_MANTBITS 31
#define DLB_ACCU_MANTBITS   30
#define DLB_XACCU_MANTBITS  46
#define DLB_NACCU_MANTBITS  62

#define DLB_SFRACT_BITS 16
#define DLB_LFRACT_BITS 32
#define DLB_ACCU_BITS   32
#define DLB_XACCU_BITS  56
#define DLB_NACCU_BITS  64

#define DLB_INT32_MAX 0x7FFFFFFF
#define DLB_INT32_MIN (-DLB_INT32_MAX - 1)

#define DLB_INT16_MAX 0x7FFF
#define DLB_INT16_MIN (-0x8000)

/******************************************************************************
macros for legacy support
******************************************************************************/
#define DLB_L(x) (DLB_SFRACT)(x)                          /* Get the low part of a word */
#define DLB_H(x) (DLB_SFRACT)((x) >> 16)                  /* Get the high part of a word */

/* The DLB_FRACT_1 values are *unsigned* and may not necessarily fit into the
 * the corresponding signed machine representation.  They exist essentially to enable
 * the compile-time constant generation.  Resulting values should be cast to appropriate
 * signed types where appropriate.
 */
#define DLB_FRACT_1L(mantbits) ((uintmax_t)1<<(mantbits))
#define DLB_FRACT_1(mantbits) (1u<<(mantbits))

/******************************************************************************
scaling macros
******************************************************************************/
#define DLB_UscaleU(i)      (i)
#define DLB_FscaleFU(x,i)   ((x)/(double)DLB_FRACT_1L((i)))

/******************************************************************************
literal conversion macros
******************************************************************************/

/** @name Ranges
 * Definitions of data type ranges.
 */
/* Unlike the DLB_FRACT_1 values the DLB_foo_MAX and DLB_foo_MIN values *are* signed
 * values, and will be representable in the appropriate computational types.
 */
#define DLB_FRACT_fMAX(mantbits) ((DLB_FRACT_1((mantbits)) - 0.5)/DLB_FRACT_1((mantbits)))
#define DLB_FRACT_fMIN(mantbits) ((0.5 - DLB_FRACT_1((mantbits)))/DLB_FRACT_1((mantbits)))
#define DLB_FRACT_MAX(mantbits) (int32_t)(DLB_FRACT_1((mantbits)) - 1)
#define DLB_FRACT_MIN(mantbits) (-DLB_FRACT_MAX((mantbits)) - 1)

#define DLB_FRACT_fMAXL(mantbits) ((DLB_FRACT_1L((mantbits)) - 0.5)/DLB_FRACT_1L((mantbits)))
#define DLB_FRACT_fMINL(mantbits) ((0.5 - DLB_FRACT_1L((mantbits)))/DLB_FRACT_1L((mantbits)))
#define DLB_FRACT_MAXL(mantbits) (intmax_t)(DLB_FRACT_1L((mantbits)) - 1)
#define DLB_FRACT_MINL(mantbits) (-DLB_FRACT_MAXL((mantbits)) - 1)

/* Now parameterize those abstract range values according to the basic types: */

/* An expression to convert a floating point (double) value into a fixed-point value of specified type,
 * scaled according to the specified number of mantissa bits, and rounded to nearest (not zero): */
#define _two_to_(n) \
    (2.0 * (1<<((n)-1))) /* for when 1 <= n <= 31 */

#define _iqfu(a, sigbits) \
    (int)(_two_to_(sigbits) * (a) + ((a) < 0.0f ? -0.5f : 0.5f))

#define _isatq(a, q) \
    ( (a) <= -1.0f               ? -1 - (int)((1u << q) - 1) \
    : 1.0f - _half_to(q) <= (a)  ? (int)((1u << q) - 1)      \
    : _iqfu(a, q) )

#define _fsatq(a,q) \
    (_half_to(q) * _isatq(a, q))


#define _dlb_tcF(type, a, mantbits) \
        ( (DLB_FRACT_fMAX((mantbits)) <= (a)) \
        ? (type)DLB_FRACT_MAX((mantbits)) \
        : ( ((a) < DLB_FRACT_fMIN((mantbits))) \
          ? (type)DLB_FRACT_MIN((mantbits)) \
          : ( (a) < 0.0) \
            ? (type)(DLB_FRACT_1((mantbits)) * (a) - 0.5) \
            : (type)(DLB_FRACT_1((mantbits)) * (a) + 0.5) ))

#define _dlb_tcI(type, a, s, mantbits)  _dlb_tcF(type, 1.0*a/((intmax_t)1 << (s)), mantbits)
  

#define _dlb_tcFL(type, a, mantbits) \
        ( (DLB_FRACT_fMAXL((mantbits)) <= (a)) \
        ? (type)DLB_FRACT_MAXL((mantbits)) \
        : ( ((a) < DLB_FRACT_fMINL((mantbits))) \
          ? (type)DLB_FRACT_MINL((mantbits)) \
          : ( (a) < 0.0) \
            ? (type)(DLB_FRACT_1L((mantbits)) * (a) - 0.5) \
            : (type)(DLB_FRACT_1L((mantbits)) * (a) + 0.5) ))

/* internal-use functions */
#define _dlb_scf(a)        (DLB_SFRACT)_dlb_tcF(DLB_SFRACT, (a), DLB_SFRACT_MANTBITS)
#define _dlb_sciu(a,s)     (DLB_SFRACT)_dlb_tcI(DLB_SFRACT, (a), (s), DLB_SFRACT_MANTBITS)
#define _dlb_scfi(a,hr)    (DLB_SFRACT)_dlb_tcF(DLB_SFRACT, (a), (DLB_SFRACT_MANTBITS-(hr)))
#define _dlb_lcf(a)        _dlb_tcF(DLB_LFRACT, (a), DLB_LFRACT_MANTBITS)
#define _dlb_lciu(a,s)     _dlb_tcI(DLB_LFRACT, (a), (s), DLB_LFRACT_MANTBITS)
#define _dlb_acf(a)        _dlb_tcF(DLB_ACCU,   (a), DLB_ACCU_MANTBITS)
#define _dlb_xcf(a)        _dlb_tcFL(DLB_XACCU,  (a), DLB_XACCU_MANTBITS)
#define _dlb_ncf(a)        _dlb_tcFL(DLB_NACCU,  (a), DLB_NACCU_MANTBITS)
#define _dlb_lcfi(a,hr)    _dlb_tcF(DLB_LFRACT, (a), (DLB_LFRACT_MANTBITS-(hr)))
#define _dlb_acfi(a,hr)    _dlb_tcF(DLB_ACCU,   (a), (DLB_ACCU_MANTBITS-(hr)))
#define _dlb_xcfi(a,hr)    _dlb_tcFL(DLB_XACCU,  (a), (DLB_XACCU_MANTBITS-(hr)))
#define _dlb_ncfi(a,hr)    _dlb_tcFL(DLB_NACCU,  (a), (DLB_NACCU_MANTBITS-(hr)))

/* export-use macros */
#define DLB_ScF(a)     _dlb_scf((a))
#define DLB_ScIU(a,s)  _dlb_sciu((a), (s))
#define DLB_LcF(a)     _dlb_lcf((a))
#define DLB_LcIU(a,s)  _dlb_lciu((a), (s))
#define DLB_ScFU(a,hr) _dlb_scfi((a),(hr))
#define DLB_LcFU(a,hr) _dlb_lcfi((a),(hr))

/* The following mechanics arranges for floating point values to be quantised and saturated to a certain bit
 * depth: */

#define _dlb_half_to(n) \
    ((0.5f)/(1<<((n)-1))) /* for 1 <= n <= 31 */

#define _dlb_two_to(n) \
    (2.0f*(1<<((n)-1))) /* for when 1 <= n <= 31 */

#define _dlb_iqfu(a, sigbits) \
    (int)(_dlb_two_to(sigbits) * (a) + ((a) < 0.0f ? -0.5f : 0.5f))

#define _dlb_isatq(a, q) \
    ( (a) <= -1.0f                   ? -1 - (int)((1u << q) - 1) \
    : 1.0f - _dlb_half_to(q) <= (a)  ? (int)((1u << q) - 1)      \
    : _dlb_iqfu(a, q) )

#define _dlb_fsatq(a,q) \
    (_dlb_half_to(q) * _dlb_isatq(a, q))

#define DLB_LcqF(a)     _dlb_lcf(_dlb_fsatq((a), 15))
#define DLB_LcqFU(a,hr) _dlb_lcfi(_dlb_fsatq((a), 15), (hr))
#define DLB_ScqF(a)     _dlb_scf(_dlb_fsatq((a), 15))
#define DLB_ScqFU(a,hr) _dlb_scfi(_dlb_fsatq((a), 15), (hr))
#define DLB_FcqFU(a,q)  _dlb_fsatq((a), (q))
#define DLB_FcqF(a)     _dlb_fsatq((a), 15)

/* helper functions: factor common operation patterns out, to minimise bugs */
#ifndef DLB_SGN_SHIFT31
#define _dlb_sgn(a) (-((int32_t)(a) < 0))  /* default form uses no undefined behaviour */
#else
#define _dlb_sgn(a) ((a) >> 31)
#endif  /* DLB_SGN_VERn */

static DLB_PURE_FN inline int32_t
_dlb_sat(int32_t a, unsigned int n) {
    DLB_assert(n <= 31, "shift", "Shift count negative."); {
    int sgn = _dlb_sgn(a);
    return sgn == (a >> n) ? a
                           : ((DLB_INT32_MAX >> (31 - n)) ^ sgn); } }

static DLB_PURE_FN inline int32_t   /* saturating left-shift */
_dlb_sshlu(int32_t a, unsigned int s) {
    DLB_assert(s <= 31, "shift", "Shift count too large."); {
    int sgn = _dlb_sgn(a);
    return sgn == (a >> (31 - s)) ? (a << s)
                                  : (DLB_INT32_MAX ^ sgn); } }

static DLB_PURE_FN inline int32_t
_dlb_sat64(int64_t a, unsigned int n) {
    DLB_assert(n <= 31, "shift", "Shift count too large."); {
    int sgn = -(a < 0);
    return sgn == (int)(a >> n) ? (int32_t)a
                                : ((DLB_INT32_MAX >> (31 - n)) ^ sgn); } }

static DLB_PURE_FN inline int32_t
_dlb_sadd(int32_t a, int32_t b) {
    int32_t sum = (int32_t)((uint32_t)a + (uint32_t)b); /* casts avoid undefined behaviour on overflow */
    int sgn = _dlb_sgn(sum);
    return ((a ^ sum) & (b ^ sum)) >= 0 ? sum : DLB_INT32_MIN ^ sgn; }

static DLB_PURE_FN inline int32_t
_dlb_ssub(int32_t a, int32_t b) {
    int32_t sum = (int32_t)((uint32_t)a - (uint32_t)b);
    int sgn = _dlb_sgn(sum);
    return ((a ^ sum) & (~b ^ sum)) >= 0 ? sum : DLB_INT32_MIN ^ sgn; }

static DLB_PURE_FN inline int32_t
_dlb_sdadd(int32_t a, int32_t b) {
    int32_t sum = (int32_t)((uint32_t)a + ((uint32_t)b<<1));
    int sgn = _dlb_sgn(sum);
    return ((a ^ sum) & (b ^ sum)) >= 0 ? sum : DLB_INT32_MIN ^ sgn; }

static DLB_PURE_FN inline int32_t
_dlb_sdsub(int32_t a, int32_t b) {
    int32_t sum = (int32_t)((uint32_t)a - ((uint32_t)b<<1));
    int sgn = _dlb_sgn(sum);
    return ((a ^ sum) & (~b ^ sum)) >= 0 ? sum : DLB_INT32_MIN ^ sgn; }

static DLB_PURE_FN inline int32_t
_dlb_dbl(int32_t a) { /* return _dlb_sadd(a, a); */
    int32_t sum = (int32_t)((uint32_t)a + (uint32_t)a);
    int sgn = _dlb_sgn(sum);
    return (a ^ sum) >= 0 ? sum : DLB_INT32_MIN ^ sgn; }

static DLB_PURE_FN inline uint32_t
_dlb_ones32(uint32_t a) {
    a = a - ((a >> 1) & 0x55555555);
    a = (((a >> 2) & 0x33333333) + (a & 0x33333333));
    a = (((a >> 4) + a) & 0x0f0f0f0f);
    a += (a >> 8);
    a += (a >> 16);
    return(a & 0x3f); }

static DLB_PURE_FN inline uint32_t
_dlb_smear32(uint32_t a) {
    a |= a>>1;
    a |= a>>2;
    a |= a>>4;
    a |= a>>8;
    a |= a>>16;
    return a; }

#if (defined (__GNUC__) && !defined(__TI_COMPILER_VERSION__))
/* gcc provides an intrinsic for clz
 * that is unfortunately undefined if the argument is zero 
 * Pay attention that TI doesn't have __builtin_clz(). By
 * default, TI's code is built with -gcc flag. And it will
 * define __GNUC__.
 */
static DLB_PURE_FN inline uint32_t
_dlb_clz32(uint32_t a) {
    return 0 == a ? 32
                  : __builtin_clz(a); }

#else
static DLB_PURE_FN inline uint32_t
_dlb_clz32(uint32_t a) {
    return 32 - _dlb_ones32(_dlb_smear32(a)); }
#endif

static DLB_PURE_FN inline uint32_t
_dlb_clz64(uint64_t a) {
    uint32_t h = (uint32_t)(a >> 32);
    uint32_t l = (uint32_t)(a & (~0u));
    return h ? _dlb_clz32(h) : 32 + _dlb_clz32(l); }

static DLB_PURE_FN inline int32_t _dlb_max(int32_t a, int32_t b) {
    return a >= b ? a : b; }

#ifdef DLB_ABS_FAST_APPROX
static DLB_PURE_FN inline int32_t _dlb_abs(int32_t a) {
    return a ^ _dlb_sgn(a); }
#else  /* !DLB_ABS_FAST_APPROX */
static DLB_PURE_FN inline int32_t _dlb_abs(int32_t a) {
    return _dlb_max(a, -a); }
#endif /* !DLB_ABS_FAST_APPROX */

/* Now the implementations... */

/** @name Control Value Conversions
 * Generally useful conversions between types that might be used at runtime.
 */
/*
 * To interface with control code, it is useful to be able to convert between
 * native C types (short and int) and Hummingbird types (DLB_SFRACT and DLB_LFRACT). These
 * functions allow for the conversions. Note that when we store in a C type,
 * we put the data in the least significant bits, such that the radix point is
 * at DLB_NATIVE_FRAC_BITS, though most of the time you should not be doing any
 * processing on a native datatype.
 */
#ifndef DLB_NATIVE_FRAC_BITS
#define DLB_NATIVE_FRAC_BITS (15)
#endif

static DLB_PURE_FN inline int
DLB_I_S(DLB_SFRACT x) {
    return (int)x; }

static DLB_PURE_FN inline int
DLB_IsrndS(DLB_SFRACT x) {
    return (int)x; }

static DLB_PURE_FN inline DLB_SFRACT
DLB_S_I(int x) {
    return (DLB_SFRACT)x; }

static DLB_PURE_FN inline int
DLB_I_LU(DLB_LFRACT x, unsigned int hr) {
    DLB_assert(hr <= 16, "convert with headroom", "Headroom is too large.");
    return (int)(x >> (16 - hr)); }  /* 16 == DLB_LFRACT_MANTBITS - DLB_NATIVE_FRAC_BITS */

static DLB_PURE_FN inline int
DLB_I_L(DLB_LFRACT x) {
    return (int)(x >> 16); }  /* 16 == DLB_LFRACT_MANTBITS - DLB_NATIVE_FRAC_BITS */

static DLB_PURE_FN inline int
DLB_IsrndL(DLB_LFRACT x) {
    return (int)(_dlb_sadd(x, 32768) >> 16); }  /* 16 == DLB_LFRACT_MANTBITS - DLB_NATIVE_FRAC_BITS */

static DLB_PURE_FN inline int
DLB_IsrndLU(DLB_LFRACT x, unsigned fp) {
    int rnd = (1 << (DLB_LFRACT_MANTBITS - 1)) >> fp;
    return (int)(_dlb_sadd(x, rnd) >> (DLB_LFRACT_MANTBITS - fp)); }

static DLB_PURE_FN inline DLB_LFRACT
DLB_L_IU(int x, unsigned int hr) {
    DLB_assert(hr <= 16, "convert with headroom", "Headroom is too large.");
    return (DLB_LFRACT)((int32_t)x << (16 - hr)); }  /* 16 == DLB_LFRACT_MANTBITS - DLB_NATIVE_FRAC_BITS */

static DLB_PURE_FN inline DLB_LFRACT
DLB_L_I(int x) {
    return (DLB_LFRACT)((int32_t)x << 16); }

/* That's it for "DLB_NATIVE_FRAC_BITS" functions, the rest of the conversions use explicit
 * (numerical) alignments */

static DLB_PURE_FN inline int
DLB_IintLU(DLB_LFRACT a, unsigned int fp) {
    DLB_assert(fp <= DLB_LFRACT_MANTBITS, "shift", "Shift count negative.");
    return a >> (DLB_LFRACT_MANTBITS-fp); }

static DLB_PURE_FN inline int
DLB_IintLUU(DLB_LFRACT a, unsigned int fp, unsigned int hr) {
    DLB_assert(fp+hr <= DLB_LFRACT_MANTBITS, "shift", "Shift count negative.");
    return a >> (DLB_LFRACT_MANTBITS-fp-hr); }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LmantLUU(DLB_LFRACT a, unsigned int fp, unsigned int hr) {
    DLB_assert(fp+hr+1 <= 31, "shift", "Shift count too large.");
    return (DLB_LFRACT) ((uint32_t)(a << (fp+hr+1))>>(hr+1)); }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LmantLU(DLB_LFRACT a, unsigned int fp) {
    DLB_assert(fp+1 <= 31, "shift", "Shift count too large.");
    return (DLB_LFRACT) ((uint32_t)(a << (fp+1)) >> 1); }

static DLB_PURE_FN inline double
DLB_F_S(DLB_SFRACT a) {
    static const double scale = 1.0/0x8000;
    return (double)a * scale; }

static DLB_PURE_FN inline double
DLB_F_LU(DLB_LFRACT a, unsigned int hr) {
    static const double scale = (1.0/0x10000)/0x8000;
    DLB_assert(hr < 31, "shift", "Shift count too large.");
    return (double)a * (1 << hr) * scale; }

static DLB_PURE_FN inline double
DLB_F_L(DLB_LFRACT a) {
    static const double scale = (1.0/0x10000)/0x8000;
    return (double)a * scale; }

static DLB_PURE_FN inline double
DLB_F_AU(DLB_ACCU a, unsigned int hr) {
    static const double scale = (1.0/0x10000)/0x4000;
    DLB_assert(hr < 31, "shift", "Shift count too large.");
    return (double)a * (1 << hr) * scale; }

static DLB_PURE_FN inline double
DLB_F_A(DLB_ACCU a) {
    static const double scale = (1.0/0x10000)/0x4000;
    return (double)a * scale; }

static DLB_PURE_FN inline double
DLB_F_XU(DLB_XACCU a, unsigned int hr) {
    static const double scale = ((1.0/0x8000)/0x10000)/0x8000;
    DLB_assert(hr < 31, "shift", "Shift count too large.");
    return (double)a * (1 << hr) * scale; }

static DLB_PURE_FN inline double
DLB_F_X(DLB_XACCU a) {
    static const double scale = ((1.0/0x8000)/0x10000)/0x8000;
    return (double)a * scale; }

static DLB_PURE_FN inline double
DLB_F_NU(DLB_NACCU a, unsigned int hr) {
    static const double scale = (((1.0/0x10000)/0x10000)/0x8000)/0x8000;
    DLB_assert(hr < 16, "shift", "Shift count too large.");
    return (double)a * (1 << 2*hr) * scale; }

static DLB_PURE_FN inline double
DLB_F_N(DLB_NACCU a) {
    static const double scale = (((1.0/0x10000)/0x10000)/0x8000)/0x8000;
    return (double)a * scale; }

/*
 * Narrowing type conversions:
 *
 * truncation (trunc),
 * rounding   (rnd),
 * saturation (sat),
 * rounding+saturation (srnd),
 * and round down, i.e. round toward negtive infinity (rndd)
 *
 */

static DLB_PURE_FN inline DLB_SFRACT
DLB_StruncL(DLB_LFRACT a) {
    return (DLB_SFRACT)(a >> 16); }

static DLB_PURE_FN inline DLB_SFRACT
DLB_StruncAU(DLB_ACCU a, unsigned int hr) {
    DLB_assert(hr <= 15, "convert with headroom", "Headroom is too large.");
    return (DLB_SFRACT)(a >> (15 - hr)); }

static DLB_PURE_FN inline DLB_SFRACT
DLB_StruncA(DLB_ACCU a) {
    return (DLB_SFRACT)(a >> 15); }

static DLB_PURE_FN inline DLB_SFRACT
DLB_SsshlAU(DLB_ACCU a, unsigned i) {
  a = i <= 15 ? (a >> (15 - i)) : _dlb_sshlu(a, (i - 15));
  return (DLB_SFRACT)_dlb_sat(a, 15); }

static DLB_PURE_FN inline DLB_SFRACT
DLB_SshlAU(DLB_ACCU a, unsigned i) {
  return (DLB_SFRACT)(i <= 15 ? (a >> (15 - i)) : (a << (i - 15))); }

static DLB_PURE_FN inline DLB_SFRACT
DLB_StruncX(DLB_XACCU a) {
    return (DLB_SFRACT)(a >> 31); }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LtruncAU(DLB_ACCU a, unsigned int hr) {
    (void)hr; /* UNUSED */
    return (DLB_LFRACT)(a << 1); }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LtruncA(DLB_ACCU a) {
    return (DLB_LFRACT)(a << 1); }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LshrAU(DLB_ACCU a, unsigned i) {
  return i ? (DLB_LFRACT)(a >> (i-1)) : (DLB_LFRACT)(a<<1); }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LshlAU(DLB_ACCU a, unsigned i) {
  return (DLB_LFRACT)(a << (i+1)); }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LsshlAU(DLB_ACCU a, unsigned i) {
  return _dlb_sshlu(a, i + 1); }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LtruncXU(DLB_XACCU a, unsigned int hr) {
    (void)hr; /* UNUSED */
    return (DLB_LFRACT)(a >> 15); }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LtruncX(DLB_XACCU a) {
    return (DLB_LFRACT)(a >> 15); }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LtruncNU(DLB_NACCU a, unsigned int hr) {
    DLB_assert(hr <= 31, "convert with headroom", "Headroom is too large.");
    return (DLB_LFRACT)(a >> (31 - hr)); }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LtruncN(DLB_NACCU a) {
    return (DLB_LFRACT)(a >> 31); }

static DLB_PURE_FN inline DLB_ACCU
DLB_AtruncX(DLB_XACCU a) {
    return (DLB_ACCU)(a >> 16); }

static DLB_PURE_FN inline DLB_ACCU
DLB_AtruncNU(DLB_NACCU a, unsigned int hr) {
    DLB_assert(hr <= 32, "convert with headroom", "Headroom is too large.");
    return (DLB_ACCU)(a >> (32 - hr)); }

static DLB_PURE_FN inline DLB_ACCU
DLB_AtruncN(DLB_NACCU a) {
    return (DLB_ACCU)(a >> 32); }

static DLB_PURE_FN inline DLB_XACCU
DLB_XtruncNU(DLB_NACCU a, unsigned int hr) {
    DLB_assert(hr <= 16, "convert with headroom", "Headroom is too large.");
    return (DLB_XACCU)(a >> (16 - hr)); }

static DLB_PURE_FN inline DLB_XACCU
DLB_XtruncN(DLB_NACCU a) {
    return (DLB_XACCU)(a >> 16); }

static DLB_PURE_FN inline DLB_SFRACT
DLB_SsatL(DLB_LFRACT a) {
    return (DLB_SFRACT)(a >> 16); } /* can't saturate */

static DLB_PURE_FN inline DLB_SFRACT
DLB_SsatAU(DLB_ACCU a, unsigned int hr) {
    DLB_assert(hr <= 15, "convert with headroom", "Headroom is too large.");
    return (DLB_SFRACT)_dlb_sat(a >> (15 - hr), 15); }

static DLB_PURE_FN inline DLB_SFRACT
DLB_SsatA(DLB_ACCU a) {
    return (DLB_SFRACT)_dlb_sat(a >> 15, 15); }

static DLB_PURE_FN inline DLB_SFRACT
DLB_SsatX(DLB_XACCU a) {
    return (DLB_SFRACT)_dlb_sat64(a >> 31, 15); }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LsatAU(DLB_ACCU a, unsigned int hr) {
    DLB_assert(hr <= 30, "shift", "Shift count too large.");
    return _dlb_sshlu(a, hr + 1) >> hr; }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LsatA(DLB_ACCU a) {
    return _dlb_dbl(a); }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LsatXU(DLB_XACCU a, unsigned int hr) {
    return _dlb_sat64(a >> 15, 31 - hr); }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LsatX(DLB_XACCU a) {
    return _dlb_sat64(a >> 15, 31); }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LsatNU(DLB_NACCU a, unsigned int hr) {
    DLB_assert(hr <= 31, "convert with headroom", "Headroom is too large.");
    return _dlb_sat64(a >> (31 - hr), 31 - hr); }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LsatN(DLB_NACCU a) {
    return _dlb_sat64(a >> 31, 31); }

static DLB_PURE_FN inline DLB_SFRACT
DLB_SrndL(DLB_LFRACT a) {
    int32_t rnd = 0x8000;
    return (DLB_SFRACT)((a + rnd) >> 16); }

static DLB_PURE_FN inline DLB_SFRACT
DLB_SrndAU(DLB_ACCU a, unsigned int hr) {
    int32_t rnd = 0x4000 >> hr;
    DLB_assert(hr <= 15, "convert with headroom", "Headroom is too large.");
    return (DLB_SFRACT)((a + rnd) >> (15 - hr)); }

static DLB_PURE_FN inline DLB_SFRACT
DLB_SrndA(DLB_ACCU a) {
    int32_t rnd = 0x4000;
    return (DLB_SFRACT)((a + rnd) >> 15); }

static DLB_PURE_FN inline DLB_SFRACT
DLB_SrndXU(DLB_XACCU a, unsigned int hr) {
    int32_t rnd = 0x40000000 >> hr;
    DLB_assert(hr <= 31, "convert with headroom", "Headroom is too large.");
    return (DLB_SFRACT)((a + rnd) >> (31 - hr)); }

static DLB_PURE_FN inline DLB_SFRACT
DLB_SrndX(DLB_XACCU a) {
    int32_t rnd = 0x40000000;
    return (DLB_SFRACT)((a + rnd) >> 31); }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LrndAU(DLB_ACCU a, unsigned int hr) {
    (void)hr; /* UNUSED */
    return a << 1; }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LrndA(DLB_ACCU a) {
    return a << 1; }

#if defined(DLB_REALLY_ROUND_LONG)

static DLB_PURE_FN inline DLB_LFRACT
DLB_LrndXU(DLB_XACCU a, unsigned int hr) {
    int32_t rnd = 0x4000;
    return (DLB_LFRACT)((a + rnd) >> 15); }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LrndX(DLB_XACCU a) {
    int32_t rnd = 0x4000;
    return (DLB_LFRACT)((a + rnd) >> 15); }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LrndNU(DLB_NACCU a, unsigned int hr) {
    int64_t rnd = 0x40000000 >> hr;
    DLB_assert(hr <= 31, "convert with headroom", "Headroom is too large.");
    return (DLB_LFRACT)((a + rnd) >> (31 - hr)); }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LrndN(DLB_NACCU a) {
    int64_t rnd = 0x40000000;
    return (DLB_LFRACT)((a + rnd) >> 31); }

static DLB_PURE_FN inline DLB_ACCU
DLB_ArndXU(DLB_XACCU a, unsigned int hr) {
    int32_t rnd = 0x8000;
    (void)hr; /* UNUSED */
    return (DLB_ACCU)((a + rnd) >> 16); }

static DLB_PURE_FN inline DLB_ACCU
DLB_ArndX(DLB_XACCU a) {
    int32_t rnd = 0x8000;
    return (DLB_ACCU)((a + rnd) >> 16); }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LsrndXU(DLB_XACCU a, unsigned int hr) {
    return _dlb_sat64((a + 0x4000) >> 15, 31 - hr); }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LsrndX(DLB_XACCU a) {
    return _dlb_sat64((a + 0x4000) >> 15, 31); }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LsrndNU(DLB_NACCU a, unsigned int hr) {
    DLB_assert(hr <= 31, "convert with headroom", "Headroom is too large.");
    return _dlb_sat64((a + (0x40000000 >> hr)) >> (31 - hr), 31 - hr); }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LsrndN(DLB_NACCU a) {
    return _dlb_sat64((a + 0x40000000) >> 31, 31); }

#else /* !DLB_REALLY_ROUND_LONG */

static DLB_PURE_FN inline DLB_LFRACT
DLB_LrndXU(DLB_XACCU a, unsigned int hr) { return DLB_LtruncXU(a, hr); }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LrndX(DLB_XACCU a) { return DLB_LtruncX(a); }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LrndNU(DLB_NACCU a, unsigned int hr) { return DLB_LtruncNU(a, hr); }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LrndN(DLB_NACCU a) { return DLB_LtruncN(a); }

static DLB_PURE_FN inline DLB_ACCU
DLB_ArndXU(DLB_XACCU a, unsigned int hr) {
    (void)hr; /* UNUSED */
    return (DLB_ACCU)(a >> 16); }

static DLB_PURE_FN inline DLB_ACCU
DLB_ArndX(DLB_XACCU a) { return DLB_AtruncX(a); }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LsrndXU(DLB_XACCU a, unsigned int hr) { return DLB_LsatXU(a, hr); }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LsrndX(DLB_XACCU a) { return DLB_LsatX(a); }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LsrndNU(DLB_NACCU a, unsigned int hr) { return DLB_LsatNU(a, hr); }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LsrndN(DLB_NACCU a) { return DLB_LsatN(a); }

#endif /* DLB_REALLY_ROUND_LONG */

static DLB_PURE_FN inline DLB_SFRACT
DLB_SsrndL(DLB_LFRACT a) {
    int32_t rnd = 0x8000;
    return (DLB_SFRACT)_dlb_sat(_dlb_sadd(a, rnd) >> 16, 15); }

static DLB_PURE_FN inline DLB_SFRACT
DLB_SsrndAU(DLB_ACCU a, unsigned int hr) {
    int32_t rnd = 0x4000 >> hr;
    DLB_assert(hr <= 15, "convert with headroom", "Headroom is too large.");
    return (DLB_SFRACT)_dlb_sat(_dlb_sadd(a, rnd) >> (15 - hr), 15); }

static DLB_PURE_FN inline DLB_SFRACT
DLB_SsrndA(DLB_ACCU a) {
    int32_t rnd = 0x4000;
    return (DLB_SFRACT)_dlb_sat(_dlb_sadd(a, rnd) >> 15, 15); }

static DLB_PURE_FN inline DLB_SFRACT
DLB_SsrndXU(DLB_XACCU a, unsigned int hr) {
    int32_t rnd = 0x40000000 >> hr;
    DLB_assert(hr <= 31, "convert with headroom", "Headroom is too large.");
    return (DLB_SFRACT)_dlb_sat64((a + rnd) >> (31 - hr), 15); }

static DLB_PURE_FN inline DLB_SFRACT
DLB_SsrndX(DLB_XACCU a) {
    int32_t rnd = 0x40000000;
    return (DLB_SFRACT)_dlb_sat64((a + rnd) >> 31, 15); }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LsrndAU(DLB_ACCU a, unsigned int hr) {  /* same as plain LsatAU, because no rounding on left shift */
    return DLB_LsatAU(a, hr); }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LsrndA(DLB_ACCU a) {
    return _dlb_dbl(a); }

static DLB_PURE_FN inline DLB_SFRACT
DLB_SrnddL(DLB_LFRACT a) {
    return (DLB_SFRACT)(a >> (DLB_LFRACT_MANTBITS - DLB_SFRACT_MANTBITS)); }

static DLB_PURE_FN inline DLB_SFRACT
DLB_SrnddA(DLB_ACCU a) {
    return (DLB_SFRACT)(a >> (DLB_ACCU_MANTBITS - DLB_SFRACT_MANTBITS)); }

static DLB_PURE_FN inline DLB_SFRACT
DLB_SrnddX(DLB_XACCU a) {
    return (DLB_SFRACT)(a >> (DLB_XACCU_MANTBITS - DLB_SFRACT_MANTBITS)); }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LrnddA(DLB_ACCU a) {
    /* shift left due to "DLB_LFRACT_MANTBITS" is larger than "DLB_ACCU_MANTBITS" for this backend */
    return (DLB_LFRACT)(a << (DLB_LFRACT_MANTBITS - DLB_ACCU_MANTBITS)); }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LrnddX(DLB_XACCU a) {
    return (DLB_LFRACT)(a >> (DLB_XACCU_MANTBITS - DLB_LFRACT_MANTBITS)); }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LrnddN(DLB_NACCU a) {
    return (DLB_LFRACT)(a >> (DLB_NACCU_MANTBITS - DLB_LFRACT_MANTBITS)); }

static DLB_PURE_FN inline DLB_ACCU
DLB_ArnddX(DLB_XACCU a) {
    return (DLB_ACCU)(a >> (DLB_XACCU_MANTBITS - DLB_ACCU_MANTBITS)); }

static DLB_PURE_FN inline DLB_ACCU
DLB_ArnddN(DLB_NACCU a) {
    return (DLB_ACCU)(a >> (DLB_NACCU_MANTBITS - DLB_ACCU_MANTBITS)); }

static DLB_PURE_FN inline DLB_XACCU
DLB_XrnddN(DLB_NACCU a) {
    return (DLB_XACCU)(a >> (DLB_NACCU_MANTBITS - DLB_XACCU_MANTBITS)); }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LrnddLU(DLB_LFRACT a, unsigned int bits) {
    unsigned chopbits = DLB_LFRACT_MANTBITS - bits;
    DLB_assert(bits <= DLB_LFRACT_MANTBITS
              , "DLB_LrnddLU"
              , "The number of bits to be kept should no more than DLB_LFRACT_MANTBITS."
              );
    return (a >> chopbits) << chopbits; }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LrnddXU(DLB_XACCU a, unsigned int bits) {
    unsigned chopbits = DLB_XACCU_MANTBITS - bits;
    unsigned shlbits = DLB_LFRACT_MANTBITS - bits;
    DLB_assert(bits <= DLB_LFRACT_MANTBITS
              , "DLB_LrnddXU"
              , "The number of bits to be kept should no more than DLB_LFRACT_MANTBITS."
              );
    return ((DLB_LFRACT)(a >> chopbits) <<  shlbits); }


/*
 * Widening type conversions
 *
 * These align fractional points and type cast, but do
 * not usually round or saturate.
 */
static DLB_PURE_FN inline DLB_ACCU
DLB_A_LU(DLB_LFRACT x, unsigned int hr) {
    (void)hr; /* UNUSED */
    return (DLB_ACCU)(x >> 1); }

static DLB_PURE_FN inline DLB_ACCU
DLB_A_L(DLB_LFRACT x) {
    return (DLB_ACCU)(x >> 1); }

static DLB_PURE_FN inline DLB_ACCU
DLB_A_SU(DLB_SFRACT x, unsigned int hr) {
    DLB_assert(hr <= 15, "convert with headroom", "Headroom is too large.");
    return (DLB_ACCU)x << (15 - hr); }

static DLB_PURE_FN inline DLB_ACCU
DLB_A_S(DLB_SFRACT x) {
    return (DLB_ACCU)x << 15; }

static DLB_PURE_FN inline DLB_LFRACT
DLB_L_SU(DLB_SFRACT x, unsigned int hr) {
    DLB_assert(hr <= 16, "convert with headroom", "Headroom is too large.");
    return (DLB_LFRACT)x << (16 - hr); }

static DLB_PURE_FN inline DLB_LFRACT
DLB_L_S(DLB_SFRACT x) {
    return (DLB_LFRACT)x << 16; }

static DLB_PURE_FN inline DLB_XACCU
DLB_X_S(DLB_SFRACT x) {
    return (DLB_XACCU)x << 31; }

static DLB_PURE_FN inline DLB_XACCU
DLB_X_LU(DLB_LFRACT x, unsigned int hr) {
    (void)hr; /* UNUSED */
    return (DLB_XACCU)x << 15; }

static DLB_PURE_FN inline DLB_XACCU
DLB_X_L(DLB_LFRACT x) {
    return (DLB_XACCU)x << 15; }

static DLB_PURE_FN inline DLB_XACCU
DLB_X_AU(DLB_ACCU x, unsigned int hr) {
    (void)hr; /* UNUSED */
    return (DLB_XACCU)x << 16; }

static DLB_PURE_FN inline DLB_XACCU
DLB_X_A(DLB_ACCU x) {
    return (DLB_XACCU)x << 16; }

static DLB_PURE_FN inline DLB_NACCU
DLB_N_LU(DLB_LFRACT x, unsigned int hr) {
    DLB_assert(hr <= 31, "convert with headroom", "Headroom is too large.");
    return (DLB_NACCU)x << (31 - hr); }

static DLB_PURE_FN inline DLB_NACCU
DLB_N_L(DLB_LFRACT x) {
    return (DLB_NACCU)x << 31; }

static DLB_PURE_FN inline DLB_NACCU
DLB_N_A(DLB_ACCU x) {
    return (DLB_NACCU)x << 32; }

static DLB_PURE_FN inline DLB_NACCU
DLB_N_X(DLB_XACCU x) {
    return (DLB_NACCU)x << 16; }

/* fixed-format data interchange conversions.
 *
 * whether or not these are widening or narrowing depend on the LFRACT and SFRACT types
 */
static DLB_PURE_FN inline DLB_SFRACT
DLB_S_16(int16_t x) {
    return (DLB_SFRACT)x; }

static DLB_PURE_FN inline DLB_SFRACT
DLB_S_32(int32_t s) {
    return (DLB_SFRACT)(s >> 16); }

static DLB_PURE_FN inline int16_t
DLB_16srndS(DLB_SFRACT a) {
    return (int16_t)a; }

static DLB_PURE_FN inline int16_t
DLB_16sfloorS(DLB_SFRACT a) {
    return (int16_t)a; }

static DLB_PURE_FN inline int32_t
DLB_32srndS(DLB_SFRACT a) {
    return (int32_t)a << 16; }

static DLB_PURE_FN inline int32_t
DLB_32sfloorS(DLB_SFRACT a) {
    return (int32_t)a << 16; }

static DLB_PURE_FN inline DLB_LFRACT
DLB_L_32U(int32_t s, unsigned int hr) {
    DLB_assert(hr <= 31, "convert with headroom", "Headroom is too large.");
    return (DLB_LFRACT)(s >> hr); }

static DLB_PURE_FN inline DLB_LFRACT
DLB_L_32(int32_t s) {
    return (DLB_LFRACT)s; }

static DLB_PURE_FN inline DLB_LFRACT
DLB_L_16U(int16_t s, unsigned int hr) {
    DLB_assert(hr <= 16, "convert with headroom", "Headroom is too large.");
    return (DLB_LFRACT)s << (16 - hr); }

static DLB_PURE_FN inline DLB_LFRACT
DLB_L_16(int16_t s) {
    return (DLB_LFRACT)s << 16; }

static DLB_PURE_FN inline int32_t
DLB_32srndLU(DLB_LFRACT a, unsigned int hr) {
    return _dlb_sshlu(a, hr); }

static DLB_PURE_FN inline int32_t
DLB_32srndL(DLB_LFRACT a) {
    return a; }

static DLB_PURE_FN inline int32_t
DLB_32sfloorL(DLB_LFRACT a) {
    return a; }

static DLB_PURE_FN inline int16_t
DLB_16srndLU(DLB_LFRACT a, unsigned int hr) {
    DLB_assert(hr <= 16, "convert with headroom", "Headroom is too large.");
    return (int16_t)_dlb_sat(_dlb_sadd(a, (0x8000>>hr)) >> (16 - hr), 15); }

static DLB_PURE_FN inline int16_t
DLB_16srndL(DLB_LFRACT a) {
    return (int16_t)_dlb_sat(_dlb_sadd(a, 0x8000) >> 16, 15); }

static DLB_PURE_FN inline int16_t
DLB_16sfloorL(DLB_LFRACT a) {
    return (int16_t)_dlb_sat(a >> 16, 15); }

static DLB_PURE_FN inline DLB_SFRACT
DLB_S_M(intmax_t m) {
    return (DLB_SFRACT)m; }

static DLB_PURE_FN inline DLB_LFRACT
DLB_L_M(intmax_t m) {
    return (DLB_LFRACT)m; }

static DLB_PURE_FN inline DLB_ACCU
DLB_A_M(intmax_t m) {
    return (DLB_ACCU)m; }

static DLB_PURE_FN inline DLB_XACCU
DLB_X_M(intmax_t m) {
    return (DLB_XACCU)m; }

static DLB_PURE_FN inline DLB_NACCU
DLB_N_M(intmax_t m) {
    return (DLB_NACCU)m; }

static DLB_PURE_FN inline intmax_t
DLB_M_S(DLB_SFRACT x) {
    return (intmax_t)x; }

static DLB_PURE_FN inline intmax_t
DLB_M_L(DLB_LFRACT x) {
    return (intmax_t)x; }

static DLB_PURE_FN inline intmax_t
DLB_M_A(DLB_ACCU x) {
    return (intmax_t)x; }

static DLB_PURE_FN inline intmax_t
DLB_M_X(DLB_XACCU x) {
    return (intmax_t)x; }

static DLB_PURE_FN inline intmax_t
DLB_M_N(DLB_NACCU x) {
    return (intmax_t)x; }

static DLB_PURE_FN inline DLB_ACCU
DLB_AaddAA(DLB_ACCU a, DLB_ACCU b) { return a + b; }

static DLB_PURE_FN inline DLB_ACCU
DLB_AsubAA(DLB_ACCU a, DLB_ACCU b) { return a - b; }

static DLB_PURE_FN inline DLB_SFRACT
DLB_SmaxSS(DLB_SFRACT a, DLB_SFRACT b) {
    return (DLB_SFRACT)(a < b ? b : a); }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LmaxLL(DLB_LFRACT a, DLB_LFRACT b) {
    return a < b ? b : a; }

static DLB_PURE_FN inline DLB_ACCU
DLB_AmaxAA(DLB_ACCU a, DLB_ACCU b) {
    return a < b ? b : a; }

static DLB_PURE_FN inline DLB_XACCU
DLB_XmaxXX(DLB_XACCU a, DLB_XACCU b) {
    return a < b ? b : a; }

static DLB_PURE_FN inline DLB_XACCU
DLB_NmaxNN(DLB_NACCU a, DLB_NACCU b) {
    return a < b ? b : a; }

static DLB_PURE_FN inline DLB_ACCU
DLB_AabsA(DLB_ACCU a) { return _dlb_abs(a); }

static DLB_PURE_FN inline DLB_XACCU
DLB_XabsX(DLB_XACCU a) { return DLB_XmaxXX(a, -a); }

static DLB_PURE_FN inline DLB_NACCU
DLB_NabsN(DLB_NACCU a) { return DLB_NmaxNN(a, -a); }

/* multiplication implementations */

static DLB_PURE_FN inline DLB_ACCU
DLB_AmpySS(DLB_SFRACT a, DLB_SFRACT b) {
    return (DLB_ACCU)a * b; }

#ifdef DLB_METHOD_SHORT_MULS

static DLB_PURE_FN inline DLB_ACCU
DLB_AmpyLS(DLB_LFRACT a, DLB_SFRACT c) {
    int16_t  b1 =           DLB_H(a);
    uint16_t b0 = (uint16_t)DLB_L(a);
    int32_t r1 = (int32_t)b1 * c;
    int32_t r0 = (int32_t)b0 * c;
    return (DLB_ACCU)(r1 + (r0 >> 16)); }

static DLB_PURE_FN inline DLB_XACCU
DLB_XmpyLS(DLB_LFRACT a, DLB_SFRACT c) {
    int16_t  b1 =           DLB_H(a);
    uint16_t b0 = (uint16_t)DLB_L(a);
    int32_t r1 = (int32_t)b1 * c;
    int32_t r0 = (int32_t)b0 * c;
    return (((DLB_XACCU)r1 << 16) + r0); }

static DLB_PURE_FN inline DLB_ACCU
DLB_AmpyLL(DLB_LFRACT a, DLB_LFRACT c) {
    int16_t  b1 = DLB_H(a);
    uint16_t b0 = (uint16_t)DLB_L(a);
    int16_t  c1 = DLB_H(c);
    uint16_t c0 = (uint16_t)DLB_L(c);
    int32_t r01 = (int32_t)b0 * c1;
    int32_t r11 = (int32_t)b1 * c1;
    int32_t r10 = (int32_t)b1 * c0;
#ifdef    DLB_MPYLL_DROPS_LOWxLOW
    return (DLB_ACCU)(r11 + (r01 >> 16) + (r10 >> 16)); }
#else /* !DLB_MPYLL_DROPS_LOWxLOW */
    uint32_t r00 = (uint32_t)b0 * c0;
    int32_t lsb = ((r01 & 0xFFFF) + (r10 & 0xFFFF) + (r00 >> 16)) >> 16;
    return (DLB_ACCU)(r11 + (r01 >> 16) + (r10 >> 16) + lsb); }
#endif /* DLB_MPYLL_DROPS_LOWxLOW */

static DLB_PURE_FN inline DLB_NACCU
DLB_NmpyLL(DLB_LFRACT a, DLB_LFRACT c) {
    int16_t  b1 = DLB_H(a);
    uint16_t b0 = (uint16_t)DLB_L(a);
    int16_t  c1 = DLB_H(c);
    uint16_t c0 = (uint16_t)DLB_L(c);
    int32_t r01 = (int32_t)b0 * c1;
    int32_t r11 = (int32_t)b1 * c1;
    int32_t r10 = (int32_t)b1 * c0;
    uint32_t r00 = (uint32_t)b0 * c0;
    return (((DLB_NACCU)r11 << 32) + ((DLB_NACCU)r01 << 16) + ((DLB_NACCU)r10 << 16) + r00); }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LmpyLL(DLB_LFRACT a, DLB_LFRACT b) {
#ifdef DLB_MPYLL_DROPS_LOWxLOW
  /* This implementation of the 32x32-bit multiplication makes use
     of three 16-bit multiplications, instead of four 16-bit 
     multiplications. The multiplication of both low 16-bit parts is
     kept out. This kind of simplified multiplication is typically
     implemented for processors where no native 32x32-bit multiplication
     is available.
     It is important to stress that this method, although it is faster
     than calculating all four partial products, has the side effect that,
     in case of the multiplication of two small negative values, the calculated
     value can also be negative. For example, given the multiplication
     of x * x:

     if x = -0.0000152583 (ffff8001),

     x * x (full version)   =  0.0000000000 (00000000)
     x * x (simpl. version) = -0.0000000009 (fffffffe)

     if x = -0.0000000009 (fffffffe),

     x * x (full version)   =  0.0000000000 (00000000)
     x * x (simpl. version) = -0.0000000009 (fffffffe)

     This effect shows up, because of the sign extension of the negative
     numbers. The upper parts of the 32-bit operands (see the variables
     "uh" and "vh" in the source-code below) should be zero in this case,
     but they still hold 0xffff. Therefore, when they are used in the
     multiplication, they generate a small negative number.
  */
  DLB_LFRACT product;
  int16_t uh,vh;
  uint16_t ul,vl;

  uh = DLB_H(a);
  ul = DLB_L(a);
  vh = DLB_H(b);
  vl = DLB_L(b);

  product = (uh * vh) * 2 + ((uh * vl) >> 15) + ((ul * vh) >> 15);

  return product;
#else
#ifdef DLB_LMPY_USES_LSB
  return DLB_LtruncN(DLB_NmpyLL(a, b));
#else /* !DLB_LMPY_USES_LSB */
  return DLB_LtruncA(DLB_AmpyLL(a, b));
#endif /* !DLB_LMPY_USES_LSB */
#endif
}

#else /* !DLB_METHOD_SHORT_MULS: don't use short multiplies: */

static DLB_PURE_FN inline DLB_ACCU
DLB_AmpyLS(DLB_LFRACT a, DLB_SFRACT c) {
#ifdef DLB_AVOID_ARMCC_BUG_WITH_SMULWX_INSTRUCTIONS
    int64_t product = (int64_t) a * c;
    return (DLB_ACCU) (product >> 16); }
#else
    return (DLB_ACCU)(((int64_t)a * c) >> 16); }
#endif

static DLB_PURE_FN inline DLB_XACCU
DLB_XmpyLS(DLB_LFRACT a, DLB_SFRACT c) {
    return (DLB_XACCU)a * c; }

static DLB_PURE_FN inline DLB_ACCU
DLB_AmpyLL(DLB_LFRACT a, DLB_LFRACT c) {
    return (DLB_ACCU)(((int64_t)a * c) >> 32); }

static DLB_PURE_FN inline DLB_NACCU
DLB_NmpyLL(DLB_LFRACT a, DLB_LFRACT b) {
    return (DLB_NACCU)a * b; }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LmpyLL(DLB_LFRACT a, DLB_LFRACT b) {
#ifdef DLB_LMPY_USES_LSB
    return DLB_LtruncN(DLB_NmpyLL(a, b));
#else /* !DLB_LMPY_USES_LSB */
    return DLB_LtruncA(DLB_AmpyLL(a, b));
#endif /* !DLB_LMPY_USES_LSB */
}

#endif /* DLB_METHOD_SHORT_MULS */

static DLB_PURE_FN inline DLB_ACCU
DLB_AmacASS(DLB_ACCU a, DLB_SFRACT b, DLB_SFRACT c) {
    return a + DLB_AmpySS(b, c); }

static DLB_PURE_FN inline DLB_ACCU
DLB_AmacALS(DLB_ACCU a, DLB_LFRACT b, DLB_SFRACT c) {
    return a + DLB_AmpyLS(b, c); }

static DLB_PURE_FN inline DLB_ACCU
DLB_AmacALL(DLB_ACCU a, DLB_LFRACT b, DLB_LFRACT c) {
    return a + DLB_AmpyLL(b, c); }

static DLB_PURE_FN inline DLB_ACCU
DLB_AmsuASS(DLB_ACCU a, DLB_SFRACT b, DLB_SFRACT c) {
    return a - DLB_AmpySS(b, c); }

static DLB_PURE_FN inline DLB_ACCU
DLB_AmsuALS(DLB_ACCU a, DLB_LFRACT b, DLB_SFRACT c) {
    return a - DLB_AmpyLS(b, c); }

static DLB_PURE_FN inline DLB_ACCU
DLB_AmsuALL(DLB_ACCU a, DLB_LFRACT b, DLB_LFRACT c) {
    return a - DLB_AmpyLL(b, c); }

static DLB_PURE_FN inline DLB_XACCU
DLB_XmacXLS(DLB_XACCU acc, DLB_LFRACT b, DLB_SFRACT c) {
    return acc + DLB_XmpyLS(b, c); }

static DLB_PURE_FN inline DLB_XACCU
DLB_XmsuXLS(DLB_XACCU acc, DLB_LFRACT b, DLB_SFRACT c) {
    return acc - DLB_XmpyLS(b, c); }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LmpyLS(DLB_LFRACT a, DLB_SFRACT b) {
#ifdef DLB_LMPY_USES_LSB
    return DLB_LtruncX(DLB_XmpyLS(a, b));
#else
    return DLB_LtruncA(DLB_AmpyLS(a, b));
#endif
}

static DLB_PURE_FN inline DLB_LFRACT
DLB_LsmpyLS(DLB_LFRACT a, DLB_SFRACT b) {
#ifdef DLB_LMPY_USES_LSB
    return DLB_LsatX(DLB_XmpyLS(a, b));
#else
    return DLB_LsatA(DLB_AmpyLS(a, b));
#endif
}

static DLB_PURE_FN inline DLB_ACCU
DLB_AmpyAS(DLB_ACCU a, DLB_SFRACT c) {
    return DLB_AmpyLS(_dlb_dbl(a), c); }

/* Mixed integer / fractional multiplications */
static DLB_PURE_FN inline DLB_LFRACT
DLB_LmpyLI(DLB_LFRACT a, int i) {
    int32_t r;
    r = DLB_32srndL(a);
    r = r * i;
    return DLB_L_32(r); }

static DLB_PURE_FN inline int
DLB_ImpyIL(int i, DLB_LFRACT a) {
    int r;
    r = (int)(((intmax_t)a * i ) >> DLB_LFRACT_MANTBITS);
    return r; }

static DLB_PURE_FN inline int
DLB_IrmpyIL(int i, DLB_LFRACT a) {
    int r;
    int rnd = 1 << (DLB_LFRACT_MANTBITS - 1);
    r = (int)(((intmax_t)a * i + rnd) >> DLB_LFRACT_MANTBITS);
    return r; }

static DLB_PURE_FN inline unsigned
DLB_UrmpyUL(unsigned i, DLB_LFRACT a) {
    unsigned r;
    unsigned rnd = 1 << (DLB_LFRACT_MANTBITS - 1);
    DLB_assert(0 <= a, "input check", "Negative input for unsigned multiplication.");
    r = (unsigned)((((uint64_t)i * (unsigned)a) + rnd )>> DLB_LFRACT_MANTBITS);
    return r; }

static DLB_PURE_FN inline int
DLB_ImpyIS(int i, DLB_SFRACT a) {
    int r;
    r = (int)(((intmax_t)a * i ) >> DLB_SFRACT_MANTBITS);
    return r; }

static DLB_PURE_FN inline int
DLB_IrmpyIS(int i, DLB_SFRACT a) {
    int r;
    int rnd = 1 << (DLB_SFRACT_MANTBITS - 1);
    r = (int)(((intmax_t)a * i + rnd) >> DLB_SFRACT_MANTBITS);
    return r; }

static DLB_PURE_FN inline unsigned
DLB_UrmpyUS(unsigned i, DLB_SFRACT a) {
    unsigned r;
    unsigned rnd = 1 << (DLB_SFRACT_MANTBITS - 1);
    DLB_assert(0 <= a, "input check", "Negative input for unsigned multiplication.");
    r = (unsigned)((((uint64_t)i * (unsigned)a) + rnd )>> DLB_SFRACT_MANTBITS);
    return r; }

static DLB_PURE_FN inline DLB_ACCU
DLB_AmacAAS(DLB_ACCU a, DLB_ACCU b, DLB_SFRACT c) {
    return a + DLB_AmpyAS(b, c); }

static DLB_PURE_FN inline DLB_ACCU
DLB_AmsuAAS(DLB_ACCU a, DLB_ACCU b, DLB_SFRACT c) {
    return a - DLB_AmpyAS(b, c); }

#define _dlb_iclipiii(x, lo, hi) \
    (x <= lo ? lo :  hi <= x ? hi : x)

static DLB_PURE_FN inline int
DLB_IlimaI(int i) {
    return _dlb_iclipiii(i, -DLB_ACCU_MANTBITS, DLB_ACCU_MANTBITS); }

static DLB_PURE_FN inline int
DLB_IlimxI(int i) {
    return _dlb_iclipiii(i, -DLB_XACCU_MANTBITS, DLB_XACCU_MANTBITS); }

static DLB_PURE_FN inline int
DLB_IlimnI(int i) {
    return _dlb_iclipiii(i, -DLB_NACCU_MANTBITS, DLB_NACCU_MANTBITS); }

static DLB_PURE_FN inline DLB_ACCU
DLB_AshlAU(DLB_ACCU a, unsigned int shift) {
    DLB_assert(shift < DLB_ACCU_BITS, "shift", "Shift count too large.");
    return a << shift; }

static DLB_PURE_FN inline DLB_ACCU
DLB_AshrAU(DLB_ACCU a, unsigned int shift) {
    DLB_assert(shift < DLB_ACCU_BITS, "shift", "Shift count too large.");
    return a >> shift; }    /* needs a round ? */
    /* Trust compiler to do arithmetic shift on signed,
     * otherwise you'll just have to make this more
     * complicated...  */

static DLB_PURE_FN inline DLB_LFRACT
DLB_LabsdiffLL(DLB_LFRACT a, DLB_LFRACT b) {
    return a <= b ? b - a : a - b; }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LsaddLS(DLB_LFRACT r, DLB_SFRACT c) {
    return _dlb_sadd(r, (int32_t)c << 16); }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LssubLS(DLB_LFRACT r, DLB_SFRACT c) {
    return _dlb_ssub(r, (int32_t)c << 16); }

static DLB_PURE_FN inline unsigned int
DLB_UpnormA(DLB_ACCU a) {
    unsigned int zeros = _dlb_clz32(a);
    return zeros < 2 ? 0 : zeros - 2; }

static DLB_PURE_FN inline unsigned int
DLB_UnormA(DLB_ACCU a) {
    unsigned int zeros = _dlb_clz32(a ^ (a>>31));
    return zeros < 2 ? 0 : zeros - 2; }

static DLB_PURE_FN inline DLB_XACCU
DLB_XmpyXS(DLB_XACCU a, DLB_SFRACT c) {
    int32_t ah = (int32_t)(a >> 16);
    int32_t al = (int32_t)a & 0xFFFF;  /* the bits lost in ah from a */
    return DLB_XmpyLS(ah << 1, c)
         + DLB_AmpyLS(al << 1, c); }

static DLB_PURE_FN inline DLB_XACCU
DLB_XmacXXS(DLB_XACCU a, DLB_XACCU b, DLB_SFRACT c) {
    return a + DLB_XmpyXS(b, c); }

static DLB_PURE_FN inline DLB_XACCU
DLB_XmsuXXS(DLB_XACCU a, DLB_XACCU b, DLB_SFRACT c) {
    return a - DLB_XmpyXS(b, c); }

static DLB_PURE_FN inline DLB_XACCU
DLB_XshrXU(DLB_XACCU a, unsigned int k) {
    DLB_assert(k <= DLB_XACCU_MANTBITS, "shift", "Shift count too large.");
    return a >> k; }

static DLB_PURE_FN inline DLB_XACCU
DLB_XshlXU(DLB_XACCU a, unsigned int k) {
    DLB_assert(k <= DLB_XACCU_MANTBITS, "shift", "Shift count too large.");
    return a << k; }

static DLB_PURE_FN inline DLB_XACCU
DLB_XaddXX(DLB_XACCU a, DLB_XACCU b) {
    return a + b; }

static DLB_PURE_FN inline DLB_XACCU
DLB_XsubXX(DLB_XACCU a, DLB_XACCU b) {
    return a - b; }

static DLB_PURE_FN inline unsigned int
DLB_UpnormX(DLB_XACCU a) {
    unsigned int zeros = _dlb_clz64(a);
    return zeros < 18 ? 0 : zeros - 18; }     /* 18 == 64 - 46 = zeros expected left of mantissa */

static DLB_PURE_FN inline unsigned int
DLB_UnormX(DLB_XACCU a) {
    return DLB_UpnormX(a ^ (a >> 63)); }  /* convert leading 1s (if any) to leading 0s */

static DLB_PURE_FN inline DLB_NACCU
DLB_NmacNLL(DLB_NACCU a, DLB_LFRACT b, DLB_LFRACT c) {
    return a + DLB_NmpyLL(b, c); }

static DLB_PURE_FN inline DLB_NACCU
DLB_NmsuNLL(DLB_NACCU a, DLB_LFRACT b, DLB_LFRACT c) {
    return a - DLB_NmpyLL(b, c); }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LsmpyLL(DLB_LFRACT a, DLB_LFRACT b) {
#ifdef DLB_LMPY_USES_LSB
    return DLB_LsatN(DLB_NmpyLL(a,b));
#else
    return DLB_LsatA(DLB_AmpyLL(a, b));
#endif
}

static DLB_PURE_FN inline DLB_NACCU
DLB_NaddNN(DLB_NACCU a, DLB_NACCU b) {
    return a + b; }

static DLB_PURE_FN inline DLB_NACCU
DLB_NsubNN(DLB_NACCU a, DLB_NACCU b) {
    return a - b; }

static DLB_PURE_FN inline DLB_NACCU
DLB_NshrNU(DLB_NACCU a, unsigned int n) {
    DLB_assert(n <= DLB_NACCU_MANTBITS, "shift", "Shift count too large.");
    return a >> n; }

static DLB_PURE_FN inline DLB_NACCU
DLB_NshlNU(DLB_NACCU a, unsigned int n) {
    DLB_assert(n <= DLB_NACCU_MANTBITS, "shift", "Shift count too large.");
    return a << n; }

static DLB_PURE_FN inline DLB_NACCU
DLB_NshlNI(DLB_NACCU a, int n) {
    return n < 0 ? a >> (unsigned)(-n) : a << (unsigned)n; }

static DLB_PURE_FN inline DLB_NACCU
DLB_NshrNI(DLB_NACCU a, int n) {
    return n < 0 ? a << (unsigned)(-n) : a >> (unsigned)n; }

static DLB_PURE_FN inline unsigned int
DLB_UpnormN(DLB_NACCU a) {
    unsigned int zeros = _dlb_clz64(a);
    return zeros < 2 ? 0 : zeros - 2; }

static DLB_PURE_FN inline unsigned int
DLB_UnormN(DLB_NACCU a) {
    return DLB_UpnormN(a ^ (a >> 63)); }

static DLB_PURE_FN inline DLB_SFRACT
DLB_SrmpySS(DLB_SFRACT a, DLB_SFRACT b) {
    return (DLB_SFRACT)(_dlb_sadd((int32_t)a * b, 0x4000) >> 15); }

static DLB_PURE_FN inline DLB_SFRACT
DLB_S_F(double a) {
    return DLB_SsrndA((DLB_ACCU)(a * 0x40000000)); }

static DLB_PURE_FN inline DLB_SFRACT
DLB_SqF(double a) {
    return DLB_ScqF(a); }

static DLB_PURE_FN inline DLB_LFRACT
DLB_L_F(double a) {
    static const double d_max = 1.0 + 1.0/DLB_INT32_MIN;
    static const double d_one = -(double)DLB_INT32_MIN;
    return a < -1.0 ? DLB_INT32_MIN
         : d_max < a ? DLB_INT32_MAX
         : 0.0 <= a ? (DLB_LFRACT)(a * d_one + 0.5)
         :            (DLB_LFRACT)(a * d_one - 0.5); }

static DLB_PURE_FN inline DLB_LFRACT
DLB_L_FU(double a, unsigned int hr) {
    int32_t neg_one = DLB_INT32_MIN>>hr;
    double d_max = 1.0 + 1.0/neg_one;
    double d_one = -(double)neg_one;
    return a < -1.0 ? neg_one
         : d_max < a ? ~neg_one
         : 0.0 <= a ? (DLB_LFRACT)(a * d_one + 0.5)
         :            (DLB_LFRACT)(a * d_one - 0.5); }


static DLB_PURE_FN inline DLB_LFRACT
DLB_LqFU(double a, unsigned int hr) {
    return DLB_LcqFU(a, hr); }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LqF(double a) {
    return DLB_LcqF(a); }

static DLB_PURE_FN inline DLB_ACCU
DLB_A_F(double a) {
    return _dlb_acf(a); }

static DLB_PURE_FN inline DLB_XACCU
DLB_X_F(double a) {
    return _dlb_xcf(a); }

static DLB_PURE_FN inline DLB_NACCU
DLB_N_F(double a) {
    return _dlb_ncf(a); }

static DLB_PURE_FN inline DLB_ACCU
DLB_A_FU(double a, unsigned int hr) {
    return _dlb_acfi(a, hr); }

static DLB_PURE_FN inline DLB_XACCU
DLB_X_FU(double a, unsigned int hr) {
    return _dlb_xcfi(a, hr); }

static DLB_PURE_FN inline DLB_NACCU
DLB_N_FU(double a, unsigned int hr) {
    return _dlb_ncfi(a, 2*hr); }

static DLB_PURE_FN inline DLB_SFRACT
DLB_SminSS(DLB_SFRACT a, DLB_SFRACT b) {
    return (DLB_SFRACT)(a <= b ? a : b); }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LminLL(DLB_LFRACT a, DLB_LFRACT b) {
    return a <= b ? a : b; }

static DLB_PURE_FN inline DLB_ACCU
DLB_AminAA(DLB_ACCU a, DLB_ACCU b) {
    return a <= b ? a : b; }

static DLB_PURE_FN inline DLB_XACCU
DLB_XminXX(DLB_XACCU a, DLB_XACCU b) {
    return a <= b ? a : b; }

static DLB_PURE_FN inline DLB_NACCU
DLB_NminNN(DLB_NACCU a, DLB_NACCU b) {
    return a <= b ? a : b; }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LclipALL(DLB_ACCU x, DLB_LFRACT lo, DLB_LFRACT hi) {
    return x <= DLB_A_L(lo) ? lo : DLB_A_L(hi) < x ? hi : DLB_LrndA(x) ; }

static DLB_PURE_FN inline DLB_SFRACT
DLB_SclipSSS(DLB_SFRACT x, DLB_SFRACT lo, DLB_SFRACT hi) {
    return DLB_SminSS(DLB_SmaxSS(x, lo), hi); }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LclipLLL(DLB_LFRACT x, DLB_LFRACT lo, DLB_LFRACT hi) {
    return DLB_LminLL(DLB_LmaxLL(x, lo), hi); }

static DLB_PURE_FN inline DLB_ACCU
DLB_AclipAAA(DLB_ACCU x, DLB_ACCU lo, DLB_ACCU hi) {
    return DLB_AminAA(DLB_AmaxAA(x, lo), hi); }

static DLB_PURE_FN inline int
DLB_IltSS(DLB_SFRACT a, DLB_SFRACT b) {
    return a < b; }

static DLB_PURE_FN inline int
DLB_IleqSS(DLB_SFRACT a, DLB_SFRACT b) {
    return a <= b; }

static DLB_PURE_FN inline int
DLB_IeqSS(DLB_SFRACT a, DLB_SFRACT b) {
    return a == b; }

static DLB_PURE_FN inline int
DLB_IltLL(DLB_LFRACT a, DLB_LFRACT b) {
    return a < b; }

static DLB_PURE_FN inline int
DLB_IleqLL(DLB_LFRACT a, DLB_LFRACT b) {
    return a <= b; }

static DLB_PURE_FN inline int
DLB_IeqLL(DLB_LFRACT a, DLB_LFRACT b) {
    return a == b; }

static DLB_PURE_FN inline int
DLB_IltAA(DLB_ACCU a, DLB_ACCU b) {
    return a < b; }

static DLB_PURE_FN inline int
DLB_IleqAA(DLB_ACCU a, DLB_ACCU b) {
    return a <= b; }

static DLB_PURE_FN inline int
DLB_IeqAA(DLB_ACCU a, DLB_ACCU b) {
    return a == b; }

static DLB_PURE_FN inline int
DLB_IltXX(DLB_XACCU a, DLB_XACCU b) {
    return a < b; }

static DLB_PURE_FN inline int
DLB_IleqXX(DLB_XACCU a, DLB_XACCU b) {
    return a <= b; }

static DLB_PURE_FN inline int
DLB_IeqXX(DLB_XACCU a, DLB_XACCU b) {
    return a == b; }

static DLB_PURE_FN inline int
DLB_IltNN(DLB_NACCU a, DLB_NACCU b) {
    return a < b; }

static DLB_PURE_FN inline int
DLB_IleqNN(DLB_NACCU a, DLB_NACCU b) {
    return a <= b; }

static DLB_PURE_FN inline int
DLB_IeqNN(DLB_NACCU a, DLB_NACCU b) {
    return a == b; }

static DLB_PURE_FN inline DLB_SFRACT
DLB_SshlSU(DLB_SFRACT a, unsigned int s) {
    DLB_assert(s < DLB_SFRACT_BITS, "shift", "Shift count too large.");
    return (DLB_SFRACT)(a << s); }

static DLB_PURE_FN inline DLB_SFRACT
DLB_SsshlSU(DLB_SFRACT a, unsigned int s) {
    DLB_assert(s < DLB_SFRACT_BITS, "shift", "Shift count too large.");
    return (DLB_SFRACT)_dlb_sat((int32_t)a << s, 15); }

static DLB_PURE_FN inline DLB_SFRACT
DLB_SshrSU(DLB_SFRACT a, unsigned int s) {
    DLB_assert(s < DLB_SFRACT_BITS, "shift", "Shift count too large.");
    return (DLB_SFRACT)(a >> s); }

static DLB_PURE_FN inline DLB_SFRACT
DLB_SshlSI(DLB_SFRACT a, int s) {
    DLB_assert
        (-DLB_SFRACT_BITS < s && s < DLB_SFRACT_BITS
        ,"shift"
        ,"Shift count out of range."
        );
    return (DLB_SFRACT)(s < 0 ? a >> (unsigned)(-s) : a << (unsigned)s); }

static DLB_PURE_FN inline DLB_SFRACT
DLB_SshrSI(DLB_SFRACT a, int s) {
    DLB_assert
        (-DLB_SFRACT_BITS < s && s < DLB_SFRACT_BITS
        ,"shift"
        ,"Shift count out of range."
        );
    return (DLB_SFRACT)(s < 0 ? a << (unsigned)(-s) : a >> (unsigned)s); }

static DLB_PURE_FN inline DLB_SFRACT
DLB_SsshlSI(DLB_SFRACT a, int s) {
    DLB_assert
        (-DLB_SFRACT_BITS < s && s < DLB_SFRACT_BITS
        ,"shift"
        ,"Shift count out of range."
        );
    return (DLB_SFRACT)(0 <= s ? DLB_SsshlSU(a, s) : DLB_SshrSU(a, -s)); }

static DLB_PURE_FN inline DLB_SFRACT
DLB_SsshrSI(DLB_SFRACT a, int s) {
    DLB_assert
        (-DLB_SFRACT_BITS < s && s < DLB_SFRACT_BITS
        ,"shift"
        ,"Shift count out of range."
        );
    return (DLB_SFRACT)(0 <= s ? DLB_SshrSU(a, s) : DLB_SsshlSU(a, -s)); }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LshlLU(DLB_LFRACT a, unsigned int s) {
    DLB_assert(s < DLB_LFRACT_BITS, "shift", "Shift count too large.");
    return (DLB_LFRACT)(a << s); }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LsshlLU(DLB_LFRACT a, unsigned int s) {
    DLB_assert(s < DLB_LFRACT_BITS, "shift", "Shift count too large.");
    return _dlb_sshlu(a, s); }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LshrLU(DLB_LFRACT a, unsigned int s) {
    DLB_assert(s < DLB_LFRACT_BITS, "shift", "Shift count too large.");
    return (DLB_LFRACT)(a >> s); }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LshlLI(DLB_LFRACT a, int s) {
    DLB_assert
        (-DLB_LFRACT_BITS < s && s < DLB_LFRACT_BITS
        ,"shift"
        ,"Shift count out of range."
        );
    return (DLB_LFRACT)(s < 0 ? a >> (unsigned)(-s) : a << (unsigned)s); }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LshrLI(DLB_LFRACT a, int s) {
    DLB_assert
        (-DLB_LFRACT_BITS < s && s < DLB_LFRACT_BITS
        ,"shift"
        ,"Shift count out of range."
        );
    return (DLB_LFRACT)(s < 0 ? a << (unsigned)(-s) : a >> (unsigned)s); }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LsshlLI(DLB_LFRACT a, int s) {
    return 0 <= s ? DLB_LsshlLU(a, s) : DLB_LshrLU(a, -s); }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LsshrLI(DLB_LFRACT a, int s) {
    return 0 <= s ? DLB_LshrLU(a, s) : DLB_LsshlLU(a, -s); }

static DLB_PURE_FN inline DLB_ACCU
DLB_AshlAI(DLB_ACCU a, int s) {
    DLB_assert
        (-DLB_ACCU_BITS < s && s < DLB_ACCU_BITS
        ,"shift"
        ,"Shift count out of range."
        );
    return (0 <= s) ? a << (unsigned)s : a >> (unsigned)(-s); }

static DLB_PURE_FN inline DLB_ACCU
DLB_AshrAI(DLB_ACCU a, int s) {
    DLB_assert
        (-DLB_ACCU_BITS < s && s < DLB_ACCU_BITS
        ,"shift"
        ,"Shift count out of range."
        );
    return (0 <= s) ? a >> (unsigned)s : a << (unsigned)(-s); }

static DLB_PURE_FN inline DLB_XACCU
DLB_XshlXI(DLB_XACCU a, int s) {
    DLB_assert
        (-DLB_XACCU_BITS < s && s < DLB_XACCU_BITS
        ,"shift"
        ,"Shift count out of range."
        );
    return (0 <= s) ? a << (unsigned)s : a >> (unsigned)(-s); }

static DLB_PURE_FN inline DLB_XACCU
DLB_XshrXI(DLB_XACCU a, int s) {
    DLB_assert
        (-DLB_XACCU_BITS < s && s < DLB_XACCU_BITS
        ,"shift"
        ,"Shift count out of range."
        );
    return (0 <= s) ? a >> (unsigned)s : a << (unsigned)(-s); }

static DLB_PURE_FN inline int
DLB_IlimsI(int i) {
    return _dlb_iclipiii(i, -DLB_SFRACT_MANTBITS, DLB_SFRACT_MANTBITS); }

static DLB_PURE_FN inline int
DLB_IlimlI(int i) {
    return _dlb_iclipiii(i, -DLB_LFRACT_MANTBITS, DLB_LFRACT_MANTBITS); }

static DLB_PURE_FN inline unsigned int
DLB_UpnormS(DLB_SFRACT a) {
    return _dlb_clz32(a) - 17; }

static DLB_PURE_FN inline unsigned int
DLB_UnormS(DLB_SFRACT a) {
    return _dlb_clz32(a ^ (a >> 15)) - 17; }

static DLB_PURE_FN inline unsigned int
DLB_UpnormL(DLB_LFRACT a) {
    return _dlb_clz32(a) - 1; }

static DLB_PURE_FN inline unsigned int
DLB_UnormL(DLB_LFRACT a) {
    return _dlb_clz32(a ^ _dlb_sgn(a)) - 1; }

static DLB_PURE_FN inline DLB_SFRACT
DLB_SaddSS(DLB_SFRACT a, DLB_SFRACT b) {
    return (DLB_SFRACT)(a + b); }

static DLB_PURE_FN inline DLB_SFRACT
DLB_SsubSS(DLB_SFRACT a, DLB_SFRACT b) {
    return (DLB_SFRACT)(a - b); }

static DLB_PURE_FN inline DLB_SFRACT
DLB_SsaddSS(DLB_SFRACT a, DLB_SFRACT b) {
    return (DLB_SFRACT)_dlb_sat(a + b, 15); }

static DLB_PURE_FN inline DLB_SFRACT
DLB_SssubSS(DLB_SFRACT a, DLB_SFRACT b) {
    return (DLB_SFRACT)_dlb_sat(a - b, 15); }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LaddLL(DLB_LFRACT a, DLB_LFRACT b) {
    return a + b; }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LsubLL(DLB_LFRACT a, DLB_LFRACT b) {
    return a - b; }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LsaddLL(DLB_LFRACT a, DLB_LFRACT b) {
    return _dlb_sadd(a, b); }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LssubLL(DLB_LFRACT a, DLB_LFRACT b) {
    return _dlb_ssub(a, b); }

static DLB_PURE_FN inline DLB_SFRACT
DLB_SnegS(DLB_SFRACT a) {
    return (DLB_SFRACT)(-a); }

static DLB_PURE_FN inline DLB_SFRACT
DLB_SsnegS(DLB_SFRACT a) {
    return (DLB_SFRACT)(a == DLB_INT16_MIN ? DLB_INT16_MAX : -a); }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LnegL(DLB_LFRACT a) {
    return -a; }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LsnegL(DLB_LFRACT a) {
    return a == DLB_INT32_MIN ? DLB_INT32_MAX : -a; }

static DLB_PURE_FN inline DLB_ACCU
DLB_AnegA(DLB_ACCU a) {
    return -a; }

static DLB_PURE_FN inline DLB_XACCU
DLB_XnegX(DLB_XACCU a) {
    return -a; }

static DLB_PURE_FN inline DLB_NACCU
DLB_NnegN(DLB_NACCU a) {
    return -a; }

static DLB_PURE_FN inline DLB_SFRACT
DLB_SabsS(DLB_SFRACT a) {
    return (DLB_SFRACT)_dlb_abs(a); }

#ifdef DLB_ABS_FAST_APPROX
static DLB_PURE_FN inline DLB_SFRACT
DLB_SsabsS(DLB_SFRACT a) {
    return (DLB_SFRACT)_dlb_abs(a); }    /* approximation does not overflow */
#else  /* !DLB_ABS_FAST_APPROX */
static DLB_PURE_FN inline DLB_SFRACT
DLB_SsabsS(DLB_SFRACT a) {
    return DLB_SmaxSS(a, DLB_SsnegS(a)); }
#endif /* !DLB_ABS_FAST_APPROX */

static DLB_PURE_FN inline DLB_LFRACT
DLB_LabsL(DLB_LFRACT a) {
    return _dlb_abs(a); }

#ifdef DLB_ABS_FAST_APPROX
static DLB_PURE_FN inline DLB_LFRACT
DLB_LsabsL(DLB_LFRACT a) {
    return _dlb_abs(a); }
#else  /* !DLB_ABS_FAST_APPROX */
static DLB_PURE_FN inline DLB_LFRACT
DLB_LsabsL(DLB_LFRACT a) {
    return DLB_LmaxLL(a, DLB_LsnegL(a)); }
#endif /* !DLB_ABS_FAST_APPROX */

static DLB_PURE_FN inline DLB_LFRACT
DLB_LmpySS(DLB_SFRACT a, DLB_SFRACT b) {
    return DLB_LtruncA(DLB_AmpySS(a, b)); }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LsmpySS(DLB_SFRACT a, DLB_SFRACT b) {
    return DLB_LsatA(DLB_AmpySS(a, b)); }

static DLB_PURE_FN inline DLB_SFRACT
DLB_SsrmpySS(DLB_SFRACT a, DLB_SFRACT b) {
    return DLB_SsrndA(DLB_AmpySS(a, b)); }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LmacLSS(DLB_LFRACT a, DLB_SFRACT x, DLB_SFRACT y) {
    return a + DLB_LtruncA(DLB_AmpySS(x, y)); }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LsmacLSS(DLB_LFRACT a, DLB_SFRACT x, DLB_SFRACT y) {
#ifdef DLB_LMAC_USES_LMPY
    return DLB_LsaddLL(a, DLB_LsmpySS(x, y));
#else
    return _dlb_sdadd(a, DLB_AmpySS(x, y));
#endif
}

static DLB_PURE_FN inline DLB_LFRACT
DLB_LmsuLSS(DLB_LFRACT a, DLB_SFRACT x, DLB_SFRACT y) {
    return a - DLB_LtruncA(DLB_AmpySS(x, y)); }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LsmsuLSS(DLB_LFRACT a, DLB_SFRACT x, DLB_SFRACT y) {
#ifdef DLB_LMAC_USES_LMPY
    return DLB_LssubLL(a, DLB_LsmpySS(x, y));
#else
    return _dlb_sdsub(a, DLB_AmpySS(x, y));
#endif
}

static DLB_PURE_FN inline DLB_LFRACT
DLB_LmacLLS(DLB_LFRACT a, DLB_LFRACT x, DLB_SFRACT y) {
#ifdef DLB_LMAC_USES_LMPY
    return a + DLB_LmpyLS(x, y);
#else
    return a + DLB_LtruncA(DLB_AmpyLS(x, y));
#endif
}

static DLB_PURE_FN inline DLB_LFRACT
DLB_LsmacLLS(DLB_LFRACT a, DLB_LFRACT x, DLB_SFRACT y) {
#ifdef DLB_LMAC_USES_LMPY
    return DLB_LsaddLL(a, DLB_LsmpyLS(x, y));
#else
    return _dlb_sdadd(a, DLB_AmpyLS(x, y));
#endif
}

static DLB_PURE_FN inline DLB_LFRACT
DLB_LmsuLLS(DLB_LFRACT a, DLB_LFRACT x, DLB_SFRACT y) {
#ifdef DLB_LMAC_USES_LMPY
    return a - DLB_LmpyLS(x, y);
#else
    return a - DLB_LtruncA(DLB_AmpyLS(x, y));
#endif
}

static DLB_PURE_FN inline DLB_LFRACT
DLB_LsmsuLLS(DLB_LFRACT a, DLB_LFRACT x, DLB_SFRACT y) {
#ifdef DLB_LMAC_USES_LMPY
    return DLB_LssubLL(a, DLB_LsmpyLS(x, y));
#else
    return _dlb_sdsub(a, DLB_AmpyLS(x, y));
#endif
}

static DLB_PURE_FN inline DLB_LFRACT
DLB_LmacLLL(DLB_LFRACT a, DLB_LFRACT x, DLB_LFRACT y) {
    return a + DLB_LmpyLL(x, y); }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LsmacLLL(DLB_LFRACT a, DLB_LFRACT x, DLB_LFRACT y) {
#ifdef DLB_LMAC_USES_LMPY
    return DLB_LsaddLL(a, DLB_LsmpyLL(x, y));
#else
    return _dlb_sdadd(a, DLB_AmpyLL(x, y));
#endif
}

static DLB_PURE_FN inline DLB_LFRACT
DLB_LmsuLLL(DLB_LFRACT a, DLB_LFRACT x, DLB_LFRACT y) {
    return a - DLB_LmpyLL(x, y); }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LsmsuLLL(DLB_LFRACT a, DLB_LFRACT x, DLB_LFRACT y) {
#ifdef DLB_LMAC_USES_LMPY
    return DLB_LssubLL(a, DLB_LsmpyLL(x, y));
#else
    return _dlb_sdsub(a, DLB_AmpyLL(x, y));
#endif
}

static DLB_PURE_FN inline DLB_LFRACT
DLB_LsrndLU(DLB_LFRACT a, unsigned int s) {
    unsigned lsbits = DLB_LFRACT_MANTBITS - s;
    DLB_assert(s < DLB_LFRACT_MANTBITS
              , "quantize_round"
              , "The number of bits to be kept should be less than DLB_LFRACT_MANTBITS.");
    return (_dlb_sadd(a, 1<<(lsbits-1)) >> lsbits) << lsbits; }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LtruncLU(DLB_LFRACT a, unsigned int s) {
    unsigned lsbits = DLB_LFRACT_MANTBITS - s;
    DLB_assert(s < DLB_LFRACT_MANTBITS
              , "quantize"
              , "The number of bits to be kept should be less than DLB_LFRACT_MANTBITS.");
    return (a >> lsbits) << lsbits; }

static DLB_PURE_FN inline DLB_ACCU   DLB_AstoreA(DLB_ACCU a)   { return a; }
static DLB_PURE_FN inline DLB_XACCU  DLB_XstoreX(DLB_XACCU a)  { return a; }
static DLB_PURE_FN inline DLB_NACCU  DLB_NstoreN(DLB_NACCU a)  { return a; }
static DLB_PURE_FN inline DLB_SFRACT DLB_SflushS(DLB_SFRACT a) { return a; }
static DLB_PURE_FN inline DLB_LFRACT DLB_LflushL(DLB_LFRACT a) { return a; }
static DLB_PURE_FN inline DLB_ACCU   DLB_AflushA(DLB_ACCU   a) { return a; }
static DLB_PURE_FN inline DLB_XACCU  DLB_XflushX(DLB_XACCU  a) { return a; }
static DLB_PURE_FN inline DLB_NACCU  DLB_NflushN(DLB_NACCU  a) { return a; }

/* Conversions between headroom and plain versions of same types */
static DLB_PURE_FN inline DLB_LFRACT
DLB_LleftLU(DLB_LFRACT a, unsigned int hr) {
    return _dlb_sshlu(a, hr); }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LheadLU(DLB_LFRACT a, unsigned int hr) {
    DLB_assert(hr <= DLB_LFRACT_MANTBITS, "convert with headroom", "Headroom is too large.");
#if defined(DLB_REALLY_ROUND_LONG)
    return hr > 0 ? (DLB_LFRACT)_dlb_sadd(a, 1<<(hr-1)) >> hr : a;
#else
    return a >> hr;
#endif
}


#ifdef DLB_INTRINSICS_INCLUDE_DEPRECATED_OPS
/* Should do pre- and post-scaling by using LheadLU and LleftLU. */
static DLB_PURE_FN inline DLB_ACCU
DLB_AleftAU(DLB_ACCU a, unsigned int hr) {
    return _dlb_sshlu(a, hr); }

static DLB_PURE_FN inline DLB_XACCU
DLB_XleftXU(DLB_XACCU a, unsigned int hr) {
    int64_t sgn = a >> (DLB_XACCU_BITS-1);
    int64_t sat = (((uint64_t)1<<(DLB_XACCU_BITS-1)) - 1);
    int64_t lim = sat >> hr;
    DLB_assert(hr <= DLB_XACCU_MANTBITS, "convert with headroom", "Headroom is too large.");
    return (a ^ sgn) <= lim ? a << hr : sat ^ sgn; }

static DLB_PURE_FN inline DLB_NACCU
DLB_NleftNU(DLB_NACCU a, unsigned int hr) {
    int64_t sgn = a >> (DLB_NACCU_BITS-1);
    int64_t sat = ((uint64_t)1 << (DLB_NACCU_BITS-1)) - 1;
    int64_t lim = sat >> 2 * hr;
    DLB_assert(hr*2 <= DLB_NACCU_MANTBITS, "convert with headroom", "Headroom is too large.");
    return (a ^ sgn) <= lim ? a << 2*hr : sat ^ sgn; }

static DLB_PURE_FN inline DLB_ACCU
DLB_AheadAU(DLB_ACCU a, unsigned int hr) {
    DLB_assert(hr <= DLB_ACCU_MANTBITS, "convert with headroom", "Headroom is too large.");
#if defined(DLB_REALLY_ROUND_LONG)
    return hr > 0 ? (DLB_ACCU)_dlb_sadd(a, 1<<(hr-1)) >> hr : a;
#else
    return a >> hr;
#endif
}

static DLB_PURE_FN inline DLB_XACCU
DLB_XheadXU(DLB_XACCU a, unsigned int hr) {
    DLB_assert(hr <= DLB_XACCU_MANTBITS, "convert with headroom", "Headroom is too large.");
#if defined(DLB_REALLY_ROUND_LONG)
    return hr > 0 ? (a + ((DLB_XACCU)1<<(hr-1))) >> hr : a;
#else
    return a >> hr;
#endif
}

static DLB_PURE_FN inline DLB_NACCU
DLB_NheadNU(DLB_NACCU a, unsigned int hr) {
    DLB_assert(hr*2 <= DLB_NACCU_MANTBITS, "convert with headroom", "Headroom is too large.");
#if defined(DLB_REALLY_ROUND_LONG)
    return hr > 0 ? (a + ((DLB_NACCU)1<<(2*hr-1))) >> 2*hr : a;
#else
    return a >> (hr*2);
#endif
}

/* DLB_XcF* and DLB_NcF* are deprecated ops and will not be supported in future releases */
/* A sample alternative construction would be as follows:
         const DLB_LFRACT Lfoo = DLB_LcF(bar);
         DLB_NACCU Nfoo = DLB_N_L(Lfoo);
*/
#define DLB_AcF(a)     _dlb_acf((a))
#define DLB_XcF(a)     _dlb_xcf((a))
#define DLB_NcF(a)     _dlb_ncf((a))
#define DLB_AcFU(a,hr) _dlb_acfi((a),(hr))
#define DLB_XcFU(a,hr) _dlb_xcfi((a),(hr))
#define DLB_NcFU(a,hr) _dlb_ncfi((a), 2*(hr))

/* in v1.5 the name changed to reflect the reality of saturation of the output value */
#define DLB_IrndS       DLB_IsrndS
#define DLB_IrndL       DLB_IsrndL
#endif /* DLB_INTRINSICS_INCLUDE_DEPRECATED_OPS */

/** @} */
