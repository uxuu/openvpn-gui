#ifndef COMMON_H
#define COMMON_H
#include <locale.h>
static __inline void DbgPrintf(const TCHAR *fmt, ...)
{
    TCHAR buf[1024] = { 0 };
    va_list ap;
    va_start(ap, fmt);
    _locale_t locale = _create_locale(LC_ALL, ".utf8");
    _vsntprintf_l(buf, _countof(buf) - 1, fmt, locale, ap);
    _free_locale(locale);
    va_end(ap);
    OutputDebugString(buf);
}

#endif /* COMMON_H */
