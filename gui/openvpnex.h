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

typedef struct
{
    void (*WriteStatusLog)(connection_t *c, LPCWSTR prefix, LPCWSTR msg);
    void (*InitStatusPage)(connection_t *c);
    void (*ReleaseStatusPage)(connection_t *c);
    void (*ShowPage)(connection_t *c, BOOL bShow);
    BOOL (*ShowWindow)(HWND hWnd, int nCmdShow);
} mgmt_hook_t;

extern options_t o;
extern mgmt_hook_t mgmt_hook;

void ImportConfigFileFromDisk();
void ShowSettingsDialog();
void OnNotifyTray(LPARAM lParam);

void InitManagementHook();

#ifdef __cplusplus
}
#endif

#endif //OPENVPNEX_H
