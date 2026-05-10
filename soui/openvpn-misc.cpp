/**
 * @file openvpn-misc.c
 * @brief Implements misc C functionalities.
 * @details This file contains the implements of various misc C functions and utilities.
 * @author UxGood <uxgood.org@gmail.com>
 * @date 2026-05-10
 */

#include <stdlib.h>
#include <stdarg.h>
#include <locale.h>

#include "openvpn-misc.h"

/**
 * @brief Prints a formatted debug message to the debug output.
 *
 * This function formats a message using the provided format string and arguments,
 * and outputs it to the debug output stream. The message is encoded in UTF-8.
 *
 * @param fmt The format string.
 * @param ... The arguments for the format string.
 */

void DbgPrintf(const TCHAR *fmt, ...)
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