/****************************************************************************
 *
 *
 * Copyright (c) 2010 Dolby International AB.
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
 * @brief Vector utility routines: inline implementations
 *
 */

#ifndef DLB_VECINLINES_H
#define DLB_VECINLINES_H

#include "dlb_intrinsics.h"

#define _dlb_vec_inlines_pad(x, N) ((((x) + (N) - 1)/(N))*(N))

/** When allocating memory for a vector of n*DLB_LFRACTs, you should allocate
 * DLB_LVEC_size(n) items. */
#ifndef DLB_LVEC_PADDING
#define DLB_LVEC_PADDING (4)
#endif
#define DLB_LVEC_size(n) (_dlb_vec_inlines_pad((n), DLB_LVEC_PADDING))

/** When allocating memory for a vector of n*DLB_SFRACTs, you should allocate
 * DLB_SVEC_size(n) items. */
#ifndef DLB_SVEC_PADDING
#define DLB_SVEC_PADDING (4)
#endif
#define DLB_SVEC_size(n) (_dlb_vec_inlines_pad((n), DLB_SVEC_PADDING))

/** @brief compute the dot-product (sum of products) of two DLB_LFRACT vectors of length n */
static inline
DLB_NACCU
DLB_vec_NdotLLU
	(const DLB_LFRACT *DLB_RESTRICT a
	,const DLB_LFRACT *DLB_RESTRICT b
	,unsigned int n
	)
{
	unsigned int i;
	DLB_NACCU acc = DLB_NmpyLL(a[0], b[0]);
	for (i = 1; i < n; i++)
		acc = DLB_NmacNLL(acc, a[i], b[i]);
	return acc;
}

static inline
DLB_SFRACT
DLB_vec_Sabs_maxSU
	(const DLB_SFRACT *a
	,unsigned int n
	)
{
	unsigned int i;
	DLB_SFRACT min = a[0];
	DLB_SFRACT max = a[0];
	for (i = 1; i < n; i++)
	{
		max = DLB_SmaxSS(max, a[i]);
		min = DLB_SminSS(min, a[i]);
	}
	return DLB_SmaxSS(max, DLB_SsnegS(min));
}

static inline
DLB_LFRACT
DLB_vec_Labs_maxLU
	(const DLB_LFRACT *a
	,unsigned int n
	)
{
	unsigned int i;
	DLB_LFRACT min = a[0];
	DLB_LFRACT max = a[0];
	for (i = 1; i < n; i++)
	{
		max = DLB_LmaxLL(max, a[i]);
		min = DLB_LminLL(min, a[i]);
	}
	return DLB_LmaxLL(max, DLB_LsnegL(min));
}

static inline
unsigned int
DLB_vec_UnormSU
	(const DLB_SFRACT *a
	,unsigned int n
	)
{
	return DLB_UnormS(DLB_vec_Sabs_maxSU(a, n));
}

static inline
unsigned int
DLB_vec_UnormLU
	(const DLB_LFRACT *a
	,unsigned int n
	)
{
	return DLB_UnormL(DLB_vec_Labs_maxLU(a, n));
}

static inline
void
DLB_vec_LmaskLSU
	(DLB_LFRACT dest[]
	,const DLB_LFRACT x[]
	,const DLB_SFRACT w[]
	,unsigned int n
	)
{
	unsigned int i;
	for (i = 0; i < n; ++i)
		dest[i] = DLB_LmpyLS(x[i], w[i]);
}

static inline
void
DLB_vec_Lmask_rev_coefLSU
	(DLB_LFRACT dest[]
	,const DLB_LFRACT x[]
	,const DLB_SFRACT w[]
	,unsigned int n
	)
{
	unsigned int i, j = n - 1;
	for (i = 0; i < n; ++i, --j)
		dest[i] = DLB_LmpyLS (x[i], w[j]);
}

/* I've made this vector shifter bi-directional because it is plausible
 * that a platform will have a native bi-directional shift operation, and
 * we don't want to prevent that from being used.
 * I've made the reference implementation use uni-directional shifts so that
 * the (majority) platforms that only have these will perform adequately: no
 * condition testing in the inner-loop.
 */
static inline
void
DLB_vec_SshlSIU_inplace
	(DLB_SFRACT *a
	,int shift
	,unsigned int n
	)
{
	shift = DLB_IlimsI(shift);
	if (0 < shift)
	{
		unsigned int i, sh = (unsigned int)shift;
		for (i = 0; i < n; i++)
			a[i] = DLB_SshlSU(a[i], sh);
	}
	else if (shift < 0)
	{
		unsigned int i, sh = (unsigned int)(-shift);
		for (i = 0; i < n; i++)
			a[i] = DLB_SshrSU(a[i], sh);
	}
	/* no need to do anything if shift == 0 */
}

