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

extern MainDlg *pMainDlg;
HRESULT AttrHandler(IObject *pObj, const IStringW *attrName, const IStringW *attrValue, BOOL bLoading);

TaskSingleton *TaskSingleton::getInstance()
{
    static TaskSingleton instance;
    return &instance;
}

void TaskSingleton::InitStatusPage(connection_t *c)
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

void TaskSingleton::ReleaseStatusPage(connection_t *c)
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

void TaskSingleton::ShowStatusPage(connection_t *c, BOOL bShow)
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

    if (param->str)
    {
        LPWSTR wstr = Widen(param->str);
        auto *pEdit = pWnd->FindChildByName2<SEdit>(L"edt_challenge");

        if (wstr)
        {
            pEdit->SetAttribute(L"cueText", wstr, FALSE);
        }

        free(wstr);

        /* Set/Remove style ES_PASSWORD by SetWindowLong(GWL_STYLE) does nothing,
         * send EM_SETPASSWORDCHAR just works. */
        if (param->flags & FLAG_CR_ECHO)
        {
            pEdit->SetAttribute(L"password", L"0", FALSE);
        }

    }
    else if (param->flags & FLAG_CR_TYPE_CONCAT)
    {
        pWnd->FindChildByName2<SEdit>(L"edt_challenge")->SetAttribute(L"cueText", LoadLocalizedString(IDS_NFO_OTP_PROMPT), FALSE);
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

    pPage->SetUserData(reinterpret_cast<ULONG_PTR>(param->c));
    pWnd->GetParent()->GetEventSet()->subscribeEvent(EventMouseClick::EventID, Subscriber(&TaskSingleton::OnMouseClick, this));
    //pWnd->FindChildByName2<SButton>(L"btn_confirm")->EnableWindow(FALSE);
    pWnd->FindChildByName2<SButton>(L"btn_confirm")->GetEventSet()->subscribeEvent(EventCmd::EventID,Subscriber(&TaskSingleton::OnButtonClick, this));
    pWnd->FindChildByName2<SButton>(L"btn_cancel")->GetEventSet()->subscribeEvent(EventCmd::EventID,Subscriber(&TaskSingleton::OnButtonClick, this));
    pWnd->FindChildByName2<SCheckBox>(L"chk_savepass")->GetEventSet()->subscribeEvent(EventCmd::EventID,Subscriber(&TaskSingleton::OnCheckBoxClick, this));
    pWnd->FindChildByName2<SButton>(L"btn_password")->GetEventSet()->subscribeEvent(EventCmd::EventID,Subscriber(&TaskSingleton::OnButtonClick, this));
    pWnd->FindChildByName2<SEdit>(L"edt_username")->GetEventSet()->subscribeEvent(EventRENotify::EventID,Subscriber(&TaskSingleton::OnEditNotify, this));
    pWnd->FindChildByName2<SEdit>(L"edt_password")->GetEventSet()->subscribeEvent(EventRENotify::EventID,Subscriber(&TaskSingleton::OnEditNotify, this));
    if (dialogId == ID_DLG_AUTH_CHALLENGE)
    {
        pWnd->FindChildByName2<SButton>(L"btn_challenge")->GetEventSet()->subscribeEvent(EventCmd::EventID,Subscriber(&TaskSingleton::OnButtonClick, this));
        pWnd->FindChildByName2<SEdit>(L"edt_challenge")->GetEventSet()->subscribeEvent(EventRENotify::EventID,Subscriber(&TaskSingleton::OnEditNotify, this));
    }
    pWnd->SetVisible(TRUE);
    ShowStatusPage(param->c, TRUE);
}

