#include "randgen.h"

#include <windows.h>
#include <wincrypt.h>

#include <stdio.h>
#include <assert.h>

struct randgen *randgen_open(const char *device)
{
    HCRYPTPROV prov;

    (void) device;

    if (!CryptAcquireContextW(&prov, 0, 0, PROV_RSA_FULL,
        CRYPT_VERIFYCONTEXT | CRYPT_SILENT))
    {
        fprintf(stderr, "failed to open random generator: CryptAcquireContextW failed\n");
        return NULL;
    }

    return (struct randgen *) prov;
}

int randgen_close(struct randgen *randgen)
{
    assert(randgen != NULL);
    CryptReleaseContext((HCRYPTPROV) randgen, 0);
    return 0;
}

int randgen_generate(struct randgen *randgen, void *buf, size_t size)
{
    assert(randgen != NULL);
    assert(buf != NULL);
    assert(size > 0);

    if (!CryptGenRandom((HCRYPTPROV) randgen, size, buf))
        return -1;

    return 0;
}
