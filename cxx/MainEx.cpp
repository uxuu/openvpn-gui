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

/**
 * @brief Shows or hides the main window.
 * @param bShow TRUE to show the window, FALSE to hide it.
 */
VOID WINAPI MainWindowShow(BOOL bShow)
{
    if (pMainDlg != NULL)
    {
        pMainDlg->ShowWindow(bShow?SW_SHOW:SW_HIDE);
    }
}

/**
 * @brief Retrieves the image decoder name.
 * @return Returns the name of the image decoder.
 */
const TCHAR *GetImgDecoder()
{
    return _T("imgdecoder-gdip");
}

/**
 * @brief Retrieves the render factory.
 * @param ref Pointer to the render factory object.
 */
void GetRenderFactory(IObjRef** ref)
{
    pComMgr->CreateRender_Skia(ref);
    //pComMgr->CreateRender_GDI(ref);
}

/**
 * @brief Loads resources for the application.
 * @param souiFac Pointer to the SouiFactory instance.
 * @param hInstance Handle to the application instance.
 * @return Returns a pointer to the resource provider.
 */
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

/**
 * @brief Initializes the main window.
 * @param hInstance Handle to the application instance.
 */
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

/**
 * @brief Runs the message loop for the application.
 * @return Returns the exit code of the message loop.
 */
DWORD WINAPI RunMessageLoop()
{
    return pApp->Run(pMainDlg->m_hWnd);
}

/**
 * @brief Releases resources associated with the main window.
 */
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

// 定义一个模板函数来处理消息
template<mgmt_rtmsg_type msg_type>
static void HandleMessage(connection_t* c, char* msg)
{
    DbgPrintf(_T("%s(%d): %hs"), _T(__FUNCTION__), __LINE__, msg);
    msg_handler[msg_type](c, msg);
}

/**
 * @brief Reinitializes the management interface.
 */
void WINAPI ReInitManagement()
{
    mgmt_rtmsg_handler handler[] = {
        { ready_,    HandleMessage<ready_> },      { hold_,     HandleMessage<hold_> },
        { log_,      HandleMessage<log_> },        { state_,    HandleMessage<state_> },
        { password_, HandleMessage<password_> },   { proxy_,    HandleMessage<proxy_> },
        { stop_,     HandleMessage<stop_> },       { needok_,   HandleMessage<needok_> },
        { needstr_,  HandleMessage<needstr_> },    { echo_,     HandleMessage<echo_> },
        { bytecount_, HandleMessage<bytecount_> }, { infomsg_,  HandleMessage<infomsg_> },
        { timeout_,  HandleMessage<timeout_> },    { mgmt_rtmsg_type_max, NULL }
    };
    memcpy((void *)msg_handler, rtmsg_handler, sizeof(rtmsg_handler));
    InitManagement(handler);
}