void TaskSingleton::InitGenericPassDialog(auth_param_t *param, UINT dialogId)
{
    param->c->dialogId = dialogId;
    auto *pPage = GetStatusPage(param->c);
    auto *pWnd = GetStatusWindow(param->c, pPage);

    WCHAR *wstr = Widen(param->str);
    if (!wstr)
    {
        WriteStatusLog(
            param->c, L"GUI> ", L"Error converting challenge string to widechar", false);
        ShowStatusPage(param->c, FALSE);
        return;
    }
    if (param->flags & FLAG_CR_TYPE_CRV1 || param->flags & FLAG_CR_TYPE_CRTEXT)
    {
        pWnd->FindChildByName2<SStatic>(L"txt_description")->SetWindowText(wstr);

        /* Set password echo on if needed */
        if (param->flags & FLAG_CR_ECHO)
        {
            pWnd->FindChildByName2<SEdit>(L"edt_response")->SetAttribute(L"password", L"0", FALSE);
        }
    }
    else if (param->flags & FLAG_PASS_TOKEN)
    {
        pWnd->FindChildByName2<SStatic>(L"txt_description")->SetWindowText(LoadLocalizedString(IDS_NFO_TOKEN_PASSWORD_REQUEST, param->id));
    }
    else
    {
        WriteStatusLog(param->c, L"GUI> ", L"Unknown password request", false);
        pWnd->FindChildByName2<SStatic>(L"txt_description")->SetWindowText(wstr);
    }
    free(wstr);

    if (param->c->state == resuming)
    {
        ForceForegroundWindow(pMainDlg->GetHwnd());
    }
    else
    {
        SetForegroundWindow(pMainDlg->GetHwnd());
    }

    /* If response is not required hide the response field */
    if ((param->flags & FLAG_CR_TYPE_CRV1 || param->flags & FLAG_CR_TYPE_CRTEXT)
        && !(param->flags & FLAG_CR_RESPONSE))
    {
        pWnd->FindChildByName2<SStatic>(L"txt_response")->GetParent()->SetVisible(FALSE);
    }
    else
    {
        /* disable OK button until response is filled-in */
        pWnd->FindChildByName2<SButton>(L"btn_confirm")->EnableWindow(FALSE);
    }

    pPage->SetUserData(reinterpret_cast<ULONG_PTR>(param->c));
    pWnd->GetParent()->GetEventSet()->subscribeEvent(EventMouseClick::EventID, Subscriber(&TaskSingleton::OnMouseClick, this));
    pWnd->FindChildByName2<SButton>(L"btn_confirm")->GetEventSet()->subscribeEvent(EventCmd::EventID,Subscriber(&TaskSingleton::OnButtonClick, this));
    pWnd->FindChildByName2<SButton>(L"btn_cancel")->GetEventSet()->subscribeEvent(EventCmd::EventID,Subscriber(&TaskSingleton::OnButtonClick, this));
    pWnd->FindChildByName2<SCheckBox>(L"chk_savepass")->GetEventSet()->subscribeEvent(EventCmd::EventID,Subscriber(&TaskSingleton::OnCheckBoxClick, this));
    pWnd->FindChildByName2<SButton>(L"btn_password")->GetEventSet()->subscribeEvent(EventCmd::EventID,Subscriber(&TaskSingleton::OnButtonClick, this));
    pWnd->FindChildByName2<SEdit>(L"edt_response")->GetEventSet()->subscribeEvent(EventRENotify::EventID,Subscriber(&TaskSingleton::OnEditNotify, this));
    pWnd->SetVisible(TRUE);
    ShowStatusPage(param->c, TRUE);
}

void TaskSingleton::InitPrivKeyPassDialog(connection_t *c, UINT dialogId)
{
    WCHAR passphrase[KEY_PASS_LEN];
    c->dialogId = dialogId;
    auto *pPage = GetStatusPage(c);
    auto *pWnd = GetStatusWindow(c, pPage);

    if (RecallKeyPass(c->config_name, passphrase) && wcslen(passphrase)
        && c->failed_psw_attempts == 0)
    {
        /* Use the saved password and skip the dialog */
        pWnd->FindChildByName2<SEdit>(L"edt_passphrase")->SetWindowText(passphrase);
        SecureZeroMemory(passphrase, sizeof(passphrase));
        return ;
    }
    if (c->flags & FLAG_DISABLE_SAVE_PASS)
    {
        pWnd->FindChildByName2<SCheckBox>(L"chk_savepass")->GetParent()->SetVisible(FALSE);
    }
    else if (c->flags & FLAG_SAVE_KEY_PASS)
    {
        pWnd->FindChildByName2<SCheckBox>(L"chk_savepass")->SetCheck(TRUE);
    }
    if (c->failed_psw_attempts > 0)
    {
        pWnd->FindChildByName2<SStatic>(L"txt_warning")->SetWindowText(LoadLocalizedString(IDS_NFO_KEY_PASS_RETRY));
    }
    if (c->state == resuming)
    {
        ForceForegroundWindow(pMainDlg->GetHwnd());
    }
    else
    {
        SetForegroundWindow(pMainDlg->GetHwnd());
    }

    pPage->SetUserData(reinterpret_cast<ULONG_PTR>(c));
    /* disable OK button by default - not disabled in resources */
    //pWnd->FindChildByName2<SButton>(L"btn_confirm")->EnableWindow(FALSE);
    pWnd->GetParent()->GetEventSet()->subscribeEvent(EventMouseClick::EventID, Subscriber(&TaskSingleton::OnMouseClick, this));
    pWnd->FindChildByName2<SButton>(L"btn_confirm")->GetEventSet()->subscribeEvent(EventCmd::EventID,Subscriber(&TaskSingleton::OnButtonClick, this));
    pWnd->FindChildByName2<SButton>(L"btn_cancel")->GetEventSet()->subscribeEvent(EventCmd::EventID,Subscriber(&TaskSingleton::OnButtonClick, this));
    pWnd->FindChildByName2<SCheckBox>(L"chk_savepass")->GetEventSet()->subscribeEvent(EventCmd::EventID,Subscriber(&TaskSingleton::OnCheckBoxClick, this));
    pWnd->FindChildByName2<SButton>(L"btn_passphrase")->GetEventSet()->subscribeEvent(EventCmd::EventID,Subscriber(&TaskSingleton::OnButtonClick, this));
    pWnd->FindChildByName2<SEdit>(L"edt_passphrase")->GetEventSet()->subscribeEvent(EventRENotify::EventID,Subscriber(&TaskSingleton::OnEditNotify, this));
    pWnd->SetVisible(TRUE);
    ShowStatusPage(c, TRUE);
}

