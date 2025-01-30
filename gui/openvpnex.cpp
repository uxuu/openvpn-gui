#include "stdafx.h"
#include "openvpnex.h"
#include "MainDlg.h"

#include <helper/SFunctor.hpp>

extern "C" static mgmt_msg_func msg_handler[mgmt_rtmsg_type_max];
extern CMainDlg* pDlgMain;

int InitStatusPage(connection_t* c)
{
    STaskHelper::sendTask(pDlgMain, pDlgMain, &CMainDlg::OnInitStatusPage, c);
    return 0;
}

int ReleaseStatusPage(connection_t* c)
{
    TCHAR buf[MAX_NAME];
    _stprintf_s(buf, _countof(buf), _T("page_%08x"), c->id);
    STaskHelper::sendTask(pDlgMain, pDlgMain, &CMainDlg::OnReleaseStatusPage, c);
    return 0;
}

void ShowPage(LPCTSTR pszName, BOOL bTitle)
{
    pDlgMain->ShowPage(pszName, bTitle);
}

static void OnReady(connection_t* c, char* msg)
{
    WCHAR buf[MAX_NAME];
    MultiByteToWideChar(CP_UTF8, 0, msg, MAX_NAME, buf, _countof(buf) - 1);
    DbgPrintf(_T("%s(%d): %ls"), _T(__FUNCTION__), __LINE__, buf);
    msg_handler[ready_](c, msg);
    STaskHelper::sendTask(pDlgMain, pDlgMain, &CMainDlg::OnStateChanged, c);
}

static void OnHold(connection_t* c, char* msg)
{
    WCHAR buf[MAX_NAME];
    MultiByteToWideChar(CP_UTF8, 0, msg, MAX_NAME, buf, _countof(buf) - 1);
    DbgPrintf(_T("%s(%d): %ls"), _T(__FUNCTION__), __LINE__, buf);
    msg_handler[hold_](c, msg);
    STaskHelper::sendTask(pDlgMain, pDlgMain, &CMainDlg::OnStateChanged, c);
}

static void OnLogLine(connection_t* c, char* msg)
{
    WCHAR buf[MAX_NAME];
    MultiByteToWideChar(CP_UTF8, 0, msg, MAX_NAME, buf, _countof(buf) - 1);
    DbgPrintf(_T("%s(%d): %ls"), _T(__FUNCTION__), __LINE__, buf);
    msg_handler[log_](c, msg);
    STaskHelper::sendTask(pDlgMain, pDlgMain, &CMainDlg::OnLogLine, c, msg);
}

static void OnStateChange(connection_t* c, char* msg)
{
    WCHAR buf[MAX_NAME];
    MultiByteToWideChar(CP_UTF8, 0, msg, MAX_NAME, buf, _countof(buf) - 1);
    DbgPrintf(_T("%s(%d): %ls"), _T(__FUNCTION__), __LINE__, buf);
    msg_handler[state_](c, msg);
    STaskHelper::sendTask(pDlgMain, pDlgMain, &CMainDlg::OnStateChanged, c);
}

static void OnPassword(connection_t* c, char* msg)
{
    WCHAR buf[MAX_NAME];
    MultiByteToWideChar(CP_UTF8, 0, msg, MAX_NAME, buf, _countof(buf) - 1);
    DbgPrintf(_T("%s(%d): %ls"), _T(__FUNCTION__), __LINE__, buf);
    msg_handler[password_](c, msg);
}

static void OnProxy(connection_t* c, char* line)
{
    WCHAR buf[MAX_NAME];
    MultiByteToWideChar(CP_UTF8, 0, line, MAX_NAME, buf, _countof(buf) - 1);
    DbgPrintf(_T("%s(%d): %ls"), _T(__FUNCTION__), __LINE__, buf);
    msg_handler[proxy_](c, line);
}

