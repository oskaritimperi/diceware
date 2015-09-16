#include "randgen.h"

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

struct randgen *randgen_open(const char *device)
{
    const char *filename = "/dev/urandom";
    FILE *fp;

    if (device)
        filename = device;

    if ((fp = fopen(filename, "r")) == NULL)
    {
        fprintf(stderr, "failed to open random generator (%s): %s\n", filename, strerror(errno));
        return NULL;
    }

    return (struct randgen *) fp;
}

int randgen_close(struct randgen *randgen)
{
    assert(randgen != NULL);
    fclose((FILE *) randgen);
    return 0;
}

int randgen_generate(struct randgen *randgen, void *buf, size_t size)
{
    assert(randgen != NULL);
    assert(buf != NULL);
    assert(size > 0);

    if (fread(buf, 1, size, (FILE *) randgen) != size)
        return -1;

    return 0;
}
