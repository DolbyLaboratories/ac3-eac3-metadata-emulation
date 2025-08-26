/****************************************************************************
 *
 *
 * Copyright (c) 2010-2013 Dolby International AB.
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
 * @section GENERIC_FLOAT32
 * Abstraction for floating point single precision architecture.
 * The backend can be compiled with different preprocessor directives which influence the backend behavior.
 * The list of defines can be found at @ref float32_behavior_defines "backend description".
 * @}
 */

/****************************************************************************//**
 * @file
 * @brief Abstraction for floating point single precision architecture
 * @{
 * @section float32_behavior_defines Behavior-Altering Preprocessor Directives
 * The following symbols may be defined (perhaps implicitly, by the compiler)
 * to alter behaviour in various ways:
 * 
 * @par DLB_DO_FLOAT_SATURATE
 * All saturating operations (e.g. L*s*addLS()) will explicitly saturate by clipping to the range [-1,1].
 * @par DLB_MUST_FLUSH_DENORMS
 * Floating point values less than FLT_MIN can be set to 0.0f for performance reasons by
 * a call to the provided flush operation. If the define is not set these operations return
 * the input value.
 *
 * @}
 * @internal
 * @{
 */
/*
processor abstraction naming macros
******************************************************************************/
/* A short descriptive name for the processor abstraction.
 * It should be a valid C identifier and be suitable for printing as a string
 * for diagnostics.
 *
 */
#ifndef DLB_BACKEND_NAME
#define DLB_BACKEND_NAME float32
#endif /* DLB_BACKEND_NAME not previously declared */

/*
processor-specific base types
******************************************************************************/
typedef float DLB_SFRACT;
typedef float DLB_LFRACT;
typedef float DLB_ACCU;
typedef double DLB_XACCU;
typedef double DLB_NACCU;

#define DLB_DOUBLE_MANTBITS 53

#define DLB_SFRACT_MANTBITS 24
#define DLB_LFRACT_MANTBITS 24
#define DLB_ACCU_MANTBITS   24
#define DLB_XACCU_MANTBITS  DLB_DOUBLE_MANTBITS
#define DLB_NACCU_MANTBITS  DLB_DOUBLE_MANTBITS

#define DLB_SFRACT_BITS 32
#define DLB_LFRACT_BITS 32
#define DLB_ACCU_BITS   32
#define DLB_XACCU_BITS  64
#define DLB_NACCU_BITS  64

#ifndef DLB_INTRINSICS_NAME
#define DLB_INTRINSICS_NAME DLB_BACKEND_NAME
#endif

#include <math.h>  /* for the system definition of floorf(), fminf(), fmaxf(), ldexpf() and frexpf() */
#include <float.h> /* necessary for FLT_MIN, DBL_MIN */

/* Define the "flush" point for the DLB_MUST_FLUSH_DENORMS "one block's worth of rounding" above the
 * hardware denormal range, so that manual flushing can be done at block boundaries (of various sorts)
 * rather than at every operation
 */
#define _dlb_flush_eps  (DBL_MIN*512)
#define _dlb_flushf_eps (FLT_MIN*512)

/*
 * C99 extends math.h with equivalent versions that use float arguments
 * and results instead of doubles.  Clearly we want to use these here,
 * as this is a 32-bit float backend.  However, we still want this
 * code to "work" and be compatible with C89/C90 compilers and libraries
 * that only have the double precision versions.  So we provide wrappers
 * that provide float interfaces by type-casting.  However, it is
 * fairly common practice for some pre-C99 compilers to also provide
 * these functions and we want to be able to use them if they exist.
 * Unfortunately, there is no way to tell whether a function exists
 * in a library at compile-time, so we can't enable that behaviour
 * automatically.  So we introduce another compile-time symbol:
 * DLB_HAVE_C99_FLOAT_MATH_FN that can be set in the compilation
 * environment of pre-C99 compilers to tell us *not* to do the
 * double-precision wrapping.
 */

#if !(defined(DLB_HAVE_C99_FLOAT_MATH_FN) || (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L))
/* some non-c99 math.h define these anyway, so we should avoid multiple definitions: */
#define floorf(x_) ((float)floor(x_))
#define fminf(x_, y_) ((float)fmin(x_, y_))
#define fmaxf(x_, y_) ((float)fmax(x_, y_))
#define fabsf(x_) ((float)fabs(x_))
#define ldexpf(x_,i_) ((float)ldexp((x_),(i_)))
#define frexpf(x_,p_) ((float)frexp((x_),(p_)))
#endif /* no c99 math: wrap old double-precision definitions to allow degraded functionality */

/*
scaling macros
******************************************************************************/
#ifdef DLB_DO_FLOAT_SATURATE
/* If we care about saturation, coefficients need to be scaled down */
#   define DLB_UscaleU(i)      (i)
#   define DLB_FscaleFU(x,i)   ((x) / ((uintmax_t)1<<(i)))
#else
/* If we don't care about saturation, we don't need to scale all the
 * coefficients down to fit into range.
 */
#   define DLB_UscaleU(i)      (0)
#   define DLB_FscaleFU(x,i)   (x)
#endif


/* This floating-point back-end ignores "headroom" arguments, starting with
 * these constant data conversion macros: */
#define DLB_ScFU(a,hr) ((DLB_SFRACT)(a))
#define DLB_LcFU(a,hr) ((DLB_LFRACT)(a))

#define DLB_FRACT_1(bits) (1.0f)
#define DLB_FRACT_MIN(bits) (-1.0f)
#define DLB_FRACT_MAX(bits) (1.0f)

#define DLB_LFRACT_1 ((DLB_LFRACT)1.0f)
#define DLB_SFRACT_1 ((DLB_SFRACT)1.0f)
#define DLB_ACCU_1   ((DLB_ACCU)1.0f)

#define DLB_LFRACT_MAX (DLB_LFRACT_1)
#define DLB_LFRACT_MIN (-DLB_LFRACT_1)
#define DLB_LcF(a) ((DLB_LFRACT)(a))
#define DLB_LcIU(a, s) ((DLB_LFRACT)(a)/((intmax_t)1<<(s)))

#define DLB_SFRACT_MAX (DLB_SFRACT_1)
#define DLB_SFRACT_MIN (-DLB_SFRACT_1)
#define DLB_ScF(a) ((DLB_SFRACT)(a))
#define DLB_ScIU(a, s) ((DLB_SFRACT)(a)/((intmax_t)1<<(s)))

#define DLB_ACCU_MAX (DLB_ACCU_1)
#define DLB_ACCU_MIN (-DLB_ACCU_1)

#define DLB_MAX_SHIFT   60

/* icky legacy bit-bashing */
#define DLB_L(a) (a)
#define DLB_H(a) (a)

#ifdef DLB_OPT_USE_LIBM
/* Ideally, one would hope that the compiler would have intrinsic fmaxf, fminf and fabsf that
 * emitted the corresponding sse instructions, but alas gcc 4.2 on my Mac seems not to.
 */
#ifndef DLB_HAVE_FMAXF
#define _dlb_fmaxf fmaxf
#define _dlb_fmax  fmax
#endif
#ifndef DLB_HAVE_FMINF
#define _dlb_fminf fminf
#define _dlb_fmin  fmin
#endif
#ifndef DLB_HAVE_FABSF
#define _dlb_fabsf fabsf
#define _dlb_fabs  fabs
#endif

#else /* !defined(DLB_OPT_USE_LIBM): do it ourselves */
#ifndef DLB_HAVE_FMAXF
static DLB_PURE_FN inline float _dlb_fmaxf(float a, float b)   { return a < b ? b : a; }
static DLB_PURE_FN inline double _dlb_fmax(double a, double b) { return a < b ? b : a; }
#endif
#ifndef DLB_HAVE_FMINF
static DLB_PURE_FN inline float _dlb_fminf(float a, float b)   { return a < b ? a : b; }
static DLB_PURE_FN inline double _dlb_fmin(double a, double b) { return a < b ? a : b; }
#endif
#ifndef DLB_HAVE_FABSF
static DLB_PURE_FN inline float _dlb_fabsf(float a)  { return _dlb_fmaxf(a, -a); }
static DLB_PURE_FN inline double _dlb_fabs(double a) { return _dlb_fmax(a, -a); }
#endif
#endif /* DLB_OPT_USE_LIBM */

static DLB_PURE_FN inline float _dlb_fclip(float a, float lo, float hi) { return _dlb_fminf(_dlb_fmaxf(a, lo), hi); }

static DLB_PURE_FN inline long _dlb_lmax(long a, long b) { return a < b ? b : a; }
static DLB_PURE_FN inline long _dlb_lmin(long a, long b) { return a < b ? a : b; }
static DLB_PURE_FN inline long _dlb_lclip(long a, long lo, long hi) { return _dlb_lmin( _dlb_lmax(a, lo), hi); }

