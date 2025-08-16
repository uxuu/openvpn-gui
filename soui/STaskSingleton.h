#pragma once

#include "STabCtrlEx.h"

class STaskSingleton {
public:
    static STaskSingleton *getInstance();
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
    STaskSingleton() {};
    STaskSingleton(STaskSingleton&) = delete;
};