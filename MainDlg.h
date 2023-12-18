// MainDlg.h : interface of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////
#pragma once
#include "..\controls.extend\gif\SSkinGif.h"
#include "..\controls.extend\gif\SGifPlayer.h"
#include "STreeAdapter.h"
extern "C" {
    void CloseApplication(HWND hwnd, BOOL ask_user);
}
class CMainDlg : public SHostWnd
               , public SDpiHandler<CMainDlg>
{
public:
    CMainDlg();
    ~CMainDlg();

    void OnClose();
    int OnCreate(LPCREATESTRUCT lpCreateStruct);
    void OnDestroy();
    void OnShowWindow(BOOL bShow, UINT nStatus);
    BOOL OnInitDialog(HWND wndFocus, LPARAM lInitParam);
    BOOL OnRefreshItems(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL bHandled)
    {
        DbgPrintf(L"[%s:%d]", _T(__FUNCTION__), __LINE__);
        m_pTreeAdapter->RefreshItems();
        return TRUE;
    };
    void OnBtnClose();
    void OnBtnMaximize();
    void OnBtnRestore();
    void OnBtnMinimize();
    void OnBtnSet();

    void OnBtnHome();
    void OnBtnLogin();
    void OnBtnDetail();
    void OnBtnOption();
    BOOL OnTurn3D(EventArgs *pEvt);
    void OnChangeStatus(int state);
    void OnHotKey(int nHotKeyID, UINT uModifiers, UINT uVirtKey);

    void ShowPage(LPCTSTR pszName, BOOL bTitle = TRUE);
    STDMETHOD_(void, onStateChanged)(THIS_) {
        m_pTreeAdapter->RefreshItems();
        DbgPrintf(L"[%s:%d]", _T(__FUNCTION__), __LINE__);
    }
    HWND            m_hWnd2;

protected:

    EVENT_MAP_BEGIN()
        EVENT_NAME_COMMAND(L"btn_close", OnBtnClose)
        EVENT_NAME_COMMAND(L"btn_min", OnBtnMinimize)
        EVENT_NAME_COMMAND(L"btn_max", OnBtnMaximize)
        EVENT_NAME_COMMAND(L"btn_restore", OnBtnRestore)
        EVENT_NAME_COMMAND(L"btn_set", OnBtnSet)
        EVENT_NAME_COMMAND(L"btn_home", OnBtnHome)
        EVENT_NAME_COMMAND(L"btn_login", OnBtnLogin)
        EVENT_NAME_COMMAND(L"btn_detail", OnBtnDetail)
        EVENT_NAME_COMMAND(L"btn_option", OnBtnOption)
        EVENT_NAME_HANDLER(L"turn3d", EventTurn3d::EventID, OnTurn3D)
        EVENT_MAP_END()

        BEGIN_MSG_MAP_EX(CMainDlg)
        MSG_WM_HOTKEY(OnHotKey)
        MSG_WM_CREATE(OnCreate)
        MSG_WM_DESTROY(OnDestroy)
        MSG_WM_INITDIALOG(OnInitDialog)
        MSG_WM_CLOSE(OnClose)
        MSG_WM_SHOWWINDOW(OnShowWindow)
        MESSAGE_HANDLER(WM_USER+0x2000, OnRefreshItems)
        CHAIN_MSG_MAP(SHostWnd)
		CHAIN_MSG_MAP(SDpiHandler<CMainDlg>)
        REFLECT_NOTIFICATIONS_EX()
    END_MSG_MAP()
private:
    STreeAdapter* 	m_pTreeAdapter;
    BOOL         	m_bLayoutInited;
};