/* Conceptually assert(0 <= n && n <= 31) here, but can't really do that in a compile-time
 * context, unfortunately.  Be careful! */
#define _two_to(n) (0 < (unsigned)(n) ? (2.0f)*(1 << (unsigned)((n)-1)) : 1.0f)

/* compile time 0.5^^n for constant n (requires a division, so only good for compile time) */
#define _half_to(n) ((1.0f)/_two_to(n))

#ifdef DLB_DO_FLOAT_SATURATE
#define _dlb_fsat(a) _dlb_fmaxf(_dlb_fminf(a, 1.0f), -1.0f)
#else /* !DLB_DO_FLOAT_SATURATE */
#define _dlb_fsat(a) (a)
#endif /* DLB_DO_FLOAT_SATURATE */
#define _fsat(a) _dlb_fsat(a)

#define _iqfu(a, sigbits) \
    ((int)(_two_to(sigbits) * (a) + ((a) < 0.0f ? -0.5f : 0.5f)))

#define _iqfu15(a) ((int)(32768.0f * (a) + ((a) < 0.0f ? -0.5f : 0.5f)))

#define _isatq(a, q) \
    ( (a) <= -1.0f               ? -1 - (int)((1u << (q)) - 1) \
    : 1.0f - _half_to(q) <= (a)  ?      (int)((1u << (q)) - 1) \
    : _iqfu((a), (q)) )

#define _isatq15(a) \
    ( (a) <= -1.0f               ? -32768 \
    : (32767.0f/32768.0f) <= (a)   ?  32767 \
    : _iqfu15(a))

#define _fsatq(a,q) (_half_to(q) * _isatq((a), (q)))
#define _fsatq15(a) ((1.0f/32768.0f) * _isatq15(a))

#define _fcqf(a,q) (_half_to(q) * _iqfu((a), (q)))

#define DLB_LcqF(a)     _fsatq15(a)
#define DLB_LcqFU(a,hr) _fsatq15(a)
#define DLB_ScqF(a)     _fsatq15(a)
#define DLB_ScqFU(a,hr) _fsatq15(a)

#define DLB_FcqFU(a,q)  _fsatq((a), (q))
#define DLB_FcqF(a)     _fsatq15(a)

/* API parameter conversion set
 * API can specify DLB_NATIVE_FRAC_BITS as it pleases */
#ifndef DLB_NATIVE_FRAC_BITS
#define DLB_NATIVE_FRAC_BITS (15)
#endif

#ifdef DLB_MUST_FLUSH_DENORMS
#define _flushf(x) (_dlb_flushf_eps < _dlb_fabsf(x) ? (x) : 0.0f)
#define _flush(x)  (_dlb_flush_eps  < _dlb_fabs(x)  ? (x) : 0.0)
#else
#define _flushf(x) (x)
#define _flush(x) (x)
#endif /* DLB_MUST_FLUSH_DENORMS */

/* The C standard apparently guarantees that this union construct will
 * do what we expect but C++ allows it to fall under the "strict aliasing"
 * rules, which might cause problems. */

/* Actually, the guarantee only holds for unions with chars, but gcc
 * apparently does honour the union with int form.  So we need some extra
 * fallbacks.  Trouble with the char-alias "solution" is that it is inherently
 * endian-sensitive, so we need both explicitly little-endian and big-endian
 * versionsn of these helper conversion functions.
 * Also: it isn't possible to return a unsigned char array[4] from a function, so
 * we have to implement that direction as a reference modifier.  Yuck.
 */

#ifndef DLB_ENDIAN_BIG
static inline int32_t _dlb_asint32char4(unsigned char c[4]) {
    return c[0] + ((int32_t)c[1] << 8) + ((int32_t)c[2] << 16) + ((int32_t)((signed char)c[3]) << 24); }

static inline void _dlb_aschar4int32(unsigned char c[4], int32_t i) {
    c[0] = (unsigned char)i;
    c[1] = (unsigned char)(i>>8);
    c[2] = (unsigned char)(i>>16);
    c[3] = (unsigned char)(i>>24); }
#else /* DLB_ENDIAN_BIG */
static inline int32_t _dlb_asint32char4(unsigned char c[4]) {
    return c[3] + ((int32_t)c[2] << 8) + ((int32_t)c[1] << 16) + ((int32_t)((signed char)c[0]) << 24); }
static inline void _dlb_aschar4int32(unsigned char c[4], int32_t i) {
    c[3] = (unsigned char)i;
    c[2] = (unsigned char)(i>>8);
    c[1] = (unsigned char)(i>>16);
    c[0] = (unsigned char)(i>>24); }
#endif /* DLB_ENDIAN_BIG */

#ifndef DLB_USE_SAFE_CHAR_ALIAS
static inline int32_t _dlb_asintf(float f) {
	union {int32_t i; float f;} fi; fi.f = f; return fi.i; }

static inline float _dlb_asfloati(int32_t i) {
	union {int32_t i; float f;} fi; fi.i = i; return fi.f; }
#else /* DLB_USE_SAFE_CHAR_ALIAS */
static inline int32_t _dlb_asintf(float f) {
	union {unsigned char c[4]; float f;} fc; fc.f = f; return _dlb_asint32char4(fc.c); }

static inline float _dlb_asfloati(int32_t i) {
	union {unsigned char c[4]; float f;} fc; _dlb_aschar4int32(fc.c, i); return fc.f; }
#endif /* DLB_USE_SAFE_CHAR_ALIAS */

#ifndef DLB_OPT_USE_FREXPF
/* This is careless with inf and NaN, but we don't care... (our use cases avoid those anyway,
 * thanks to fixed-point compatability.)  Uses bit-manipulation of arg to extract exponent:
 *  will probably break for non-IEEE float and might be trouble for C++ */
static inline int _dlb_getexpf(float f) {
    int fi = _dlb_asintf(f) << 1;	/* left-shift to lose sign bit */
    return 0 == fi ? 0 : (int)(fi >> 24) - 126; }
#else /* DLB_OPT_USE_FREXPF */
static inline int _dlb_getexpf(float f) {
    int exp; (void)frexpf(f, &exp); return exp; }
#endif /* DLB_OPT_USE_FREXPF */

#ifndef DLB_OPT_USE_LDEXPF
static inline float _dlb_ldexpf(float f, int pow_2) {
    return f * _dlb_asfloati((127 + pow_2) << 23); }
#else /* fall-back to math lib */
static inline float _dlb_ldexpf(float f, int pow_2) {
    return ldexpf(f, pow_2); }
#endif /* DLB_OPT_USE_LDEXPF */

static inline float _dlb_getmantf(float f) { return _dlb_ldexpf(f, -_dlb_getexpf(f)); }

#define _dlb_shlu(a, shift) _dlb_ldexpf((a), (int)(shift))
#define _dlb_shru(a, shift) _dlb_ldexpf((a), -(int)(shift))
#define _dlb_dshlu(a, shift) ((a) * _dlb_ldexpf(1.0f, (int)(shift)))
#define _dlb_dshru(a, shift) ((a) * _dlb_ldexpf(1.0f, -(int)(shift)))

static const float _dlb_Q15 = 32768.0f;
static const float _dlb_Q31 = -(float)(INT32_MIN);
static const float _dlb_Qnative = -(float)((long)(~0L << DLB_NATIVE_FRAC_BITS));
static const float _dlb_Q15_recip = 1.0f/32768.0f;
static const float _dlb_Q31_recip = -1.0f/(float)(INT32_MIN);
static const float _dlb_Qnative_recip = -1.0f/((long)(~0L << DLB_NATIVE_FRAC_BITS));
static const long INATIVE_MAX = ~(~0L << DLB_NATIVE_FRAC_BITS);

/* Now the implementations... */
static DLB_PURE_FN inline int
DLB_I_S(DLB_SFRACT x) {
    return (int)_dlb_lmin(INATIVE_MAX, (long)floorf(x * _dlb_Qnative)); }

static DLB_PURE_FN inline int
DLB_IsrndS(DLB_SFRACT x) {
    return (int)_dlb_lmin(INATIVE_MAX, lrintf(x * _dlb_Qnative)); }

static DLB_PURE_FN inline DLB_SFRACT
DLB_S_I(int x) {
    return (DLB_SFRACT)(_dlb_Qnative_recip * x); }

static DLB_PURE_FN inline int
DLB_I_LU(DLB_LFRACT a, unsigned hr) { (void)hr;
    return (int)_dlb_lmin(INATIVE_MAX, (long)floorf(a * _dlb_Qnative)); }

static DLB_PURE_FN inline int
DLB_I_L(DLB_LFRACT a) {
    return (int)_dlb_lmin(INATIVE_MAX, (long)floorf(a * _dlb_Qnative)); }

