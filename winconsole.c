#define WINCONSOLE_NO_DEF
#include "winconsole.h"

#include <windows.h>
#include <io.h>

/*

    To correctly make console output on Windows, we need to check if the
    output stream we are writing to is a console. If it is, we use
    WriteConsoleW() to directly write to the console. Otherwise we use the
    fprintf() function. This dance makes us correctly output whatever codepage
    the console is in (we assume that our internal strings and dictionary are
    UTF-8).

*/

static int is_console(HANDLE h)
{
    DWORD mode;

    if (h == INVALID_HANDLE_VALUE)
        return 0;

    return !!(GetConsoleMode(h, &mode));
}

int winconsole_vfprintf(FILE *fp, const char *fmt, va_list ap)
{
    wchar_t *wbuf;
    va_list aq;
    DWORD nwritten;
    char *buf;
    int bufsz, wbufsz;
    HANDLE h = (HANDLE) _get_osfhandle(_fileno(fp));

    /* WriteConsoleW works only with the console */
    if (!is_console(h))
        return vfprintf(fp, fmt, ap);

    /* get the length of the buffer */
    va_copy(aq, ap);
    bufsz = vsnprintf(NULL, 0, fmt, aq);
    if (bufsz < 0)
        return bufsz;
    va_end(aq);

    if (bufsz == 0)
        return 0;

    buf = malloc(bufsz+1);
    if (!buf)
        return -1;

    /* now really print to the buffer */
    bufsz = vsnprintf(buf, bufsz+1, fmt, ap);
    if (bufsz < 0)
        return bufsz;

    /* get the length of the wide buf */
    wbufsz = MultiByteToWideChar(CP_UTF8, 0, buf, bufsz+1, NULL, 0);
    if (wbufsz == 0xFFFD || wbufsz == 0)
    {
        free(buf);
        return -1;
    }

    wbuf = malloc(sizeof(wchar_t) * wbufsz);
    if (!wbuf)
    {
        free(buf);
        return -1;
    }

    MultiByteToWideChar(CP_UTF8, 0, buf, bufsz+1, wbuf, wbufsz);

    if (!WriteConsoleW(h, wbuf, wbufsz-1, &nwritten, NULL))
        return -1;

    return nwritten;
}

int winconsole_fprintf(FILE *fp, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    int ret = winconsole_vfprintf(fp, fmt, ap);
    va_end(ap);
    return ret;
}

int winconsole_vprintf(const char *fmt, va_list ap)
{
    return winconsole_vfprintf(stdout, fmt, ap);
}

int winconsole_printf(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    int ret = winconsole_vprintf(fmt, ap);
    va_end(ap);
    return ret;
}
