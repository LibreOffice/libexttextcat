/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/**
 * fingerprint.c -- Routines for creating an n-gram fingerprint of a
 * buffer.
 *
 * Copyright (c) 2003, WiseGuys Internet B.V.
 * All rights reserved.
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
 * A fingerprint is a list of most common n-grams, ordered by
 * frequency. (Note that we can use other strings than n-grams, for
 * instance entire words.)
 *
 * HOW DOES IT WORK?
 *
 * - Buffer is sliced up into n-grams
 * - N-grams are inserted into a hash table that records their frequency
 * - The table entries are filtered through a N-sized heap to
 *   get the N most frequent n-grams.
 *
 * The reason why we go through the trouble of doing a partial
 * (heap)sort is that a full quicksort behaves horribly on the data:
 * most n-grams have a very low count, resulting in a data set in
 * nearly-sorted order. This causes quicksort to behave very badly.
 * Heapsort, on the other hand, behaves handsomely: worst case is
 * Mlog(N) for M n-grams filtered through a N-sized heap.
 *
 * REVISION HISTORY
 *
 * Mar 28, 2003 frank@wise-guys.nl -- created
 *
 * TODO:
 * - put table/heap datastructure in a separate file.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "common_impl.h"
#include "wg_mempool.h"
#include "constants.h"

#include "utf8misc.h"
#include "fingerprint.h"

#define TABLESIZE  (1<<TABLEPOW)
#define TABLEMASK  ((TABLESIZE)-1)

typedef struct
{

    sint2 rank;
    char str[MAXNGRAMSIZE + 1];

} ngram_t;

typedef struct fp_s
{

    const char *name;
    ngram_t *fprint;
    uint4 size;
    uint4 mindocsize;
    boole utfaware;

} fp_t;

typedef struct entry_s
{
    char str[MAXNGRAMSIZE + 1];
    unsigned int cnt;
    struct entry_s *next;
} entry_t;

typedef struct table_s
{
    void *pool;
    entry_t **table;
    entry_t *heap;

    struct table_s *next;

    uint4 heapsize;
    uint4 size;
} table_t;

/* 
 * fast and furious little hash function
 *
 * (Note that we could use some kind of rolling checksum, and update it
 * during n-gram construction)
 */
static uint4 simplehash(const char *p, int len)
{
    uint4 h = len * 13;
    while (*p)
    {
        h = (h << 5) - h + *p++;
    }
    return h;
}

/* increases frequency of ngram(p,len) */
static int increasefreq(table_t * t, char *p, int len,
                        int issameimpl(char *, char *, int))
{
    uint4 hash = simplehash(p, len) & TABLEMASK;
    entry_t *entry = t->table[hash];

    while (entry)
    {
        if (issameimpl(entry->str, p, len))
        {
            /*** Found it! ***/
            entry->cnt++;
            return 1;
        }
        else
        {
            entry = entry->next;
        }
    }

    /*** Not found, so create ***/
    entry = (entry_t *) (wgmempool_alloc(t->pool, sizeof(entry_t)));
    strncpy(entry->str, p, MAXNGRAMSIZE);
    entry->str[MAXNGRAMSIZE] = 0;
    entry->cnt = 1;

    entry->next = t->table[hash];
    t->table[hash] = entry;

    return 1;
}

#define GREATER(x,y) ((x).cnt > (y).cnt)
#define LESS(x,y)    ((x).cnt < (y).cnt)

static void siftup(table_t * t, unsigned int child)
{
    entry_t *heap = t->heap;
    unsigned int parent = (child - 1) >> 1;
    entry_t tmp;

    while (child > 0)
    {
        if (GREATER(heap[parent], heap[child]))
        {
            memcpy(&tmp, &heap[parent], sizeof(entry_t));
            memcpy(&heap[parent], &heap[child], sizeof(entry_t));
            memcpy(&heap[child], &tmp, sizeof(entry_t));
        }
        else
            return;

        child = parent;
        parent = (child - 1) >> 1;
    }
}