static DLB_PURE_FN inline int
DLB_IsrndL(DLB_LFRACT a) {
    return (int)_dlb_lmin(INATIVE_MAX, lrintf(a * _dlb_Qnative)); }

static DLB_PURE_FN inline int
DLB_IsrndLU(DLB_LFRACT a, unsigned fp) {
    return (int)_dlb_lmin(~(~0L << fp), lrintf(_dlb_shlu(a, fp))); }

static DLB_PURE_FN inline DLB_LFRACT
DLB_L_IU(int a, unsigned hr) { (void)hr;
    return (DLB_LFRACT)(_dlb_Qnative_recip * a); }

static DLB_PURE_FN inline DLB_LFRACT
DLB_L_I(int a) {
    return (DLB_LFRACT)(_dlb_Qnative_recip * a); }

static DLB_PURE_FN inline DLB_LFRACT
DLB_L_FU(double f, unsigned hr) {(void)hr; /* UNUSED */
    return (DLB_LFRACT) f; }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LqFU(double f, unsigned hr) {(void)hr; /* UNUSED */
    return (DLB_LFRACT)DLB_LcqFU(f, hr); }

static DLB_PURE_FN inline DLB_ACCU
DLB_A_FU(double f, unsigned hr) { (void)hr;
    return (DLB_LFRACT) f; }

static DLB_PURE_FN inline DLB_XACCU
DLB_X_FU(double f, unsigned hr) { (void)hr;
    return (DLB_XACCU) f; }

static DLB_PURE_FN inline DLB_NACCU
DLB_N_FU(double f, unsigned hr) { (void)hr;
    return (DLB_NACCU) f; }

static DLB_PURE_FN inline DLB_LFRACT 
DLB_LmantLUU(DLB_LFRACT a, unsigned fp, unsigned hr) {
    (void) hr;
    a = _dlb_shlu(a, fp);
    return a - (DLB_LFRACT) floorf(a); }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LmantLU(DLB_LFRACT a, unsigned fp) {
    a = _dlb_shlu(a, fp);
    return a - (DLB_LFRACT) floorf(a); }

static DLB_PURE_FN inline double
DLB_F_S(DLB_SFRACT a) {
    return (double)a; }

static DLB_PURE_FN inline double 
DLB_F_AU(DLB_ACCU a, unsigned hr) {  (void)hr;
    return (double)a; }

static DLB_PURE_FN inline double 
DLB_F_A(DLB_ACCU a) { 
    return (double)a; }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LtruncAU(DLB_ACCU a, unsigned hr) { (void)hr;
    return (DLB_LFRACT)a; }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LtruncA(DLB_ACCU a) {
    return (DLB_LFRACT)a; }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LshrAU(DLB_ACCU a, unsigned shift) {
    DLB_assert(shift <= DLB_MAX_SHIFT, "shift", "Shift count too large.");
    return (DLB_LFRACT)_dlb_shru(a, shift); }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LshlAU(DLB_ACCU a, unsigned shift) {
    DLB_assert(shift <= DLB_MAX_SHIFT, "shift", "Shift count too large.");
    return (DLB_LFRACT)_dlb_shlu(a, shift); }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LsshlAU(DLB_ACCU a, unsigned shift) {
    DLB_assert(shift <= DLB_MAX_SHIFT, "shift", "Shift count too large.");
    return (DLB_LFRACT)_fsat(_dlb_shlu(a, shift)); }

static DLB_PURE_FN inline DLB_SFRACT
DLB_StruncAU(DLB_ACCU a, unsigned hr) { (void)hr;
    return (DLB_SFRACT)a; }

static DLB_PURE_FN inline DLB_SFRACT
DLB_StruncA(DLB_ACCU a) {
    return (DLB_SFRACT)a; }

static DLB_PURE_FN inline DLB_SFRACT
DLB_SshlAU(DLB_ACCU a, unsigned shift) {
    DLB_assert(shift <= DLB_MAX_SHIFT, "shift", "Shift count too large.");
    return (DLB_SFRACT)_dlb_shlu(a, shift); }

static DLB_PURE_FN inline DLB_SFRACT
DLB_SsshlAU(DLB_ACCU a, unsigned shift) {
    DLB_assert(shift <= DLB_MAX_SHIFT, "shift", "Shift count too large.");
    return (DLB_SFRACT)_fsat(_dlb_shlu(a, shift)); }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LsrndAU(DLB_ACCU a, unsigned hr) { (void)hr;
    return (DLB_LFRACT)_fsat(a); }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LsrndA(DLB_ACCU a) {
    return (DLB_LFRACT)_fsat(a); }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LsatAU(DLB_ACCU a, unsigned hr) { (void)hr;
    return (DLB_LFRACT)_fsat(a); }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LsatA(DLB_ACCU a) {
    return (DLB_LFRACT)_fsat(a); }

static DLB_PURE_FN inline DLB_SFRACT
DLB_SsrndAU(DLB_ACCU a, unsigned hr) { (void)hr;
    return (DLB_SFRACT)_fsat(a); }

static DLB_PURE_FN inline DLB_SFRACT
DLB_SsrndA(DLB_ACCU a) {
    return (DLB_SFRACT)_fsat(a); }

static DLB_PURE_FN inline DLB_SFRACT
DLB_SsatAU(DLB_ACCU a, unsigned hr) { (void)hr;
    return (DLB_SFRACT)_fsat(a); }

static DLB_PURE_FN inline DLB_SFRACT
DLB_SsatA(DLB_ACCU a) {
    return (DLB_SFRACT)_fsat(a); }

static DLB_PURE_FN inline DLB_ACCU
DLB_A_LU(DLB_LFRACT a, unsigned hr) { (void)hr;
    return (DLB_ACCU)a; }

static DLB_PURE_FN inline DLB_ACCU
DLB_A_L(DLB_LFRACT a) {
    return (DLB_ACCU)a; }

static DLB_PURE_FN inline DLB_ACCU
DLB_A_SU(DLB_SFRACT a, unsigned hr) { (void)hr;
    return (DLB_ACCU)a; }

static DLB_PURE_FN inline DLB_ACCU
DLB_A_S(DLB_SFRACT a) {
    return (DLB_ACCU)a; }

static DLB_PURE_FN inline DLB_LFRACT
DLB_L_SU(DLB_SFRACT a, unsigned hr) { (void)hr;
    return (DLB_LFRACT)a; }

static DLB_PURE_FN inline DLB_LFRACT
DLB_L_S(DLB_SFRACT a) {
    return (DLB_LFRACT)a; }

static DLB_PURE_FN inline DLB_ACCU
DLB_AaddAA(DLB_ACCU a, DLB_ACCU b) {
    return a + b; }

static DLB_PURE_FN inline DLB_ACCU
DLB_AsubAA(DLB_ACCU a, DLB_ACCU b) {
    return a - b; }

static DLB_PURE_FN inline DLB_SFRACT
DLB_SmaxSS(DLB_SFRACT a, DLB_SFRACT b) {
    return _dlb_fmaxf(a, b); }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LmaxLL(DLB_LFRACT a, DLB_LFRACT b) {
    return _dlb_fmaxf(a, b); }

static DLB_PURE_FN inline DLB_ACCU
DLB_AmaxAA(DLB_ACCU a, DLB_ACCU b) {
    return _dlb_fmaxf(a, b); }

static DLB_PURE_FN inline DLB_XACCU
DLB_XmaxXX(DLB_XACCU a, DLB_XACCU b) {
    return _dlb_fmax(a, b); }

static DLB_PURE_FN inline DLB_NACCU
DLB_NmaxNN(DLB_NACCU a, DLB_NACCU b) {
    return _dlb_fmax(a, b); }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LabsL(DLB_LFRACT a) {
    return _dlb_fabsf(a); }

static DLB_PURE_FN inline DLB_ACCU
DLB_AabsA(DLB_ACCU a) {
    return _dlb_fabsf(a); }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LflushL(DLB_LFRACT x) {
    return _flushf(x); }

static DLB_PURE_FN inline DLB_ACCU
DLB_AflushA(DLB_ACCU x) {
    return _flushf(x); }

static DLB_PURE_FN inline DLB_ACCU
DLB_AstoreA(DLB_ACCU a) {
    return _flushf(a); }

static DLB_PURE_FN inline DLB_ACCU
DLB_AmpySS(DLB_SFRACT a, DLB_SFRACT b) {
    return (DLB_ACCU)a * b; }

static DLB_PURE_FN inline DLB_ACCU
DLB_AmpyLS(DLB_LFRACT a, DLB_SFRACT b) {
    return (DLB_ACCU)a * b; }

