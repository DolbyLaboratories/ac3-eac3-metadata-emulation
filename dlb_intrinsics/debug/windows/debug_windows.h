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
 * @brief Debug helpers for Windows platform
 */

#include <stdio.h>
#include <stdarg.h>
#include <windows.h>

#define BUFFER_SIZE 1024

#if (defined _MSC_VER && (_MSC_VER <= 1200))
#define vsnprintf _vsnprintf
#endif

static inline
void
dlb_raw_print
	(const char *msg
	,va_list ap
	)
{
	va_list ap_copy = ap;
	char buff[BUFFER_SIZE];
	vsnprintf(buff, BUFFER_SIZE - 1, msg, ap);
	OutputDebugStringA(buff);
	vfprintf(stderr, msg, ap_copy);
}

static inline
void
dlb_abort()
{
	/* If you are looking here, you may be wondering why your code hit a
	 * breakpoint when you didn't set any.
	 * The most likely scenario is that your code tripped a DLB_assert()
	 * or hit some other error case (DLB_print(DLB_ERROR() "...")).
	 * The text for the error should appear in the "Output" tab in
	 * Visual Studio, and if you are in a console application, then
	 * it will also be sent to stderr.
	 */
	DebugBreak();
}
