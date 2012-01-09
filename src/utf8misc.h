/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/***************************************************************************
 *   Copyright (C) 2006 by Jocelyn Merand                                  *
 *   joc.mer@gmail.com                                                     *
 *                                                                         *
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
 ***************************************************************************/

#ifndef _UTF8_MISC_H_
#define _UTF8_MISC_H_

#ifdef __cplusplus
extern "C"
{
#endif

    /* 
     * Is used to jump to the next start of char
     * of course it's only usefull when encoding is utf-8
     * This function have been added by Jocelyn Merand to use libtextcat in OOo
     */
    const char *utf8_next_char(const char *str);

    /* 
     * Copy the char in str to dest of course it's only usefull when encoding
     * is utf8 and the symbol is encoded with more than 1 char return the
     * number of char jumped This function have been added by Jocelyn Merand to
     * use libtextcat in OOo
     */
    int utf8_charcopy(const char *str, char *dest);


    /* 
     * checks if n-gram lex is a prefix of key and of length len len is the
     * number of unicode code points strlen("€") == 3 but len == 1
     */
    int utf8_issame(char *lex, char *key, int len);


    /* 
     * len is the number of unicode code points
     * strlen("€") == 3 but len == 1
     */
    extern int utf8_strlen(const char *str);
#ifdef __cplusplus
}
#endif

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