static DLB_PURE_FN inline DLB_ACCU
DLB_AmpyLL(DLB_LFRACT a, DLB_LFRACT b) {
    return (DLB_ACCU)a * b; }

static DLB_PURE_FN inline DLB_ACCU
DLB_AmpyAS(DLB_ACCU a, DLB_SFRACT c) {
    return a * c; }

static DLB_PURE_FN inline DLB_ACCU
DLB_AmacASS(DLB_ACCU a, DLB_SFRACT b, DLB_SFRACT c) {
    return a + (DLB_ACCU)b * c; }

static DLB_PURE_FN inline DLB_ACCU
DLB_AmacALS(DLB_ACCU a, DLB_LFRACT b, DLB_SFRACT c) {
    return a + (DLB_ACCU)b * c; }

static DLB_PURE_FN inline DLB_ACCU
DLB_AmacALL(DLB_ACCU a, DLB_LFRACT b, DLB_LFRACT c) {
    return a + (DLB_ACCU)b * c; }

static DLB_PURE_FN inline DLB_ACCU
DLB_AmsuASS(DLB_ACCU a, DLB_SFRACT b, DLB_SFRACT c) {
    return a - (DLB_ACCU)b * c; }

static DLB_PURE_FN inline DLB_ACCU
DLB_AmsuALS(DLB_ACCU a, DLB_LFRACT b, DLB_SFRACT c) {
    return a - (DLB_ACCU)b * c; }

static DLB_PURE_FN inline DLB_ACCU
DLB_AmsuALL(DLB_ACCU a, DLB_LFRACT b, DLB_LFRACT c) {
    return a - (DLB_ACCU)b * c; }

static DLB_PURE_FN inline DLB_ACCU
DLB_AmacAAS(DLB_ACCU a, DLB_ACCU b, DLB_SFRACT c) {
    return a + b * c; }

static DLB_PURE_FN inline DLB_ACCU
DLB_AmsuAAS(DLB_ACCU a, DLB_ACCU b, DLB_SFRACT c) {
    return a - b * c; }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LmpyLS(DLB_LFRACT a, DLB_SFRACT b) {
    return (DLB_LFRACT)((DLB_ACCU)a * b); }

static DLB_PURE_FN inline DLB_ACCU
DLB_AshlAU(DLB_ACCU a, unsigned shift) {
    DLB_assert(shift <= DLB_MAX_SHIFT, "shift", "Shift count too large.");
    return _dlb_shlu(a, shift); }

static DLB_PURE_FN inline DLB_ACCU
DLB_AshrAU(DLB_ACCU a, unsigned shift) {
    DLB_assert(shift <= DLB_MAX_SHIFT, "shift", "Shift count too large.");
    return _dlb_shru(a, shift); }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LabsdiffLL(DLB_LFRACT a, DLB_LFRACT b) {
    return _dlb_fabsf(a - b); }

static DLB_PURE_FN inline unsigned int
_upnorm(float a) {
    static const float norm_min = 1.0f/((intmax_t)1<<DLB_MAX_SHIFT);
    /* it is possible that norm_min needs to be FLT_MIN for bug-compatability with previous versions */
    int exp;
    (void)frexpf(a, &exp);
    return _dlb_fabsf(a) <= norm_min ? DLB_MAX_SHIFT
         : 0 < exp ? 0u
         :             (unsigned int)(-exp); }

static DLB_PURE_FN inline unsigned int
_unorm(float a) {
    static const float norm_min = 1.0f/((intmax_t)1<<DLB_MAX_SHIFT);
    /* it is possible that norm_min needs to be FLT_MIN for bug-compatability with previous versions */
    int exp;
    float m = frexpf(a, &exp);
    return _dlb_fabsf(a) <= norm_min ? DLB_MAX_SHIFT
         : 0 < exp ? 0u
         : -0.5f == m ? (unsigned int)(1-exp)    /* XXX: ugly, but de tests fail otherwise */
         :             (unsigned int)(-exp); }

static DLB_PURE_FN inline unsigned int
DLB_UpnormA(DLB_ACCU a) {
    return _upnorm(a); }

static DLB_PURE_FN inline unsigned int
DLB_UnormA(DLB_ACCU a) {
    return _unorm(a); }

static DLB_PURE_FN inline DLB_LFRACT
DLB_L_F(double s) {
    return (DLB_LFRACT)s; }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LqF(double s) {
    return (DLB_LFRACT)_fsatq15(s); }

static DLB_PURE_FN inline DLB_ACCU
DLB_A_F(double s) {
    return (DLB_ACCU)s; }

static DLB_PURE_FN inline DLB_XACCU
DLB_X_F(double s) {
    return (DLB_XACCU)s; }

static DLB_PURE_FN inline DLB_NACCU
DLB_N_F(double s) {
    return (DLB_NACCU)s; }

static DLB_PURE_FN inline double
DLB_F_LU(DLB_LFRACT s, unsigned hr) { (void)hr;
    return (double)s; }

static DLB_PURE_FN inline double
DLB_F_L(DLB_LFRACT s) {
    return (double)s; }

static DLB_PURE_FN inline int16_t
DLB_16srndS(DLB_SFRACT a) {
    return (int16_t)_dlb_lclip(lrintf(_dlb_Q15 * a), INT16_MIN, INT16_MAX); }

static DLB_PURE_FN inline int16_t
DLB_16sfloorS(DLB_SFRACT a) {
    return (int16_t)_dlb_lclip((long)floorf(_dlb_Q15 * a), INT16_MIN, INT16_MAX); }

static DLB_PURE_FN inline int32_t
DLB_32srndS(DLB_SFRACT a) {
    return 1.0f <= a ? INT32_MAX /* can't do this with clip, because not enough precision in float */
         : (int32_t)lrintf(_dlb_fmaxf(_dlb_Q31 * a, (float)INT32_MIN)); }

static DLB_PURE_FN inline int32_t
DLB_32sfloorS(DLB_SFRACT a) {
    return (int32_t)floorf(_dlb_fclip(_dlb_Q31 * a, (float)INT32_MIN, (float)0x7fffff80)); }

static DLB_PURE_FN inline DLB_SFRACT 
DLB_S_16(int16_t a) {
    return _dlb_Q15_recip * a; }

static DLB_PURE_FN inline DLB_SFRACT
DLB_S_32(int32_t a) {
    return _dlb_Q31_recip * a; }

static DLB_PURE_FN inline int16_t
DLB_16srndLU(DLB_LFRACT a, unsigned hr) { (void)hr;
    return (int16_t)_dlb_lclip(lrintf(_dlb_Q15 * a), INT16_MIN, INT16_MAX); }

static DLB_PURE_FN inline int16_t 
DLB_16srndL(DLB_LFRACT a) {
    return (int16_t)_dlb_lclip(lrintf(_dlb_Q15 * a), INT16_MIN, INT16_MAX); }

static DLB_PURE_FN inline int16_t 
DLB_16sfloorL(DLB_LFRACT a) {
    return (int16_t)_dlb_lclip((long)floorf(_dlb_Q15 * a), INT16_MIN, INT16_MAX); }

static DLB_PURE_FN inline int32_t
DLB_32srndLU(DLB_LFRACT a, unsigned hr) { (void)hr;
    return 1.0f <= a ? INT32_MAX
         : (int32_t)lrintf(_dlb_fmaxf(_dlb_Q31 * a, (float)INT32_MIN)); }

static DLB_PURE_FN inline int32_t 
DLB_32srndL(DLB_LFRACT a) {
    return 1.0f <= a ? INT32_MAX
         : (int32_t)lrintf(_dlb_fmaxf(_dlb_Q31 * a, (float)INT32_MIN)); }

static DLB_PURE_FN inline int32_t 
DLB_32sfloorL(DLB_LFRACT a) {
    return (int32_t)floorf(_dlb_fclip(_dlb_Q31 * a, (float)INT32_MIN, (float)0x7fffff80)); }

static DLB_PURE_FN inline DLB_LFRACT
DLB_L_16U(int16_t a, unsigned hr) { (void)hr;
    return _dlb_Q15_recip * a; }

static DLB_PURE_FN inline DLB_LFRACT
DLB_L_16(int16_t s) {
    return _dlb_Q15_recip * s; }

static DLB_PURE_FN inline DLB_LFRACT
DLB_L_32U(int32_t a, unsigned hr) { (void)hr;
    return _dlb_Q31_recip * a; }

static DLB_PURE_FN inline DLB_LFRACT
DLB_L_32(int32_t s) {
    return _dlb_Q31_recip * s; }

static DLB_PURE_FN inline DLB_SFRACT
DLB_S_F(double a) {
    return (DLB_SFRACT)a; }

static DLB_PURE_FN inline DLB_SFRACT
DLB_SqF(double a) {
    return (DLB_SFRACT)_fsatq15(a); }

