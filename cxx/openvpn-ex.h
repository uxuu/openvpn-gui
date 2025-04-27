/**
 * @file openvpn-ex.h
 * @brief Header file for OpenVPN-related functionalities.
 * @details This file contains the declarations of various OpenVPN-related functions and utilities.
 * @author UxGood <uxgood.org@gmail.com>
 * @date 2025-03-08
 */

#ifndef OPENVPN_EX_H
#define OPENVPN_EX_H

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


#define GUI_REGKEY_HKCU _T("Software\\OpenVPN-GUI")
#define USE_NESTED_CONFIG_MENU ((o.config_menu_view == CONFIG_VIEW_AUTO && o.num_configs > 25)   \
|| (o.config_menu_view == CONFIG_VIEW_NESTED))

    extern options_t o;
    extern mgmt_msg_func rtmsg_handler[mgmt_rtmsg_type_max];

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

#ifdef __cplusplus
}
#endif

#endif //OPENVPN_EX_H
