#include "../stdafx.h"

#include "openvpnex.h"
#include "MainDlg.h"
#include "STreeAdapter.h"

#include <string>
#include <vector>
#include <set>

extern "C" {
#include "save_pass.h"
#include "misc.h"
}

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
    CloseApplication(o.hWnd, true);
}

int CMainDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    SetMsgHandled(FALSE);
    return 0;
}

void CMainDlg::OnDestroy()
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
    return TRUE;
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
    //auto* btn = FindChildByName2<SImageButton>(L"btn_home");
    //btn->EnableWindow(FALSE, TRUE);
}

void CMainDlg::OnBtnLogin()
{
    ShowPage(_T("page_login"));
    //auto* btn = FindChildByName2<SImageButton>(L"btn_login");
    //btn->EnableWindow(FALSE, TRUE);
}

void CMainDlg::OnBtnDetail()
{
    ShowPage(_T("page_detail"));
    //auto* btn = FindChildByName2<SImageButton>(L"btn_detail");
    //btn->EnableWindow(FALSE, TRUE);
}

void CMainDlg::OnBtnOption()
{
    ShowPage(_T("page_option"));
    //auto* btn = FindChildByName2<SImageButton>(L"btn_option");
    //btn->EnableWindow(FALSE, TRUE);
}

void CMainDlg::OnCommand( UINT uNotifyCode, int nID, HWND wndCtl )
{

    // 获取屏幕信息
    MONITORINFO mi;
    HMONITOR hm = MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTONEAREST);
    GetMonitorInfo(hm, &mi);
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
        case 10:
            SendMessage(WM_SYSCOMMAND, SC_CLOSE);
            break;
        case 9:
            ShowPage(_T("page_about"));
            break;
        case 8:
            ShowSettingsDialog();
            break;
        case 7:
            BuildFileList();
            RefreshTree();
            break;
        default:
            SetMsgHandled(FALSE);
            break;
        }
    }
}

void CMainDlg::ShowPage(int nIndex)
{
    auto *pTab = FindChildByName2<STabCtrl>(L"tab_main");
    if (pTab)
    {
        STurn3dView * pTurn3d = FindChildByName2<STurn3dView>(L"turn3d");
        if (pTurn3d && o.render == render_skia && o.tune3dview == TRUE)
        {
            auto *pWnd = dynamic_cast<SWindow*>(pTab->GetPage(nIndex));
            auto *pWnd2 = dynamic_cast<SWindow*>(pTab->GetPage(pTab->GetCurSel()));
            pTurn3d->Turn(pWnd2, pWnd, TRUE);
            pTab->SetCurSel(nIndex);
        }
        else
        {
            ShowWindow(SW_HIDE);
            pTab->SetCurSel(nIndex);
            ShowWindow(SW_SHOW);
        }
    }
}

void CMainDlg::ShowPage(LPCTSTR pszName, BOOL bTitle)
{
    auto *pTab = FindChildByName2<STabCtrl>(L"tab_main");
    int nIndex = pTab->GetPageIndex(pszName, bTitle);
    if (nIndex < 0)
    {
        DbgPrintf(_T("pszName=#%s#"), pszName);
        return;
    }
    ShowPage(nIndex);
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