static DLB_PURE_FN inline DLB_SFRACT
DLB_S_M(intmax_t a) {
    return _dlb_shru((DLB_SFRACT)a, DLB_SFRACT_MANTBITS); }

static DLB_PURE_FN inline DLB_LFRACT
DLB_L_M(intmax_t a) {
    return _dlb_shru((DLB_LFRACT)a, DLB_LFRACT_MANTBITS); }

static DLB_PURE_FN inline DLB_ACCU
DLB_A_M(intmax_t a) {
    return _dlb_shru((DLB_ACCU)a, DLB_ACCU_MANTBITS); }

static DLB_PURE_FN inline DLB_XACCU
DLB_X_M(intmax_t a) {
    return _dlb_dshru((DLB_XACCU)a, DLB_XACCU_MANTBITS); }

static DLB_PURE_FN inline DLB_NACCU
DLB_N_M(intmax_t a) {
    return _dlb_dshru((DLB_NACCU)a, DLB_NACCU_MANTBITS); }

static DLB_PURE_FN inline double
DLB_F_XU(DLB_XACCU a, unsigned hr) { (void)hr;
    return (double)a; }

static DLB_PURE_FN inline double
DLB_F_X(DLB_XACCU a) {
    return (double)a; }

static DLB_PURE_FN inline double
DLB_F_NU(DLB_NACCU a, unsigned hr) { (void)hr;
    return (double)a; }

static DLB_PURE_FN inline double
DLB_F_N(DLB_NACCU a) {
    return (double)a; }

/* C specifies that the float->int cast truncates towards zero, but we
 * want truncation towards -infinity to emulate fixed point behaviour.
 * The M_* implementations therefore call floorf() to get truncation
 * towards -infinity.
 */
static DLB_PURE_FN inline intmax_t
DLB_M_S(DLB_SFRACT a) {
    return (intmax_t)floorf(_dlb_shlu(a, DLB_SFRACT_MANTBITS)); }

static DLB_PURE_FN inline intmax_t
DLB_M_L(DLB_LFRACT a) {
    return (intmax_t)floorf(_dlb_shlu(a, DLB_LFRACT_MANTBITS)); }

static DLB_PURE_FN inline intmax_t
DLB_M_A(DLB_ACCU a) {
    return (intmax_t)floorf(_dlb_shlu(a, DLB_ACCU_MANTBITS)); }

static DLB_PURE_FN inline intmax_t
DLB_M_X(DLB_XACCU a) {
    return (intmax_t)floor(_dlb_dshlu(a, DLB_XACCU_MANTBITS)); }

static DLB_PURE_FN inline intmax_t
DLB_M_N(DLB_NACCU a) {
    return (intmax_t)floor(_dlb_dshlu(a, DLB_NACCU_MANTBITS)); }

static DLB_PURE_FN inline DLB_SFRACT
DLB_SminSS(DLB_SFRACT a, DLB_SFRACT b) {
    return _dlb_fminf(a,b); }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LminLL(DLB_LFRACT a, DLB_LFRACT b) {
    return _dlb_fminf(a,b); }

static DLB_PURE_FN inline DLB_ACCU
DLB_AminAA(DLB_ACCU a, DLB_ACCU b) {
    return _dlb_fminf(a,b); }

static DLB_PURE_FN inline DLB_XACCU
DLB_XminXX(DLB_XACCU a, DLB_XACCU b) {
    return _dlb_fmin(a,b); }

static DLB_PURE_FN inline DLB_NACCU
DLB_NminNN(DLB_NACCU a, DLB_NACCU b) {
    return _dlb_fmin(a,b); }

static DLB_PURE_FN inline DLB_SFRACT
DLB_SclipSSS(DLB_SFRACT x, DLB_SFRACT lo, DLB_SFRACT hi) {
    return DLB_SminSS(DLB_SmaxSS(x, lo), hi); }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LclipLLL(DLB_LFRACT x, DLB_LFRACT lo, DLB_LFRACT hi) {
    return DLB_LminLL(DLB_LmaxLL(x, lo), hi); }

static DLB_PURE_FN inline DLB_ACCU
DLB_AclipAAA(DLB_ACCU x, DLB_ACCU lo, DLB_ACCU hi) {
    return DLB_AminAA(DLB_AmaxAA(x, lo), hi); }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LclipALL(DLB_ACCU x, DLB_LFRACT lo, DLB_LFRACT hi) {
    return DLB_LminLL(DLB_LmaxLL((DLB_LFRACT)x, lo), hi); }

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
DLB_SshlSU(DLB_SFRACT a, unsigned sh) {
    return _dlb_shlu(a, sh); }

static DLB_PURE_FN inline DLB_SFRACT
DLB_SshrSU(DLB_SFRACT a, unsigned sh) {
    return _dlb_shru(a, sh); }

static DLB_PURE_FN inline DLB_SFRACT
DLB_SsshlSU(DLB_SFRACT a, unsigned sh) {
    a = _dlb_shlu(a, sh);
    return _fsat(a); }

static DLB_PURE_FN inline DLB_SFRACT
DLB_SshlSI(DLB_SFRACT a, int sh) {
    return 0 <= sh ? _dlb_shlu(a, sh) : _dlb_shru(a, -sh); }

static DLB_PURE_FN inline DLB_SFRACT
DLB_SshrSI(DLB_SFRACT a, int sh) {
    return 0 <= sh ? _dlb_shru(a, sh) : _dlb_shlu(a, -sh); }

static DLB_PURE_FN inline DLB_SFRACT
DLB_SsshlSI(DLB_SFRACT a, int sh) {
    a = 0 <= sh ? _dlb_shlu(a, sh) : _dlb_shru(a, -sh);
    return _fsat(a); }

static DLB_PURE_FN inline DLB_SFRACT
DLB_SsshrSI(DLB_SFRACT a, int sh) {
    a = 0 <= sh ? _dlb_shru(a, sh) : _dlb_shlu(a, -sh);
    return _fsat(a); }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LshlLU(DLB_LFRACT a, unsigned sh) {
    return _dlb_shlu(a, sh); }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LshrLU(DLB_LFRACT a, unsigned sh) {
    return _dlb_shru(a, sh); }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LsshlLU(DLB_LFRACT a, unsigned sh) {
    a = _dlb_shlu(a, sh);
    return _fsat(a); }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LshlLI(DLB_LFRACT a, int sh) {
    return 0 <= sh ? _dlb_shlu(a, sh) : _dlb_shru(a, -sh); }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LshrLI(DLB_LFRACT a, int sh) {
    return 0 <= sh ? _dlb_shru(a, sh) : _dlb_shlu(a, -sh); }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LsshlLI(DLB_LFRACT a, int sh) {
    a = 0 <= sh ? _dlb_shlu(a, sh) : _dlb_shru(a, -sh);
    return _fsat(a); }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LsshrLI(DLB_LFRACT a, int sh) {
    a = 0 <= sh ? _dlb_shru(a, sh) : _dlb_shlu(a, -sh);
    return _fsat(a); }

static DLB_PURE_FN inline DLB_ACCU
DLB_AshlAI(DLB_ACCU a, int sh) {
    return 0 <= sh ? _dlb_shlu(a, sh) : _dlb_shru(a, -sh); }

static DLB_PURE_FN inline DLB_ACCU
DLB_AshrAI(DLB_ACCU a, int sh) {
    return 0 <= sh ? _dlb_shru(a, sh) : _dlb_shlu(a, -sh); }

static DLB_PURE_FN inline DLB_XACCU
DLB_XshlXI(DLB_XACCU a, int sh) {
    return 0 <= sh ? _dlb_dshlu(a, sh) : _dlb_dshru(a, -sh); }

static DLB_PURE_FN inline DLB_XACCU
DLB_XshrXI(DLB_XACCU a, int sh) {
    return 0 <= sh ? _dlb_dshru(a, sh) : _dlb_dshlu(a, -sh); }

static DLB_PURE_FN inline DLB_XACCU
DLB_XshlXU(DLB_XACCU a, unsigned sh) {
    return _dlb_dshlu(a, sh); }

static DLB_PURE_FN inline DLB_XACCU
DLB_XshrXU(DLB_XACCU a, unsigned sh) {
    return _dlb_dshru(a, sh); }

static DLB_PURE_FN inline DLB_NACCU
DLB_NshlNU(DLB_NACCU a, unsigned sh) {
    return _dlb_dshlu(a, sh); }

static DLB_PURE_FN inline DLB_NACCU
DLB_NshrNU(DLB_NACCU a, unsigned sh) {
    return _dlb_dshru(a, sh); }

static DLB_PURE_FN inline DLB_NACCU
DLB_NshlNI(DLB_NACCU a, int sh) {
    return 0 <= sh ? _dlb_dshlu(a, sh) : _dlb_dshru(a, -sh); }

