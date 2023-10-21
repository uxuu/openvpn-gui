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

#ifdef _DEBUG
#define SYS_NAMED_RESOURCE _T("soui-sys-resourced.dll")
#else
#define SYS_NAMED_RESOURCE _T("soui-sys-resource.dll")
#endif

/*  Declare Windows procedure  */

CMainDlg *dlgMain = NULL;
SComMgr2 *pComMgr = NULL;

void DlgShowWindow(bool bShow)
{
	if (dlgMain != NULL)
	{
		dlgMain->ShowWindow(bShow?SW_SHOW:SW_HIDE);
	}
}

DWORD DlgInitWindow(void *p)
{

	HINSTANCE hInstance = GetModuleHandle(NULL);
	//HRESULT hRes = OleInitialize(NULL);
	SouiFactory souiFac;
	DWORD nRet = 0;
	if (pComMgr == NULL)
	{
		pComMgr = new SComMgr2(_T("imgdecoder-png"));
		CAutoRefPtr<SOUI::IImgDecoderFactory> pImgDecoderFactory;
		CAutoRefPtr<SOUI::IRenderFactory> pRenderFactory;
		pComMgr->CreateRender_Skia((IObjRef**)&pRenderFactory);
		pComMgr->CreateImgDecoder((IObjRef**)&pImgDecoderFactory);

		pRenderFactory->SetImgDecoderFactory(pImgDecoderFactory);

		SApplication *theApp = new SApplication(pRenderFactory, hInstance, _T("OpenVPN GUI"));

		HMODULE hSysResource = LoadLibrary(SYS_NAMED_RESOURCE);
		if (hSysResource)
		{
			CAutoRefPtr<IResProvider> sysSesProvider;
			sysSesProvider.Attach(souiFac.CreateResProvider(RES_PE));
			sysSesProvider->Init((WPARAM)hSysResource, 0);
			theApp->LoadSystemNamedResource(sysSesProvider);
		}


		CAutoRefPtr<IResProvider>   pResProvider;

		if(PathFileExists(_T("uires")))
		{
			pResProvider.Attach(souiFac.CreateResProvider(RES_FILE));
			if (!pResProvider->Init((LPARAM)_T("uires"), 0))
			{
				SASSERT(0);
				return 1;
			}
		}
		else
		{
			pResProvider.Attach(souiFac.CreateResProvider(RES_PE));
			pResProvider->Init((WPARAM)hInstance, 0);
		}


		theApp->RegisterWindowClass<STurn3dView>();
		theApp->RegisterSkinClass<SSkinVScrollbar>();

		theApp->AddResProvider(pResProvider);


		// BLOCK: Run application
		{
			if (dlgMain == NULL)
			{
				dlgMain = new CMainDlg();
				dlgMain->Create(GetActiveWindow(), 0, 0, 0, 0);
				dlgMain->SendMessage(WM_INITDIALOG);
				dlgMain->CenterWindow(dlgMain->m_hWnd);
				dlgMain->ShowWindow(SW_SHOW);

				nRet = theApp->Run(dlgMain->m_hWnd);

			}
			else
			{
				dlgMain->ShowWindow(SW_SHOW);
			}

		}

		delete theApp;
		delete pComMgr;
	}
	else
	{
		dlgMain->ShowWindow(SW_SHOW);

	}

	//OleUninitialize();
	return nRet;
}

void DlgShowPage(LPCTSTR pszName, BOOL bTitle)
{
	dlgMain->ShowPage(pszName, bTitle);
}

