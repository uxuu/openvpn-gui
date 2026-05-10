/**
 * @file openvpn-misc.h
 * @brief Header file for misc C functionalities.
 * @details This file contains the declarations of various misc C functions and utilities.
 * @author UxGood <uxgood.org@gmail.com>
 * @date 2026-05-10
 */

#ifndef SOUI_OPENVPN_MISC_H
#define SOUI_OPENVPN_MISC_H

#include <windows.h>
#include <tchar.h>

#ifdef __cplusplus
extern "C" {
#endif

void DbgPrintf(const TCHAR *fmt, ...);

#ifdef __cplusplus
}
#endif

#endif  // SOUI_OPENVPN_MISC_H