static DLB_PURE_FN inline DLB_NACCU
DLB_NshrNI(DLB_NACCU a, int sh) {
    return 0 <= sh ? _dlb_dshru(a, sh) : _dlb_dshlu(a, -sh); }

#define _dlb_iclipiii(x, lo, hi) \
    (x <= lo ? lo :  hi <= x ? hi : x)

static DLB_PURE_FN inline int
DLB_IlimsI(int i) {
    return _dlb_iclipiii(i, -DLB_MAX_SHIFT, DLB_MAX_SHIFT); }

static DLB_PURE_FN inline int
DLB_IlimlI(int i) {
    return _dlb_iclipiii(i, -DLB_MAX_SHIFT, DLB_MAX_SHIFT); }

static DLB_PURE_FN inline int
DLB_IlimaI(int i) {
    return _dlb_iclipiii(i, -DLB_MAX_SHIFT, DLB_MAX_SHIFT); }

static DLB_PURE_FN inline int
DLB_IlimxI(int i) {
    return _dlb_iclipiii(i, -DLB_MAX_SHIFT, DLB_MAX_SHIFT); }

static DLB_PURE_FN inline int
DLB_IlimnI(int i) {
    return _dlb_iclipiii(i, -DLB_MAX_SHIFT, DLB_MAX_SHIFT); }

static DLB_PURE_FN inline unsigned int
DLB_UpnormS(DLB_SFRACT a) {
    DLB_assert(a <= 1.0f, "norm", "DLB_UpnormS called with number > 1.");
    DLB_assert(a >= 0.0f, "norm", "DLB_UpnormS called with number < 0.");
    return _upnorm(a); }

static DLB_PURE_FN inline unsigned int
DLB_UnormS(DLB_SFRACT a) {
    DLB_assert(a <= 1.0f, "norm", "DLB_UnormS called with number > 1.");
    DLB_assert(a >=-1.0f, "norm", "DLB_UnormS called with number < 1.");
    return _unorm(a); }

static DLB_PURE_FN inline unsigned int
DLB_UpnormL(DLB_LFRACT a) {
    DLB_assert(a <= 1.0f, "norm", "DLB_UpnormL called with number > 1.");
    DLB_assert(a >= 0.0f, "norm", "DLB_UpnormL called with number < 0.");
    return _upnorm(a); }

static DLB_PURE_FN inline unsigned int
DLB_UnormL(DLB_LFRACT a) {
    DLB_assert(a <= 1.0f, "norm", "DLB_UnormL called with number > 1.");
    DLB_assert(a >=-1.0f, "norm", "DLB_UnormL called with number < 1.");
    return _unorm(a); }

static DLB_PURE_FN inline unsigned int
DLB_UpnormX(DLB_XACCU a) {
    DLB_assert(a >= 0.0f, "norm", "DLB_UpnormX called with number < 0.");
    return _upnorm((float)a); /* precision is unimportant here */ }

static DLB_PURE_FN inline unsigned int
DLB_UnormX(DLB_XACCU a) {
    return _unorm((float)a); }

static DLB_PURE_FN inline unsigned int
DLB_UpnormN(DLB_NACCU a) {
    DLB_assert(a >= 0.0f, "norm", "DLB_UpnormN called with number < 0.");
    return _upnorm((float)a); }

static DLB_PURE_FN inline unsigned int
DLB_UnormN(DLB_NACCU a) {
    return _unorm((float)a); }

static DLB_PURE_FN inline DLB_SFRACT
DLB_SaddSS(DLB_SFRACT a, DLB_SFRACT b) {
    return a + b; }

static DLB_PURE_FN inline DLB_SFRACT
DLB_SsubSS(DLB_SFRACT a, DLB_SFRACT b) {
    return a - b; }

static DLB_PURE_FN inline DLB_SFRACT
DLB_SsaddSS(DLB_SFRACT a, DLB_SFRACT b) {
    a =  a + b;
    return _fsat(a); }

static DLB_PURE_FN inline DLB_SFRACT
DLB_SssubSS(DLB_SFRACT a, DLB_SFRACT b) {
    a =  a - b;
    return _fsat(a); }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LaddLL(DLB_LFRACT a, DLB_LFRACT b) {
    return a + b; }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LsubLL(DLB_LFRACT a, DLB_LFRACT b) {
    return a - b; }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LsaddLL(DLB_LFRACT a, DLB_LFRACT b) {
    a =  a + b;
    return _fsat(a); }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LssubLL(DLB_LFRACT a, DLB_LFRACT b) {
    a =  a - b;
    return _fsat(a); }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LsaddLS(DLB_LFRACT a, DLB_SFRACT b) {
    a =  a + b;
    return _fsat(a); }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LssubLS(DLB_LFRACT a, DLB_SFRACT b) {
    a =  a - b;
    return _fsat(a); }

static DLB_PURE_FN inline DLB_XACCU
DLB_XaddXX(DLB_XACCU a, DLB_XACCU b) {
    return a + b; }

static DLB_PURE_FN inline DLB_XACCU
DLB_XsubXX(DLB_XACCU a, DLB_XACCU b) {
    return a - b; }

static DLB_PURE_FN inline DLB_NACCU
DLB_NaddNN(DLB_NACCU a, DLB_NACCU b) {
    return a + b; }

static DLB_PURE_FN inline DLB_NACCU
DLB_NsubNN(DLB_NACCU a, DLB_NACCU b) {
    return a - b; }

static DLB_PURE_FN inline DLB_SFRACT
DLB_SnegS(DLB_SFRACT a) {
    return -a; }

static DLB_PURE_FN inline DLB_SFRACT
DLB_SsnegS(DLB_SFRACT a) {
    return -a; }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LnegL(DLB_LFRACT a) {
    return -a; }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LsnegL(DLB_LFRACT a) {
    return -a; }

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
    return _dlb_fabsf(a); }

static DLB_PURE_FN inline DLB_SFRACT
DLB_SsabsS(DLB_SFRACT a) {
    return _dlb_fabsf(a); }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LsabsL(DLB_LFRACT a) {
    return _dlb_fabsf(a); }

static DLB_PURE_FN inline DLB_XACCU
DLB_XabsX(DLB_XACCU a) {
    return _dlb_fabs(a); }

static DLB_PURE_FN inline DLB_NACCU
DLB_NabsN(DLB_NACCU a) {
    return _dlb_fabs(a); }

static DLB_PURE_FN inline DLB_SFRACT
DLB_SsrmpySS(DLB_SFRACT a, DLB_SFRACT b) {
    return _fsat(a * b); }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LmpySS(DLB_SFRACT a, DLB_SFRACT b) {
    return a * b; }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LmpyLL(DLB_LFRACT a, DLB_LFRACT b) {
    return a * b; }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LsmpySS(DLB_SFRACT a, DLB_SFRACT b) {
    return _fsat(a * b); }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LsmpyLS(DLB_LFRACT a, DLB_SFRACT b) {
    return _fsat(a * b); }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LsmpyLL(DLB_LFRACT a, DLB_LFRACT b) {
    return _fsat(a * b); }

static DLB_PURE_FN inline DLB_XACCU
DLB_XmpyLS(DLB_LFRACT a, DLB_SFRACT b) {
    return (DLB_XACCU)a * b; }

static DLB_PURE_FN inline DLB_XACCU
DLB_XmpyXS(DLB_XACCU a, DLB_SFRACT b) {
    return a * b; }

static DLB_PURE_FN inline DLB_NACCU
DLB_NmpyLL(DLB_LFRACT a, DLB_LFRACT b) {
    return (DLB_NACCU)a * b; }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LsmacLSS(DLB_LFRACT a, DLB_SFRACT b, DLB_SFRACT c) {
    return _fsat(a + b * c); }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LsmacLLS(DLB_LFRACT a, DLB_LFRACT b, DLB_SFRACT c) {
    return _fsat(a + b * c); }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LsmacLLL(DLB_LFRACT a, DLB_LFRACT b, DLB_LFRACT c) {
    return _fsat(a + b * c); }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LmacLSS(DLB_LFRACT a, DLB_SFRACT b, DLB_SFRACT c) {
    return a + b * c; }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LmacLLS(DLB_LFRACT a, DLB_LFRACT b, DLB_SFRACT c) {
    return a + b * c; }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LmacLLL(DLB_LFRACT a, DLB_LFRACT b, DLB_LFRACT c) {
    return a + b * c; }

static DLB_PURE_FN inline DLB_XACCU
DLB_XmacXLS(DLB_XACCU a, DLB_LFRACT b, DLB_SFRACT c) {
    return a + (DLB_XACCU)b * c; }