static inline
void
DLB_vec_LshlLIU_inplace
	(DLB_LFRACT *a
	,int shift
	,unsigned int n
	)
{
	shift = DLB_IlimlI(shift);
	if (0 < shift)
	{
		unsigned int i, sh = (unsigned int)shift;
		for (i = 0; i < n; i++)
			a[i] = DLB_LshlLU(a[i], sh);
	}
	else if (shift < 0)
	{
		unsigned int i, sh = (unsigned int)(-shift);
		for (i = 0; i < n; i++)
			a[i] = DLB_LshrLU(a[i], sh);
	}
	/* no need to do anything if shift == 0 */
}

static inline
void
DLB_vec_LshlLIU
	(DLB_LFRACT *DLB_RESTRICT y
	,const DLB_LFRACT *DLB_RESTRICT x
	,int shift
	,unsigned int n
	)
{
	shift = DLB_IlimlI(shift);
	if (0 <= shift)
	{
		unsigned int i, sh = (unsigned int)shift;
		for (i = 0; i < n; i++)
			y[i] = DLB_LshlLU(x[i], sh);
	}
	else
	{
		unsigned int i, sh = (unsigned int)(-shift);
		for (i = 0; i < n; i++)
			y[i] = DLB_LshrLU(x[i], sh);
	}
}

static inline
void
DLB_vec_LsetLU
	(DLB_LFRACT *a
	,DLB_LFRACT v
	,unsigned int n
	)
{
	unsigned int i;
	for (i = 0; i < n; i++)
		a[i] = v;
}

static inline
int
DLB_vec_IeqLLU
	(const DLB_LFRACT *DLB_RESTRICT a
	,const DLB_LFRACT *DLB_RESTRICT b
	,DLB_LFRACT eps
	,unsigned int n
	)
{
	unsigned int i;
	for (i = 0; i < n; i++)
		if (DLB_IltLL(eps, DLB_LabsdiffLL(a[i], b[i]))) return 0;   /* early-out if different */
	return 1;
}

static inline
void
DLB_vec_LmpyLSU
	(DLB_LFRACT *DLB_RESTRICT a
	,const DLB_LFRACT *DLB_RESTRICT b
	,DLB_SFRACT g
	,unsigned int n
	)
{
	unsigned int i;
	for (i = 0; i < n; i++)
		a[i] = DLB_LmpyLS(b[i], g);
}

static inline
void
DLB_vec_LmpyLSU_inplace
	(DLB_LFRACT *inout
	,DLB_SFRACT g
	,unsigned int n
	)
{
    DLB_LFRACT *pio = inout;
	unsigned int i;
	for (i = 0; i < n; i++)
	{
		DLB_LFRACT in;
		
		in = *pio;
		*pio++ = DLB_LmpyLS(in, g);
    }
}

#ifndef DLB_INTRINSICS_EXCLUDE_DEPRECATED_OPS
static inline
DLB_ACCU
DLB_vec_AmaxAU
	(const DLB_ACCU *a
	,unsigned int n
	)
{
	unsigned int i;
	DLB_ACCU max = a[0];
	for (i = 1; i < n; i++)
		max = DLB_AmaxAA(max, a[i]);
	return max;
}

static inline
void
DLB_vec_AshlAIU_inplace
	(DLB_ACCU *a
	,int shift
	,unsigned int n
	)
{
	shift = DLB_IlimaI(shift);
	if (0 < shift)
	{
		unsigned int i, sh = (unsigned int)shift;
		for (i = 0; i < n; i++)
			a[i] = DLB_AshlAU(a[i], sh);
	}
	else if (shift < 0)
	{
		unsigned int i, sh = (unsigned int)(-shift);
		for (i = 0; i < n; i++)
			a[i] = DLB_AshrAU(a[i], sh);
	}
	/* no need to do anything if shift == 0 */
}

static inline
void
DLB_vec_Ltrunc_shlAUU
	(DLB_LFRACT *DLB_RESTRICT y
	,const DLB_ACCU *DLB_RESTRICT x
	,unsigned int shift
	,unsigned int n
	)
{
	unsigned int i;
	shift = (unsigned int)DLB_IlimaI((int)shift);
	if (0 < shift)
		for (i = 0; i < n; i++)
			y[i] = DLB_LtruncA( DLB_AshlAU(x[i], shift));
	else
		for (i = 0; i < n; i++)
			y[i] = DLB_LtruncA(x[i]);
}

static inline
void
DLB_vec_Lscale_upASUU
	(DLB_LFRACT *DLB_RESTRICT y
	,const DLB_ACCU *DLB_RESTRICT x
	,DLB_SFRACT c
	,unsigned int shift
	,unsigned int n
	)
{
	unsigned int i;
	shift = (unsigned int)DLB_IlimaI((int)shift);
	if (0 < shift)
		for (i = 0; i < n; i++)
			y[i] = DLB_LrndA( DLB_AshlAU( DLB_AmpyAS(x[i], c), shift));
	else
		for (i = 0; i < n; i++)
			y[i] = DLB_LrndA( DLB_AmpyAS(x[i], c));
}
#endif /* DLB_INTRINSICS_EXCLUDE_DEPRECATED_OPS */

#endif /* DLB_VECINLINES_H */
