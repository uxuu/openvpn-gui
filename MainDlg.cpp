// MainDlg.cpp : implementation of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include <string>
#include <vector>
#include "MainDlg.h"

#include <time.h>
extern "C" {
#undef MAX_NAME
#include "options.h"
#include "openvpn_config.h"

    extern options_t o;
}
extern const TCHAR *cfgProp;
extern HANDLE hEvent;
CMainDlg::CMainDlg() : SHostWnd(_T("LAYOUT:XML_MAINWND"))
{
    m_bLayoutInited = FALSE;
}

CMainDlg::~CMainDlg()
{
}

int CMainDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    UNREFERENCED_PARAMETER(lpCreateStruct);
    SetMsgHandled(FALSE);
    return 0;
}

void CMainDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
    UNREFERENCED_PARAMETER(nStatus);
    if (bShow)
    {
        AnimateHostWindow(200, AW_CENTER);
    }
}

BOOL CMainDlg::OnInitDialog(HWND hWnd, LPARAM lParam)
{
    m_bLayoutInited = TRUE;
    STreeView* pTree = FindChildByName2<STreeView>(L"tv_config");
    SASSERT(pTree);
    pTree->EnableScrollBar(SSB_HORZ, FALSE);

    m_pTreeAdapter = new STreeAdapter();
    pTree->SetAdapter(m_pTreeAdapter);
    m_pTreeAdapter->Release();
    m_pTreeAdapter->RefreshItems();

    OnLoadConfig();
    OnChangeStatus(0);
    return 0;
}

void CMainDlg::OnHotKey(int nHotKeyID, UINT uModifiers, UINT uVirtKey)
{

}

void CMainDlg::OnBtnSetting()
{
    ShowPage(_T("settings"));
}

void CMainDlg::OnBtnSet()
{
    DbgPrintf(_T("%s(%d):"), _T(__FUNCTION__), __LINE__);
}

void CMainDlg::OnBtnHome()
{
    ShowPage(_T("home"));
    m_pTreeAdapter->DeleteAllItems();
}

void CMainDlg::OnBtnLogin()
{
    ShowPage(_T("login"));
}

void CMainDlg::OnBtnSettingOK()
{
    ShowPage(_T("login"));
}

void CMainDlg::OnBtnSettingCancel()
{
    ShowPage(_T("login"));
}

BOOL CMainDlg::OnTurn3D(EventArgs *pEvt)
{
    EventTurn3d *pEvt3dTurn = (EventTurn3d*)pEvt;
    STabCtrl *pTab = FindChildByName2<STabCtrl>(L"tab_main");
    return TRUE;
}

BOOL CMainDlg::OnConnect2(){
    cur_cons = m_recons + 1;
    //return OnConnect();
    OnChangeStatus(2);
    return TRUE;
}

BOOL CMainDlg::OnConnect()
{
    OnChangeStatus(2);
    return TRUE;
}


BOOL CMainDlg::OnSaveConfig()
{
    OnBtnLogin();
    OnChangeStatus(1);
    return TRUE;
}

void CMainDlg::OnChangeStatus(int state)
{
    if (state == 0)
    {
        FindChildByName(L"img_connected")->SetVisible(FALSE, TRUE);
        FindChildByName(L"img_connecting")->SetVisible(FALSE, TRUE);
        FindChildByName(L"img_disconnected")->SetVisible(TRUE, TRUE);
        FindChildByName(L"btn_disconnect")->SetVisible(FALSE, TRUE);
        FindChildByName(L"btn_connect")->SetVisible(TRUE, TRUE);
        FindChildByName(L"connectstate")->SetAttribute(L"colorText", L"#db3c32", 1);
        FindChildByName(L"connectstate")->SetWindowText(_T("unconnected"));
    }
    else if (state == 1)
    {

        FindChildByName(L"img_connected")->SetVisible(FALSE, TRUE);
        FindChildByName(L"img_connecting")->SetVisible(TRUE, TRUE);
        FindChildByName(L"img_disconnected")->SetVisible(FALSE, TRUE);
        FindChildByName(L"btn_connect")->SetVisible(FALSE, TRUE);
        FindChildByName(L"btn_disconnect")->SetVisible(TRUE, TRUE);
        FindChildByName(L"connectstate")->SetAttribute(L"colorText", L"#db3c32", 1);
        FindChildByName(L"connectstate")->SetWindowText(_T("connecting..."));
    }
    else if (state == 2)
    {
        FindChildByName(L"img_connected")->SetVisible(TRUE, TRUE);
        FindChildByName(L"img_connecting")->SetVisible(FALSE, TRUE);
        FindChildByName(L"img_disconnected")->SetVisible(FALSE, TRUE);
        FindChildByName(L"btn_connect")->SetVisible(FALSE, TRUE);
        FindChildByName(L"btn_disconnect")->SetVisible(TRUE, TRUE);
        FindChildByName(L"connectstate")->SetAttribute(L"colorText", L"#0b8000", 1);
        FindChildByName(L"connectstate")->SetWindowText(_T("connected"));
    }
}

void CMainDlg::OnDisconnect()
{
    OnChangeStatus(0);
}

void CMainDlg::OnLoadConfig()
{

}

void CMainDlg::LoadSetting()
{

}

void CMainDlg::OnChkAutoLoginChanged()
{

}

void CMainDlg::OnChkSavePassChanged()
{

}

void CMainDlg::ShowPage(LPCTSTR pszName, BOOL bTitle)
{
    STabCtrl *pTab = FindChildByName2<STabCtrl>(L"tab_main");
    if (pTab)
    {
        STurn3dView * pTurn3d = FindChildByName2<STurn3dView>(L"turn3d");
        if (pTurn3d)
        {
            pTurn3d->Turn((SWindow *)pTab->GetPage(pTab->GetCurSel()), pTab->GetPage(pszName), bTitle);
            pTab->SetCurSel(pszName);
        }
    }
}