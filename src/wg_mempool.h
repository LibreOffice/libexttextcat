#ifndef _WGMEMPOOL_H_
#define _WGMEMPOOL_H_

#ifdef __cplusplus
extern "C" {
#endif


/*
 * mempool.c -- functions for efficient, pooled memory allocation
 *
 * THE BSD LICENSE
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 
 * - Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 
 * - Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the
 * distribution.
 * 
 * - Neither the name of the WiseGuys Internet B.V. nor the names of
 * its contributors may be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * DESCRIPTION
 *
 * These functions can be used to maintain a pool from which memory
 * can be claimed without performing numerous allocs. 
 *
 * A memory pool can increase time and space efficiency by factors
 * in situations where:
 *
 * - you need numerous allocations of little chunks of memory, e.g.
 *   a huge list of keywords.
 * - you need little or no deallocations: preferable just one big 
 *   dealloc at the end. 
 *
 * A memory pool is unfavorable in situations where you constantly
 * alloc and free memory. 
 *
 * NOTES
 *
 * - A memory pool consists of "blocks" of a fixed size, which are
 *   added as the need for extra memory grows. 
 * 
 * - Deallocation of the individual fragments is not possible. The
 *   pool can only be deallocated in its entirety.
 *
 */


#include "common.h"

/* 
 * wgmempool_Init() -- intialize a memory pool
 *
 * ARGUMENTS
 *
 * - blocksize : size of blocks of which pool consists
 * - maxstrsize: 
 *            -  > 0 : the maximum size of a string that is copied 
 *               with wgmempool_strdup(). Omits the necessity of boundschecking
 *               in wgmempool_strdup(), making is slighlty faster.
 *            -  0 : causes wgmempool_strdup() to do its own boundschecking,
 *               which makes it somewhat slower.
 *
 * RETURN VALUE
 *
 * mempool handler on success, NULL on error.
 * 
 */
extern void *wgmempool_Init(uint4 blocksize, size_t maxstrsize);


/*
 * wgmempool_Done() -- deallocate memory pool in its entirety
 */
extern void wgmempool_Done( void *handle );


/*
 * wgmempool_Reset() -- resets a memory pool for reuse
 *
 * wgmempool_Reset() preserves already claimed memory for reuse, making
 * it more time efficient than doing a wgmempool_Done() and wgmempool_Init().
 */
extern void wgmempool_Reset( void *handle );


/*
 * wgmempool_alloc() -- Allocate size bytes of memory in mempool 
 *
 * ARGUMENTS
 *
 * - size: number of bytes to claim
 *
 * RETURN VALUE
 *
 * - pointer to claimed memory on success
 * - NULL on error
 */
extern void *wgmempool_alloc( void *handle, size_t size );


/*
 * wgmempool_strdup() -- Allocate and copy a string to mempool
 *
 * ARGUMENTS
 *
 * - str : null-terminated string. If maxstrsize > 0 during
 *         init, strlen(str) may not be greater than this value.
 *
 * RETURN VALUE
 *
 * - pointer to duplicated string on success
 * - NULL on error
 */
extern char *wgmempool_strdup( void *handle, const char* str );


#ifdef __cplusplus
}
#endif


#endif

