/**
 * @file utils.h
 * @brief Utility functions for the OpenVPN GUI application.
 * @author UxGood <uxgood.org@gmail.com>
 * @date 2025-03-05
 */

#ifndef UTILS_H
#define UTILS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <stdarg.h>
#include <locale.h>
#include <debugapi.h>

/**
 * @brief Prints a formatted debug message to the debug output.
 *
 * This function formats a message using the provided format string and arguments,
 * and outputs it to the debug output stream. The message is encoded in UTF-8.
 *
 * @param fmt The format string.
 * @param ... The arguments for the format string.
 */
static __inline void DbgPrintf(const TCHAR *fmt, ...)
{
    TCHAR buf[4096] = { 0 };
    va_list ap;
    va_start(ap, fmt);
    _locale_t locale = _create_locale(LC_ALL, ".utf8");
    _vstprintf_s_l(buf, _countof(buf), fmt, locale, ap);
    _free_locale(locale);
    va_end(ap);
    OutputDebugString(buf);
}

#ifdef __cplusplus
}
#endif

#endif //UTILS_H