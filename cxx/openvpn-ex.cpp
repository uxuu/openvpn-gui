/**
 * @file openvpn-ex.cpp
 * @brief Implementation file for OpenVPN C++ extended functionalities.
 * @details This file contains the implementation of functions and utilities related to extended features of the OpenVPN functionality,
 * such as  enhanced connection status management, and runtime utility functions.
 * @author UxGood <uxgood.org@gmail.com>
 * @date 2025-03-08
 */

#include "stdafx.h"
#include "MainDlg.h"

#include <helper/SFunctor.hpp>

#include "openvpn-c.h"
#include "openvpn-ex.h"

#define GET_STATUS_PAGE_NAME(buf, c) \
    _stprintf_s(buf, _countof(buf), _T("page_%08x"), c->id)

extern MainDlg* pMainDlg;

TaskSingleton *TaskSingleton::getInstance()
{
    static TaskSingleton instance;
    return &instance;
}

void TaskSingleton::InitStatusPage(connection_t* c)
{
    TCHAR pageName[16];
    GET_STATUS_PAGE_NAME(pageName, c);
    auto* pTab = pMainDlg->FindChildByName2<STabCtrlEx>(L"tab_main");
    int nIndex = pTab->GetPageIndex(pageName, TRUE);
    if (nIndex < 0)
    {
        nIndex = pTab->InsertItem();
        pTab->SetItemTitle(nIndex, pageName);
    }

    auto* pPage = pTab->GetItem(nIndex);
    pPage->FindChildByName2<SWindow>(L"txt_name")->SetWindowText(c->config_name);
}

void TaskSingleton::ReleaseStatusPage(connection_t* c)
{
    auto* pTab = pMainDlg->FindChildByName2<STabCtrlEx>(L"tab_main");
    int nIndex = GetStatusPageIndex(c, pTab);
    if (nIndex >= 0)
    {
        if (pTab->GetCurSel() == nIndex)
        {
            pMainDlg->ShowPage(_T("page_home"));
        }
        pTab->RemoveItem(nIndex);
    }
}


void TaskSingleton::ShowStatusPage(connection_t* c, BOOL bShow)
{
    if (bShow)
    {
        pMainDlg->ShowPage(GetStatusPageIndex(c));
    } else
    {
        GetStatusWindow(c)->SetVisible(FALSE);
        pMainDlg->ShowPage(_T("page_home"), TRUE);
    }
}

int TaskSingleton::GetStatusPageIndex(connection_t *c, STabCtrlEx *pTab)
{
    TCHAR pageName[16];
    GET_STATUS_PAGE_NAME(pageName, c);
    if (pTab == NULL)
    {
        pTab = pMainDlg->FindChildByName2<STabCtrlEx>(L"tab_main");
    }
    return pTab->GetPageIndex(pageName, TRUE);
}

STabPage *TaskSingleton::GetStatusPage(connection_t *c, STabCtrlEx *pTab)
{
    if (pTab == NULL)
    {
        pTab = pMainDlg->FindChildByName2<STabCtrlEx>(L"tab_main");
    }
    int nIndex = GetStatusPageIndex(c, pTab);
    return pTab->GetItem(nIndex);
}

STabPage *TaskSingleton::GetStatusPage(SWindow *pWnd)
{
    LPCWSTR clsName = NULL;
    do
    {
        clsName = pWnd->GetObjectClass();
        if (wcscmp(clsName, L"page") == 0)
        {
            return dynamic_cast<STabPage *>(pWnd);
        }
        pWnd = pWnd->GetParent();
    } while (pWnd);
    return NULL;
}

SWindow *TaskSingleton::GetStatusWindow(connection_t *c, STabPage *pPage)
{
    if (pPage == NULL)
    {
        pPage = GetStatusPage(c);
    }
    switch (c->dialogId)
    {
        case ID_DLG_AUTH:
            return pPage->FindChildByName2<SWindow>(L"wnd_auth");
        case ID_DLG_AUTH_CHALLENGE:
            return pPage->FindChildByName2<SWindow>(L"wnd_challenge");
        case ID_DLG_PASSPHRASE:
            return pPage->FindChildByName2<SWindow>(L"wnd_passphrase");
        case ID_DLG_CHALLENGE_RESPONSE:
            return pPage->FindChildByName2<SWindow>(L"wnd_response");
        case ID_DLG_PROXY_AUTH:
            return pPage->FindChildByName2<SWindow>(L"wnd_proxy");
        default:
            return NULL;
    }
}

