/**
 * @file openvpn-mgmt.h
 * @brief Header file for OpenVPN management functionalities.
 * @details This file contains the declarations of OpenVPN management functions and utilities.
 * @author UxGood <uxgood.org@gmail.com>
 * @date 2026-05-10
 */
#ifndef SOUI_OPENVPN_MGMT_H
#define SOUI_OPENVPN_MGMT_H

#include <windows.h>

#include "openvpn-export.h"

#ifdef __cplusplus
extern "C" {
#endif

VOID WINAPI SOUI_InitManagement(mgmt_rtmsg_handler rtmsg_handler[]);

#ifdef __cplusplus
}
#endif

#endif  // SOUI_OPENVPN_MGMT_H
