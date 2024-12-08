#include "stdafx.h"
#include <windows.h>
#include <windowsx.h>
#include <shlwapi.h>
#include <prsht.h>

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <tchar.h>
#include "MainDlg.h"

#include "DlgInit.h"
#include <SouiFactory.h>
#include <helper/SFunctor.hpp>

extern "C" {
#include "registry.h"
#define GUI_REGKEY_HKCU _T("Software\\OpenVPN-GUI")
static mgmt_msg_func rtmsg_handler[mgmt_rtmsg_type_max];
}

#ifdef _DEBUG
#define SYS_NAMED_RESOURCE _T("soui-sys-resourced.dll")
#else
#define SYS_NAMED_RESOURCE _T("soui-sys-resource.dll")
#endif

/*  Declare Windows procedure  */

BOOL bCan3DView = FALSE;
CMainDlg *pDlgMain = nullptr;
SComMgr2 *pComMgr = nullptr;

void DlgShowWindow(bool bShow)
{
    if (pDlgMain != nullptr)
    {
        pDlgMain->ShowWindow(bShow?SW_SHOW:SW_HIDE);
    }
}

const TCHAR *GetImgDecoder()
{
    DWORD len = MAX_NAME;
    TCHAR buf[MAX_NAME];
    LSTATUS status = RegGetValueW(HKEY_CURRENT_USER, GUI_REGKEY_HKCU, L"decoder",
        RRF_RT_REG_SZ, nullptr, buf, &len);
    if (status != ERROR_SUCCESS || len == 0)
    {
        return _T("imgdecoder-png");
    }
    if (_tcsicmp(buf, _T("gdip")) == 0)
    {
        return _T("imgdecoder-gdip");
    }
    if (_tcsicmp(buf, _T("stb")) == 0)
    {
        return _T("imgdecoder-stb");
    }
    else if (_tcsicmp(buf, _T("wic")) == 0)
    {
        return _T("imgdecoder-wic");
    }
    if (_tcsicmp(buf, _T("png")) == 0)
    {
        return _T("imgdecoder-png");
    }
    return _T("imgdecoder-png");
}

void DlgGetRenderFactory(IObjRef** ref)
{
    DWORD len = MAX_NAME;
    TCHAR buf[MAX_NAME];

    LSTATUS status = RegGetValueW(HKEY_CURRENT_USER, GUI_REGKEY_HKCU, L"render",
        RRF_RT_REG_SZ, nullptr, buf, &len);
    if (status != ERROR_SUCCESS || len == 0)
    {
        pComMgr->CreateRender_GDI(ref);
    }
    else if (_tcsicmp(buf, _T("gdi")) == 0)
    {
        pComMgr->CreateRender_GDI(ref);
    }
    else if (_tcsicmp(buf, _T("d2d")) == 0)
    {
        pComMgr->CreateRender_D2D(ref);
    }
    else if (_tcsicmp(buf, _T("skia")) == 0)
    {
        bCan3DView = TRUE;
        pComMgr->CreateRender_Skia(ref);
    }
    else
    {
        pComMgr->CreateRender_GDI(ref);
    }
}

