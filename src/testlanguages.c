/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * testlanguages.c -- check listing, enabling and disabling the languages
 * of a fingerprint database.
 *
 * Usage: testlanguages fpdb.conf LM/ ShortTexts/en.txt
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "textcat.h"

static int failures = 0;

static void check(int condition, const char *what)
{
    if (!condition)
    {
        fprintf(stderr, "FAIL: %s\n", what);
        failures++;
    }
}

static int classifies_as_english(void *h, const char *text)
{
    const char *result = textcat_Classify(h, text, strlen(text));
    return strncmp(result, "[en--utf8]", 10) == 0;
}

int main(int argc, char **argv)
{
    void *h;
    FILE *fp;
    char text[4096];
    size_t len;
    uint4 count, i, english;

    if (argc != 4)
    {
        fprintf(stderr, "Usage: %s fpdb.conf LM/ en.txt\n", argv[0]);
        return 1;
    }

    fp = fopen(argv[3], "r");
    if (!fp)
    {
        fprintf(stderr, "Unable to open '%s'\n", argv[3]);
        return 1;
    }
    len = fread(text, 1, sizeof(text) - 1, fp);
    fclose(fp);
    text[len] = '\0';

    h = special_textcat_Init(argv[1], argv[2]);
    if (!h)
    {
        fprintf(stderr, "Unable to init using '%s'\n", argv[1]);
        return 1;
    }

    count = textcat_GetLanguageCount(h);
    check(count > 0, "the database has languages");

    english = count;
    for (i = 0; i < count; i++)
    {
        const char *name = textcat_GetLanguageName(h, i);
        check(name != NULL, "every language in range has a name");
        if (name && strcmp(name, "en--utf8") == 0)
            english = i;
        check(textcat_IsLanguageEnabled(h, i) == TC_TRUE,
              "every language starts enabled");
    }
    check(english < count, "English is in the database");

    check(textcat_GetLanguageName(h, count) == NULL,
          "a language out of range has no name");
    check(textcat_IsLanguageEnabled(h, count) == TC_FALSE,
          "a language out of range is not enabled");
    check(textcat_SetLanguageEnabled(h, count, TC_TRUE) == -1,
          "a language out of range cannot be enabled");

    if (english < count)
    {
        check(classifies_as_english(h, text), "English text is English");

        check(textcat_SetLanguageEnabled(h, english, 2) == -2,
              "only TC_TRUE and TC_FALSE are accepted");
        check(textcat_IsLanguageEnabled(h, english) == TC_TRUE,
              "a rejected value leaves the language enabled");

        check(textcat_SetLanguageEnabled(h, english, TC_FALSE) == 0,
              "English can be disabled");
        check(textcat_IsLanguageEnabled(h, english) == TC_FALSE,
              "English reads back as disabled");
        check(!classifies_as_english(h, text),
              "English text is not English with English disabled");

        check(textcat_SetLanguageEnabled(h, english, TC_TRUE) == 0,
              "English can be enabled again");
        check(classifies_as_english(h, text),
              "English text is English again");
    }

    textcat_Done(h);

    return failures ? 1 : 0;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
