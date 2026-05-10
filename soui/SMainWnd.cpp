/**
 * @file SMainWnd.cpp
 * @brief Implementation file for the main dialog class of the OpenVPN GUI application.
 * @details This source file contains the implementation of the CSMainWnd class,
 *          which manages the main dialog window and its behavior.
 * @author UxGood <uxgood.org@gmail.com>
 * @date 2025-03-05
 */

#include "souistd.h"

using namespace SOUI;

#include "openvpn-export.h"
#include "STreeAdapter.h"
#include "SPageMgr.h"
#include "SMainWnd.h"

/**
 * @brief Constructor for the SMainWnd class.
 */
SMainWnd::SMainWnd() : m_bLayoutInited(FALSE), m_bHided(FALSE), SHostWnd(_T("LAYOUT:XML_MAINWND"))
{
}

/**
 * @brief Destructor for the SMainWnd class.
 */
SMainWnd::~SMainWnd()
{
}

/**
 * @brief Handles the close event for the main dialog.
 */
void SMainWnd::OnClose()
{
    if (CloseApplication(o.hWnd, true))
    {
        OnShowWindow(FALSE, SW_HIDE);
    }
}

/**
 * @brief Handles the creation of the main dialog.
 * @param lpCreateStruct Pointer to the CREATESTRUCT structure.
 * @return Returns 0 if successful.
 */
int SMainWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    SetMsgHandled(FALSE);
    return 0;
}

/**
 * @brief Handles the destruction of the main dialog.
 */
void SMainWnd::OnDestroy()
{
    SetMsgHandled(FALSE);
}

/**
 * @brief Handles the show/hide event for the main dialog.
 * @param bShow Indicates whether the dialog is being shown or hidden.
 * @param nStatus Status of the window.
 */
void SMainWnd::OnShowWindow(BOOL bShow, UINT nStatus)
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
BOOL SMainWnd::OnInitDialog(HWND hWnd, LPARAM lParam)
{
    m_bLayoutInited = TRUE;
    SPageMgr::getSingleton(FindChildByName2<STabCtrlEx>(L"tab_main"));
    auto* pTree = FindChildByName2<STreeView>(L"tv_home");
    if (pTree)
    {
        pTree->EnableScrollBar(SSB_HORZ, FALSE);

        auto pAdapter = new STreeAdapter(pTree);
        pTree->SetAdapter(pAdapter);
        pAdapter->NotifyStateChange();
        pAdapter->Release();
    }
    SetMsgHandled(FALSE);
    return 0;
}

/**
 * @brief Handles the 3D toggle event.
 * @param pEvt Pointer to the event arguments.
 * @return Returns TRUE if handled.
 */
BOOL SMainWnd::OnTurn3D(EventArgs* pEvt)
{
    return TRUE;
}

/**
 * @brief Handles hotkey events.
 * @param nHotKeyID ID of the hotkey.
 * @param uModifiers Modifier keys.
 * @param uVirtKey Virtual key code.
 */
void SMainWnd::OnHotKey(int nHotKeyID, UINT uModifiers, UINT uVirtKey)
{
    SetMsgHandled(FALSE);
}

/**
 * @brief Handles the close button click event.
 */
void SMainWnd::OnBtnClose()
{
    SendMessage(WM_SYSCOMMAND, SC_CLOSE);
}

/**
 * @brief Handles the maximize button click event.
 */
void SMainWnd::OnBtnMaximize()
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
void SMainWnd::OnBtnRestore()
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
void SMainWnd::OnBtnMinimize()
{
    ShowHostWnd(SW_HIDE, TRUE);
}

/**
 * @brief Handles the settings button click event.
 */
void SMainWnd::OnBtnSet()
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
 * @brief Handles command events.
 * @param uNotifyCode Notification code.
 * @param nID Command ID.
 * @param wndCtl Handle to the control window.
 */
void SMainWnd::OnCommand( UINT uNotifyCode, int nID, HWND wndCtl )
{
    STreeView* pTree;
    STreeAdapter* pAdapter;
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
            SPageMgr::getSingleton().ShowPage(_T("page_about"));
            break;
        case 8:
            ShowSettingsDialog();
            break;
        case 7:
            BuildFileList();
            pTree = FindChildByName2<STreeView>(L"tv_home");
            pAdapter = dynamic_cast<STreeAdapter *>(pTree->GetAdapter());
            pAdapter->NotifyStateChange();
            break;
        case 6:
            SPageMgr::getSingleton().ShowPage(_T("page_home"));
            break;
        default:
            SetMsgHandled(FALSE);
            break;
        }
    }
}

void SMainWnd::OnMouseClick(EventArgs* pEvt)
{
    {
        auto *pTab = FindChildByName2<STabCtrl>(L"tab_main");
        auto *pPage = pTab->GetPage(pTab->GetCurSel());
        connection_t *c = reinterpret_cast<connection_t *>(pPage->GetUserData());
        if (c && c->hwndDlg)
        {
            AutoCloseCancel(c->hwndDlg);
        }
    }
}
