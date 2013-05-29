/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
#ifndef _TEXTCAT_H_
#define _TEXTCAT_H_
/* 
 * textcat.h -- routines for categorizing text
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
#include "exttextcat-version.h"
#include "common.h"
#include "textcat_properties.h"

#define TEXTCAT_RESULT_UNKNOWN_STR   "UNKNOWN"
#define TEXTCAT_RESULT_SHORT_STR     "SHORT"
#define TEXTCAT_RESULT_UNKNOWN        0
#define TEXTCAT_RESULT_SHORT         -2

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct
    {
        int score;
        const char *name;
    } candidate_t;

    /**
     * textcat_Init() - Initialize the text classifier. The textfile
     * conffile should contain a list of fingerprint filenames and
     * identification strings for the categories.  The filenames should be
     * reachable from the current working directory. The identification
     * strings will are used in the classification output.
     * 
     * Returns: handle on success, NULL on error. (At the moment, the
     * only way errors can occur, is when the library cannot read the
     * conffile, or one of the fingerprint files listed in it.)
     *
     * Replace older function (and has exacly the same behaviour)
     * see below
     */
    extern void *textcat_Init(const char *conffile);

    /**
     * special_textcat_Init() - Initialize the text classifier. This function
     * prepare the classifier as needed by OpenOffice.org. The textfile
     * conffile should contain a list of utf8 fingerprint filenames and
     * identification strings for the categories.prefix will be
     * prepended to the filenames to locate the files. The identification
     * strings will be used in the classification output.
     * 
     * Returns: handle on success, NULL on error. (At the moment, the
     * only way errors can occur, is when the library cannot read the
     * conffile, or one of the fingerprint files listed in it.)
     */
    extern void *special_textcat_Init(const char *conffile,
                                      const char *prefix);

    extern int textcat_SetProperty(void *handle, textcat_Property property,
                                   sint4 value);

    /**
     * textcat_Done() - Free up resources for handle
     */
    extern void textcat_Done(void *handle);

    /**
     * textcat_Classify() - Give the most likely categories for buffer
     * with length size.
     *
     * Returns: string containing a list of category id's, each one
     * between square brackets, "UNKNOWN" when not recognized, "SHORT" if the
     * document was too short to make a reliable assessment.
     *
     * Performace note: longer buffers take longer to process. However,
     * for many uses it is not necessary to categorize the whole buffer.
     * For language classification, a few hundred bytes will suffice.  
     */
    extern char *textcat_Classify(void *handle, const char *buffer,
                                  size_t size);


    /**
     * textcat_GetClassifyFullOutput() - Create a classifier output handler
     */
    extern candidate_t *textcat_GetClassifyFullOutput(void *handle);

    /**
     * textcat_ReleaseClassifyFullOutput() - Free up resources for the
     * classifier output handler
     */
    extern void textcat_ReleaseClassifyFullOutput(void *handle,
                                                  candidate_t * candidates);

    /**
     * textcat_ClassifyFull() - Give the most likely categories for buffer
     * with length size.
     *
     * Returns: the numbers of results.
     *
     * Performace note: longer buffers take longer to process. However,
     * for many uses it is not necessary to categorize the whole buffer.
     * For language classification, a few hundred bytes will suffice.
     */
    extern int textcat_ClassifyFull(void *handle, const char *buffer,
                                    size_t size, candidate_t * candidates);


    /**
     * textcat_Version() - Returns a string describing the version of this
     * classifier.
     */
    extern const char *textcat_Version(void);

#ifdef __cplusplus
}
#endif
#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
