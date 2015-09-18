#ifndef WINCONSOLE_H
#define WINCONSOLE_H

#include <stdio.h>

#ifndef WINCONSOLE_NO_DEF
#define vfprintf(FILE, FMT, AP) winconsole_vfprintf((FILE), (FMT), (AP))
#define fprintf(FILE, FMT, ...) winconsole_fprintf((FILE), (FMT), ##__VA_ARGS__)
#define vprintf(FMT, AP) winconsole_vprintf((FMT), (AP))
#define printf(FMT, ...) winconsole_printf((FMT), ##__VA_ARGS__)
#endif

int winconsole_vfprintf(FILE *fp, const char *fmt, va_list ap);
int winconsole_fprintf(FILE *fp, const char *fmt, ...);

int winconsole_vprintf(const char *fmt, va_list ap);
int winconsole_printf(const char *fmt, ...);

#endif
