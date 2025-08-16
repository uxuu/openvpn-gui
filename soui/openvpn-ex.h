/**
* @file openvpn-c.h
 * @brief Header file for OpenVPN-related C functionalities.
 * @details This file contains the declarations of various OpenVPN-related C functions and utilities.
 * @author UxGood <uxgood.org@gmail.com>
 * @date 2025-03-08
 */

#ifndef SOUI_OPENVPN_EX_H
#define SOUI_OPENVPN_EX_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef MAX_NAME
#undef MAX_NAME
#endif

#include "options.h"
#include "openvpn_config.h"
#include "openvpn.h"
#include "registry.h"
#include "as.h"
#include "viewlog.h"
#include "proxy.h"
#include "save_pass.h"
#include "misc.h"
#include "localization.h"
#include "openvpn-gui-res.h"

#define GUI_REGKEY_HKCU _T("Software\\OpenVPN-GUI")
#define USE_NESTED_CONFIG_MENU ((o.config_menu_view == CONFIG_VIEW_AUTO && o.num_configs > 25)   \
|| (o.config_menu_view == CONFIG_VIEW_NESTED))

    extern options_t o;

/**
 * @brief Imports a configuration file from disk.
 */
void ImportConfigFileFromDisk();

/**
 * @brief Displays the settings dialog.
 */
void ShowSettingsDialog();

/**
 * @brief Handles tray notifications.
 * @param lParam Additional message-specific information.
 */
void OnNotifyTray(LPARAM lParam);

/**
 * @brief Closes the application.
 * @param hwnd Handle to the application window.
 * @param ask_user Indicates whether to prompt the user before closing.
 * @return Returns TRUE if the application is closed successfully.
 */
BOOL CloseApplication(HWND hwnd, BOOL ask_user);

/**
 * @brief Simulates a button press.
 * @param hwnd Handle to the application window.
 * @param btn ID of the button to simulate.
 */
void SimulateButtonPress(HWND hwnd, UINT btn);

/**
 * @brief Initializes the management interface.
 * @param handler Array of management message handlers.
 */
void AutoCloseCancel(HWND hwnd);

/**
 * @brief Prints a formatted debug message to the debug output.
 *
 * This function formats a message using the provided format string and arguments,
 * and outputs it to the debug output stream. The message is encoded in UTF-8.
 *
 * @param fmt The format string.
 * @param ... The arguments for the format string.
 */
#include <locale.h>
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

#endif //SOUI_OPENVPN_EX_H
