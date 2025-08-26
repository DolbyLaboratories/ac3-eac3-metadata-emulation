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
 * @brief Debug Tags
 */

#ifndef DLB_DEBUG_TAGS_H
#define DLB_DEBUG_TAGS_H

 /**
 * @addtogroup dlb_debug The debugging module
 * This module contains macros and functions to assist with debugging the code.
 *
 * For temporary debugging, something like the following can be used:
@code DLB_print("Hello"); @endcode
 * for instrumentation of development code. This will always be printed as soon as the
 * program passes this line of code. Permanent instrumentation should be done using
 * the macros provided in dlb_debug_tags.h. E.g.:
@code DLB_print(DLB_ERROR("alloca"));
 DLB_print(DLB_WARN("dct") "Invalid DCT size.\n");
 DLB_print(DLB_INFO("qmf") "QMF size is: %d\n", size); @endcode
 * The difference between the macros is explained in the following table:
 *
<table>
<tr>
<th>macro</th>
<th>default behavior</th>
</tr>
<tr>
<td>DLB_ERROR</td>
<td>prints the message and abort</td>
</tr>
<tr>
<td>DLB_WARN</td>
<td>prints the message</td>
</tr>
<tr>
<td>DLB_INFO</td>
<td>nothing happens</td>
</tr>
</table>
 *
<h3>How to activate the debug prints</h3>
 * Compile the code with -DDLB_DEBUG (gcc) or /DDLB_DEBUG (MSVC).
 * When using dlb_intrinsics/Makefile, the following command line will do:
@code make BACKEND=<backend> <more flags> DEBUG_TOOLS=1 <target> @endcode
 *
 * Printing and stopping execution:
<table>
<tr>
<td></td>
<th>Bash Syntax</th>
</tr>
<tr>
<td>The environment variable DLB_PRINT controls what is to be printed.</td>
<td> > export DLB_PRINT=dct</td>
</tr>
<tr>
<td>The environment variable DLB_ABORT controls at which points the program will stop executing.</td>
<td> > export DLB_ABORT=dct</td>
</tr>
</table>
 *
 * Setting the DLB_ABORT to "dct" like in the example above would make the executable stop at
 * the first DLB_print() statement which is tagged with "dct". Default behavior:
<table>
<tr><td>DLB_print() statements tagged with DLB_ERROR() will be aborted by default.
The same is true for DLB_assert() statements.</td></tr>
</table>

 @{
 */

#define DLB_DEBUG_TAGS_PRE   "\x01"
#define DLB_DEBUG_TAGS_POST  ":"

/**
 * Used to decorate the tags for a call to DLB_print()
 */
#define DLB_DEBUG_TAGS(tags) DLB_DEBUG_TAGS_PRE tags DLB_DEBUG_TAGS_POST

/**
 * Information tag. Tags may be empty.
 */
#define DLB_INFO(tags)  DLB_DEBUG_TAGS(tags "+info")

/**
 * Error tag. Tags may be empty.
 */
#define DLB_ERROR(tags) DLB_DEBUG_TAGS(tags "+error")

/**
 * Warning tag. Tags may be empty.
 */
#define DLB_WARN(tags)  DLB_DEBUG_TAGS(tags "+warn")

 /** @} */

#endif
