/*
 * wg_mempool.c -- Functions for managing a memory pool. 
 *
 * Copyright (C) 2003 WiseGuys Internet B.V.
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
 */

#include "config.h"
#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif
#ifdef HAVE_STRING_H
#include <string.h>
#endif
#include "common.h"

typedef struct memblock_s {
	char *pool;             
	char *p;
	char *pend;
	struct memblock_s *next;
} memblock_t;


typedef struct mempool_s {
	memblock_t *first;	/* linked list of blocks in use */
	memblock_t *spare;      /* linked list of spare blocks */
	size_t maxallocsize;
	size_t blocksize;
} mempool_t;

#ifdef HAVE_MEMSET
#define wg_memset	memset
#else
static void* wg_memset(void* s, int c, size_t n)
{
	size_t	i;

	for(i = 0; i < n; i++) s[i] = c;

	return s;
}
#endif

static void addblock( mempool_t *h )
{
	memblock_t *block;

	/*** Spare blocks from previous round? ***/
	if ( h->spare ) {
		/*** Use spare block ***/
		block = h->spare;
		h->spare = block->next;
	}
	else {
		/*** Make a new block ***/
		block = (memblock_t *)wg_malloc( sizeof(memblock_t) );
		block->pool = (char *)wg_malloc( h->blocksize );
	}

	block->p = block->pool;
	block->pend = block->pool + h->blocksize - h->maxallocsize;
        block->next = h->first;
	h->first = block;
}


extern void *wgmempool_Init(size_t blocksize, size_t maxstrsize)
{
	mempool_t *result = (mempool_t *)wg_malloc( sizeof(mempool_t) );
	
	result->first = NULL;
	result->spare = NULL;
	result->blocksize = blocksize;
	result->maxallocsize = maxstrsize?(maxstrsize + 1):0;
	addblock( result );

	return (void *)result;
}


extern void wgmempool_Done( void *handle )
{
	mempool_t *h = (mempool_t *)handle;

	memblock_t *p;

	/*** Active blocks ***/
	p = h->first;
	while (p) {
		memblock_t *next = p->next;
		wg_free( p->pool );

		wg_memset( p, 0, sizeof(memblock_t)); /* for safety */
		wg_free( p );

		p = next;
	}

	/*** Spare blocks ***/
	p = h->spare;
	while (p) {
		memblock_t *next = p->next;
		wg_free( p->pool );

		wg_memset( p, 0, sizeof(memblock_t)); /* for safety */
		wg_free( p );

		p = next;
	}

	wg_memset( h, 0, sizeof(mempool_t)); /* for safety */
	wg_free(h);
}

extern void wgmempool_Reset( void *handle )
{
	mempool_t *h = (mempool_t *)handle;
	memblock_t *p;

	if ( !h->first ) {
		return;
	}

	/*** Find last active block ***/
	p = h->first;
	while (p->next) {
		p = p->next;
	}

	/*** Append spare list to it ***/
	p->next = h->spare;
	h->spare = h->first;
	h->first = NULL;	

	/*** Start with a new block ***/
	addblock(h);
}


extern void *wgmempool_alloc( void *handle, size_t size )
{
	void *result;
	mempool_t *h = (mempool_t *)handle;
	memblock_t *block = h->first;

	/*** Too little space left in block? ***/
	if ( block->p + size > block->pend + h->maxallocsize ) {
		addblock(h);
		block = h->first;
	}
	result = (void *)block->p;
	block->p += size;
	return result;
}



extern char *wgmempool_strdup( void *handle, const char *str )
{
	char *w, *result;
	mempool_t *h = (mempool_t *)handle;
	memblock_t *block = h->first;

	/*** Create extra room? ***/
	if ( h->maxallocsize ) {
		if ( block->p >= block->pend ) {
			addblock(h);
			block = h->first;
		}
	}
	else if ( block->p + strlen(str) + 1 >= block->pend ) {
		addblock(h);
		block = h->first;
	}

	/*** Enough room, so copy string ***/
	result = w = block->p;
        while (*str) {
		*w++ = *str++;
	}
	*w++ = '\0';
	block->p = w;
	return result;
}


extern char *wgmempool_getline( void *handle, size_t size, FILE *fp )
{
	char *result, *p;
	mempool_t *h = (mempool_t *)handle;
	memblock_t *block = h->first;

	/*** Enough space? ***/
	if ( block->p + size > block->pend + h->maxallocsize ) {
		addblock(h);
		block = h->first;
	}

	result = (char *)block->p;
        fgets(result, size, fp) ;
        
        /** end of stream? **/
        if ( feof( fp ) ) {
                return NULL;
        }
        
        /** find end of line **/
	p = result;
	while (*p && *p != '\n' && *p != '\r' ) {
		p++;
	}
	*p++ = '\0';
  
	block->p = p;
	return result;
}


