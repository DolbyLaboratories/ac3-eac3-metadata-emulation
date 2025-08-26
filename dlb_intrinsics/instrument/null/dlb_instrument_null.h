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
 * @brief Default instrumentation backend (no instrumentation)
 */

#ifndef DLB_INSTRUMENT_NULL_H
#define DLB_INSTRUMENT_NULL_H

/**
 * @addtogroup dlb_instrument
 *
 * @brief An abstract instrumentation interface.
 *
 * The instrumentation API is defined through macros to allow for multiple
 * instrumentation backends to be added to the API. Currently there are only three
 * defined:
 * @li dlb_instrument_null, which completely removes all instrumentation (Default).
 * @li dlb_instrument_count, which allows counting of dlb_instrinsic usage and SFRAC
 * coefficient memory.
 * @li dlb_instrument_profiler which lets you gather per module timing metrics.
 *
 * These are the basic steps of using the instrumentation module.
 *
 * @li Instrument your code using the instrumentation interface macros defined below.
 *
 * Typical example code might look like:
 * @code
 * #include <stdio.h>
 *
 * DLB_LFRACT sum_vec(DLB_LFRACT p[], int len)
 * {
 * 	int j;
 * 	DLB_LFRACT total = (DLB_LFRACT)0;
 *
 * 	DLB_INSTRUMENT_ENTER_PUNIT("sum_vec")
 *
 * 	for (j=0; j<len; j++)
 * 		total = DLB_LaddLL(total, p[j]);
 *
 * 	DLB_INSTRUMENT_LEAVE_PUNIT("sum_vec")
 * }
 *
 * void main()
 * {
 *
 * 	FILE *fp;
 * 	DLB_LFRACT total;
 * 	DLB_LFRACT nb[4];
 *
 * 	nb[0] = DLB_L_F(0.5000);
 * 	nb[1] = DLB_L_F(0.7071);
 * 	nb[2] = DLB_L_F(3.1416);
 * 	nb[3] = DLB_L_F(2.7183);
 *
 * 	DLB_INSTRUMENT_OPEN
 *
 * 	total = sum_vec(nb, 4);
 *
 *  # The code to report the instrumentation will vary
 *  # from instrumentation backend and how the application wants
 *  # to represent the results (via GUI or file).
 *  #
 *  # Each instrumentation backend has a sample reporting funtion
 *  # sample code which can be copied for a good example.
 *  #
 * 	DLB_INSTRUMENT_REPORT()
 *
 * 	DLB_INSTRUMENT_CLOSE
 *
 * }
 * @endcode
 *
 * @li Choose one of the instrumentation backends by adding the appropriate define
 * to your makefile or project. When no backend is chosen, the dlb_instrument_null backend
 * is used.
 *
 * @li Run your code and gather the result from the generated report output passed into
 * to DLB_INSTRUMENT_REPORT.
 *
 * @{
 *
 * @name Dlb_intrinsic Instrumentation Macros
 * @{
 */

/** Open instrumentation.
* Use this macro once before any other isntrumentation macros.
*/
#undef DLB_INSTRUMENT_OPEN
#define DLB_INSTRUMENT_OPEN()

/** Close instrumentation.
* Clean up the instrumentation. Ue this macro after you have finished
* collecting your statistics. After this macro all gathered statistics are lost.
*/
#undef DLB_INSTRUMENT_CLOSE
#define DLB_INSTRUMENT_CLOSE()

/** The given dlb instrinic is being executed.
* The instrumentation backed updates it's counts for this intrinsic. This macro
* is already incorporated into the dlb instrinsics code, and need not be called
* by the users code.
*/
#undef DLB_INSTRUMENT_OP
#define DLB_INSTRUMENT_OP(op)

/** Enter a profiling unit.
* After you enter a profiling unit, all dlb_instrinsic counts will be assocated with
* the given punit. Nested profiling units are permitted.
*/
#undef DLB_INSTRUMENT_ENTER_PUNIT
#define DLB_INSTRUMENT_ENTER_PUNIT(punit_id)

/** Enters a critical profiling unit.
* Used when the profiling is very important.
* This means all child punits are also made critical.
* Never use a critical punit inside another critical punit, the results
* are undefined. This punit section must be closed by
* DLB_INSTRUMENT_LEAVE_CRITICAL_PUNIT.
*/
#undef DLB_INSTRUMENT_ENTER_CRITICAL_PUNIT
#define DLB_INSTRUMENT_ENTER_CRITICAL_PUNIT(punit_id)

