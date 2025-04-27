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

/**
 * @brief Constructor for the MainDlg class.
 */
MainDlg::MainDlg() : SHostWnd(_T("LAYOUT:XML_MAINWND"))
{
    m_bLayoutInited = FALSE;
}

/**
 * @brief Destructor for the MainDlg class.
 */
MainDlg::~MainDlg()
{
}

/**
 * @brief Handles the close event for the main dialog.
 */
void MainDlg::OnClose()
{
    if (CloseApplication(o.hWnd, true))
    {
        ShowWindow(SW_HIDE);
    }
}

/**
 * @brief Handles the creation of the main dialog.
 * @param lpCreateStruct Pointer to the CREATESTRUCT structure.
 * @return Returns 0 if successful.
 */
int MainDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    SetMsgHandled(FALSE);
    return 0;
}

/**
 * @brief Handles the destruction of the main dialog.
 */
void MainDlg::OnDestroy()
{
    SetMsgHandled(FALSE);
}

/**
 * @brief Handles the show/hide event for the main dialog.
 * @param bShow Indicates whether the dialog is being shown or hidden.
 * @param nStatus Status of the window.
 */
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

/**
 * @brief Initializes the main dialog.
 * @param hWnd Handle to the dialog window.
 * @param lParam Additional initialization parameters.
 * @return Returns TRUE if successful.
 */
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

/**
 * @brief Handles the 3D toggle event.
 * @param pEvt Pointer to the event arguments.
 * @return Returns TRUE if handled.
 */
BOOL MainDlg::OnTurn3D(EventArgs* pEvt)
{
    return TRUE;
}

/**
 * @brief Handles hotkey events.
 * @param nHotKeyID ID of the hotkey.
 * @param uModifiers Modifier keys.
 * @param uVirtKey Virtual key code.
 */
void MainDlg::OnHotKey(int nHotKeyID, UINT uModifiers, UINT uVirtKey)
{
    SetMsgHandled(FALSE);
}

/**
 * @brief Handles the close button click event.
 */
void MainDlg::OnBtnClose()
{
    SendMessage(WM_SYSCOMMAND, SC_CLOSE);
}

/**
 * @brief Handles the maximize button click event.
 */
void MainDlg::OnBtnMaximize()
{
    SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE);
    auto* btn = FindChildByName2<SButton>(L"btn_max");
    btn->SetVisible(FALSE, TRUE);
    btn = FindChildByName2<SButton>(L"btn_restore");
    btn->SetVisible(TRUE, TRUE);
}

/**
 * @brief Handles the restore button click event.
 */
void MainDlg::OnBtnRestore()
{
    SendMessage(WM_SYSCOMMAND, SC_RESTORE);
    auto* btn = FindChildByName2<SButton>(L"btn_max");
    btn->SetVisible(TRUE, TRUE);
    btn = FindChildByName2<SButton>(L"btn_restore");
    btn->SetVisible(FALSE, TRUE);
}

/**
 * @brief Handles the minimize button click event.
 */
void MainDlg::OnBtnMinimize()
{
    ShowWindow(SW_HIDE);
}

/**
 * @brief Handles the settings button click event.
 */
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

/**
 * @brief Handles the home button click event.
 */
void MainDlg::OnBtnHome()
{
    ShowPage(_T("page_home"));
    auto* btn = FindChildByName2<SImageButton>(L"btn_home");
    //btn->EnableWindow(FALSE, TRUE);
}

/**
 * @brief Handles the login button click event.
 */
void MainDlg::OnBtnLogin()
{
    ShowPage(_T("page_login"));
    auto* btn = FindChildByName2<SImageButton>(L"btn_login");
    //btn->EnableWindow(FALSE, TRUE);
}

/**
 * @brief Handles the detail button click event.
 */
void MainDlg::OnBtnDetail()
{
    ShowPage(_T("page_detail"));
    auto* btn = FindChildByName2<SImageButton>(L"btn_detail");
    //btn->EnableWindow(FALSE, TRUE);
}

/**
 * @brief Handles the option button click event.
 */
void MainDlg::OnBtnOption()
{
    ShowPage(_T("page_option"));
    auto* btn = FindChildByName2<SImageButton>(L"btn_option");
    //btn->EnableWindow(FALSE, TRUE);
}

/**
 * @brief Handles command events.
 * @param uNotifyCode Notification code.
 * @param nID Command ID.
 * @param wndCtl Handle to the control window.
 */
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

/**
 * @brief Displays a specific page by index.
 * @param nIndex Index of the page to display.
 */
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

/**
 * @brief Displays a specific page by name.
 * @param pszName Name of the page to display.
 * @param bTitle Indicates whether the page title is used.
 */
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

/**
 * @brief Refreshes the items in the tree view.
 */
void MainDlg::RefreshTreeItems()
{
    auto* pTree = FindChildByName2<STreeView>(L"tv_home");
    if (pTree)
    {
        auto pAdapter = (STreeAdapter *)pTree->GetAdapter();
        pAdapter->RefreshItems();
    }
}