IResProvider *DlgLoadResource(SouiFactory *souiFac, HINSTANCE hInstance)
{
    DWORD len = MAX_NAME;
    TCHAR buf[MAX_NAME];
    IResProvider* pResProvider;
    LSTATUS status = RegGetValueW(HKEY_CURRENT_USER, GUI_REGKEY_HKCU, L"uires",
        RRF_RT_REG_SZ, nullptr, buf, &len);
    if (status == ERROR_SUCCESS && len > 0 && PathFileExists(buf))
    {
        pResProvider = souiFac->CreateResProvider(RES_FILE);
        if (pResProvider->Init(reinterpret_cast<LPARAM>(buf), 0))
        {
            return pResProvider;
        }
    }
    if (PathFileExists(_T("uires")))
    {
        pResProvider = souiFac->CreateResProvider(RES_FILE);
        if (pResProvider->Init(reinterpret_cast<LPARAM>(L"uires"), 0))
        {
            return pResProvider;
        }
    }
    pResProvider = souiFac->CreateResProvider(RES_PE);
    pResProvider->Init(reinterpret_cast<WPARAM>(hInstance), 0);
    return pResProvider;
}
DWORD WINAPI DlgInitWindow(HWND hWnd)
{
    DWORD nRet = 0;
    SouiFactory souiFac;
    CAutoRefPtr<IRenderFactory> pRenderFactory;
    CAutoRefPtr<IImgDecoderFactory> pImgDecoderFactory;
    HINSTANCE hInstance = GetModuleHandle(nullptr);
    if (pComMgr == nullptr)
    {
        //HRESULT hRes = OleInitialize(NULL);
        CAutoRefPtr<IResProvider> pResProvider;
        pComMgr = new SComMgr2(GetImgDecoder());

        DlgGetRenderFactory(reinterpret_cast<IObjRef**>(&pRenderFactory));
        pComMgr->CreateImgDecoder(reinterpret_cast<IObjRef**>(&pImgDecoderFactory));
        pRenderFactory->SetImgDecoderFactory(pImgDecoderFactory);

        auto* theApp = new SApplication(pRenderFactory, hInstance, _T(PACKAGE_NAME));
        theApp->RegisterWindowClass<STurn3dView>();
        theApp->RegisterSkinClass<SSkinVScrollbar>();

        HMODULE hSysResource = LoadLibrary(SYS_NAMED_RESOURCE);
        if (hSysResource)
        {
            CAutoRefPtr<IResProvider> sysSesProvider;
            sysSesProvider.Attach(souiFac.CreateResProvider(RES_PE));
            sysSesProvider->Init(reinterpret_cast<WPARAM>(hSysResource), 0);
            theApp->LoadSystemNamedResource(sysSesProvider);
        }

        pResProvider.Attach(DlgLoadResource(&souiFac, hInstance));

        theApp->AddResProvider(pResProvider);

        // BLOCK: Run application
        {
            if (pDlgMain == nullptr)
            {
                pDlgMain = new CMainDlg();
                pDlgMain->m_hWnd2 = hWnd;
                pDlgMain->Create(GetActiveWindow(), 0, 0, 300, 600);
                pDlgMain->SendMessage(WM_INITDIALOG);
                pDlgMain->CenterWindow(pDlgMain->m_hWnd);
                pDlgMain->ShowWindow(SW_SHOW);
                pDlgMain->FindChildByName(_T("turn3d"))->EnableWindow(bCan3DView);
                DlgInitManagement();
                nRet = theApp->Run(pDlgMain->m_hWnd);
            }
            else
            {
                pDlgMain->ShowWindow(SW_SHOW);
            }
        }
    }
    else
    {
        pDlgMain->ShowWindow(SW_SHOW);

    }

    return nRet;
}

void DlgShowPage(LPCTSTR pszName, BOOL bTitle)
{
    pDlgMain->ShowPage(pszName, bTitle);
}

void DlgInitManagement()
{
    mgmt_rtmsg_handler handler[] = {
    { ready_,    DlgOnReady },
    { hold_,     DlgOnHold },
    { log_,      DlgOnLogLine },
    { state_,    DlgOnStateChange },
    { password_, DlgOnPassword },
    { proxy_,    DlgOnProxy },
    { stop_,     DlgOnStop },
    { needok_,   DlgOnNeedOk },
    { needstr_,  DlgOnNeedStr },
    { echo_,     DlgOnEcho },
    { bytecount_, DlgOnByteCount },
    { infomsg_,  DlgOnInfoMsg },
    { timeout_,  DlgOnTimeout },
    { mgmt_rtmsg_type_max, nullptr }
    };
    ReInitManagement(handler, rtmsg_handler);
}