void TaskSingleton::InitProxyAuthDialog(connection_t *c, UINT dialogId)
{
    c->dialogId = dialogId;
    auto *pPage = GetStatusPage(c);
    auto *pWnd = GetStatusWindow(c, pPage);

    if (c->state == resuming)
    {
        ForceForegroundWindow(pMainDlg->GetHwnd());
    }
    else
    {
        SetForegroundWindow(pMainDlg->GetHwnd());
    }

    pPage->SetUserData(reinterpret_cast<ULONG_PTR>(c));
    pWnd->GetParent()->GetEventSet()->subscribeEvent(EventMouseClick::EventID, Subscriber(&TaskSingleton::OnMouseClick, this));
    pWnd->FindChildByName2<SButton>(L"btn_confirm")->GetEventSet()->subscribeEvent(EventCmd::EventID,Subscriber(&TaskSingleton::OnButtonClick, this));
    pWnd->FindChildByName2<SButton>(L"btn_password")->GetEventSet()->subscribeEvent(EventCmd::EventID,Subscriber(&TaskSingleton::OnButtonClick, this));
    pWnd->FindChildByName2<SEdit>(L"edt_username")->GetEventSet()->subscribeEvent(EventRENotify::EventID,Subscriber(&TaskSingleton::OnEditNotify, this));
    pWnd->FindChildByName2<SEdit>(L"edt_password")->GetEventSet()->subscribeEvent(EventRENotify::EventID,Subscriber(&TaskSingleton::OnEditNotify, this));
    pWnd->SetVisible(TRUE);
    ShowStatusPage(c, TRUE);
}

BOOL TaskSingleton::OnCheckBoxClick(EventCmd *pEvt)
{
    auto pPage = GetStatusPage(dynamic_cast<SWindow *>(pEvt->Sender()));
    connection_t *c = reinterpret_cast<connection_t *>(pPage->GetUserData());;
    SimulateButtonPress(c->hwndDlg, ID_CHK_SAVE_PASS);
    return  TRUE;
}

