/**
* @file openvpn-ex.h
 * @brief Header file for OpenVPN C++ extended functionalities.
 * @details This file provides declarations for extended features and utilities related to the OpenVPN functionality,
 * such as enhanced connection management, and utility functions for runtime operations.
 * @author UxGood <uxgood.org@gmail.com>
 * @date 2025-06-08
 */

#ifndef OPENVPN_EX_H
#define OPENVPN_EX_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef DLL_SOUI_COM
    void InitStatusPage(connection_t *c);
    void ReleaseStatusPage(connection_t *c);
    void ShowStatusPage(connection_t *c, BOOL bShow);
    void InitUserAuthDialog(auth_param_t *param, UINT dialogId);
    void InitGenericPassDialog(auth_param_t *param, UINT dialogId);
    void InitPrivKeyPassDialog(connection_t *c, UINT dialogId);
    void InitProxyAuthDialog(connection_t *c, UINT dialogId);
    void SetWindowHide(HWND hwnd, BOOL bHide);
    void SetAutoCloseText(HWND hwnd, LPCTSTR pszText);
#else
#define InitStatusPage(c)
#define ReleaseStatusPage(c)
#define ShowStatusPage(c, b)
#define InitUserAuthDialog(p, d)
#define InitGenericPassDialog(p, d)
#define InitPrivKeyPassDialog(c, d)
#define InitProxyAuthDialog(c, d)
#define SetWindowHide(h, b)
#define SetAutoCloseText(h, t)
#endif
#ifdef __cplusplus
    class TaskSingleton {
    public:
        static TaskSingleton *getInstance();
    public:
        void InitStatusPage(connection_t *c);
        void ReleaseStatusPage(connection_t *c);
        void ShowStatusPage(connection_t *c, BOOL bShow);
        int GetStatusPageIndex(connection_t *c, STabCtrlEx *pTab = NULL);
        STabPage *GetStatusPage(connection_t *c, STabCtrlEx *pTab = NULL);
        STabPage *GetStatusPage(SWindow *pWnd);
        SWindow *GetStatusWindow(connection_t *c, STabPage *pPage = NULL);
        void InitUserAuthDialog(auth_param_t *param, UINT dialogId);
        void InitGenericPassDialog(auth_param_t *param, UINT dialogId);
        void InitPrivKeyPassDialog(connection_t *c, UINT dialogId);
        void InitProxyAuthDialog(connection_t *c, UINT dialogId);
        void SetAutoCloseText(HWND hwnd, LPCTSTR pszText);
    public:
        BOOL OnCheckBoxClick(EventCmd *pEvt);
        BOOL OnButtonClick(EventCmd *pEvt);
        BOOL OnMouseClick(EventMouseClick *pEvt);
    private:
        TaskSingleton() {};
        TaskSingleton(TaskSingleton&) = delete;
    };
#endif

#ifdef __cplusplus
}
#endif

#endif //OPENVPN_EX_H
