/**
 * @file MainEx.cpp
 * @brief Implementation of main window functions for the OpenVPN GUI application.
 * @details This source file contains the implementation for the main window functions
 *          and the message loop for the OpenVPN GUI application.
 * @author UxGood <uxgood.org@gmail.com>
 * @date 2025-03-05
 */

#include "stdafx.h"
#include <windows.h>
#include <windowsx.h>
#include <shlwapi.h>

#include <stdarg.h>
#include <stdlib.h>
#include <tchar.h>

#include <SouiFactory.h>
#include <helper/SFunctor.hpp>

#include "MainDlg.h"
#include "openvpn-ex.h"

#include "MainEx.h"

static mgmt_msg_func msg_handler[mgmt_rtmsg_type_max];

#define SYS_NAMED_RESOURCE _T("soui-sys-resource.dll")

/*  Declare Windows procedure  */

MainDlg *pMainDlg = NULL;
SComMgr2 *pComMgr = NULL;
SApplication *pApp = NULL;

VOID WINAPI MainWindowShow(BOOL bShow)
{
    if (pMainDlg != NULL)
    {
        pMainDlg->ShowWindow(bShow?SW_SHOW:SW_HIDE);
    }
}

const TCHAR *GetImgDecoder()
{
    return _T("imgdecoder-gdip");
}

void GetRenderFactory(IObjRef** ref)
{
    pComMgr->CreateRender_Skia(ref);
    //pComMgr->CreateRender_GDI(ref);
}

IResProvider *LoadResource(SouiFactory *souiFac, HINSTANCE hInstance)
{
    IResProvider* pResProvider;
    if (PathFileExists(_T("uires")))
    {
        pResProvider = souiFac->CreateResProvider(RES_FILE);
        if (pResProvider->Init((LPARAM)L"uires", 0))
        {
            return pResProvider;
        }
    }
    pResProvider = souiFac->CreateResProvider(RES_PE);
    pResProvider->Init((WPARAM)hInstance, 0);
    return pResProvider;
}

VOID WINAPI MainWindowInit(HINSTANCE hInstance)
{
    DWORD nRet = 0;
    SouiFactory souiFac;
    CAutoRefPtr<IRenderFactory> pRenderFactory;
    CAutoRefPtr<IImgDecoderFactory> pImgDecoderFactory;
    //if (pComMgr == NULL)
    {
        HRESULT hRes = OleInitialize(NULL);
        SASSERT(SUCCEEDED(hRes));

        pComMgr = new SComMgr2(GetImgDecoder());

        GetRenderFactory((IObjRef**)&pRenderFactory);
        pComMgr->CreateImgDecoder((IObjRef**)&pImgDecoderFactory);
        pRenderFactory->SetImgDecoderFactory(pImgDecoderFactory);
        pImgDecoderFactory = NULL;
        pApp = new SApplication(pRenderFactory, hInstance, _T(PACKAGE_NAME));
        pRenderFactory = NULL;
        pApp->RegisterWindowClass<STurn3dView>();
        pApp->RegisterWindowClass<SGifPlayer>();
        pApp->RegisterWindowClass<STabCtrlEx>();
#if SOUI_VER1 == 4
        pApp->RegisterSkinClass<SSkinGif>();
        SSkinGif::Gdiplus_Startup();
#else
        pApp->RegisterSkinClass<SSkinAni>();
#endif

        HMODULE hSysResource = LoadLibrary(SYS_NAMED_RESOURCE);
        if (hSysResource)
        {
            CAutoRefPtr<IResProvider> pResProvider;
            pResProvider.Attach(souiFac.CreateResProvider(RES_PE));
            pResProvider->Init((WPARAM)hSysResource, 0);
            pApp->LoadSystemNamedResource(pResProvider);
        }

        CAutoRefPtr<IResProvider> pResProvider;
        pResProvider.Attach(LoadResource(&souiFac, hInstance));

        pApp->AddResProvider(pResProvider);

        //if (pMainDlg == NULL)
        {
            pMainDlg = new MainDlg();
            pMainDlg->Create(GetActiveWindow(), 0, 0, 300, 600);
            pMainDlg->SendMessage(WM_INITDIALOG);
            pMainDlg->CenterWindow(pMainDlg->m_hWnd);
            pMainDlg->ShowWindow(SW_SHOWNORMAL);
        }
    }
}

