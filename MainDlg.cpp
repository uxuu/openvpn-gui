#include "stdafx.h"
#include <string>
#include <vector>
#include "MainDlg.h"

#include <set>

extern "C" {
#undef MAX_NAME
#include "main.h"
#include "options.h"
#include "openvpn_config.h"

#include "as.h"
    void ImportConfigFileFromDisk();
    void ShowSettingsDialog();
    void OnNotifyTray(LPARAM lParam);
    void LoadAutoStartConnections(HWND hwnd);

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
    LoadAutoStartConnections(m_hWnd2);
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

void CMainDlg::OnLogLine(int iId, char *msg)
{
    time_t timestamp;
    TCHAR pageName[16];
    wchar_t datetime[24];
    const SETTEXTEX ste = {ST_SELECTION, CP_UTF8 };
    _stprintf_s(pageName, _countof(pageName), _T("page_%08x"), iId);
    auto* pTab = FindChildByName2<STabCtrlEx>(L"tab_main");
    auto* pPage = pTab->GetPage(pageName, TRUE);
    if (!pPage)
    {
        return;
    }
    auto* pLogWnd = pPage->FindChildByName2<SRichEdit>(L"log_viewer");
    if (!pLogWnd)
    {
        return;
    }

    char *flags = strchr(msg, ',') + 1;
    if (flags - 1 == nullptr)
    {
        return;
    }

    char *message = strchr(flags, ',') + 1;
    if (message - 1 == nullptr)
    {
        return;
    }
    size_t flag_size = message - flags - 1; /* message is always > flags */

    /* Remove lines from log window if it is getting full */
    if (pLogWnd->SSendMessage(EM_GETLINECOUNT, 0, 0) > MAX_LOG_LINES)
    {
        auto pos = pLogWnd->SSendMessage(EM_LINEINDEX, DEL_LOG_LINES, 0);
        pLogWnd->SSendMessage(EM_SETSEL, 0, pos);
        pLogWnd->SSendMessage(EM_REPLACESEL, FALSE, (LPARAM) _T(""));
    }

    timestamp = strtol(msg, nullptr, 10);
    struct tm *tm = localtime(&timestamp);

    wsprintf(datetime, L"%04d-%02d-%02d %02d:%02d:%02d ",
               tm->tm_year+1900, tm->tm_mon+1, tm->tm_mday,
               tm->tm_hour, tm->tm_min, tm->tm_sec);

    /* deselect current selection, if any */
    pLogWnd->SSendMessage(EM_SETSEL, (WPARAM) -1, (LPARAM) -1);

    /* change text color if Warning or Error */
    COLORREF text_clr = 0;

    if (memchr(flags, 'N', flag_size) || memchr(flags, 'F', flag_size))
    {
        text_clr = o.clr_error;
    }
    else if (memchr(flags, 'W', flag_size))
    {
        text_clr = o.clr_warning;
    }

    if (text_clr != 0)
    {
        CHARFORMAT cfm = { sizeof(CHARFORMAT),
                    CFM_COLOR|CFM_BOLD,
                    0,
                    0,
                    0,
                    text_clr,
        };
        pLogWnd->SSendMessage(EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM) &cfm);
    }

    /* Append line to log window */
    pLogWnd->SSendMessage(EM_REPLACESEL, FALSE, (LPARAM) datetime);
    pLogWnd->SSendMessage(EM_SETTEXTEX, (WPARAM) &ste, (LPARAM) message);
    pLogWnd->SSendMessage(EM_REPLACESEL, FALSE, (LPARAM) L"\n");

    /* scroll to the caret */
    pLogWnd->SSendMessage(EM_SCROLLCARET, 0, 0);
}

