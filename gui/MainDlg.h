#pragma once

extern "C" {
    void CloseApplication(HWND hwnd, BOOL ask_user);
}
class CMainDlg : public SHostWnd
               , public SDpiHandler<CMainDlg>
{
public:
    CMainDlg();
    ~CMainDlg() override;

    void OnClose();
    int OnCreate(LPCREATESTRUCT lpCreateStruct);
    void OnDestroy();
    void OnShowWindow(BOOL bShow, UINT nStatus);
    BOOL OnInitDialog(HWND wndFocus, LPARAM lInitParam);
    void OnCommand(UINT uNotifyCode, int nID, HWND wndCtl);
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
    void ShowPage(int nIndex);
    void ShowPage(LPCTSTR pszName, BOOL bTitle = TRUE);
    void RefreshTree();

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
        MSG_WM_COMMAND(OnCommand)
        CHAIN_MSG_MAP(SHostWnd)
		CHAIN_MSG_MAP(SDpiHandler<CMainDlg>)
        REFLECT_NOTIFICATIONS_EX()
    END_MSG_MAP()

protected:
    BOOL            m_bHided;
    BOOL         	m_bLayoutInited;
};