DWORD WINAPI RunMessageLoop()
{
    return pApp->Run(pMainDlg->m_hWnd);
}

VOID WINAPI MainWindowRelease()
{
    pApp->UnregisterWindowClass<SGifPlayer>();
#if SOUI_VER1 == 4
    SSkinGif::Gdiplus_Shutdown();
#endif
    delete pApp;
    delete pComMgr;
    OleUninitialize();
}

static void OnReady_(connection_t* c, char* msg)
{
    DbgPrintf(_T("%s(%d): %hs"), _T(__FUNCTION__), __LINE__, msg);
    msg_handler[ready_](c, msg);
}

static void OnHold_(connection_t* c, char* msg)
{
    DbgPrintf(_T("%s(%d): %hs"), _T(__FUNCTION__), __LINE__, msg);
    msg_handler[hold_](c, msg);
}

static void OnLogLine_(connection_t* c, char* msg)
{
    DbgPrintf(_T("%s(%d): %hs"), _T(__FUNCTION__), __LINE__, msg);
    msg_handler[log_](c, msg);
}

static void OnStateChange_(connection_t* c, char* msg)
{
    DbgPrintf(_T("%s(%d): %hs"), _T(__FUNCTION__), __LINE__, msg);
    msg_handler[state_](c, msg);
}

static void OnPassword_(connection_t* c, char* msg)
{
    DbgPrintf(_T("%s(%d): %hs"), _T(__FUNCTION__), __LINE__, msg);
    msg_handler[password_](c, msg);
}

static void OnProxy_(connection_t* c, char* msg)
{
    DbgPrintf(_T("%s(%d): %hs"), _T(__FUNCTION__), __LINE__, msg);
    msg_handler[proxy_](c, msg);
}

static void OnStop_(connection_t* c, char* msg)
{
    DbgPrintf(_T("%s(%d): %hs"), _T(__FUNCTION__), __LINE__, msg);
    msg_handler[stop_](c, msg);
}

static void OnNeedOk_(connection_t* c, char* msg)
{
    DbgPrintf(_T("%s(%d): %hs"), _T(__FUNCTION__), __LINE__, msg);
    msg_handler[needok_](c, msg);
}

static void OnNeedStr_(connection_t* c, char* msg)
{
    DbgPrintf(_T("%s(%d): %hs"), _T(__FUNCTION__), __LINE__, msg);
    msg_handler[needstr_](c, msg);
}

static void OnEcho_(connection_t* c, char* msg)
{
    DbgPrintf(_T("%s(%d): %hs"), _T(__FUNCTION__), __LINE__, msg);
    msg_handler[echo_](c, msg);
}

static void OnByteCount_(connection_t* c, char* msg)
{
    DbgPrintf(_T("%s(%d): %hs"), _T(__FUNCTION__), __LINE__, msg);
    msg_handler[bytecount_](c, msg);
}

static void OnInfoMsg_(connection_t* c, char* msg)
{
    DbgPrintf(_T("%s(%d): %hs"), _T(__FUNCTION__), __LINE__, msg);
    msg_handler[infomsg_](c, msg);
}

static void OnTimeout_(connection_t* c, char* msg)
{
    DbgPrintf(_T("%s(%d): %hs"), _T(__FUNCTION__), __LINE__, msg);
    msg_handler[timeout_](c, msg);
}

void WINAPI ReInitManagement()
{
    mgmt_rtmsg_handler handler[] = {
        { ready_,    OnReady_ },      { hold_,     OnHold_ },
        { log_,      OnLogLine_ },    { state_,    OnStateChange_ },
        { password_, OnPassword_ },   { proxy_,    OnProxy_ },
        { stop_,     OnStop_ },       { needok_,   OnNeedOk_ },
        { needstr_,  OnNeedStr_ },    { echo_,     OnEcho_ },
        { bytecount_, OnByteCount_ }, { infomsg_,  OnInfoMsg_ },
        { timeout_,  OnTimeout_ },    { mgmt_rtmsg_type_max, NULL }
    };
    memcpy((void *)msg_handler, rtmsg_handler, sizeof(rtmsg_handler));
    InitManagement(handler);
}