/*
 * createfprint.c - can be used to create a fingerprint of a document.
 *
 * Copyright (c) 2003, WiseGuys Internet B.V.
 * All rights reserved.
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
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

#include "fingerprint.h"
#include "common.h"

#define BLOCKSIZE 4096

char *myread(FILE *fp)
{
	char *buf;
	size_t size = 0;
	size_t maxsize = BLOCKSIZE*2;

	buf = (char *)wg_malloc( maxsize );
	do {
		size_t hasread = fread( buf+size, 1, BLOCKSIZE, fp );
		size += hasread;
		if ( size + BLOCKSIZE > maxsize ) {
			maxsize *= 2;
			buf = (char *)wg_realloc( buf, maxsize );
		}

	} while (!feof(stdin));

	buf[size] = '\0';
	buf = (char *)wg_realloc( buf, size+1 );

	return buf;
}


int main()
{
	void *h;
	char *buf;

	buf = myread(stdin);

	h = fp_Init(NULL);
	if ( fp_Create( h, buf, strlen(buf), 400 ) == 0 ) {
		fprintf(stderr, "There was an error creating the fingerprint\n");
		exit(-1);
	}
	fp_Print(h,stdout);
	fp_Done(h);
	wg_free(buf);

	return 0;
}