static void siftdown(table_t * t, unsigned int heapsize, uint4 parent)
{
    entry_t *heap = t->heap;
    unsigned int child = parent * 2 + 1;
    entry_t tmp;

    while (child < heapsize)
    {
        if (child + 1 < heapsize && GREATER(heap[child], heap[child + 1]))
        {
            child++;
        }
        if (GREATER(heap[parent], heap[child]))
        {
            memcpy(&tmp, &heap[parent], sizeof(entry_t));
            memcpy(&heap[parent], &heap[child], sizeof(entry_t));
            memcpy(&heap[child], &tmp, sizeof(entry_t));
        }
        else
            return;
        parent = child;
        child = (parent * 2) + 1;
    }
}

static int heapinsert(table_t * t, entry_t * item)
{
    entry_t *heap = t->heap;

    /*** Still room for an entry? ***/
    if (t->size < t->heapsize)
    {
        memcpy(&(heap[t->size]), item, sizeof(entry_t));
        siftup(t, t->size);
        t->size++;
        return 0;
    }

    /*** Worse than the worst performer? ***/
    if (LESS(*item, heap[0]))
    {
        return 0;
    }

    /*** Insert into heap and reheap ***/
    memcpy(&(heap[0]), item, sizeof(entry_t));
    siftdown(t, t->size, 0);
    return 0;
}

static int heapextract(table_t * t, entry_t * item)
{
    entry_t *p;

    if (t->size == 0)
        return 0;

    p = &(t->heap[0]);

    memcpy(item, p, sizeof(entry_t));
    if (t->size > 1)
        memcpy(&(t->heap[0]), &(t->heap[t->size - 1]), sizeof(entry_t));

    siftdown(t, t->size, 0);
    t->size--;

    return 1;
}

/*** Makes a heap of all table entries ***/
static int table2heap(table_t * t)
{
    int i;

    /*** Fill result heap ***/
    for (i = 0; i < TABLESIZE; i++)
    {
        entry_t *p = t->table[i];
        while (p)
        {
            heapinsert(t, p);
            p = p->next;
        }
    }

    return 1;
}

static table_t *inittable(uint4 maxngrams)
{
    table_t *result = (table_t *) calloc(1, sizeof(table_t));
    result->table = (entry_t **) calloc(1, sizeof(entry_t *) * TABLESIZE);
    result->pool = wgmempool_Init(10000, 10);

    result->heap = (entry_t *) malloc(sizeof(entry_t) * maxngrams);
    result->heapsize = maxngrams;
    result->size = 0;

    return result;
}

static void tabledone(table_t * t)
{
    if (!t)
        return;

    wgmempool_Done(t->pool);
    free(t->table);
    free(t->heap);
    free(t);
}

extern void *fp_Init(const char *name)
{
    fp_t *h = (fp_t *) calloc(1, sizeof(fp_t));

    h->utfaware = TC_TRUE;
    h->mindocsize = MINDOCSIZE;

    if (name)
        h->name = strdup(name);

    return (void *)h;
}

extern void fp_Done(void *handle)
{
    fp_t *h = (fp_t *) handle;

    if (h->name)
    {
        free((void *)h->name);
    }
    if (h->fprint)
    {
        free(h->fprint);
    }

    free(h);
}

extern const char *fp_Name(void *handle)
{
    fp_t *h = (fp_t *) handle;
    return h->name;
}

/**
 * Function that prepares buffer for n-grammification:
 * runs of invalid characters are collapsed to a single
 * underscore.
 *
 * Function is implemented as a finite state machine.
 */
static char *prepbuffer(const char *src, size_t bufsize, uint4 mindocsize)
{
    const char *p = src;
    char *dest = (char *)malloc(bufsize + 3);
    char *w = dest;
    char *wlimit = dest + bufsize + 1;

    if (INVALID(*p))
    {
        goto SPACE;
    }
    else if (*p == '\0')
    {
        goto END;
    }

    *w++ = '_';
    if (w == wlimit)
    {
        goto STOP;
    }

    goto WORD;


  SPACE:
    /*** Inside string of invalid characters ***/
    p++;
    if (INVALID(*p))
    {
        goto SPACE;
    }
    else if (*p == '\0')
    {
        goto END;
    }

    *w++ = '_';
    if (w == wlimit)
    {
        goto STOP;
    }

    goto WORD;

  WORD:
    /*** Inside string of valid characters ***/
    *w++ = *p++;
    if (w == wlimit)
    {
        goto END;
    }
    else if (INVALID(*p))
    {
        goto SPACE;
    }
    else if (*p == '\0')
    {
        goto STOP;
    }
    goto WORD;

  END:
    *w++ = '_';

  STOP:
    *w++ = '\0';

    /*** Docs that are too small for a fingerprint, are refused ***/
    if (w - dest < mindocsize)
    {
        free(dest);
        return NULL;
    }

    return dest;
}

