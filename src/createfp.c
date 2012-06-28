/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
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
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "constants.h"

#include "fingerprint.h"
#include "common_impl.h"

#define BLOCKSIZE 4096

char *myread(FILE * fp)
{
    char *buf, *newbuf;
    size_t size = 0;
    size_t maxsize = BLOCKSIZE * 2;

    buf = (char *)malloc(maxsize);
    do
    {
        size_t hasread = fread(buf + size, 1, BLOCKSIZE, fp);
        size += hasread;
        if (size + BLOCKSIZE > maxsize)
        {
            maxsize *= 2;
            newbuf = (char *)realloc(buf, maxsize);
            if (!newbuf)
                free(buf);
            buf = newbuf;
        }

    }
    while (!feof(stdin) && buf);

    if (buf)
    {
        buf[size] = '\0';
        newbuf = (char *)realloc(buf, size + 1);
        if (!newbuf)
            free(buf);
        buf = newbuf;
    }

    return buf;
}

int main(int argc, char **args)
{
    void *h;
    char *buf;
    int utfaware = 1;

    if ((argc > 1) && (!strcmp(args[1], "--no-utf8")))
    {
        utfaware = 0;
    }

    buf = myread(stdin);

    h = fp_Init(NULL);
    if (utfaware)
    {
        fp_SetProperty(h, TCPROP_UTF8AWARE, TC_TRUE);
    }
    else
    {
        fp_SetProperty(h, TCPROP_UTF8AWARE, TC_FALSE);
    }
    fp_SetProperty(h, TCPROP_MINIMUM_DOCUMENT_SIZE, MINDOCSIZE);
    if (fp_Create(h, buf, strlen(buf), 400) == 0)
    {
        fprintf(stderr, "There was an error creating the fingerprint\n");
        exit(-1);
    }
    fp_Print(h, stdout);
    fp_Done(h);
    free(buf);

    return 0;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