void DlgOnReady(connection_t* c, char* msg)
{
    WCHAR buf[MAX_NAME];
    MultiByteToWideChar(CP_UTF8, 0, msg, MAX_NAME, buf, _countof(buf) - 1);
    DbgPrintf(_T("%s(%d): %ls"), _T(__FUNCTION__), __LINE__, buf);
    rtmsg_handler[ready_](c, msg);
}
void DlgOnHold(connection_t* c, char* msg)
{
    WCHAR buf[MAX_NAME];
    MultiByteToWideChar(CP_UTF8, 0, msg, MAX_NAME, buf, _countof(buf) - 1);
    DbgPrintf(_T("%s(%d): %ls"), _T(__FUNCTION__), __LINE__, buf);
    rtmsg_handler[hold_](c, msg);
}
void DlgOnLogLine(connection_t* c, char* msg)
{
    WCHAR buf[MAX_NAME];
    MultiByteToWideChar(CP_UTF8, 0, msg, MAX_NAME, buf, _countof(buf) - 1);
    DbgPrintf(_T("%s(%d): %ls"), _T(__FUNCTION__), __LINE__, buf);
    rtmsg_handler[log_](c, msg);
    STaskHelper::post(pDlgMain->GetMsgLoop(), pDlgMain, &CMainDlg::OnStateChanged);
}
void DlgOnStateChange(connection_t* c, char* msg)
{
    WCHAR buf[MAX_NAME];
    MultiByteToWideChar(CP_UTF8, 0, msg, MAX_NAME, buf, _countof(buf) - 1);
    DbgPrintf(_T("%s(%d): %ls"), _T(__FUNCTION__), __LINE__, buf);
    rtmsg_handler[state_](c, msg);
    STaskHelper::post(pDlgMain->GetMsgLoop(), pDlgMain, &CMainDlg::OnStateChanged);
}
void DlgOnPassword(connection_t* c, char* msg)
{
    WCHAR buf[MAX_NAME];
    MultiByteToWideChar(CP_UTF8, 0, msg, MAX_NAME, buf, _countof(buf) - 1);
    DbgPrintf(_T("%s(%d): %ls"), _T(__FUNCTION__), __LINE__, buf);
    rtmsg_handler[password_](c, msg);
}
void DlgOnProxy(connection_t* c, char* line)
{
    WCHAR buf[MAX_NAME];
    MultiByteToWideChar(CP_UTF8, 0, line, MAX_NAME, buf, _countof(buf) - 1);
    DbgPrintf(_T("%s(%d): %ls"), _T(__FUNCTION__), __LINE__, buf);
    rtmsg_handler[proxy_](c, line);
}
void DlgOnStop(connection_t* c, char* msg)
{
    WCHAR buf[MAX_NAME];
    MultiByteToWideChar(CP_UTF8, 0, msg, MAX_NAME, buf, _countof(buf) - 1);
    DbgPrintf(_T("%s(%d): %ls"), _T(__FUNCTION__), __LINE__, buf);
    rtmsg_handler[stop_](c, msg);
}
void DlgOnNeedOk(connection_t* c, char* msg)
{
    WCHAR buf[MAX_NAME];
    MultiByteToWideChar(CP_UTF8, 0, msg, MAX_NAME, buf, _countof(buf) - 1);
    DbgPrintf(_T("%s(%d): %ls"), _T(__FUNCTION__), __LINE__, buf);
    rtmsg_handler[needok_](c, msg);
}
void DlgOnNeedStr(connection_t* c, char* msg)
{
    WCHAR buf[MAX_NAME];
    MultiByteToWideChar(CP_UTF8, 0, msg, MAX_NAME, buf, _countof(buf) - 1);
    DbgPrintf(_T("%s(%d): %ls"), _T(__FUNCTION__), __LINE__, buf);
    rtmsg_handler[needstr_](c, msg);
}
void DlgOnEcho(connection_t* c, char* msg)
{
    WCHAR buf[MAX_NAME];
    MultiByteToWideChar(CP_UTF8, 0, msg, MAX_NAME, buf, _countof(buf) - 1);
    DbgPrintf(_T("%s(%d): %ls"), _T(__FUNCTION__), __LINE__, buf);
    rtmsg_handler[echo_](c, msg);
}
void DlgOnByteCount(connection_t* c, char* msg)
{
    WCHAR buf[MAX_NAME];
    MultiByteToWideChar(CP_UTF8, 0, msg, MAX_NAME, buf, _countof(buf) - 1);
    DbgPrintf(_T("%s(%d): %ls"), _T(__FUNCTION__), __LINE__, buf);
    rtmsg_handler[bytecount_](c, msg);
    STaskHelper::post(pDlgMain->GetMsgLoop(), pDlgMain, &CMainDlg::OnStateChanged);
}
void DlgOnInfoMsg(connection_t* c, char* msg)
{
    WCHAR buf[MAX_NAME];
    MultiByteToWideChar(CP_UTF8, 0, msg, MAX_NAME, buf, _countof(buf) - 1);
    DbgPrintf(_T("%s(%d): %ls"), _T(__FUNCTION__), __LINE__, buf);
    rtmsg_handler[infomsg_](c, msg);
}
void DlgOnTimeout(connection_t* c, char* msg)
{
    WCHAR buf[MAX_NAME];
    MultiByteToWideChar(CP_UTF8, 0, msg, MAX_NAME, buf, _countof(buf) - 1);
    DbgPrintf(_T("%s(%d): %ls"), _T(__FUNCTION__), __LINE__, buf);
    rtmsg_handler[timeout_](c, msg);
}
