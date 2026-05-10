/**
 * @file SPageMgr.h
 * @brief Header file for the SPageMgr class, which provides functionality
 *        for managing tab pages and handling page interactions.
 * @details This file contains the declarations of page management functions.
 *          It provides methods for initializing, releasing, and showing status pages.
 * @author UxGood <uxgood.org@gmail.com>
 * @date 2026-05-10
 */
#ifndef SOUI_SPAGEMGR_H
#define SOUI_SPAGEMGR_H

#include <windows.h>
#include <tchar.h>

#include "openvpn-export.h"

#include "STabCtrlEx.h"

class SPageMgr
{
public:
    static SPageMgr &getSingleton (STabCtrlEx *pTab = NULL);
    static SPageMgr *getSingletonPtr (STabCtrlEx *pTab = NULL);
public:
    void ShowPage(int nIndex);
    void ShowPage(LPCTSTR pszName, BOOL bTitle = TRUE);

    void InitStatusPage(connection_t *c);
    void ReleaseStatusPage(connection_t *c);
    void ShowStatusPage(connection_t *c, BOOL bShow);
    int GetStatusPageIndex(connection_t *c);
    STabPage *GetStatusPage(connection_t *c);
    STabPage *GetStatusPage(SWindow *pWnd);
    SWindow *GetStatusWindow(connection_t *c, STabPage *pPage = NULL);
    void InitUserAuthDialog(auth_param_t *param, UINT dialogId);
    void InitGenericPassDialog(auth_param_t *param, UINT dialogId);
    void InitPrivKeyPassDialog(connection_t *c, UINT dialogId);
    void InitProxyAuthDialog(connection_t *c, UINT dialogId);
    void SetWarningText(connection_t* c, LPCTSTR pszText);
    void SetWarningColor(connection_t* c, COLORREF clr);
    void CheckEditEmpty(connection_t *c);
    BOOL ChangePasswordVisibility(SButton *pBtn);
    void HandleMessage(mgmt_rtmsg_type msg_type, connection_t* c, char* msg, BOOL bPre);
    void WriteLogLine(connection_t *c, char *msg);
    void WriteStatusLog(connection_t *c, LPCWSTR prefix, LPCWSTR msg);
public:
    BOOL OnCheckBoxClick(EventCmd *pEvt);
    BOOL OnButtonClick(EventCmd *pEvt);
    BOOL OnKeyDown(EventKeyDown *pEvt);
protected:
    void SetImageState(connection_t *c);
private:
    SPageMgr() = default;
    SPageMgr(STabCtrlEx *pTab);
    SPageMgr(SPageMgr&) = delete;

private:
    STabCtrlEx *m_pTab;
};
#endif  // SOUI_SPAGEMGR_H