/**
* this function extract all n-gram from past buffer and put them into the table "t"
* [modified] by Jocelyn Merand to accept utf-8 multi-character symbols to be used in OpenOffice
*/
static void utfcreatengramtable(table_t * t, const char *buf)
{
    char n[MAXNGRAMSIZE + 1];
    const char *p = buf;
    int i, decay;

    /*** Get all n-grams where 1<=n<=MAXNGRAMSIZE. Allow underscores only at borders. ***/
    while (1)
    {

        const char *q = p;
        char *m = n;

        /*** First char may be an underscore ***/
        decay = utf8_charcopy(q, m);    /* [modified] previously *q++ = *m++ */

        q += decay;             /* [modified] */
        m += decay;             /* [modified] */
        *m = '\0';

        increasefreq(t, n, 1, utf8_issame);


        if (*q == '\0')
            return;

        /*** Let the compiler unroll this ***/
        for (i = 2; i <= MAXNGRAMSYMBOL; i++)
        {
            decay = utf8_charcopy(q, m);    /* [modified] like above */
            m += decay;
            *m = '\0';

            increasefreq(t, n, i, utf8_issame);

            if (*q == '_')
                break;
            q += decay;
            if (*q == '\0')
                return;
        }

        p = utf8_next_char(p);  /* [modified] */
    }
    return;
}

/* checks if n-gram lex is a prefix of key and of length len */
static int issame(char *lex, char *key, int len)
{
    int i;
    for (i = 0; i < len; i++)
    {
        if (key[i] != lex[i])
        {
            return 0;
        }
    }
    if (lex[i] != 0)
    {
        return 0;
    }
    return 1;
}

static void createngramtable(table_t * t, const char *buf)
{
    char n[MAXNGRAMSIZE + 1];
    const char *p = buf;
    int i;

    /*** Get all n-grams where 1<=n<=MAXNGRAMSIZE. Allow underscores only at borders. ***/
    for (;; p++)
    {

        const char *q = p;
        char *m = n;

        /*** First char may be an underscore ***/
        *m++ = *q++;
        *m = '\0';

        increasefreq(t, n, 1, issame);

        if (*q == '\0')
        {
            return;
        }

        /*** Let the compiler unroll this ***/
        for (i = 2; i <= MAXNGRAMSIZE; i++)
        {

            *m++ = *q;
            *m = '\0';

            increasefreq(t, n, i, issame);

            if (*q == '_')
                break;
            q++;
            if (*q == '\0')
            {
                return;
            }
        }
    }
    return;
}


static int mystrcmp(const char *a, const char *b)
{
    while (*a && *a == *b)
    {
        a++;
        b++;
    }
    return (*a - *b);
}


static int ngramcmp_str(const void *a, const void *b)
{
    ngram_t *x = (ngram_t *) a;
    ngram_t *y = (ngram_t *) b;

    return mystrcmp(x->str, y->str);
}

static int ngramcmp_rank(const void *a, const void *b)
{
    ngram_t *x = (ngram_t *) a;
    ngram_t *y = (ngram_t *) b;

    return x->rank - y->rank;
}

extern int fp_SetProperty(void *handle, textcat_Property property, sint4 value)
{
    fp_t *h = (fp_t *) handle;
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
        h->mindocsize = (uint4) value;
        return 0;
        break;
    default:
        break;
    }
    return -1;
}

/**
 * Create a fingerprint:
 * - record the frequency of each unique n-gram in a hash table
 * - take the most frequent n-grams
 * - sort them alphabetically, recording their relative rank
 */