/** Leave a profiling unit.
* Dlb_instrinsic counts will nolonger be associated with the given profiling unit.
*/
#undef DLB_INSTRUMENT_LEAVE_PUNIT
#define DLB_INSTRUMENT_LEAVE_PUNIT(punit_id)

/** Leave a critical profiling unit.
* Must be called only for the corresponding DLB_INSTRUMENT_ENTER_CRITICAL_PUNIT()
* call.
*/
#undef DLB_INSTRUMENT_LEAVE_CRITICAL_PUNIT
#define DLB_INSTRUMENT_LEAVE_CRITICAL_PUNIT(punit_id)


/** Enter a new frame.
* Notify the instumentation backend that a new frame has been entered.
*/
#undef DLB_INSTRUMENT_ENTER_FRAME
#define DLB_INSTRUMENT_ENTER_FRAME

/** Leave a frame.
* Notify the instumentation backend that the frame has been completed.
* This operation is not implemented in the current implementation.
*/
#undef DLB_INSTRUMENT_LEAVE_FRAME
#define DLB_INSTRUMENT_LEAVE_FRAME


/** Suspend counting of dlb_instrinsics on the given profiling unit.
* This operation is not implemented in the current implementation.
*/
#undef DLB_INSTRUMENT_SUSPEND
#define DLB_INSTRUMENT_SUSPEND(punit_id)

/** Resume counting of dlb_intrinsics on the given profiling unit.
* This is not implemented in the current implementation
*/
#undef DLB_INSTRUMENT_RESUME
#define DLB_INSTRUMENT_RESUME(punit_id)

/** Open stack frame instrumentation. Call from the same stack level
* as the processing you wish to estimate stack usage.
* This is not implemented in the current implementation
*/
#undef DLB_INSTRUMENT_STACK_OPEN
#define DLB_INSTRUMENT_STACK_OPEN

/** Close stack frame instrumentation. Call from the same stack level
* as you called DLB_INSTRUMENT_STACK_OPEN, after your processing is complete.
* This is not implemented in the current implementation
*/
#undef DLB_INSTRUMENT_STACK_CLOSE
#define DLB_INSTRUMENT_STACK_CLOSE

/** Returns a instrumentation backend specific structure containing
 * the results of profiling since the initialisation.
 * See instrumentation backend sub modules for more details..
 */
#undef DLB_INSTRUMENT_REPORT
#define DLB_INSTRUMENT_REPORT()

/** Use this macro when you are entering a section of code that is executed
* once for the given profiling unit. Any subsequent instrumentation calls
* will be associated with the given profiling unit. This is is useful for
* delineating functions associated with SFRAC coefficient rom counting for
* a profiling unit. Use in conjuction with DLB_INSTRUMENT_LEAVE_INIT
*/
#undef DLB_INSTRUMENT_ENTER_INIT
#define DLB_INSTRUMENT_ENTER_INIT(punit)

/** Size the data, pointed to by the given pointer, and add it to the
* SFRACT coefficient rom count for the current profiling unit.
*/
#undef DLB_INSTRUMENT_PSFRAC
#define DLB_INSTRUMENT_PSFRAC(p)

/** Size the passed data structure and add it to the SFRACT coefficient
* rom count for the current profiling unit. The data is assumed to be sizable by
* the c fuction sizeof.
*/
#undef DLB_INSTRUMENT_SFRAC
#define DLB_INSTRUMENT_SFRAC(c)

/** Size the passed data structure and add it to the SFRACT coefficient
* rom count for the current profiling unit. The data structure t is an array of
* n pointers. Each pointer points to data sizeable by the c fuction sizeof. The
* pointers may be null.
*/
#undef DLB_INSTRUMENT_TSFRAC
#define DLB_INSTRUMENT_TSFRAC(t, n)

/** Size the passed data structure and add it to the SFRACT coefficient
* rom count for the current profiling unit. The data structure t is an array of
* n pointers. Each pointer points to data sizeable by the c fuction sizeof. The
* pointers may be null.
*/
#undef DLB_INSTRUMENT_T2SFRAC
#define DLB_INSTRUMENT_T2SFRAC(t, n, m)

/** Increment the SFRACT coefficient rom count by n SFRACs.
*/
#undef DLB_INSTRUMENT_NSFRAC
#define DLB_INSTRUMENT_NSFRAC(n)

/** Use this macro when you are leaving a section of code that is executed
* once for the given profiling unit. Any subsequent instrumentations calls
* will be no longer be associated with the given profiling unit. Use in
* conjuection with DLB_INSTRUMENT_ENTER_INIT
*/
#undef DLB_INSTRUMENT_LEAVE_INIT
#define DLB_INSTRUMENT_LEAVE_INIT(punit)

/** @} */
/** @} */

#endif
