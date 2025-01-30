#ifndef OPENVPNEX_H
#define OPENVPNEX_H

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

void ImportConfigFileFromDisk();
void ShowSettingsDialog();
void OnNotifyTray(LPARAM lParam);
void LoadAutoStartConnections(HWND hwnd);
extern options_t o;

int InitStatusPage(connection_t* c);
int ReleaseStatusPage(connection_t* c);
void InitManagementEx();
void OnWriteStatusLog(connection_t *c, LPCWSTR prefix, LPCWSTR msg);
#ifdef __cplusplus
}
#endif

#endif //OPENVPNEX_H