BOOL TaskSingleton::OnButtonClick(EventCmd *pEvt)
{
    WCHAR username[USER_PASS_LEN] = L"";
    WCHAR password[USER_PASS_LEN] = L"";
    auto* pBtn =  dynamic_cast<SButton*>(pEvt->Sender());
    auto* pPage = GetStatusPage(pBtn);
    connection_t *c = reinterpret_cast<connection_t *>(pPage->GetUserData());;
    if (wcscmp(pBtn->GetName(), L"btn_confirm") == 0)
    {
        auto* pWnd = GetStatusWindow(c, pPage);
        switch (c->dialogId)
        {
            case ID_DLG_AUTH_CHALLENGE:
                pWnd->FindChildByName2<SEdit>(L"edt_challenge")->GetWindowText(password, USER_PASS_LEN, TRUE);
                SetDlgItemTextW(c->hwndDlg, ID_EDT_AUTH_CHALLENGE, password);
            case ID_DLG_AUTH:
                pWnd->FindChildByName2<SEdit>(L"edt_username")->GetWindowText(username, USER_PASS_LEN, TRUE);
                SetDlgItemTextW(c->hwndDlg, ID_EDT_AUTH_USER, username);
                pWnd->FindChildByName2<SEdit>(L"edt_password")->GetWindowText(password, USER_PASS_LEN, TRUE);
                SetDlgItemTextW(c->hwndDlg, ID_EDT_AUTH_PASS, password);
                break;
            case ID_DLG_CHALLENGE_RESPONSE:
                pWnd->FindChildByName2<SEdit>(L"edt_response")->GetWindowText(password, USER_PASS_LEN, TRUE);
                SetDlgItemTextW(c->hwndDlg, ID_EDT_RESPONSE, password);
                break;
            case ID_DLG_PROXY_AUTH:
                pWnd->FindChildByName2<SEdit>(L"edt_username")->GetWindowText(username, USER_PASS_LEN, TRUE);
                SetDlgItemTextW(c->hwndDlg, ID_EDT_PROXY_USER, username);
                pWnd->FindChildByName2<SEdit>(L"edt_password")->GetWindowText(password, USER_PASS_LEN, TRUE);
                SetDlgItemTextW(c->hwndDlg, ID_EDT_PROXY_PASS, password);
                break;
            case ID_DLG_PASSPHRASE:
                pWnd->FindChildByName2<SEdit>(L"edt_passphrase")->GetWindowText(password, USER_PASS_LEN, TRUE);
                SetDlgItemTextW(c->hwndDlg, ID_EDT_PASSPHRASE, password);
                break;
            default:
                return FALSE;
        }
        SecureZeroMemory(password, sizeof(password));
        SimulateButtonPress(c->hwndDlg, IDOK);
    }
    else if (wcscmp(pBtn->GetName(), L"btn_cancel") ==0)
    {
        SimulateButtonPress(c->hwndDlg, IDCANCEL);
    }
    else
    {
        ChangePasswordVisibility(pBtn);
    }
    return  TRUE;
}

BOOL TaskSingleton::OnMouseClick(EventMouseClick *pEvt)
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

BOOL TaskSingleton::OnEditNotify(EventRENotify *pEvt)
{
    BOOL bEnable = TRUE;
    auto *pPage =  GetStatusPage(dynamic_cast<SWindow *>(pEvt->Sender()));
    connection_t *c = reinterpret_cast<connection_t *>(pPage->GetUserData());
    auto* pWnd = GetStatusWindow(c, pPage);
    switch (c->dialogId)
    {
        case ID_DLG_AUTH_CHALLENGE:
            if (pWnd->FindChildByName2<SEdit>(L"edt_challenge")->GetWindowTextLength() == 0)
            {
                bEnable = FALSE;
                break;
            }
        case ID_DLG_AUTH:
            if (pWnd->FindChildByName2<SEdit>(L"edt_password")->GetWindowTextLength() == 0)
            {
                bEnable = FALSE;
                break;
            }
            if (pWnd->FindChildByName2<SEdit>(L"edt_username")->GetWindowTextLength() == 0)
            {
                bEnable = FALSE;
                break;
            }
            break;
        case ID_DLG_CHALLENGE_RESPONSE:
            if (pWnd->FindChildByName2<SEdit>(L"edt_response")->GetWindowTextLength() == 0)
            {
                bEnable = FALSE;
                break;
            }
            break;
        case ID_DLG_PROXY_AUTH:
            if (pWnd->FindChildByName2<SEdit>(L"edt_password")->GetWindowTextLength() == 0)
            {
                bEnable = FALSE;
                break;
            }
            if (pWnd->FindChildByName2<SEdit>(L"edt_username")->GetWindowTextLength() == 0)
            {
                bEnable = FALSE;
                break;
            }
            break;
        case ID_DLG_PASSPHRASE:
            if (pWnd->FindChildByName2<SEdit>(L"edt_passphrase")->GetWindowTextLength() == 0)
            {
                bEnable = FALSE;
                break;
            }
            break;
        default:
            return FALSE;
    }
    pWnd->FindChildByName2<SButton>(L"btn_confirm")->EnableWindow(bEnable);
    pWnd->FindChildByName2<SButton>(L"btn_confirm")->Invalidate();
    return TRUE;
}