extern int fp_Create(void *handle, const char *buffer, uint4 bufsize,
                     uint4 maxngrams)
{
    sint4 i = 0;
    table_t *t = NULL;
    char *tmp = NULL;

    fp_t *h = (fp_t *) handle;

    if (bufsize < h->mindocsize)
        return 0;

    /*** Throw out all invalid chars ***/
    tmp = prepbuffer(buffer, bufsize, h->mindocsize);
    /* printf("Cleaned buffer : %s\n",tmp); */
    if (tmp == NULL)
        return 0;
    t = inittable(maxngrams);
    /* printf("Table initialized\n"); */

    /*** Create a hash table containing n-gram counts ***/
    if (h->utfaware)
    {
        utfcreatengramtable(t, tmp);
    }
    else
    {
        createngramtable(t, tmp);
    }
    /* printf("Table created\n"); */
    /*** Take the top N n-grams and add them to the profile ***/
    table2heap(t);
    maxngrams = WGMIN(maxngrams, t->size);

    h->fprint = (ngram_t *) malloc(sizeof(ngram_t) * maxngrams);
    h->size = maxngrams;

    /*** Pull n-grams out of heap (backwards) ***/
    for (i = maxngrams - 1; i >= 0; i--)
    {
        entry_t tmp2;

        heapextract(t, &tmp2);

        /*** the string and its rank is all we need ***/
        strcpy(h->fprint[i].str, tmp2.str);
        h->fprint[i].rank = i;
    }

    tabledone(t);
    free(tmp);

    /*** Sort n-grams alphabetically, for easy comparison ***/
    qsort(h->fprint, h->size, sizeof(ngram_t), ngramcmp_str);
    return 1;
}

extern int fp_Read(void *handle, const char *fname, int maxngrams)
{
    fp_t *h = (fp_t *) handle;
    FILE *fp;
    char line[1024];
    int cnt = 0;

    fp = fopen(fname, "r");
    if (!fp)
    {
#ifdef VERBOSE
        fprintf(stderr, "Failed to open fingerprint file '%s'\n", fname);
#endif
        return 0;
    }

    h->fprint = (ngram_t *) malloc(maxngrams * sizeof(ngram_t));

    while (cnt < maxngrams && wg_getline(line, 1024, fp))
    {

        char *p;

        wg_trim(line, line);

        p = strpbrk(line, " \t");
        if (p)
        {
            *p = '\0';
        }

        if (strlen(line) > MAXNGRAMSIZE)
        {
            continue;
        }

        strcpy(h->fprint[cnt].str, line);
        h->fprint[cnt].rank = cnt;

        cnt++;
    }

    h->size = cnt;

    /*** Sort n-grams, for easy comparison later on ***/
    qsort(h->fprint, h->size, sizeof(ngram_t), ngramcmp_str);

    fclose(fp);

    return 1;
}

extern void fp_Print(void *handle, FILE * fp)
{
    uint4 i;
    fp_t *h = (fp_t *) handle;
    ngram_t *tmp = (ngram_t *) malloc(sizeof(ngram_t) * h->size);

    /*** Make a temporary and sort it on rank ***/
    memcpy(tmp, h->fprint, h->size * sizeof(ngram_t));
    qsort(tmp, h->size, sizeof(ngram_t), ngramcmp_rank);

    for (i = 0; i < h->size; i++)
    {
        /* fprintf( fp, "%s\t%i\n", tmp[i].str, tmp[i].rank ); */
        fprintf(fp, "%s\n", tmp[i].str);
    }
    free(tmp);
}



extern sint4 fp_Compare(void *cat, void *unknown, int cutoff)
{
    fp_t *c = (fp_t *) cat;
    fp_t *u = (fp_t *) unknown;
    uint4 i = 0;
    uint4 j = 0;
    sint4 sum = 0;

    /*** Compare the profiles in mergesort fashion ***/
    while (i < c->size && j < u->size)
    {

        int cmp = mystrcmp(c->fprint[i].str, u->fprint[j].str);

        if (cmp < 0)
        {
            i++;
        }
        else if (cmp == 0)
        {
            sum += abs(c->fprint[i].rank - u->fprint[j].rank);
            if (sum > cutoff)
                return MAXSCORE;
            i++;
            j++;
        }
        else
        {
            sum += MAXOUTOFPLACE;
            if (sum > cutoff)
                return MAXSCORE;
            j++;
        }
    }

    /*** Process tail of unknown, if any ***/
    while (j < u->size)
    {
        sum += MAXOUTOFPLACE;
        if (sum > cutoff)
            return MAXSCORE;
        j++;
    }

    return sum;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
