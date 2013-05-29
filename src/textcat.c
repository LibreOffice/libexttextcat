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
    uint4 mindocsize;

    char output[MAXOUTPUTSIZE];
    candidate_t *tmp_candidates;
    boole utfaware;
} textcat_t;


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
    if (h->tmp_candidates != NULL)
    {
        textcat_ReleaseClassifyFullOutput(h, h->tmp_candidates);
    }
    free(h->fprint);
    free(h->fprint_disable);
    free(h);

}

extern int textcat_SetProperty(void *handle, textcat_Property property,
                               sint4 value)
{
    textcat_t *h = (textcat_t *) handle;
    switch (property)
    {
    case TCPROP_UTF8AWARE:
        if ((value == TC_TRUE) || (value == TC_FALSE))
        {
            h->utfaware = value;
            return 0;
        }
        return -2;
        break;
    case TCPROP_MINIMUM_DOCUMENT_SIZE:
        if (value > 0)
        {
            h->mindocsize = value;
            return 0;
        }
        return -2;
        break;
    default:
        break;
    }
    return -1;
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
    char *finger_print_file_name;
    size_t finger_print_file_name_size;
    size_t prefix_size;
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
    h->mindocsize = MINDOCSIZE;
    h->fprint = (void **)malloc(sizeof(void *) * h->maxsize);
    h->fprint_disable =
        (unsigned char *)malloc(sizeof(unsigned char) * h->maxsize);
    /* added to store the state of languages */
    h->tmp_candidates = NULL;
    h->utfaware = TC_TRUE;

    prefix_size = strlen(prefix);
    finger_print_file_name_size = prefix_size + 1;
    finger_print_file_name =
        (char *)malloc(sizeof(char) * (finger_print_file_name_size + 1024));
    finger_print_file_name[0] = '\0';
    strcat(finger_print_file_name, prefix);

    while (wg_getline(line, 1024, fp))
    {
        char *p;
        char *segment[4];

        /*** Skip comments ***/
        if ((p = strchr(line, '#')))
        {
            *p = '\0';
        }

        if (wg_split(segment, line, line, 4) < 2)
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
                                         sizeof(unsigned char) * h->maxsize);
        }

        /*** Load data ***/
        if ((h->fprint[h->size] = fp_Init(segment[1])) == NULL)
        {
            goto BAILOUT;
        }

        while (prefix_size + strlen(segment[0]) > finger_print_file_name_size)
        {
            char *tmp;
            size_t tmp_size = finger_print_file_name_size * 2;
            tmp =
                (char *)realloc(finger_print_file_name,
                                sizeof(char) * (tmp_size + 1));
            if (tmp == NULL)
            {
                goto BAILOUT;
            }
            else
            {
                finger_print_file_name = tmp;
                finger_print_file_name_size = tmp_size;
            }
        }
        finger_print_file_name[prefix_size] = '\0';
        strcat(finger_print_file_name, segment[0]);

        if (fp_Read(h->fprint[h->size], finger_print_file_name, 400) == 0)
            goto BAILOUT;
        h->fprint_disable[h->size] = 0xF0;  /* 0xF0 is the code for enabled
                                               languages, 0x0F is for disabled 
                                             */
        h->size++;
    }

    free(finger_print_file_name);

    fclose(fp);
    return h;

  BAILOUT:
    free(finger_print_file_name);
    fclose(fp);
    textcat_Done(h);
    return NULL;
}

extern candidate_t *textcat_GetClassifyFullOutput(void *handle)
{
    textcat_t *h = (textcat_t *) handle;
    return (candidate_t *) malloc(sizeof(candidate_t) * h->size);
}

extern void textcat_ReleaseClassifyFullOutput(void *handle,
                                              candidate_t * candidates)
{
    if (candidates != NULL)
    {
        free(candidates);
    }
}

extern char *textcat_Classify(void *handle, const char *buffer, size_t size)
{
    textcat_t *h = (textcat_t *) handle;
    char *result = h->output;
    uint4 i, cnt;

    if (h->tmp_candidates == NULL)
    {
        h->tmp_candidates = textcat_GetClassifyFullOutput(h);
    }

    cnt = textcat_ClassifyFull(h, buffer, size, h->tmp_candidates);

    switch (cnt)
    {
    case TEXTCAT_RESULT_UNKNOWN:
        result = TEXTCAT_RESULT_UNKNOWN_STR;
        break;
    case TEXTCAT_RESULT_SHORT:
        result = TEXTCAT_RESULT_SHORT_STR;
        break;
    default:
        {
            const char *plimit = result + MAXOUTPUTSIZE;
            char *p = result;

            *p = '\0';
            for (i = 0; i < cnt; i++)
            {
                p = wg_strgmov(p, "[", plimit);
                p = wg_strgmov(p, h->tmp_candidates[i].name, plimit);
                p = wg_strgmov(p, "]", plimit);
            }
        }
    }

    return result;
}


extern int textcat_ClassifyFull(void *handle, const char *buffer, size_t size,
                                candidate_t * candidates)
{
    textcat_t *h = (textcat_t *) handle;
    uint4 i, cnt = 0;
    int minscore = MAXSCORE;
    int threshold = minscore;

    void *unknown;

    unknown = fp_Init(NULL);
    fp_SetProperty(unknown, TCPROP_UTF8AWARE, h->utfaware);
    fp_SetProperty(unknown, TCPROP_MINIMUM_DOCUMENT_SIZE, h->mindocsize);
    if (fp_Create(unknown, buffer, size, MAXNGRAMS) == 0)
    {
        /*** Too little information ***/
        fp_Done(unknown);
        return TEXTCAT_RESULT_SHORT;
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

    fp_Done(unknown);
    /*** The verdict ***/
    if (cnt == MAXCANDIDATES + 1)
    {
        return TEXTCAT_RESULT_UNKNOWN;
    }
    else
    {
        qsort(candidates, cnt, sizeof(candidate_t), cmpcandidates);
        return cnt;
    }
}

extern const char *textcat_Version(void)
{
    return EXTTEXTCAT_VERSION;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