void TaskSingleton::InitUserAuthDialog(auth_param_t *param, UINT dialogId)
{
    WCHAR username[USER_PASS_LEN] = L"";
    WCHAR password[USER_PASS_LEN] = L"";
    param->c->dialogId = dialogId;
    auto *pPage = GetStatusPage(param->c);
    auto *pWnd = GetStatusWindow(param->c, pPage);
    pWnd->SetVisible(TRUE);
    pPage->SetUserData(reinterpret_cast<ULONG_PTR>(param->c));
    pWnd->GetParent()->GetEventSet()->subscribeEvent(EventMouseClick::EventID, Subscriber(&TaskSingleton::OnMouseClick, this));
    ShowStatusPage(param->c, TRUE);
    if (param->str)
    {
        LPWSTR wstr = Widen(param->str);
        auto *pEdit = pWnd->FindChildByName2<SEdit>(L"edt_challenge");

        if (wstr)
        {
            pWnd->FindChildByName2<SStatic>(L"txt_challenge")->SetWindowText(wstr);
        }

        free(wstr);

        /* Set/Remove style ES_PASSWORD by SetWindowLong(GWL_STYLE) does nothing,
         * send EM_SETPASSWORDCHAR just works. */
        if (param->flags & FLAG_CR_ECHO)
        {
            pEdit->SetAttribute(L"password", L"", TRUE);
        }

    }
    else if (param->flags & FLAG_CR_TYPE_CONCAT)
    {
        pWnd->FindChildByName2<SStatic>(L"txt_challenge")->SetWindowText(LoadLocalizedString(IDS_NFO_OTP_PROMPT));

    }
    if (RecallUsername(param->c->config_name, username))
    {
        pWnd->FindChildByName2<SEdit>(L"edt_username")->SetWindowText(username);
        pWnd->FindChildByName2<SEdit>(L"edt_password")->SetFocus();
    }
    if (RecallAuthPass(param->c->config_name, password))
    {
        pWnd->FindChildByName2<SEdit>(L"edt_password")->SetWindowText(password);
        if (username[0] != L'\0' && !(param->flags & (FLAG_CR_TYPE_SCRV1|FLAG_CR_TYPE_CONCAT))
            && password[0] != L'\0' && param->c->failed_auth_attempts == 0)
        {
            /* user/pass available and no challenge response needed: skip dialog
             * if silent_connection is on, else auto submit after a few seconds.
             * User can interrupt.
             */
            pWnd->FindChildByName2<SButton>(L"btn_confirm")->SetFocus();
        }
        /* if auth failed, highlight password so that user can type over */
        else if (param->c->failed_auth_attempts)
        {
            pWnd->FindChildByName2<SEdit>(L"edt_password")->SetSel(0, -1,0);
            pWnd->FindChildByName2<SEdit>(L"edt_password")->SetFocus();
        }
        else if (param->flags & (FLAG_CR_TYPE_SCRV1|FLAG_CR_TYPE_CONCAT))
        {
            pWnd->FindChildByName2<SEdit>(L"edt_challenge")->SetFocus();
        }
        SecureZeroMemory(password, sizeof(password));
    }
    if (param->c->flags & FLAG_DISABLE_SAVE_PASS)
    {
        pWnd->FindChildByName2<SCheckBox>(L"chk_savepass")->SetVisible(FALSE);
    }
    else if (param->c->flags & FLAG_SAVE_AUTH_PASS)
    {
        pWnd->FindChildByName2<SCheckBox>(L"chk_savepass")->SetCheck(TRUE);
    }

    if (param->c->failed_auth_attempts > 0)
    {
        pWnd->FindChildByName2<SStatic>(L"txt_warning")->SetWindowText(LoadLocalizedString(IDS_NFO_AUTH_PASS_RETRY));
    }

    if (param->c->state == resuming)
    {
        ForceForegroundWindow(pMainDlg->GetHwnd());
    }
    else
    {
        SetForegroundWindow(pMainDlg->GetHwnd());
    }
    pWnd->FindChildByName2<SButton>(L"btn_confirm")->GetEventSet()->subscribeEvent(EventCmd::EventID,Subscriber(&TaskSingleton::OnButtonClick, this));
    pWnd->FindChildByName2<SButton>(L"btn_cancel")->GetEventSet()->subscribeEvent(EventCmd::EventID,Subscriber(&TaskSingleton::OnButtonClick, this));
    pWnd->FindChildByName2<SCheckBox>(L"chk_savepass")->GetEventSet()->subscribeEvent(EventCmd::EventID,Subscriber(&TaskSingleton::OnCheckBoxClick, this));
}

