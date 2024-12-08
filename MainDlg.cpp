#include "stdafx.h"
#include <string>
#include <vector>
#include "MainDlg.h"

#include <set>

extern "C" {
#undef MAX_NAME
#include "options.h"
#include "openvpn_config.h"

#include "as.h"
    void ImportConfigFileFromDisk();
    void ShowSettingsDialog();
    void OnNotifyTray(LPARAM lParam);

}

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
    ShowWindow(SW_HIDE);
    CloseApplication(m_hWnd2, true);
}

int CMainDlg::OnCreate2(LPCREATESTRUCT lpCreateStruct)
{
    SetMsgHandled(FALSE);
    return 0;
}

void CMainDlg::OnDestroy2()
{
    SetMsgHandled(FALSE);
}

void CMainDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
    if (bShow)
    {
        m_bHided = FALSE;
        AnimateHostWindow(100, AW_CENTER);
    }
    else if (!m_bHided)
    {
        m_bHided = TRUE;
        AnimateHostWindow(100, AW_CENTER | AW_HIDE);
    }
}

BOOL CMainDlg::OnInitDialog(HWND hWnd, LPARAM lParam)
{
    m_bLayoutInited = TRUE;
    auto* pTree = FindChildByName2<STreeView>(L"tv_home");
    if (pTree)
    {
        pTree->EnableScrollBar(SSB_HORZ, FALSE);

        auto pAdapter = new STreeAdapter(pTree);
        pTree->SetAdapter(pAdapter);
        pAdapter->RefreshItems();
        pAdapter->Release();
    }
    SetMsgHandled(FALSE);
    return 0;
}

BOOL CMainDlg::OnTurn3D(EventArgs* pEvt)
{
    //EventTurn3d* pEvt3dTurn = (EventTurn3d*)pEvt;
    //STabCtrl* pTab = FindChildByName2<STabCtrl>(L"tab_main");
    return TRUE;
}

void CMainDlg::OnStateChanged()
{
    RefreshTree();
}

void CMainDlg::OnHotKey(int nHotKeyID, UINT uModifiers, UINT uVirtKey)
{
    SetMsgHandled(FALSE);
}

void CMainDlg::OnBtnClose()
{
    SendMessage(WM_SYSCOMMAND, SC_CLOSE);
}

void CMainDlg::OnBtnMaximize()
{
    SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE);
    auto* btn = FindChildByName2<SButton>(L"btn_max");
    btn->SetVisible(FALSE, TRUE);
    btn = FindChildByName2<SButton>(L"btn_restore");
    btn->SetVisible(TRUE, TRUE);
}

void CMainDlg::OnBtnRestore()
{
    SendMessage(WM_SYSCOMMAND, SC_RESTORE);
    auto* btn = FindChildByName2<SButton>(L"btn_max");
    btn->SetVisible(TRUE, TRUE);
    btn = FindChildByName2<SButton>(L"btn_restore");
    btn->SetVisible(FALSE, TRUE);
}

void CMainDlg::OnBtnMinimize()
{
    ShowWindow(SW_HIDE);
}

void CMainDlg::OnBtnSet()
{
    CPoint pt;
    CRect rc;
    this->GetNative()->ClientToScreen(&pt);
    FindChildByName(_T("btn_set"))->GetWindowRect(&rc);
    pt.Offset(rc.left, rc.bottom);
    SMenu menu;
    menu.LoadMenu(_T("SMenu:menu"));
    menu.TrackPopupMenu(0,pt.x,pt.y,m_hWnd, nullptr, this->GetScale());
}

void CMainDlg::OnBtnHome()
{
    ShowPage(_T("page_home"));
    auto* btn = FindChildByName2<SImageButton>(L"btn_home");
    btn->EnableWindow(FALSE, TRUE);
}

void CMainDlg::OnBtnLogin()
{
    ShowPage(_T("page_login"));
    auto* btn = FindChildByName2<SImageButton>(L"btn_login");
    btn->EnableWindow(FALSE, TRUE);
}

void CMainDlg::OnBtnDetail()
{
    ShowPage(_T("page_detail"));
    auto* btn = FindChildByName2<SImageButton>(L"btn_detail");
    btn->EnableWindow(FALSE, TRUE);
}

void CMainDlg::OnBtnOption()
{
    ShowPage(_T("page_option"));
    auto* btn = FindChildByName2<SImageButton>(L"btn_option");
    btn->EnableWindow(FALSE, TRUE);
}

void CMainDlg::OnCommand2( UINT uNotifyCode, int nID, HWND wndCtl )
{
    if(uNotifyCode==0)
    {
        switch (nID)
        {
        case 21:
            ImportConfigFileFromDisk();
            break;
        case 22:
            ImportConfigFromAS();
            break;
        case 23:
            ImportConfigFromURL();
            break;
        case 9:
            SendMessage(WM_SYSCOMMAND, SC_CLOSE);
            break;
        case 8:
            ShowSettingsDialog();
            break;
        case 7:
            RefreshTree();
            break;
        default:
            SetMsgHandled(FALSE);
            break;
        }
    }
}

void CMainDlg::ShowPage(LPCTSTR pszName, BOOL bTitle)
{
    auto *pTab = FindChildByName2<STabCtrl>(L"tab_main");
    if (pTab)
    {
        STurn3dView * pTurn3d = FindChildByName2<STurn3dView>(L"turn3d");
        if (pTurn3d && !pTurn3d->IsDisabled())
        {
            auto *pWnd = pTab->GetPage(pszName);
            auto *pWnd2 = dynamic_cast<SWindow*>(pTab->GetPage(pTab->GetCurSel()));
            pTurn3d->Turn(pWnd2, pWnd, bTitle);
        }
        else
        {
            AnimateHostWindow(100, AW_CENTER | AW_HIDE);
            pTab->SetCurSel(pszName);
            AnimateHostWindow(100, AW_CENTER);
        }
    }
    auto* btn = FindChildByName2<SImageButton>(L"btn_home");
    btn->EnableWindow(TRUE, TRUE);
    btn = FindChildByName2<SImageButton>(L"btn_login");
    btn->EnableWindow(TRUE, TRUE);
    btn = FindChildByName2<SImageButton>(L"btn_detail");
    btn->EnableWindow(TRUE, TRUE);
    btn = FindChildByName2<SImageButton>(L"btn_option");
    btn->EnableWindow(TRUE, TRUE);
}

void CMainDlg::RefreshTree()
{
    auto* pTree = FindChildByName2<STreeView>(L"tv_home");
    if (pTree)
    {
        auto* pAdapter = dynamic_cast<STreeAdapter *>(pTree->GetAdapter());
        if (pAdapter)
        {
            pAdapter->RefreshItems();
        }
    }
}
