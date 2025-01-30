#include "stdafx.h"
#include <windows.h>
#include <windowsx.h>
#include <shlwapi.h>

#include <stdarg.h>
#include <stdlib.h>
#include <tchar.h>

#include "openvpnex.h"
#include "MainDlg.h"

#include "Init.h"

#include <SouiFactory.h>

#ifdef _DEBUG
#define SYS_NAMED_RESOURCE _T("soui-sys-resource2d.dll")
#else
#define SYS_NAMED_RESOURCE _T("soui-sys-resource2.dll")
#endif

/*  Declare Windows procedure  */

BOOL bCan3DView = FALSE;
CMainDlg *pDlgMain = nullptr;
SComMgr2 *pComMgr = nullptr;

VOID WINAPI ShowMainWindow(BOOL bShow)
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

void GetRenderFactory(IObjRef** ref)
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

IResProvider *LoadResource(SouiFactory *souiFac, HINSTANCE hInstance)
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

DWORD WINAPI InitMainWindow(HWND hWnd)
{
    DWORD nRet = 0;
    SouiFactory souiFac;
    CAutoRefPtr<IRenderFactory> pRenderFactory;
    CAutoRefPtr<IImgDecoderFactory> pImgDecoderFactory;
    HINSTANCE hInstance = GetModuleHandle(nullptr);
    if (pComMgr == nullptr)
    {
        HRESULT hRes = OleInitialize(NULL);
        CAutoRefPtr<IResProvider> pResProvider;
        pComMgr = new SComMgr2(GetImgDecoder());

        GetRenderFactory(reinterpret_cast<IObjRef**>(&pRenderFactory));
        pComMgr->CreateImgDecoder(reinterpret_cast<IObjRef**>(&pImgDecoderFactory));
        pRenderFactory->SetImgDecoderFactory(pImgDecoderFactory);
        pImgDecoderFactory = nullptr;
        auto* theApp = new SApplication(pRenderFactory, hInstance, _T(PACKAGE_NAME));
        pRenderFactory = nullptr;
        theApp->RegisterWindowClass<STurn3dView>();
        theApp->RegisterWindowClass<STabCtrlEx>();
        theApp->RegisterWindowClass<SGifPlayer>();
        theApp->RegisterSkinClass<SSkinGif>();
        theApp->RegisterSkinClass<SSkinVScrollbar>();
        SSkinGif::Gdiplus_Startup();

        HMODULE hSysResource = LoadLibrary(SYS_NAMED_RESOURCE);
        if (hSysResource)
        {
            CAutoRefPtr<IResProvider> sysSesProvider;
            sysSesProvider.Attach(souiFac.CreateResProvider(RES_PE));
            sysSesProvider->Init(reinterpret_cast<WPARAM>(hSysResource), 0);
            theApp->LoadSystemNamedResource(sysSesProvider);
        }

        pResProvider.Attach(LoadResource(&souiFac, hInstance));

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
                pDlgMain->ShowWindow(SW_SHOWNORMAL);
                pDlgMain->FindChildByName(_T("turn3d"))->EnableWindow(bCan3DView);
                nRet = theApp->Run(pDlgMain->m_hWnd);
            }
            else
            {
                pDlgMain->ShowWindow(SW_SHOW);
            }
        }
        theApp->UnregisterWindowClass<SGifPlayer>();
        delete theApp;
        SSkinGif::Gdiplus_Shutdown();
    }
    else
    {
        pDlgMain->ShowWindow(SW_SHOW);
    }
    delete pComMgr;
    OleUninitialize();
    return nRet;
}