BOOL TaskSingleton::OnCheckBoxClick(EventCmd* pEvt)
{
    auto pPage = GetStatusPage(dynamic_cast<SWindow *>(pEvt->Sender()));
    connection_t *c = reinterpret_cast<connection_t *>(pPage->GetUserData());;
    SimulateButtonPress(c->hwndDlg, ID_CHK_SAVE_PASS);
    return  TRUE;
}

BOOL TaskSingleton::OnButtonClick(EventCmd* pEvt)
{
    WCHAR username[USER_PASS_LEN] = L"";
    WCHAR password[USER_PASS_LEN] = L"";
    auto* pBtn =  dynamic_cast<SButton*>(pEvt->Sender());
    auto* pPage = GetStatusPage(pBtn);
    connection_t *c = reinterpret_cast<connection_t *>(pPage->GetUserData());;
    if (wcscmp(pBtn->GetName(), L"btn_confirm") == 0)
    {
        auto* pWnd = GetStatusWindow(c, pPage);
        pWnd->FindChildByName2<SEdit>(L"edt_username")->GetWindowText(username, USER_PASS_LEN, TRUE);
        SetDlgItemTextW(c->hwndDlg, ID_EDT_AUTH_USER, username);
        pWnd->FindChildByName2<SEdit>(L"edt_password")->GetWindowText(password, USER_PASS_LEN, TRUE);
        SetDlgItemTextW(c->hwndDlg, ID_EDT_AUTH_PASS, password);
        if (c->dialogId == ID_DLG_AUTH_CHALLENGE)
        {
            pWnd->FindChildByName2<SCheckBox>(L"edt_challenge")->GetWindowText(password, USER_PASS_LEN, TRUE);
            SetDlgItemTextW(c->hwndDlg, ID_EDT_AUTH_CHALLENGE, password);
        }
        SecureZeroMemory(password, sizeof(password));
        SimulateButtonPress(c->hwndDlg, IDOK);
    }
    else if (wcscmp(pBtn->GetName(), L"btn_cancel") ==0)
    {
        SimulateButtonPress(c->hwndDlg, IDCANCEL);
    }
    return  TRUE;
}

BOOL TaskSingleton::OnMouseClick(EventMouseClick* pEvt)
{
    if (pEvt->clickId == MOUSE_LBTN_DOWN || pEvt->clickId == MOUSE_RBTN_DOWN)
    {
        auto *pPage =  GetStatusPage(dynamic_cast<SWindow *>(pEvt->Sender()));
        connection_t *c = reinterpret_cast<connection_t *>(pPage->GetUserData());
        AutoCloseCancel(c->hwndDlg);
        return  TRUE;
    }
    return FALSE;
}

void TaskSingleton::SetAutoCloseText(HWND hwnd, LPCTSTR pszText)
{
    auth_param_t *param = static_cast<auth_param_t *>(GetPropW(hwnd, cfgProp));
    auto *pWnd = GetStatusWindow(param->c);
    pWnd->FindChildByName2<SStatic>(L"txt_warning")->SetWindowText(pszText);
}

void InitStatusPage(connection_t* c)
{
    auto *obj = TaskSingleton::getInstance();
    STaskHelper::sendTask(pMainDlg, obj, &TaskSingleton::InitStatusPage, c);
}

void ReleaseStatusPage(connection_t* c)
{
    auto *obj = TaskSingleton::getInstance();
    STaskHelper::sendTask(pMainDlg, obj, &TaskSingleton::ReleaseStatusPage, c);
}

void ShowStatusPage(connection_t* c, BOOL bShow)
{
    auto *obj = TaskSingleton::getInstance();
    STaskHelper::sendTask(pMainDlg, obj, &TaskSingleton::ShowStatusPage, c, bShow);
}

void InitUserAuthDialog(auth_param_t *param, UINT dialogId)
{
    auto *obj = TaskSingleton::getInstance();
    STaskHelper::sendTask(pMainDlg, obj, &TaskSingleton::InitUserAuthDialog, param, dialogId);
}

void SetWindowHide(HWND hwnd, BOOL bHide)
{
    WINDOWPLACEMENT wp;
    wp.length = sizeof(WINDOWPLACEMENT);
    wp.flags = WPF_RESTORETOMAXIMIZED;
    wp.showCmd = bHide?SW_HIDE:SW_SHOW;
    SetWindowPlacement(hwnd, &wp);
}

void SetAutoCloseText(HWND hwnd, LPCTSTR pszText)
{
    auto *obj = TaskSingleton::getInstance();
    STaskHelper::sendTask(pMainDlg, obj, &TaskSingleton::SetAutoCloseText, hwnd, pszText);
}