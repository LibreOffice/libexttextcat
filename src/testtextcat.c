/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/* 
 * testtextcat.c -- a simple commandline classifier. Feed it input on
 * standard in and it will feed you a classification on standard out.
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <stdlib.h>
#include <string.h>

#include "textcat.h"
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

int main(int argc, char **argv)
{
    void *h;
    char *result;
    char *buf;
    const char *conf;
    int utfaware = TC_TRUE;

    if ((argc > 3) && (!strcmp(argv[3], "--no-utf8")))
    {
        utfaware = 0;
    }


    conf = argc > 1 ? argv[1] : "fpdb.conf";
    if (argc > 2)
        h = special_textcat_Init(conf, argv[2]);
    else
        h = textcat_Init(conf);
    if (!h)
    {
        fprintf(stderr, "Unable to init using '%s', Aborting.\n", conf);
        exit(-1);
    }
    textcat_SetProperty(h, TCPROP_UTF8AWARE, utfaware ? TC_TRUE : TC_FALSE);

    buf = myread(stdin);

    /*** We only need a little text to determine the language ***/
    buf[1024] = '\0';
    result = textcat_Classify(h, buf, strlen(buf) + 1);
    printf("%s\n", result);

    textcat_Done(h);

    free(buf);

    return 0;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
