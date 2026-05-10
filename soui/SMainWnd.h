/**
 * @file SMainWnd.h
 * @brief Header file for the main dialog class of the OpenVPN GUI application.
 * @details This header file contains the declaration of the SMainWnd class,
 *          which manages the main dialog window and its behavior.
 * @author UxGood <uxgood.org@gmail.com>
 * @date 2025-03-05
 */

#ifndef SOUI_MAINWND_H
#define SOUI_MAINWND_H

#include <STurn3DView.h>

class SMainWnd : public SHostWnd
               , public SDpiHandler<SMainWnd>
{
public:
    /**
     * @brief Constructor for the MainDlg class.
     */
    SMainWnd();

    /**
     * @brief Destructor for the MainDlg class.
     */
    ~SMainWnd() OVERRIDE;

    /**
     * @brief Handles the close event for the main dialog.
     */
    void OnClose();

    /**
     * @brief Handles the creation of the main dialog.
     * @param lpCreateStruct Pointer to the CREATESTRUCT structure.
     * @return Returns 0 if successful.
     */
    int OnCreate(LPCREATESTRUCT lpCreateStruct);

    /**
     * @brief Handles the destruction of the main dialog.
     */
    void OnDestroy();

    /**
     * @brief Handles the show window event for the main dialog.
     * @param bShow Indicates whether the window is being shown or hidden.
     * @param nStatus Specifies the status of the window.
     */
    void OnShowWindow(BOOL bShow, UINT nStatus);

    /**
     * @brief Initializes the main dialog.
     * @param wndFocus Handle to the window that will receive the focus.
     * @param lInitParam Additional initialization parameters.
     * @return Returns TRUE if successful.
     */
    BOOL OnInitDialog(HWND wndFocus, LPARAM lInitParam);

    /**
     * @brief Handles command events for the main dialog.
     * @param uNotifyCode Notification code.
     * @param nID Identifier of the command.
     * @param wndCtl Handle to the control sending the command.
     */
    void OnCommand(UINT uNotifyCode, int nID, HWND wndCtl);

    /**
     * @brief Handles the close button click event.
     */
    void OnBtnClose();

    /**
     * @brief Handles the maximize button click event.
     */
    void OnBtnMaximize();

    /**
     * @brief Handles the restore button click event.
     */
    void OnBtnRestore();

    /**
     * @brief Handles the minimize button click event.
     */
    void OnBtnMinimize();

    /**
     * @brief Handles the set button click event.
     */
    void OnBtnSet();

    /**
     * @brief Handles the 3D turn event.
     * @param pEvt Pointer to the event arguments.
     * @return Returns TRUE if successful.
     */
    BOOL OnTurn3D(EventArgs *pEvt);

    /**
     * @brief Handles hotkey events for the main dialog.
     * @param nHotKeyID Identifier of the hotkey.
     * @param uModifiers Modifier keys.
     * @param uVirtKey Virtual key code.
     */
    void OnHotKey(int nHotKeyID, UINT uModifiers, UINT uVirtKey);

    /**
     * @brief Displays a page by index.
     * @param nIndex Index of the page to display.
     */
    void ShowPage(int nIndex);

    void OnMouseClick(EventArgs* pEvt);

    /**
     * @brief Displays a page by name.
     * @param pszName Name of the page to display.
     * @param bTitle Indicates whether to show the title.
     */
    void ShowPage(LPCTSTR pszName, BOOL bTitle = TRUE);

protected:

    EVENT_MAP_BEGIN()
        EVENT_NAME_COMMAND(L"btn_close", OnBtnClose)
        EVENT_NAME_COMMAND(L"btn_min", OnBtnMinimize)
        EVENT_NAME_COMMAND(L"btn_max", OnBtnMaximize)
        EVENT_NAME_COMMAND(L"btn_restore", OnBtnRestore)
        EVENT_NAME_COMMAND(L"btn_set", OnBtnSet)
        EVENT_NAME_HANDLER(L"turn3d", EventTurn3d::EventID, OnTurn3D)
        EVENT_HANDLER(EVT_MOUSE_CLICK, OnMouseClick)
    EVENT_MAP_END()

    BEGIN_MSG_MAP_EX(SMainWnd)
        MSG_WM_HOTKEY(OnHotKey)
        MSG_WM_CREATE(OnCreate)
        MSG_WM_DESTROY(OnDestroy)
        MSG_WM_INITDIALOG(OnInitDialog)
        MSG_WM_CLOSE(OnClose)
        MSG_WM_SHOWWINDOW(OnShowWindow)
        MSG_WM_COMMAND(OnCommand)
        CHAIN_MSG_MAP(SHostWnd)
		CHAIN_MSG_MAP(SDpiHandler<SMainWnd>)
        REFLECT_NOTIFICATIONS_EX()
    END_MSG_MAP()

protected:
    BOOL            m_bHided;
    BOOL         	m_bLayoutInited;
};

#endif // SOUI_MAINWND_H
