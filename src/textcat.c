/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/**
 * textcat.c -- routines for categorizing text
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
 *
 * DESCRIPTION
 *
 * These routines use the N-gram fingerprinting technique as described
 * in Cavnar and Trenkle, (1994.), N-Gram-Based Text Categorization.
 * (cf. http://www.nonlineardynamics.com/trenkle/)
 *
 * REVISION HISTORY
 *
 * Mar 27, 2003 frank@wise-guys.nl -- created
 *
 * IMPROVEMENTS:
 * - If two n-grams have the same frequency count, choose the shortest
 * - Use a better similarity measure (the article suggests Wilcoxon rank test)
 * - The profiles are matched one by one, which results in redundant lookups.
 * - Make the thingy reentrant as well as thread-safe. (Reentrancy is abandoned
 *   by the use of the output buffer in textcat_t.)
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <string.h>

#include "common_impl.h"
#include "fingerprint.h"
#include "textcat.h"
#include "constants.h"


typedef struct
{

    void **fprint;
    unsigned char *fprint_disable;
    uint4 size;
    uint4 maxsize;

    char output[MAXOUTPUTSIZE];

} textcat_t;


typedef struct
{
    int score;
    const char *name;
} candidate_t;


static int cmpcandidates(const void *a, const void *b)
{
    const candidate_t *x = (const candidate_t *)a;
    const candidate_t *y = (const candidate_t *)b;
    return (x->score - y->score);
}


extern void textcat_Done(void *handle)
{
    textcat_t *h = (textcat_t *) handle;
    uint4 i;

    for (i = 0; i < h->size; i++)
    {
        fp_Done(h->fprint[i]);
    }
    free(h->fprint);
    free(h->fprint_disable);
    free(h);

}

/** Replaces older function */
extern void *textcat_Init(const char *conffile)
{
    return special_textcat_Init(conffile, DEFAULT_FINGERPRINTS_PATH);
}

/**
 * Originaly this function had only one parameter (conffile) it has been modified since OOo use
 * Basicaly prefix is the directory path where fingerprints are stored
 */
extern void *special_textcat_Init(const char *conffile, const char *prefix)
{
    textcat_t *h;
    char line[1024];
    FILE *fp;

    fp = fopen(conffile, "r");
    if (!fp)
    {
#ifdef VERBOSE
        fprintf(stderr, "Failed to open config file '%s'\n", conffile);
#endif
        return NULL;
    }

    h = (textcat_t *) malloc(sizeof(textcat_t));
    h->size = 0;
    h->maxsize = 16;
    h->fprint = (void **)malloc(sizeof(void *) * h->maxsize);
    h->fprint_disable = (unsigned char *)malloc(sizeof(unsigned char) * h->maxsize);
    /* added to store the state of languages */
    while (wg_getline(line, 1024, fp))
    {
        char *p;
        char *segment[4];
        char finger_print_file_name[512 + 1];
        int res;

        /*** Skip comments ***/
        if ((p = strchr(line, '#')))
        {
            *p = '\0';
        }
        if ((res = wg_split(segment, line, line, 4)) < 2)
        {
            continue;
        }

        /*** Ensure enough space ***/
        if (h->size == h->maxsize)
        {
            h->maxsize *= 2;
            h->fprint =
                (void **)realloc(h->fprint, sizeof(void *) * h->maxsize);
            h->fprint_disable =
                (unsigned char *)realloc(h->fprint_disable,
                                            sizeof(unsigned char) *
                                            h->maxsize);
        }

        /*** Load data ***/
        if ((h->fprint[h->size] = fp_Init(segment[1])) == NULL)
        {
            goto BAILOUT;
        }

        /*** Check filename overflow ***/
        finger_print_file_name[0] = finger_print_file_name[512] = '\0';
        strncat(finger_print_file_name, prefix, 512);
        strncat(finger_print_file_name, segment[0], 512);
        if (finger_print_file_name[512] != '\0')
        {
            goto BAILOUT;
        }

        if (fp_Read(h->fprint[h->size], finger_print_file_name, 400) == 0)
        {
            goto BAILOUT;
        }
        h->fprint_disable[h->size] = 0xF0;  /* 0xF0 is the code for enabled
                                               languages, 0x0F is for disabled */
        h->size++;
    }

    fclose(fp);
    return h;

  BAILOUT:
    textcat_Done(h);            /* don't leak h */
    fclose(fp);
    return NULL;

}

extern char *textcat_Classify(void *handle, const char *buffer, size_t size)
{
    textcat_t *h = (textcat_t *) handle;
    int minscore = MAXSCORE;
    int threshold = minscore;
    char *result = h->output;
    void *unknown;
    uint4 i, cnt;

    candidate_t *candidates =
        (candidate_t *) malloc(sizeof(candidate_t) * h->size);

    unknown = fp_Init(NULL);
    if (fp_Create(unknown, buffer, size, MAXNGRAMS) == 0)
    {
        /*** Too little information ***/
        result = _TEXTCAT_RESULT_SHORT;
        goto READY;
    }

    /*** Calculate the score for each category. ***/
    for (i = 0; i < h->size; i++)
    {
        int score;
        if (h->fprint_disable[i] & 0x0F)
        {                       /* if this language is disabled */
            score = MAXSCORE;
        }
        else
        {
            score = fp_Compare(h->fprint[i], unknown, threshold);
            /* printf("Score for %s : %i\n", fp_Name(h->fprint[i]), score); */
        }
        candidates[i].score = score;
        candidates[i].name = fp_Name(h->fprint[i]);
        if (score < minscore)
        {
            minscore = score;
            threshold = (int)((double)score * THRESHOLDVALUE);
        }
    }

    /*** Find the best performers ***/
    for (i = 0, cnt = 0; i < h->size; i++)
    {
        if (candidates[i].score < threshold)
        {
            if (++cnt == MAXCANDIDATES + 1)
            {
                break;
            }

            memcpy(&candidates[cnt - 1], &candidates[i], sizeof(candidate_t));

        }
    }

    /*** The verdict ***/
    if (cnt == MAXCANDIDATES + 1)
    {
        result = _TEXTCAT_RESULT_UNKOWN;
    }
    else
    {
        const char *plimit = result + MAXOUTPUTSIZE;
        char *p = result;

        qsort(candidates, cnt, sizeof(candidate_t), cmpcandidates);

        *p = '\0';
        for (i = 0; i < cnt; i++)
        {
            p = wg_strgmov(p, "[", plimit);
            p = wg_strgmov(p, candidates[i].name, plimit);
            p = wg_strgmov(p, "]", plimit);
        }
    }
  READY:
    fp_Done(unknown);
    free(candidates);
    return result;
}

extern const char *textcat_Version(void)
{
    return EXTTEXTCAT_VERSION;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
