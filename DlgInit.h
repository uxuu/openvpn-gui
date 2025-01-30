#ifndef DLG_INIT_H
#define DLG_INIT_H

#ifdef __cplusplus
extern "C"{
#undef MAX_NAME
#include "options.h"
#include "openvpn_config.h"
#endif

    DWORD WINAPI DlgInitWindow(HWND hWnd);
    void DlgShowWindow(bool bTrue);
    void DlgShowPage(LPCTSTR pszName, BOOL bTitle);
    void DlgInitManagement();
    int DlgInitStatusPage(connection_t* c);
    int DlgReleaseStatusPage(connection_t* c);

    void DlgOnReady(connection_t*, char*);
    void DlgOnHold(connection_t*, char*);
    void DlgOnLogLine(connection_t*, char*);
    void DlgOnStateChange(connection_t*, char*);
    void DlgOnPassword(connection_t*, char*);
    void DlgOnStop(connection_t*, char*);
    void DlgOnNeedOk(connection_t*, char*);
    void DlgOnNeedStr(connection_t*, char*);
    void DlgOnEcho(connection_t*, char*);
    void DlgOnByteCount(connection_t*, char*);
    void DlgOnInfoMsg(connection_t*, char*);
    void DlgOnTimeout(connection_t*, char*);
    void DlgOnProxy(connection_t*, char* );
    void DlgOnWriteStatusLog(connection_t *, LPCWSTR, LPCWSTR);

#ifdef __cplusplus
}
#endif /* C++ */
#endif
