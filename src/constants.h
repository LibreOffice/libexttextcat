/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
#ifndef _CONSTANTS_H_
#define _CONSTANTS_H_

/* 
 * constants.h -- some constants used throughout the code. Not pretty,
 * but certainly convenient.
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
#include <limits.h>

/* Reported matches are those fingerprints with a score less than best score * 
   THRESHOLDVALUE (i.e. a THRESHOLDVALUE of 1.03 means matches must score
   within 3% from the best score.) */
#define THRESHOLDVALUE  1.03

/* If more than MAXCANDIDATES matches are found, the classifier reports
   unknown, because the input is obviously confusing. */
#define MAXCANDIDATES   5

/* The size of the buffer used to report the classification. */
#define MAXOUTPUTSIZE 1024

/* Maximum number of n-grams in a fingerprint */
#define MAXNGRAMS  400

/* Maximum number of character of an n-gram? */
#define MAXNGRAMSYMBOL 5

/* Maximum size of the string representing an n-gram (must be greater than
   number of symbol) */
#define MAXNGRAMSIZE (MAXNGRAMSYMBOL*4)

/* Which characters are not acceptable in n-grams? */
#define INVALID(c) (isspace((unsigned char)c) || isdigit((unsigned char)c))

/* Minimum size (in characters) for accepting a document */
#define MINDOCSIZE 1

/* Maximum penalty for missing an n-gram in fingerprint */
#define MAXOUTOFPLACE 400

/* Size of hash table is 2^TABLEPOW. */
#define TABLEPOW  13

#define MAXSCORE  INT_MAX

/* where the fingerprints files are stored */
#define DEFAULT_FINGERPRINTS_PATH ""

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