static void OnStop(connection_t* c, char* msg)
{
    WCHAR buf[MAX_NAME];
    MultiByteToWideChar(CP_UTF8, 0, msg, MAX_NAME, buf, _countof(buf) - 1);
    DbgPrintf(_T("%s(%d): %ls"), _T(__FUNCTION__), __LINE__, buf);
    msg_handler[stop_](c, msg);
    STaskHelper::sendTask(pDlgMain, pDlgMain, &CMainDlg::OnStateChanged, c);
}

static void OnNeedOk(connection_t* c, char* msg)
{
    WCHAR buf[MAX_NAME];
    MultiByteToWideChar(CP_UTF8, 0, msg, MAX_NAME, buf, _countof(buf) - 1);
    DbgPrintf(_T("%s(%d): %ls"), _T(__FUNCTION__), __LINE__, buf);
    msg_handler[needok_](c, msg);
}

static void OnNeedStr(connection_t* c, char* msg)
{
    WCHAR buf[MAX_NAME];
    MultiByteToWideChar(CP_UTF8, 0, msg, MAX_NAME, buf, _countof(buf) - 1);
    DbgPrintf(_T("%s(%d): %ls"), _T(__FUNCTION__), __LINE__, buf);
    msg_handler[needstr_](c, msg);
}

static void OnEcho(connection_t* c, char* msg)
{
    WCHAR buf[MAX_NAME];
    MultiByteToWideChar(CP_UTF8, 0, msg, MAX_NAME, buf, _countof(buf) - 1);
    DbgPrintf(_T("%s(%d): %ls"), _T(__FUNCTION__), __LINE__, buf);
    msg_handler[echo_](c, msg);
}

static void OnByteCount(connection_t* c, char* msg)
{
    WCHAR buf[MAX_NAME];
    MultiByteToWideChar(CP_UTF8, 0, msg, MAX_NAME, buf, _countof(buf) - 1);
    DbgPrintf(_T("%s(%d): %ls"), _T(__FUNCTION__), __LINE__, buf);
    msg_handler[bytecount_](c, msg);
    STaskHelper::sendTask(pDlgMain, pDlgMain, &CMainDlg::OnStateChanged, c);
}

static void OnInfoMsg(connection_t* c, char* msg)
{
    WCHAR buf[MAX_NAME];
    MultiByteToWideChar(CP_UTF8, 0, msg, MAX_NAME, buf, _countof(buf) - 1);
    DbgPrintf(_T("%s(%d): %ls"), _T(__FUNCTION__), __LINE__, buf);
    msg_handler[infomsg_](c, msg);
}

static void OnTimeout(connection_t* c, char* msg)
{
    WCHAR buf[MAX_NAME];
    MultiByteToWideChar(CP_UTF8, 0, msg, MAX_NAME, buf, _countof(buf) - 1);
    DbgPrintf(_T("%s(%d): %ls"), _T(__FUNCTION__), __LINE__, buf);
    msg_handler[timeout_](c, msg);
}

void OnWriteStatusLog(connection_t *c, LPCWSTR prefix, LPCWSTR msg)
{
    DbgPrintf(_T("%s(%d): %ls%ls"), _T(__FUNCTION__), __LINE__, prefix, msg);
    STaskHelper::sendTask(pDlgMain, pDlgMain, &CMainDlg::OnWriteStatusLog, c, prefix,  msg);
}

void InitManagementEx()
{
    mgmt_rtmsg_handler handler[] = {
        { ready_,    OnReady },
        { hold_,     OnHold },
        { log_,      OnLogLine },
        { state_,    OnStateChange },
        { password_, OnPassword },
        { proxy_,    OnProxy },
        { stop_,     OnStop },
        { needok_,   OnNeedOk },
        { needstr_,  OnNeedStr },
        { echo_,     OnEcho },
        { bytecount_, OnByteCount },
        { infomsg_,  OnInfoMsg },
        { timeout_,  OnTimeout },
        { mgmt_rtmsg_type_max, nullptr }
    };
    ReInitManagement(handler, msg_handler);
}