void CMainDlg::OnWriteStatusLog(int iId, LPCWSTR prefix, LPCWSTR msg)
{
    time_t now;
    TCHAR pageName[16];
    wchar_t datetime[24];
    const SETTEXTEX ste = {ST_SELECTION, CP_UTF8 };
    _stprintf_s(pageName, _countof(pageName), _T("page_%08x"), iId);
    auto* pTab = FindChildByName2<STabCtrlEx>(L"tab_main");
    auto* pPage = pTab->GetPage(pageName, TRUE);
    if (!pPage)
    {
        return;
    }
    auto* pLogWnd = pPage->FindChildByName2<SRichEdit>(L"log_viewer");
    if (!pLogWnd)
    {
        return;
    }

    now = time(nullptr);
    struct tm *tm = localtime(&now);

    wsprintf(datetime, L"%04d-%02d-%02d %02d:%02d:%02d ",
               tm->tm_year+1900, tm->tm_mon+1, tm->tm_mday,
               tm->tm_hour, tm->tm_min, tm->tm_sec);
    /* change text color if Warning or Error */
    COLORREF text_clr = 0;

    if (wcsstr(prefix, L"ERROR"))
    {
        text_clr = o.clr_error;
    }
    else if (wcsstr(prefix, L"WARNING"))
    {
        text_clr = o.clr_warning;
    }

    if (text_clr != 0)
    {
        CHARFORMAT cfm = { sizeof(CHARFORMAT),
                           CFM_COLOR|CFM_BOLD,
                           0,
                            0,
                             0,
                           text_clr,
        };
        pLogWnd->SSendMessage(EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM) &cfm);
    }


    /* Remove lines from log window if it is getting full */
    if (pLogWnd->SSendMessage(EM_GETLINECOUNT, 0, 0) > MAX_LOG_LINES)
    {
        int pos = pLogWnd->SSendMessage(EM_LINEINDEX, DEL_LOG_LINES, 0);
        pLogWnd->SSendMessage(EM_SETSEL, 0, pos);
        pLogWnd->SSendMessage(EM_REPLACESEL, FALSE, (LPARAM) _T(""));
    }
    /* Append line to log window */
    pLogWnd->SSendMessage(EM_SETSEL, (WPARAM) -1, (LPARAM) -1);
    pLogWnd->SSendMessage(EM_REPLACESEL, FALSE, (LPARAM) datetime);
    pLogWnd->SSendMessage(EM_REPLACESEL, FALSE, (LPARAM) prefix);
    pLogWnd->SSendMessage(EM_REPLACESEL, FALSE, (LPARAM) msg);
    pLogWnd->SSendMessage(EM_REPLACESEL, FALSE, (LPARAM) L"\n");
}
int CMainDlg::OnInitStatusPage(int iId)
{
    TCHAR pageName[16];
    _stprintf_s(pageName, _countof(pageName), _T("page_%08x"), iId);
    auto* pTab = FindChildByName2<STabCtrlEx>(L"tab_main");
    int nIndex = pTab->GetPageIndex(pageName, TRUE);
    if (nIndex >= 0)
    {
        ShowPage(nIndex);
        return nIndex;
    }
    nIndex = pTab->InsertItem();
    pTab->SetItemTitle(nIndex, pageName);
    ShowPage(nIndex);
    return nIndex;
}

int CMainDlg::OnUninitStatusPage(int iId)
{
    TCHAR pageName[16];
    _stprintf_s(pageName, _countof(pageName), _T("page_%08x"), iId);
    auto* pTab = FindChildByName2<STabCtrlEx>(L"tab_main");
    int nIndex = pTab->GetPageIndex(pageName, TRUE);
    if (nIndex >= 0)
    {
        if (pTab->GetCurSel() == nIndex)
        {
            ShowPage(_T("page_home"));
        }
        pTab->RemoveItem(nIndex);
        return nIndex;
    }
    return -1;
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
    auto* pTab = FindChildByName2<STabCtrlEx>(L"tab_main");
    int nIndex = pTab->InsertItem();
    DbgPrintf(_T("nIndex=%d\n"), nIndex);
    pTab->SetItemTitle(nIndex, L"about");
    ShowPage(nIndex);
    //ShowPage(_T("page_option"));
    //auto* btn = FindChildByName2<SImageButton>(L"btn_option");
    //btn->EnableWindow(FALSE, TRUE);
}

void CMainDlg::OnCommand2( UINT uNotifyCode, int nID, HWND wndCtl )
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
        if (pTurn3d && !pTurn3d->IsDisabled())
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
    ShowPage(nIndex);
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
