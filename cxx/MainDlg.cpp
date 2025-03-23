/**
 * @file MainDlg.cpp
 * @brief Implementation file for the main dialog class of the OpenVPN GUI application.
 * @details This source file contains the implementation of the CMainDlg class,
 *          which manages the main dialog window and its behavior.
 * @author UxGood <uxgood.org@gmail.com>
 * @date 2025-03-05
 */

#include "stdafx.h"

#include "MainDlg.h"
#include "openvpn-ex.h"
#include "STreeAdapter.h"

extern "C" {
#include "save_pass.h"
#include "misc.h"
}

MainDlg::MainDlg() : SHostWnd(_T("LAYOUT:XML_MAINWND"))
{
    m_bLayoutInited = FALSE;
}

MainDlg::~MainDlg()
{
}

void MainDlg::OnClose()
{
    if (CloseApplication(o.hWnd, true))
    {
        ShowWindow(SW_HIDE);
    }
}

int MainDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    SetMsgHandled(FALSE);
    return 0;
}

void MainDlg::OnDestroy()
{
    SetMsgHandled(FALSE);
}

void MainDlg::OnShowWindow(BOOL bShow, UINT nStatus)
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

BOOL MainDlg::OnInitDialog(HWND hWnd, LPARAM lParam)
{
    m_bLayoutInited = TRUE;
    auto* pTree = FindChildByName2<STreeView>(L"tv_home");
    if (pTree)
    {
        pTree->EnableScrollBar(SSB_HORZ, FALSE);

        auto pAdapter = new STreeAdapter(pTree);
        pTree->SetAdapter(pAdapter);
        pAdapter->Release();
        pAdapter->RefreshItems();
    }
    SetMsgHandled(FALSE);
    return 0;
}

BOOL MainDlg::OnTurn3D(EventArgs* pEvt)
{
    return TRUE;
}

void MainDlg::OnHotKey(int nHotKeyID, UINT uModifiers, UINT uVirtKey)
{
    SetMsgHandled(FALSE);
}

void MainDlg::OnBtnClose()
{
    SendMessage(WM_SYSCOMMAND, SC_CLOSE);
}

void MainDlg::OnBtnMaximize()
{
    SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE);
    auto* btn = FindChildByName2<SButton>(L"btn_max");
    btn->SetVisible(FALSE, TRUE);
    btn = FindChildByName2<SButton>(L"btn_restore");
    btn->SetVisible(TRUE, TRUE);
}

void MainDlg::OnBtnRestore()
{
    SendMessage(WM_SYSCOMMAND, SC_RESTORE);
    auto* btn = FindChildByName2<SButton>(L"btn_max");
    btn->SetVisible(TRUE, TRUE);
    btn = FindChildByName2<SButton>(L"btn_restore");
    btn->SetVisible(FALSE, TRUE);
}

void MainDlg::OnBtnMinimize()
{
    ShowWindow(SW_HIDE);
}

void MainDlg::OnBtnSet()
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

void MainDlg::OnBtnHome()
{
    ShowPage(_T("page_home"));
    auto* btn = FindChildByName2<SImageButton>(L"btn_home");
    btn->EnableWindow(FALSE, TRUE);
}

void MainDlg::OnBtnLogin()
{
    ShowPage(_T("page_login"));
    auto* btn = FindChildByName2<SImageButton>(L"btn_login");
    btn->EnableWindow(FALSE, TRUE);
}

void MainDlg::OnBtnDetail()
{
    ShowPage(_T("page_detail"));
    auto* btn = FindChildByName2<SImageButton>(L"btn_detail");
    btn->EnableWindow(FALSE, TRUE);
}

void MainDlg::OnBtnOption()
{
    ShowPage(_T("page_option"));
    auto* btn = FindChildByName2<SImageButton>(L"btn_option");
    btn->EnableWindow(FALSE, TRUE);
}

void MainDlg::OnCommand( UINT uNotifyCode, int nID, HWND wndCtl )
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
            break;
        case 22:
            break;
        case 23:
            break;
        case 10:
            SendMessage(WM_SYSCOMMAND, SC_CLOSE);
            break;
        case 9:
            ShowPage(_T("page_about"));
            break;
        case 8:
            break;
        case 7:
            break;
        default:
            SetMsgHandled(FALSE);
            break;
        }
    }
}

void MainDlg::ShowPage(int nIndex)
{
    auto *pTab = FindChildByName2<STabCtrl>(L"tab_main");
    if (pTab)
    {
        ShowWindow(SW_HIDE);
        pTab->SetCurSel(nIndex);
        ShowWindow(SW_SHOW);
    }
}

void MainDlg::ShowPage(LPCTSTR pszName, BOOL bTitle)
{
    auto *pTab = FindChildByName2<STabCtrl>(L"tab_main");
    int nIndex = pTab->GetPageIndex(pszName, bTitle);
    if (nIndex < 0)
    {
        return;
    }
    ShowPage(nIndex);
}

void MainDlg::RefreshTree()
{
    auto* pTree = FindChildByName2<STreeView>(L"tv_home");
    if (pTree)
    {

    }
}