static DLB_PURE_FN inline DLB_XACCU
DLB_XmacXXS(DLB_XACCU a, DLB_XACCU b, DLB_SFRACT c) {
    return a + b * c; }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LsmsuLSS(DLB_LFRACT a, DLB_SFRACT b, DLB_SFRACT c) {
    return _fsat(a - b * c); }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LsmsuLLS(DLB_LFRACT a, DLB_LFRACT b, DLB_SFRACT c) {
    return _fsat(a - b * c); }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LsmsuLLL(DLB_LFRACT a, DLB_LFRACT b, DLB_LFRACT c) {
    return _fsat(a - b * c); }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LmsuLSS(DLB_LFRACT a, DLB_SFRACT b, DLB_SFRACT c) {
    return a - b * c; }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LmsuLLS(DLB_LFRACT a, DLB_LFRACT b, DLB_SFRACT c) {
    return a - b * c; }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LmsuLLL(DLB_LFRACT a, DLB_LFRACT b, DLB_LFRACT c) {
    return a - b * c; }

static DLB_PURE_FN inline DLB_XACCU
DLB_XmsuXLS(DLB_XACCU a, DLB_LFRACT b, DLB_SFRACT c) {
    return a - (DLB_XACCU)b * c; }

static DLB_PURE_FN inline DLB_XACCU
DLB_XmsuXXS(DLB_XACCU a, DLB_XACCU b, DLB_SFRACT c) {
    return a - b * c; }

static DLB_PURE_FN inline DLB_NACCU
DLB_NmacNLL(DLB_NACCU a, DLB_LFRACT b, DLB_LFRACT c) {
    return a + (DLB_NACCU)b * c; }

static DLB_PURE_FN inline DLB_NACCU
DLB_NmsuNLL(DLB_NACCU a, DLB_LFRACT b, DLB_LFRACT c) {
    return a - (DLB_NACCU)b * c; }

static DLB_PURE_FN inline DLB_XACCU
DLB_X_S(DLB_SFRACT a) {
    return (DLB_XACCU)a; }

static DLB_PURE_FN inline DLB_XACCU
DLB_X_LU(DLB_LFRACT a, unsigned hr) { (void)hr;
    return (DLB_XACCU)a; }

static DLB_PURE_FN inline DLB_XACCU
DLB_X_L(DLB_LFRACT a) {
    return (DLB_XACCU)a; }

static DLB_PURE_FN inline DLB_XACCU
DLB_X_AU(DLB_ACCU a, unsigned hr) { (void)hr;
    return (DLB_XACCU)a; }

static DLB_PURE_FN inline DLB_XACCU
DLB_X_A(DLB_ACCU a) {
    return (DLB_XACCU)a; }

static DLB_PURE_FN inline DLB_NACCU
DLB_N_LU(DLB_LFRACT a, unsigned hr) { (void)hr;
    return (DLB_NACCU)a; }

static DLB_PURE_FN inline DLB_NACCU
DLB_N_L(DLB_LFRACT a) {
    return (DLB_NACCU)a; }

static DLB_PURE_FN inline DLB_NACCU
DLB_N_A(DLB_ACCU a) {
    return (DLB_NACCU)a; }

static DLB_PURE_FN inline DLB_NACCU
DLB_N_X(DLB_XACCU a) {
    return (DLB_NACCU)a; }

static DLB_PURE_FN inline DLB_SFRACT
DLB_StruncL(DLB_LFRACT a) {
    return (DLB_SFRACT)a; }

static DLB_PURE_FN inline DLB_SFRACT
DLB_SsatL(DLB_LFRACT a) {
    return (DLB_SFRACT)_fsat(a); }

static DLB_PURE_FN inline DLB_SFRACT
DLB_SrndL(DLB_LFRACT a) {
    return (DLB_SFRACT)a; }

static DLB_PURE_FN inline DLB_SFRACT
DLB_SsrndL(DLB_LFRACT a) {
    return (DLB_SFRACT)_fsat(a); }

static DLB_PURE_FN inline DLB_SFRACT
DLB_SrndAU(DLB_ACCU a, unsigned hr) { (void)hr;
    return (DLB_SFRACT)a; }

static DLB_PURE_FN inline DLB_SFRACT
DLB_SrndA(DLB_ACCU a) {
    return (DLB_SFRACT)a; }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LrndAU(DLB_ACCU a, unsigned hr) { (void)hr;
    return (DLB_LFRACT)a; }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LrndA(DLB_ACCU a) {
    return (DLB_LFRACT)a; }

static DLB_PURE_FN inline DLB_SFRACT
DLB_StruncX(DLB_XACCU a) {
    return (DLB_SFRACT)a; }

static DLB_PURE_FN inline DLB_SFRACT
DLB_SsatX(DLB_XACCU a) {
    return (DLB_SFRACT)_fsat((float)a); }

static DLB_PURE_FN inline DLB_SFRACT
DLB_SrndXU(DLB_XACCU a, unsigned hr) { (void)hr;
    return (DLB_SFRACT)a; }

static DLB_PURE_FN inline DLB_SFRACT
DLB_SrndX(DLB_XACCU a) {
    return (DLB_SFRACT)a; }

static DLB_PURE_FN inline DLB_SFRACT
DLB_SsrndXU(DLB_XACCU a, unsigned hr) { (void)hr;
    return (DLB_SFRACT)_fsat((float)a); }

static DLB_PURE_FN inline DLB_SFRACT
DLB_SsrndX(DLB_XACCU a) {
    return (DLB_SFRACT)_fsat((float)a); }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LtruncXU(DLB_XACCU a, unsigned hr) { (void)hr;
    return (DLB_LFRACT)a; }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LtruncX(DLB_XACCU a) {
    return (DLB_LFRACT)a; }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LsatXU(DLB_XACCU a, unsigned hr) { (void)hr;
    return (DLB_LFRACT)_fsat((float)a); }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LsatX(DLB_XACCU a) {
    return (DLB_LFRACT)_fsat((float)a); }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LrndXU(DLB_XACCU a, unsigned hr) { (void)hr;
    return (DLB_LFRACT)a; }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LrndX(DLB_XACCU a) {
    return (DLB_LFRACT)a; }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LsrndXU(DLB_XACCU a, unsigned hr) { (void)hr;
    return (DLB_LFRACT)_fsat((float)a); }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LsrndX(DLB_XACCU a) {
    return (DLB_LFRACT)_fsat((float)a); }

static DLB_PURE_FN inline DLB_ACCU
DLB_AtruncX(DLB_XACCU a) {
    return (DLB_ACCU)a; }

static DLB_PURE_FN inline DLB_ACCU
DLB_ArndXU(DLB_XACCU a, unsigned hr) { (void)hr;
    return (DLB_ACCU)a; }

static DLB_PURE_FN inline DLB_ACCU
DLB_ArndX(DLB_XACCU a) {
    return (DLB_ACCU)a; }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LtruncNU(DLB_NACCU a, unsigned hr) { (void)hr;
    return (DLB_LFRACT)a; }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LtruncN(DLB_NACCU a) {
    return (DLB_LFRACT)a; }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LsatNU(DLB_NACCU a, unsigned hr) { (void)hr;
    return (DLB_LFRACT)_fsat((float)a); }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LsatN(DLB_NACCU a) {
    return (DLB_LFRACT)_fsat((float)a); }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LrndNU(DLB_NACCU a, unsigned hr) { (void)hr;
    return (DLB_LFRACT)a; }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LrndN(DLB_NACCU a) {
    return (DLB_LFRACT)a; }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LsrndNU(DLB_NACCU a, unsigned hr) { (void)hr;
    return (DLB_LFRACT)_fsat((float)a); }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LsrndN(DLB_NACCU a) {
    return (DLB_LFRACT)_fsat((float)a); }

static DLB_PURE_FN inline DLB_ACCU
DLB_AtruncNU(DLB_NACCU a, unsigned hr) { (void)hr;
    return (DLB_ACCU)a; }

static DLB_PURE_FN inline DLB_ACCU
DLB_AtruncN(DLB_NACCU a) {
    return (DLB_ACCU)a; }

static DLB_PURE_FN inline DLB_XACCU
DLB_XtruncNU(DLB_NACCU a, unsigned hr) { (void)hr;
    return (DLB_XACCU)a; }

static DLB_PURE_FN inline DLB_XACCU
DLB_XtruncN(DLB_NACCU a) {
    return (DLB_XACCU)a; }

static DLB_PURE_FN inline DLB_SFRACT
DLB_SrnddL(DLB_LFRACT a) {
    return _dlb_shru(floorf(_dlb_shlu(a, DLB_SFRACT_MANTBITS)), DLB_SFRACT_MANTBITS); }

static DLB_PURE_FN inline DLB_SFRACT
DLB_SrnddA(DLB_ACCU a) {
    return _dlb_shru(floorf(_dlb_shlu(a, DLB_SFRACT_MANTBITS)), DLB_SFRACT_MANTBITS); }

