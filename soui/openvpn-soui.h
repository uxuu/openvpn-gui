/**
 * @file openvpn-soui.h
 * @brief Main extension functions for the OpenVPN GUI application.
 * @details This header file contains declarations for the main window functions
 *          and the message loop for the OpenVPN GUI application.
 * @author UxGood <uxgood.org@gmail.com>
 * @date 2025-03-05
 */

#ifndef SOUI_SOUI_H
#define SOUI_SOUI_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initializes the main window.
 * @param hInstance Handle to the application instance.
 */
VOID WINAPI SOUI_Init(HINSTANCE hInstance);

/**
 * @brief Releases resources associated with the main window.
 */
VOID WINAPI SOUI_Release();

/**
 * @brief Runs the message loop for the application.
 * @return Returns the exit code of the message loop.
 */
DWORD WINAPI SOUI_Run();

/**
 * @brief Shows or hides the main window.
 * @param bShow TRUE to show the window, FALSE to hide it.
 */
VOID WINAPI SOUI_ShowMainWnd(BOOL bShow);

VOID WINAPI SOUI_InitManagement(mgmt_rtmsg_handler rtmsg_handler[]);

#if defined(DLL_SOUI_COM) || defined(LIB_SOUI_COM)
void SOUI_InitStatusPage(connection_t *c);
void SOUI_ReleaseStatusPage(connection_t *c);
void SOUI_ShowStatusPage(connection_t *c, BOOL bShow);
void SOUI_InitUserAuthDialog(auth_param_t *param, UINT dialogId);
void SOUI_InitGenericPassDialog(auth_param_t *param, UINT dialogId);
void SOUI_InitPrivKeyPassDialog(connection_t *c, UINT dialogId);
void SOUI_InitProxyAuthDialog(connection_t *c, UINT dialogId);
void SOUI_SetWindowHide(HWND hwnd, BOOL bHide);
void SOUI_SetAutoCloseText(HWND hwnd, LPCTSTR pszText);
void SOUI_SetWarningText(connection_t *c, LPCTSTR pszText);
void SOUI_SetWarningColor(connection_t *c, COLORREF clr);
void SOUI_WriteLogLine(connection_t *c, char *msg);
void SOUI_WriteStatusLog(connection_t *c, LPCWSTR prefix, LPCWSTR msg);
#else
#define SOUI_InitStatusPage(c)
#define SOUI_ReleaseStatusPage(c)
#define SOUI_ShowStatusPage(c, b)
#define SOUI_InitUserAuthDialog(p, d)
#define SOUI_InitGenericPassDialog(p, d)
#define SOUI_InitPrivKeyPassDialog(c, d)
#define SOUI_InitProxyAuthDialog(c, d)
#define SOUI_SetWindowHide(h, b)
#define SOUI_SetAutoCloseText(h, t)
#define SOUI_SetWarningText(c, t)
#define SOUI_SetWarningColor(c, clr)
#define SOUI_WriteLogLine(c, m)
#define SOUI_WriteStatusLog(c, p, m)
#endif

#ifdef __cplusplus
}
#endif

#endif //SOUI_SOUI_H
