#include "randgen.h"
#include "htable.h"

#include <assert.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

/*
    Generates a random id of length `size-1` consisting of characters in the set
    [1-6] and places it in the buffer pointed to by `id`. The buffer is NULL
    terminated.
*/
int get_random_id(struct randgen *randgen, char *id, size_t size)
{
    assert(randgen != NULL);
    assert(id != NULL);
    assert(size > 0);

    if (randgen_generate(randgen, id, size-1) == -1)
        return -1;

    id[--size] = '\0';

    while (size > 0)
    {
        unsigned char q = ((unsigned char *) id)[size-1];
        id[--size] = '1' + (q % 6);
    }

    return 0;
}

/*
    Makes a copy of the supplied string. If `len` is < 0, strlen() is used to
    calculate the string length. The resulting string is always NULL terminated.
*/
char *my_strdup(const char *s, int len)
{
    char *result;
    assert(s != NULL);
    if (len < 0)
        len = strlen(s);
    result = (char *) malloc(sizeof(char) * (len+1));
    memcpy(result, s, len);
    result[len] = '\0';
    return result;
}

/*
    Parse a line from a diceware dictionary file. An acceptable line is of the
    form:

        11111 aaabbbccc

    where 11111 is the word id in decimal digits and aaabbbccc is a word
    stopping at first whitespace.

    Returns a pointer to a htable_entry on success, NULL otherwise. Calls
    exit() if memory could not be allocated.
*/
struct htable_entry *parse_line(const char *line)
{
    struct htable_entry *entry;
    char id[32];
    char word[256];

    assert(line != NULL);

    if (sscanf(line, "%31[1-6] %255s", id, word) != 2)
    {
        return NULL;
    }

    entry = (struct htable_entry *) malloc(sizeof(*entry));
    if (!entry)
    {
        fprintf(stderr, "%s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    entry->id = my_strdup(id, -1);
    entry->word = my_strdup(word, -1);

    return entry;
}

/*
    Loads a diceware dictionary file. Returns 0 on success and -1 on error.
*/
int load_dictionary(const char *filename, struct htable *dict)
{
    FILE *fp;
    int rc = -1;
    struct htable_entry *entry;
    char line[256];

    if ((fp = fopen(filename, "r")) == NULL)
    {
        fprintf(stderr, "could not open %s: %s\n", filename, strerror(errno));
        goto cleanup;
    }

    while (1)
    {
        if (!fgets(line, sizeof(line), fp))
            break;

        if ((entry = parse_line(line)) == NULL)
            continue;

        htable_put(dict, entry);
    }

    rc = 0;

cleanup:

    if (fp)
        fclose(fp);

    return rc;
}

/*
    Returns a random word from a dictionary.
*/
char *get_random_word(struct randgen *randgen, struct htable *dict)
{
    struct htable_entry *entry;
    char id[6];

    assert(randgen != NULL);
    assert(dict != NULL);

    if (get_random_id(randgen, id, sizeof(id)) == -1)
    {
        fprintf(stderr, "%s\n", "get_random_id failed");
        return NULL;
    }

    entry = htable_get(dict, id);

    if (!entry)
    {
        fprintf(stderr, "no such word: %s\n", id);
        return NULL;
    }

    return entry->word;
}

void usage(const char *prog)
{
    static const char *usagestr =
        "%s [-d DICTIONARY] [-n COUNT]"
#ifndef _WIN32
        " [-r DEVICE]"
#endif
        "\n\n"
        "\t-d use DICTIONARY as the dictionary file\n"
        "\t-n output COUNT words\n"
#ifndef _WIN32
        "\t-r read random bytes from DEVICE\n"
#endif
        "\n";

    fprintf(stderr, usagestr, prog);

    exit(EXIT_FAILURE);
}

int main(int argc, char **argv)
{
    const char *dictfilename = "diceware.txt";
    const char *randomdevice = NULL;

    int words = 6;
    int i = 0;
    struct htable dict;
    struct randgen *randgen = NULL;
    int rc = 0;

    memset(&dict, 0, sizeof(dict));

    for (i = 1; i < argc; ++i)
    {
        if (!strcmp(argv[i], "-d") && i+1 < argc)
        {
            dictfilename = argv[++i];
        }
        else if (!strcmp(argv[i], "-n") && i+1 < argc)
        {
            words = atoi(argv[++i]);
        }
#ifndef _WIN32
        else if (!strcmp(argv[i], "-r") && i+1 < argc)
        {
            randomdevice = argv[++i];
        }
#endif
        else if (!strcmp(argv[i], "-h"))
        {
            usage(argv[0]);
        }
        else
        {
            usage(argv[0]);
        }
    }

    if ((randgen = randgen_open(randomdevice)) == NULL)
        goto error;

    htable_init(&dict, 11000);

    if (load_dictionary(dictfilename, &dict) == -1)
        goto error;

    for (i = 0; i < words; ++i)
    {
        char *word = get_random_word(randgen, &dict);
        if (!word)
        {
            goto error;
        }
        printf("%s%s", i ? " " : "", word);
    }

    printf("\n");

    goto finally;

error:

    rc = 1;

finally:

    randgen_close(randgen);

    htable_deinit(&dict);

    return rc;
}