BOOL TaskSingleton::ChangePasswordVisibility(SButton *pBtn)
{
    SStringT attr;
    auto *pPage = GetStatusPage(pBtn);
    connection_t *c = reinterpret_cast<connection_t *>(pPage->GetUserData());;
    auto *pWnd = GetStatusWindow(c, pPage);
    SEdit *pEdit = NULL;
    if (wcscmp(pBtn->GetName(), L"btn_password") == 0)
    {
        pEdit = pWnd->FindChildByName2<SEdit>(L"edt_password");
    }
    else if (wcscmp(pBtn->GetName(), L"btn_passphrase") == 0)
    {
        pEdit = pWnd->FindChildByName2<SEdit>(L"edt_passphrase");
    }
    else if (wcscmp(pBtn->GetName(), L"btn_response") == 0)
    {
        pEdit = pWnd->FindChildByName2<SEdit>(L"edt_response");
    }
    else if (wcscmp(pBtn->GetName(), L"btn_challenge") == 0)
    {
        pEdit = pWnd->FindChildByName2<SEdit>(L"edt_challenge");
    }
    else
    {
        return FALSE;
    }
    pEdit->GetAttribute(L"passwordChar", &attr);
    if (attr.Compare(L"") == 0)
    {
        pEdit->SetAttribute(L"passwordChar", L"*", FALSE);
    }
    else
    {
        pEdit->SetAttribute(L"passwordChar", L"\0", FALSE);
    }
    return TRUE;
}

void TaskSingleton::SetWarningText(connection_t* c, LPCTSTR pszText)
{
    auto *pWnd = GetStatusWindow(c);
    pWnd->FindChildByName2<SStatic>(L"txt_warning")->SetWindowText(pszText);
}

void TaskSingleton::SetWarningColor(connection_t* c, COLORREF clr)
{
    WCHAR szColor[8];
    wsprintf(szColor, L"#%02x%02x%02x", GetRValue(clr), GetGValue(clr), GetBValue(clr));
    auto *pWnd = GetStatusWindow(c);
    pWnd->FindChildByName2<SStatic>(L"txt_warning")->SetAttribute(L"colorText", szColor);
}

void InitStatusPage(connection_t *c)
{
    auto *obj = TaskSingleton::getInstance();
    STaskHelper::sendTask(pMainDlg, obj, &TaskSingleton::InitStatusPage, c);
}

void ReleaseStatusPage(connection_t *c)
{
    auto *obj = TaskSingleton::getInstance();
    STaskHelper::sendTask(pMainDlg, obj, &TaskSingleton::ReleaseStatusPage, c);
}

void ShowStatusPage(connection_t *c, BOOL bShow)
{
    auto *obj = TaskSingleton::getInstance();
    STaskHelper::sendTask(pMainDlg, obj, &TaskSingleton::ShowStatusPage, c, bShow);
}

void InitUserAuthDialog(auth_param_t *param, UINT dialogId)
{
    auto *obj = TaskSingleton::getInstance();
    STaskHelper::sendTask(pMainDlg, obj, &TaskSingleton::InitUserAuthDialog, param, dialogId);
}

void InitGenericPassDialog(auth_param_t *param, UINT dialogId)
{
    auto *obj = TaskSingleton::getInstance();
    STaskHelper::sendTask(pMainDlg, obj, &TaskSingleton::InitGenericPassDialog, param, dialogId);
}

void InitPrivKeyPassDialog(connection_t *c, UINT dialogId)
{
    auto *obj = TaskSingleton::getInstance();
    STaskHelper::sendTask(pMainDlg, obj, &TaskSingleton::InitPrivKeyPassDialog, c, dialogId);
}

void InitProxyAuthDialog(connection_t *c, UINT dialogId)
{
    auto *obj = TaskSingleton::getInstance();
    STaskHelper::sendTask(pMainDlg, obj, &TaskSingleton::InitProxyAuthDialog, c, dialogId);
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
    auth_param_t *param = static_cast<auth_param_t *>(GetPropW(hwnd, cfgProp));
    auto *obj = TaskSingleton::getInstance();
    STaskHelper::sendTask(pMainDlg, obj, &TaskSingleton::SetWarningText, param->c, pszText);
}

void SetWarningText(connection_t* c, LPCTSTR pszText)
{
    auto *obj = TaskSingleton::getInstance();
    STaskHelper::sendTask(pMainDlg, obj, &TaskSingleton::SetWarningText, c, pszText);
}

void SetWarningColor(connection_t* c,COLORREF clr)
{
    auto *obj = TaskSingleton::getInstance();
    STaskHelper::sendTask(pMainDlg, obj, &TaskSingleton::SetWarningColor, c, clr);
}