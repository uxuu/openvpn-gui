#pragma once

class MgmtMisc
{
public:
    MgmtMisc();
    ~MgmtMisc();

public:
    void OnReady(connection_t* c, char* msg);
    void OnHold(connection_t* c, char* msg);
    void OnLogLine(connection_t* c, char* msg);
    void OnStateChange(connection_t* c, char* msg);
    void OnPassword(connection_t* c, char* msg);
    void OnProxy(connection_t* c, char* msg);
    void OnStop(connection_t* c, char* msg);
    void OnNeedOk(connection_t* c, char* msg);
    void OnNeedStr(connection_t* c, char* msg);
    void OnEcho(connection_t* c, char* msg);
    void OnByteCount(connection_t* c, char* msg);
    void OnInfoMsg(connection_t* c, char* msg);
    void OnTimeout(connection_t* c, char* msg);
public:
    void WriteStatusLog(connection_t *c, LPCWSTR prefix, LPCWSTR msg);
    void InitStatusPage(connection_t* c);
    void ReleaseStatusPage(connection_t* c);
    void ShowPage(connection_t* c, BOOL bShow = TRUE);
public:
    BOOL ManagementCommandFromInput(connection_t *c, LPCSTR fmt, SWindow *pWnd, LPCTSTR name);
    BOOL ManagementCommandFromInputBase64(connection_t *c, LPCSTR fmt, SWindow *pWnd, LPCTSTR name);
    BOOL ManagementCommandFromTwoInputsBase64(connection_t *c, LPCSTR fmt, SWindow *pWnd, LPCTSTR name, LPCTSTR name2);

public:
    BOOL AuthWindow_OnConfirmClick(EventCmd* pEvt);
    BOOL AuthWindow_OnCancelClick(EventCmd* pEvt);
private:
    STabPage *GetPage(connection_t *c);
    void LoadAuthDialogStatus(SWindow *pWnd, auth_param_t *param);
};