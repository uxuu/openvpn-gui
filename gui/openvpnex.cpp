#include "stdafx.h"
#include "openvpnex.h"
#include "MainDlg.h"
#include "MgmtMisc.h"

#include <helper/SFunctor.hpp>

static mgmt_msg_func msg_handler[mgmt_rtmsg_type_max];
extern CMainDlg *pDlgMain;
extern MgmtMisc *pMgmtMisc;

static BOOL _ShowWindow(HWND hWnd, int nCmdShow)
{
    connection_t *c = static_cast<connection_t*>(GetProp(hWnd, cfgProp));
    if (c && hWnd == c->hwndStatus && nCmdShow == SW_SHOW)
    {
        return TRUE;
    }
    return ::ShowWindow(hWnd, nCmdShow);
}

static void _InitStatusPage(connection_t* c)
{
    STaskHelper::sendTask(pDlgMain, pMgmtMisc, &MgmtMisc::InitStatusPage, c);
}

static void _ReleaseStatusPage(connection_t* c)
{
    STaskHelper::sendTask(pDlgMain, pMgmtMisc, &MgmtMisc::ReleaseStatusPage, c);
}

static void _ShowPage(connection_t* c, BOOL bShow)
{
    STaskHelper::sendTask(pDlgMain, pMgmtMisc, &MgmtMisc::ShowPage, c, bShow);
}

static void _OnReady(connection_t* c, char* msg)
{
    DbgPrintf(_T("%s(%d): %hs"), _T(__FUNCTION__), __LINE__, msg);
    msg_handler[ready_](c, msg);
    STaskHelper::sendTask(pDlgMain, pMgmtMisc, &MgmtMisc::OnReady, c, msg);
}

static void _OnHold(connection_t* c, char* msg)
{
    DbgPrintf(_T("%s(%d): %hs"), _T(__FUNCTION__), __LINE__, msg);
    msg_handler[hold_](c, msg);
    STaskHelper::sendTask(pDlgMain, pMgmtMisc, &MgmtMisc::OnHold, c, msg);
}

static void _OnLogLine(connection_t* c, char* msg)
{
    DbgPrintf(_T("%s(%d): %hs"), _T(__FUNCTION__), __LINE__, msg);
    msg_handler[log_](c, msg);
    STaskHelper::sendTask(pDlgMain, pMgmtMisc, &MgmtMisc::OnLogLine, c, msg);
}

static void _OnStateChange(connection_t* c, char* msg)
{
    DbgPrintf(_T("%s(%d): %hs"), _T(__FUNCTION__), __LINE__, msg);
    STaskHelper::sendTask(pDlgMain, pMgmtMisc, &MgmtMisc::OnStateChange, c, msg);
    msg_handler[state_](c, msg);
}

static void _OnPassword(connection_t* c, char* msg)
{
    DbgPrintf(_T("%s(%d): %hs"), _T(__FUNCTION__), __LINE__, msg);
    //msg_handler[password_](c, msg);
    STaskHelper::sendTask(pDlgMain, pMgmtMisc, &MgmtMisc::OnPassword, c, msg);
}

static void _OnProxy(connection_t* c, char* msg)
{
    DbgPrintf(_T("%s(%d): %hs"), _T(__FUNCTION__), __LINE__, msg);
    msg_handler[proxy_](c, msg);
    STaskHelper::sendTask(pDlgMain, pMgmtMisc, &MgmtMisc::OnProxy, c, msg);
}

static void _OnStop(connection_t* c, char* msg)
{
    DbgPrintf(_T("%s(%d): %hs"), _T(__FUNCTION__), __LINE__, msg);
    msg_handler[stop_](c, msg);
    STaskHelper::sendTask(pDlgMain, pMgmtMisc, &MgmtMisc::OnStop, c, msg);
}

static void _OnNeedOk(connection_t* c, char* msg)
{
    DbgPrintf(_T("%s(%d): %hs"), _T(__FUNCTION__), __LINE__, msg);
    msg_handler[needok_](c, msg);
    STaskHelper::sendTask(pDlgMain, pMgmtMisc, &MgmtMisc::OnNeedOk, c, msg);
}

static void _OnNeedStr(connection_t* c, char* msg)
{
    DbgPrintf(_T("%s(%d): %hs"), _T(__FUNCTION__), __LINE__, msg);
    msg_handler[needstr_](c, msg);
    STaskHelper::sendTask(pDlgMain, pMgmtMisc, &MgmtMisc::OnNeedStr, c, msg);
}

static void _OnEcho(connection_t* c, char* msg)
{
    DbgPrintf(_T("%s(%d): %hs"), _T(__FUNCTION__), __LINE__, msg);
    msg_handler[echo_](c, msg);
    STaskHelper::sendTask(pDlgMain, pMgmtMisc, &MgmtMisc::OnEcho, c, msg);
}

static void _OnByteCount(connection_t* c, char* msg)
{
    DbgPrintf(_T("%s(%d): %hs"), _T(__FUNCTION__), __LINE__, msg);
    msg_handler[bytecount_](c, msg);
    STaskHelper::sendTask(pDlgMain, pMgmtMisc, &MgmtMisc::OnByteCount, c, msg);
}

static void _OnInfoMsg(connection_t* c, char* msg)
{
    DbgPrintf(_T("%s(%d): %hs"), _T(__FUNCTION__), __LINE__, msg);
    msg_handler[infomsg_](c, msg);
    STaskHelper::sendTask(pDlgMain, pMgmtMisc, &MgmtMisc::OnInfoMsg, c, msg);
}

static void _OnTimeout(connection_t* c, char* msg)
{
    DbgPrintf(_T("%s(%d): %hs"), _T(__FUNCTION__), __LINE__, msg);
    msg_handler[timeout_](c, msg);
    STaskHelper::sendTask(pDlgMain, pMgmtMisc, &MgmtMisc::OnTimeout, c, msg);
}

static void _WriteStatusLog(connection_t *c, LPCWSTR prefix, LPCWSTR msg)
{
    DbgPrintf(_T("%s(%d): %ls%ls"), _T(__FUNCTION__), __LINE__, prefix, msg);
    STaskHelper::sendTask(pDlgMain, pMgmtMisc, &MgmtMisc::WriteStatusLog, c, prefix, msg);
}

void InitManagementHook()
{
    mgmt_hook = {
        _WriteStatusLog,
        _InitStatusPage,
        _ReleaseStatusPage,
        _ShowPage,
        _ShowWindow,
    };
    mgmt_rtmsg_handler handler[] = {
        { ready_,    _OnReady },
        { hold_,     _OnHold },
        { log_,      _OnLogLine },
        { state_,    _OnStateChange },
        { password_, _OnPassword },
        { proxy_,    _OnProxy },
        { stop_,     _OnStop },
        { needok_,   _OnNeedOk },
        { needstr_,  _OnNeedStr },
        { echo_,     _OnEcho },
        { bytecount_, _OnByteCount },
        { infomsg_,  _OnInfoMsg },
        { timeout_,  _OnTimeout },
        { mgmt_rtmsg_type_max, nullptr }
    };
    ReInitManagement(handler, msg_handler);
}