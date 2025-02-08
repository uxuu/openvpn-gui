#include "stdafx.h"
#include <windows.h>
#include <windowsx.h>
#include <shlwapi.h>

#include <stdarg.h>
#include <stdlib.h>
#include <tchar.h>

#include "openvpnex.h"
#include "MainDlg.h"
#include "MgmtMisc.h"

#include "Init.h"

#include <SouiFactory.h>


#ifdef _DEBUG
#define SYS_NAMED_RESOURCE _T("soui-sys-resource2d.dll")
#else
#define SYS_NAMED_RESOURCE _T("soui-sys-resource2.dll")
#endif

/*  Declare Windows procedure  */

CMainDlg *pDlgMain = nullptr;
SComMgr2 *pComMgr = nullptr;
MgmtMisc *pMgmtMisc = nullptr;
SApplication *pApp = nullptr;

VOID WINAPI ShowMainWindow(BOOL bShow)
{
    if (pDlgMain != nullptr)
    {
        pDlgMain->ShowWindow(bShow?SW_SHOW:SW_HIDE);
    }
}

const TCHAR *GetImgDecoder()
{
    switch (o.imgdecoder)
    {
    case imgdecoder_gdip:
        return _T("imgdecoder-gdip");
    case imgdecoder_stb:
        return _T("imgdecoder-stb");
    case imgdecoder_wic:
        return _T("imgdecoder-wic");
    case imgdecoder_png:
    case imgdecoder_unknown:
    default:
        return _T("imgdecoder-png");
    }
}

void GetRenderFactory(IObjRef** ref)
{
    switch (o.render)
    {
    case render_d2d:
        pComMgr->CreateRender_D2D(ref);
        break;
    case render_skia:
        pComMgr->CreateRender_Skia(ref);
        break;
    case render_gdi:
    case render_unknown:
    default:
        pComMgr->CreateRender_GDI(ref);
        break;
    }
}

IResProvider *LoadResource(SouiFactory *souiFac, HINSTANCE hInstance)
{
    IResProvider* pResProvider;
    if (PathFileExists(o.uires_path))
    {
        pResProvider = souiFac->CreateResProvider(RES_FILE);
        if (pResProvider->Init(reinterpret_cast<LPARAM>(o.uires_path), 0))
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

VOID WINAPI InitMainWindow()
{
    DWORD nRet = 0;
    SouiFactory souiFac;
    CAutoRefPtr<IRenderFactory> pRenderFactory;
    CAutoRefPtr<IImgDecoderFactory> pImgDecoderFactory;
    if (pComMgr == nullptr)
    {
        pMgmtMisc = new MgmtMisc();
        HRESULT hRes = OleInitialize(NULL);
        CAutoRefPtr<IResProvider> pResProvider;
        pComMgr = new SComMgr2(GetImgDecoder());

        GetRenderFactory(reinterpret_cast<IObjRef**>(&pRenderFactory));
        pComMgr->CreateImgDecoder(reinterpret_cast<IObjRef**>(&pImgDecoderFactory));
        pRenderFactory->SetImgDecoderFactory(pImgDecoderFactory);
        pImgDecoderFactory = nullptr;
        pApp = new SApplication(pRenderFactory, o.hInstance, _T(PACKAGE_NAME));
        pRenderFactory = nullptr;
        pApp->RegisterWindowClass<STurn3dView>();
        pApp->RegisterWindowClass<STabCtrlEx>();
        pApp->RegisterWindowClass<SGifPlayer>();
        pApp->RegisterSkinClass<SSkinGif>();
        pApp->RegisterSkinClass<SSkinVScrollbar>();
        SSkinGif::Gdiplus_Startup();

        HMODULE hSysResource = LoadLibrary(SYS_NAMED_RESOURCE);
        if (hSysResource)
        {
            CAutoRefPtr<IResProvider> sysSesProvider;
            sysSesProvider.Attach(souiFac.CreateResProvider(RES_PE));
            sysSesProvider->Init(reinterpret_cast<WPARAM>(hSysResource), 0);
            pApp->LoadSystemNamedResource(sysSesProvider);
        }

        pResProvider.Attach(LoadResource(&souiFac, o.hInstance));

        pApp->AddResProvider(pResProvider);

        if (pDlgMain == nullptr)
        {
            pDlgMain = new CMainDlg();
            pDlgMain->Create(GetActiveWindow(), 0, 0, 300, 600);
            pDlgMain->SendMessage(WM_INITDIALOG);
            pDlgMain->CenterWindow(pDlgMain->m_hWnd);
            pDlgMain->ShowWindow(SW_SHOWNORMAL);
        }
    }
}

DWORD WINAPI RunMessageLoop()
{
    return pApp->Run(pDlgMain->m_hWnd);
}

VOID WINAPI ReleaseMainWindow()
{
    pApp->UnregisterWindowClass<SGifPlayer>();
    delete pApp;
    SSkinGif::Gdiplus_Shutdown();
    delete pComMgr;
    OleUninitialize();
}