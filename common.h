#ifndef COMMON_H
#define COMMON_H

static __inline void DbgPrintf(const TCHAR *fmt, ...)
{
    TCHAR buf[1024] = { 0 };
    va_list ap;
    va_start(ap, fmt);
    _vsntprintf(buf, _countof(buf) - 1, fmt, ap);
    va_end(ap);
    OutputDebugString(buf);
}

#endif /* COMMON_H */
