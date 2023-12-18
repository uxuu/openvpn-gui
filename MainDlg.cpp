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
void CMainDlg::OnClose()
{
    DbgPrintf(_T("%s(%d):"), _T(__FUNCTION__), __LINE__);
    AnimateHostWindow(100, AW_CENTER | AW_HIDE);
    CloseApplication(m_hWnd2, true);
}

int CMainDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    UNREFERENCED_PARAMETER(lpCreateStruct);
    SetMsgHandled(FALSE);
    return 0;
}

void CMainDlg::OnDestroy()
{
    DbgPrintf(_T("%s(%d):"), _T(__FUNCTION__), __LINE__);
}

void CMainDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
    UNREFERENCED_PARAMETER(nStatus);
    if (bShow)
    {
        AnimateHostWindow(100, AW_CENTER);
    }
}

BOOL CMainDlg::OnInitDialog(HWND hWnd, LPARAM lParam)
{
    m_bLayoutInited = TRUE;
    STreeView* pTree = FindChildByName2<STreeView>(L"tv_config");
    if (pTree)
    {
        pTree->EnableScrollBar(SSB_HORZ, FALSE);

        m_pTreeAdapter = new STreeAdapter();
        pTree->SetAdapter(m_pTreeAdapter);
        m_pTreeAdapter->Release();
        m_pTreeAdapter->RefreshItems();
    }
    return 0;
}

BOOL CMainDlg::OnTurn3D(EventArgs* pEvt)
{
    EventTurn3d* pEvt3dTurn = (EventTurn3d*)pEvt;
    STabCtrl* pTab = FindChildByName2<STabCtrl>(L"tab_main");
    return TRUE;
}

void CMainDlg::OnChangeStatus(int state)
{
    m_pTreeAdapter->RefreshItems();
}

void CMainDlg::OnHotKey(int nHotKeyID, UINT uModifiers, UINT uVirtKey)
{

}

void CMainDlg::OnBtnClose()
{
    DbgPrintf(_T("%s(%d):"), _T(__FUNCTION__), __LINE__);
    SendMessage(WM_SYSCOMMAND, SC_CLOSE);
}

void CMainDlg::OnBtnMaximize()
{
    DbgPrintf(_T("%s(%d):"), _T(__FUNCTION__), __LINE__);
    SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE);
    SButton* btn = FindChildByName2<SButton>(L"btn_max");
    btn->SetVisible(FALSE, TRUE);
    btn = FindChildByName2<SButton>(L"btn_restore");
    btn->SetVisible(TRUE, TRUE);
}

void CMainDlg::OnBtnRestore()
{
    DbgPrintf(_T("%s(%d):"), _T(__FUNCTION__), __LINE__);
    SendMessage(WM_SYSCOMMAND, SC_RESTORE);
    SButton* btn = FindChildByName2<SButton>(L"btn_max");
    btn->SetVisible(TRUE, TRUE);
    btn = FindChildByName2<SButton>(L"btn_restore");
    btn->SetVisible(FALSE, TRUE);
}

void CMainDlg::OnBtnMinimize()
{
    DbgPrintf(_T("%s(%d):"), _T(__FUNCTION__), __LINE__);
    SendMessage(WM_SYSCOMMAND, SC_MINIMIZE);
}

void CMainDlg::OnBtnSet()
{
    DbgPrintf(_T("%s(%d):"), _T(__FUNCTION__), __LINE__);
    m_pTreeAdapter->RefreshItems();
}

void CMainDlg::OnBtnHome()
{
    ShowPage(_T("page_home"));
    SImageButton* btn = FindChildByName2<SImageButton>(L"btn_home");
    btn->EnableWindow(FALSE, TRUE);
}

void CMainDlg::OnBtnLogin()
{
    ShowPage(_T("page_login"));
    SImageButton* btn = FindChildByName2<SImageButton>(L"btn_login");
    btn->EnableWindow(FALSE, TRUE);
}

void CMainDlg::OnBtnDetail()
{
    ShowPage(_T("page_detail"));
    SImageButton* btn = FindChildByName2<SImageButton>(L"btn_detail");
    btn->EnableWindow(FALSE, TRUE);
}

void CMainDlg::OnBtnOption()
{
    ShowPage(_T("page_option"));
    SImageButton* btn = FindChildByName2<SImageButton>(L"btn_option");
    btn->EnableWindow(FALSE, TRUE);
}

void CMainDlg::ShowPage(LPCTSTR pszName, BOOL bTitle)
{
    STabCtrl *pTab = FindChildByName2<STabCtrl>(L"tab_main");
    DbgPrintf(L"[%s]%s",_T(__FUNCTION__), pszName);
    if (pTab)
    {
        DbgPrintf(L"[%s]%s2", _T(__FUNCTION__), pszName);
        //STurn3dView * pTurn3d = FindChildByName2<STurn3dView>(L"turn3d");
        //if (pTurn3d)
        //{
        //    pTurn3d->Turn((SWindow *)pTab->GetPage(pTab->GetCurSel()), pTab->GetPage(pszName), bTitle);
        //}
        AnimateHostWindow(100, AW_CENTER | AW_HIDE);
        pTab->SetCurSel(pszName);
        AnimateHostWindow(100, AW_CENTER);
    }
    SImageButton* btn = FindChildByName2<SImageButton>(L"btn_home");
    btn->EnableWindow(TRUE, TRUE);
    btn = FindChildByName2<SImageButton>(L"btn_login");
    btn->EnableWindow(TRUE, TRUE);
    btn = FindChildByName2<SImageButton>(L"btn_detail");
    btn->EnableWindow(TRUE, TRUE);
    btn = FindChildByName2<SImageButton>(L"btn_option");
    btn->EnableWindow(TRUE, TRUE);
}