static DLB_PURE_FN inline DLB_SFRACT
DLB_SrnddX(DLB_XACCU a) {
    return _dlb_shru((DLB_SFRACT)floor(_dlb_dshlu(a, DLB_SFRACT_MANTBITS)), DLB_SFRACT_MANTBITS); }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LrnddA(DLB_ACCU a) {
    return _dlb_shru(floorf(_dlb_shlu(a, DLB_LFRACT_MANTBITS)), DLB_LFRACT_MANTBITS); }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LrnddX(DLB_XACCU a) {
    return _dlb_shru((DLB_LFRACT)floor(_dlb_dshlu(a, DLB_LFRACT_MANTBITS)), DLB_LFRACT_MANTBITS); }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LrnddN(DLB_NACCU a) {
    return _dlb_shru((DLB_LFRACT)floor(_dlb_dshlu(a, DLB_LFRACT_MANTBITS)), DLB_LFRACT_MANTBITS); }

static DLB_PURE_FN inline DLB_ACCU
DLB_ArnddX(DLB_XACCU a) {
    return _dlb_shru((DLB_ACCU)floor(_dlb_dshlu(a, DLB_ACCU_MANTBITS)), DLB_ACCU_MANTBITS); }

static DLB_PURE_FN inline DLB_ACCU
DLB_ArnddN(DLB_NACCU a) {
    return _dlb_shru((DLB_ACCU)floor(_dlb_dshlu(a, DLB_ACCU_MANTBITS)), DLB_ACCU_MANTBITS); }

static DLB_PURE_FN inline DLB_XACCU
DLB_XrnddN(DLB_NACCU a) {
    return _dlb_dshru(floor(_dlb_dshlu(a, DLB_XACCU_MANTBITS)), DLB_XACCU_MANTBITS); }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LrnddLU(DLB_LFRACT a, unsigned int bits) {
    DLB_assert(bits <= DLB_LFRACT_MANTBITS
              , "DLB_LrnddLU"
              , "The number of bits to be kept should be no more than DLB_LFRACT_MANTBITS.\n"
              );
    return _dlb_shru(floorf(_dlb_shlu(a, bits)), bits); }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LrnddXU(DLB_XACCU a, unsigned int bits) {
    DLB_assert(bits <= DLB_LFRACT_MANTBITS
              , "DLB_LrnddXU"
              , "The number of bits to be kept should be no more than DLB_LFRACT_MANTBITS.\n"
              );
    return _dlb_shru((DLB_LFRACT)floor(_dlb_dshlu(a, bits)), bits); }


static DLB_PURE_FN inline DLB_XACCU
DLB_XstoreX(DLB_XACCU a) {
    return _flush(a); }

static DLB_PURE_FN inline DLB_NACCU
DLB_NstoreN(DLB_NACCU a) {
    return _flush(a); }

static DLB_PURE_FN inline DLB_SFRACT
DLB_SflushS(DLB_SFRACT x) {
    return _flushf(x); }

static DLB_PURE_FN inline DLB_XACCU
DLB_XflushX(DLB_XACCU a) {
    return _flush(a); }

static DLB_PURE_FN inline DLB_NACCU
DLB_NflushN(DLB_NACCU a) {
    return _flush(a); }

static DLB_PURE_FN inline int
DLB_IintLU(DLB_LFRACT a, unsigned fp) {
    DLB_assert(fp <= DLB_LFRACT_MANTBITS, "fp", "Shift count too large.");
    return (int) floorf(_dlb_shlu(a, fp)); }

static DLB_PURE_FN inline int
DLB_IintLUU(DLB_LFRACT a, unsigned fp, unsigned hr) { (void) hr;
    DLB_assert(fp <= DLB_LFRACT_MANTBITS, "fp", "Shift count too large.");
    return (int) floorf(_dlb_shlu(a, fp)); }

static DLB_PURE_FN inline DLB_SFRACT
DLB_SrmpySS(DLB_SFRACT a, DLB_SFRACT b) {
    return (DLB_SFRACT)((DLB_ACCU)a * b); }

/* Mixed integer / fractional multiplications */
static DLB_PURE_FN inline DLB_LFRACT
DLB_LmpyLI(DLB_LFRACT a, int i) {
	return a * i; }

static DLB_PURE_FN inline int
DLB_ImpyIL(int i, DLB_LFRACT a) {
	return (int)floorf(i * a); }

static DLB_PURE_FN inline int
DLB_IrmpyIL(int i, DLB_LFRACT a) {
	return (int)lrintf(i * a); }

static DLB_PURE_FN inline unsigned
DLB_UrmpyUL(unsigned i, DLB_LFRACT a) {
	DLB_assert(0 <= a, "input check", "Negative input for unsigned multiplication.");
	return (unsigned)lrintf(i * a); }

static DLB_PURE_FN inline int
DLB_ImpyIS(int i, DLB_SFRACT a) {
	return (int)floorf(i * a); }

static DLB_PURE_FN inline int
DLB_IrmpyIS(int i, DLB_SFRACT a) {
	return (int)lrintf(i * a); }

static DLB_PURE_FN inline unsigned
DLB_UrmpyUS(unsigned i, DLB_SFRACT a) {
	DLB_assert(0 <= a, "input check", "Negative input for unsigned multiplication.");
	return (unsigned)lrintf(i * a); }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LsrndLU(DLB_LFRACT a, unsigned s) {
    DLB_assert(s < DLB_LFRACT_MANTBITS
              , "quantize_round"
              , "The number of bits to be kept should be less than DLB_LFRACT_MANTBITS.");
    /* the min() here is to saturate at one (s-scaled) lsb smaller than 1.0 */
    return _dlb_fminf(1.0f - _dlb_shru(1.0f,s), _dlb_shru(floorf(_dlb_shlu(a, s) + 0.5f), s)); }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LtruncLU(DLB_LFRACT a, unsigned s) {
    DLB_assert(s < DLB_LFRACT_MANTBITS
              , "quantize"
              , "The number of bits to be kept should be less than DLB_LFRACT_MANTBITS.");
    return _dlb_shru(floorf(_dlb_shlu(a, s)), s); }

/* Conversions between headroom and plain versions of same types
 * This is a no-op for all types in floating point... */
static DLB_PURE_FN inline DLB_LFRACT
DLB_LleftLU(DLB_LFRACT x, unsigned hr) { (void)hr;
    return x; }

static DLB_PURE_FN inline DLB_LFRACT
DLB_LheadLU(DLB_LFRACT x, unsigned hr) { (void)hr;
    return x; }


#ifdef DLB_INTRINSICS_INCLUDE_DEPRECATED_OPS
/* Should do pre- and post-scaling by using LheadLU and LleftLU. */
static DLB_PURE_FN inline DLB_ACCU
DLB_AleftAU(DLB_ACCU x, unsigned hr) { (void)hr;
    return x; }

static DLB_PURE_FN inline DLB_XACCU
DLB_XleftXU(DLB_XACCU x, unsigned hr) { (void)hr;
    return x; }

static DLB_PURE_FN inline DLB_NACCU
DLB_NleftNU(DLB_NACCU x, unsigned hr) { (void)hr;
    return x; }
	
static DLB_PURE_FN inline DLB_ACCU
DLB_AheadAU(DLB_ACCU x, unsigned hr) { (void)hr;
    return x; }

static DLB_PURE_FN inline DLB_XACCU
DLB_XheadXU(DLB_XACCU x, unsigned hr) { (void)hr;
    return x; }

static DLB_PURE_FN inline DLB_NACCU
DLB_NheadNU(DLB_NACCU x, unsigned hr) { (void)hr;
    return x; }
	
/* DLB_AcF*, DLB_XcF* and DLB_NcF* are deprecated ops and will not be supported in future releases */
/* A sample alternative construction would be as follows:
         const DLB_LFRACT Lfoo = DLB_LcF(bar);
         DLB_NACCU Nfoo = DLB_N_L(Lfoo);
*/

#define DLB_AcF(a)     (a)
#define DLB_XcF(a)     (a)
#define DLB_NcF(a)     (a)
#define DLB_AcFU(a,hr) ((DLB_ACCU)(a))
#define DLB_XcFU(a,hr) ((DLB_XACCU)(a))
#define DLB_NcFU(a,hr) ((DLB_NACCU)(a))

/* in v1.5 the "op" name changed to reflect the saturation that was actually happneing */
#define DLB_IrndS DLB_IsrndS
#define DLB_IrndL DLB_IsrndL

#endif /* DLB_INTRINSICS_INCLUDE_DEPRECATED_OPS */

/** @} */

#define DLB_METHOD_IS_FLOAT /* so that code knows what arithmetic model is being used */
