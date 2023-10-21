// MainDlg.h : interface of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////
#pragma once
#include "..\controls.extend\gif\SSkinGif.h"
#include "..\controls.extend\gif\SGifPlayer.h"
#include "SListAdapter.h"

class CMainDlg : public SHostWnd
{
public:
	CMainDlg();
	~CMainDlg();

	void OnClose()
	{
		AnimateHostWindow(200,AW_CENTER|AW_HIDE);
        PostMessage(WM_QUIT);
	}
	void OnClose2()
	{
		AnimateHostWindow(200, AW_CENTER | AW_HIDE);
		PostMessage(WM_QUIT);
	}
	void OnMaximize()
	{
		SendMessage(WM_SYSCOMMAND,SC_MAXIMIZE);
	}
	void OnRestore()
	{
		SendMessage(WM_SYSCOMMAND,SC_RESTORE);
	}
	void OnMinimize()
	{
		SendMessage(WM_SYSCOMMAND,SC_MINIMIZE);
	}

	int OnCreate(LPCREATESTRUCT lpCreateStruct);
	void OnShowWindow(BOOL bShow, UINT nStatus);
    BOOL OnInitDialog(HWND wndFocus, LPARAM lInitParam);

    void OnBtnSetting();
	void OnBtnSet();
	void OnBtnHome();
	void OnBtnLogin();
    void OnBtnSettingOK();
    void OnBtnSettingCancel();
    BOOL OnTurn3D(EventArgs *pEvt);
	BOOL OnConnect2();
	BOOL OnConnect();
	BOOL OnSaveConfig();
	void OnDisconnect();
	void OnChangeStatus(int state);
	void OnLoadConfig();
	void LoadSetting();
	void OnHotKey(int nHotKeyID, UINT uModifiers, UINT uVirtKey);
	void OnChkAutoLoginChanged();
	void OnChkSavePassChanged();
	bool bConnected;
	DWORD			m_reconnect;
	int				m_recons;
	int             cur_cons;
	virtual void OnListItemClick(int nType, const std::string& strID);
	virtual void OnListItemRClick(int nType, const std::string& strID);
	void ShowPage(LPCTSTR pszName, BOOL bTitle = TRUE);
	void AddConfigItem(void *c, LPCTSTR pszName, LPCTSTR pszID);

protected:

	EVENT_MAP_BEGIN()
		EVENT_NAME_COMMAND(L"btn_close", OnClose)
		EVENT_NAME_COMMAND(L"btn_min", OnMinimize)
		EVENT_NAME_COMMAND(L"btn_set", OnBtnSet)
		EVENT_NAME_COMMAND(L"btn_setting", OnBtnSetting)
		EVENT_NAME_COMMAND(L"btn_home", OnBtnHome)
		EVENT_NAME_COMMAND(L"btn_login", OnBtnLogin)
		EVENT_NAME_COMMAND(L"btn_setting1", OnBtnSetting)
		EVENT_NAME_COMMAND(L"btn_home1", OnBtnHome)
		EVENT_NAME_COMMAND(L"btn_login1", OnBtnLogin)
		EVENT_NAME_COMMAND(L"btn_setting_ok", OnBtnSettingOK)
		EVENT_NAME_COMMAND(L"btn_setting_cancel", OnBtnSettingCancel)
		EVENT_NAME_HANDLER(L"turn3d", EventTurn3d::EventID, OnTurn3D)
		EVENT_NAME_COMMAND(L"btn_connect", OnConnect2);
		EVENT_NAME_COMMAND(L"btn_save", OnSaveConfig);
		EVENT_NAME_COMMAND(L"btn_disconnect", OnDisconnect);
		EVENT_NAME_COMMAND(L"chk_autologin", OnChkAutoLoginChanged);
		EVENT_NAME_COMMAND(L"chk_savepass", OnChkSavePassChanged);
		EVENT_MAP_END()

	BEGIN_MSG_MAP_EX(CMainDlg)
		MSG_WM_HOTKEY(OnHotKey)
		MSG_WM_CREATE(OnCreate)
        MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_CLOSE(OnClose)
		MSG_WM_SHOWWINDOW(OnShowWindow)
		CHAIN_MSG_MAP(SHostWnd)
		REFLECT_NOTIFICATIONS_EX()
	END_MSG_MAP()
private:
	SListAdapter* m_pListAdapter;
	BOOL			m_bLayoutInited;
};
