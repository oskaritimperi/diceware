#ifndef RANDGEN_H
#define RANDGEN_H

#include <stdlib.h>

struct randgen;

struct randgen *randgen_open(const char *device);
int randgen_close(struct randgen *randgen);
int randgen_generate(struct randgen *randgen, void *buf, size_t size);

#endif
