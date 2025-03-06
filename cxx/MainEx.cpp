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

#include "MainDlg.h"

#include "MainEx.h"

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
        pApp->RegisterSkinClass<SSkinVScrollbar>();

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
    delete pApp;
    delete pComMgr;
    OleUninitialize();
}