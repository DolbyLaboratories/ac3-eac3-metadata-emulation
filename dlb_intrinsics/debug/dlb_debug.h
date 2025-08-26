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
 * @brief Dolby Intrinsics debug functions header file
 */

/**************************************************************************//**
@anchor debugging_abstraction
@name Debugging Abstraction

The debug abstraction provides a printf()-like function, and an assert()-like 
macro. Each call can have a set of tags applied to it, which are used to 
decide whether or not to print the message, and if the message is fatal.

The particular backend defined will decide exactly what it means to print
or abort.

To choose which messages to print, set the DLB_PRINT environment variable. It
should be defined as a simple set-expression using a "." for an intersection, "+"
for a union, and tags for sets. For example "DLB_PRINT=info+warn" will enable
printing of warning and information messages. "DLB_PRINT=info.foo+warn"
will enable printing of warning messages, and information messages for the
component "foo". If not set, the default behaviour will print messages 
tagged with "warn".

Similarly, you can set the DLB_ABORT environment variable to choose which
messages cause an abort to be triggered. The syntax is identical. Any message
which triggers an abort will be printed. If not set, the default behaviour
will abort (and print) on messages tagged with "error".

When specifying DLB_PRINT or DLB_ABORT, you may use the special values "all"
or "none" for the universal set or the empty set. Note that setting either
of these variables to an empty string will mean that the default behaviour
will be used, rather than specifying an empty set.
******************************************************************************/
/* @{ */

 /**
 @addtogroup dlb_debug
 @{
 */

/** DLB_print is for debugging messages. Depending on the debug backend, the
 * message may end up being sent to different places.
 * You should prefix the format string with a set of tags as produced by
 * DLB_DEBUG_TAGS, or a higher level macro which calls DLB_DEBUG_TAGS.
 * Without the tag prefix, your message will be printed regardless of debugging
 * settings, so this is useful for temporary debugging code.
 */
void DLB_print(const char *str,...);

/** DLB_printif is closely related to DLB_print. 
 * The only difference is that it will check the string 'tag' if the string 'str'
 * should be printed or not. 'str' does not need to contain any tags.
 * DLB_printif is useful for 'untagged' debug prints, e.g. in case you want 
 * multi-line prints or you want to print several values in one line using loops.
 */
void DLB_printif(const char *tag, const char *str,...);


/** DLB_is_debug_tag checks if the input string contains an active debug tag.
 * The function returns 1 if it found a tag specified in the environment
 * variable DLB_PRINT, 0 if not.
 */
int DLB_is_debug_tag(const char *str);


/** DLB_assert is a version of assert() which integrates better into the
 * debugging framework we have here. You may pass in any number of tags
 * delimited by '.', or leave tags empty. The string is not a printf() style
 * format string.
 */
#ifdef DLB_DEBUG
#define DLB_assert(expr, tags, str) \
    do { \
        if (!(expr)) \
            DLB_print(DLB_ERROR(tags) "%s(%d): %s\n", __FILE__, __LINE__, str); \
    } while (0)
#else
#define DLB_assert(expr, tags, str)
#endif

#ifdef DLB_DEBUG
#define DLB_assert_xargs(expr, tags, str) \
    do { \
        if (!(expr)) \
            DLB_print(DLB_ERROR(tags) DLB_FMT_XARGS ": %s\n" DLB_PASS_XARGS, str); \
    } while (0)
#else
#define DLB_assert_xargs(expr, tags, str)
#endif

/** DLB_const_assert will prevent code from compiling if the const_expr given
 * is not satisfied. The error message you get won't be particularly nice
 * however it will hopefully have the given name in it.
 */
#define DLB_const_assert(const_expr, name) \
    do { \
        int name[(const_expr) != 0]; \
        (void)name; \
    } while (0)

 /** @} */
/* @} */
