/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/* 
 * common.c -- miscellanious helper functions.
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
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include "common_impl.h"

extern char *wg_getline(char *line, int size, FILE * fp)
{
    char *p;

    if (fgets(line, size, fp) == NULL)
    {
        return NULL;
    }

        /** kill term null **/
    if ((p = strpbrk(line, "\n\r")))
    {
        *p = '\0';
    }

    return line;
}



/* 
 * wg_split: split a line into segments, using whitespace-sequences as separators.
 *
 * ARGUMENTS:
 * - result:
 *
 *  After the split, this array contains pointers to the start of each
 *  detected segment. Must be preallocated and at least as large as
 *  maxsegments. The pointers point into the dest buffer.
 *
 * - dest:
 *
 *  String into which result points as an index. Must be preallocated, and
 *  at least as big as src. You can use src as dest, but in that case src
 *  is overwritten!
 *
 * - src:
 *
 *  The string to split. Sequences of whitespace are treated as separators, unless
 *  escaped. There are two ways to escape: by using single quotes (anything
 *  between single quotes is treated as one segment), or by using a backslash
 *  to escape the next character. The backslash escape works inside quotation
 *  as well.
 *
 *  Example:
 *
 *  "It\'s   very\ easy   'to  use WiseGuys\' wg_split()' function" is split into:
 *
 *  "It's"
 *  "very easy"
 *  "to  use WiseGuys' wg_split()"
 *  "function"
 *
 * - maxsegments:
 *
 *  The maximum number of segments. If the splitter runs out of segments,
 *  the remainder of the string is stored in the last segment.
 *
 * RETURN VALUE:
 * The number of segments found.
 */
unsigned int wg_split(char **result, char *dest, char *src, int maxsegments)
{
    char *p = src;
    char *w = dest;
    int cnt = 0;
    int state = 0;

    if (maxsegments == 0)
    {
        return 0;
    }

    maxsegments--;

    while (cnt < maxsegments && *p)
    {

        switch (state)
        {
        case 0:
            /*** Skip spaces ***/
            while (isspace((unsigned char)*p))
            {
                p++;
            }

            /*** Start segment ***/
            result[cnt] = w;
            cnt++;
            state = 1;

            /*** fall through **/

        case 1:
            /*** Unquoted segment ***/
            while (*p)
            {
                if (isspace((unsigned char)*p))
                {
                    *w++ = '\0';
                    p++;
                    state = 0;
                    break;
                }
                else if (*p == '\'')
                {
                    /*** Start quotation ***/
                    p++;
                    state = 2;
                    break;
                }
                else if (*p == '\\' && p[1])
                {
                    /*** Escape ***/
                    p++;
                    *w++ = *p++;
                }
                else
                {
                    *w++ = *p++;
                }
            }
            break;

        case 2:
            /*** Inside quotes ***/
            while (*p)
            {
                if (*p == '\'')
                {
                    p++;
                    break;
                }
                else if (*p == '\\' && p[1])
                {
                    /*** Escape ***/
                    p++;
                    *w++ = *p++;
                }
                else
                {
                    *w++ = *p++;
                }
            }
            state = 1;
            break;

        }
    }

    if (!*p)
    {
        *w = '\0';
        return cnt;
    }

    /*** We ran out of segments; copy the remainder of the string into last segment ***/
    result[cnt++] = w;
    while (*p)
    {
        *w++ = *p++;
    }
    *w = '\0';
    return cnt;
}

/**
 * wg_strgmov -- a guarded strcpy() variation
 *
 * copies src to dest (including terminating zero), and returns
 * pointer to position of terminating zero in dest. The function is
 * guaranteed not to write past destlimit. If the copy couldn't be
 * finished, the function returns NULL after restoring the first
 * character in dest for your convenience (since this is usually a zero).
 */
char *wg_strgmov(char *dest, const char *src, const char *destlimit)
{
    char tmp, *w;

    if (!dest || dest >= destlimit)
    {
        return NULL;
    }

    tmp = *dest;
    w = dest;

    while (*src)
    {

        *w++ = *src++;

        if (w == destlimit)
        {
            /*** restore old situation ***/
            *dest = tmp;
            return NULL;
        }
    }

    *w = '\0';
    return w;

}

/* 
 * wg_trim() -- remove whitespace surrounding a string.
 *
 * Example: "   bla   bla   bla   " becomes "bla   bla   bla" after trimming.
 *
 * ARGUMENTS
 * - dest : After the trim, this will point to the trimmed string.
 *          Must be preallocated and at least as big as src. You can
 *          use src as dest.
 * - src : Points to the string to be trimmed.
 *
 * RETURNS:
 * dest
 */
char *wg_trim(char *dest, const char *src)
{
    char *lastnonspace = &dest[-1];
    const char *p = src;
    char *w = dest;

    while (isspace((unsigned char)*p))
    {
        p++;
    }
    while (*p)
    {
        if (!isspace((unsigned char)*p))
        {
            lastnonspace = w;
        }
        *w++ = *p++;
    }
    lastnonspace[1] = '\0';

    return dest;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
