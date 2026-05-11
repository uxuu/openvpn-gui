/**
 * @file SPageMgr.cpp
 * @brief Source file for the SPageMgr class, which provides functionality
 *        for managing tab pages and handling page interactions.
 * @details This file contains the implementation of page management functions.
 *          It provides methods for initializing, releasing, and showing status pages.
 * @author UxGood <uxgood.org@gmail.com>
 * @date 2026-05-10
 */

#include <souistd.h>
#include <helper/SFunctor.hpp>
#include <STurn3DView.h>

using namespace SOUI;

#include "STreeAdapter.h"
#include "SPageMgr.h"

#include "openvpn-export.h"

#define PAGE_NAME(buf, c) _stprintf_s(buf, _countof(buf), _T("page_%08x"), c->id)

SPageMgr &SPageMgr::getSingleton(STabCtrlEx *pTab)
{
    static SPageMgr inst;
    if (pTab != NULL)
    {
        inst.m_pTab = pTab;
    }
    return inst;
}

SPageMgr *SPageMgr::getSingletonPtr(STabCtrlEx *pTab)
{
    return &getSingleton(pTab);
}

void SPageMgr::ShowPage(int nIndex)
{
    if (!m_pTab)
    {
        return;
    }

    if (nIndex < 0 || nIndex == m_pTab->GetCurSel())
    {
        return;
    }
    SRootWindow *pRootWnd = sobj_cast<SRootWindow>(m_pTab->GetRoot());
    SHostWnd *pHostWnd = NULL;
    if(pRootWnd)
    {
        pHostWnd = pRootWnd->GetHostWnd();
    }
    if (pHostWnd)
    {
        pHostWnd->ShowHostWnd(SW_HIDE, TRUE);
    }
    m_pTab->SetCurSel(nIndex);
    if (pHostWnd)
    {
        pHostWnd->ShowHostWnd(SW_SHOW, TRUE);
    }
}

void SPageMgr::ShowPage(LPCTSTR pszName, BOOL bTitle)
{
    if (!m_pTab)
    {
        return;
    }

    ShowPage(m_pTab->GetPageIndex(pszName, bTitle));
}

void SPageMgr::InitStatusPage(connection_t *c)
{
    if (!m_pTab)
    {
        return;
    }

    TCHAR pageName[16];
    PAGE_NAME(pageName, c);
    int nIndex = m_pTab->GetPageIndex(pageName, TRUE);
    if (nIndex < 0)
    {
        nIndex = m_pTab->InsertItem();
        m_pTab->SetItemTitle(nIndex, pageName);
    }

    auto* pPage = m_pTab->GetItem(nIndex);
    if (!pPage)
    {
        return;
    }
    pPage->FindChildByName2<SWindow>(L"txt_name")->SetWindowText(c->config_name);
}

void SPageMgr::ReleaseStatusPage(connection_t *c)
{
    if (!m_pTab)
    {
        return;
    }

    int nIndex = GetStatusPageIndex(c);
    if (nIndex < 0)
    {
        return;
    }
    if (m_pTab->GetCurSel() == nIndex)
    {
        ShowPage(_T("page_home"), TRUE);
    }
    m_pTab->RemoveItem(nIndex);
}

void SPageMgr::ShowStatusPage(connection_t *c, BOOL bShow)
{
    if (bShow)
    {
        ShowPage(GetStatusPageIndex(c));
    }
    else
    {
        GetStatusWindow(c)->SetVisible(FALSE);
        ShowPage(_T("page_home"), TRUE);
    }
}

int SPageMgr::GetStatusPageIndex(connection_t *c)
{
    TCHAR pageName[16];
    PAGE_NAME(pageName, c);

    if (!m_pTab)
    {
        return -1;
    }

    return m_pTab->GetPageIndex(pageName, TRUE);
}

STabPage *SPageMgr::GetStatusPage(connection_t *c)
{
    if (!m_pTab)
    {
        return NULL;
    }

    int nIndex = GetStatusPageIndex(c);
    if (nIndex < 0)
    {
        return NULL;
    }
    return m_pTab->GetItem(nIndex);
}

STabPage *SPageMgr::GetStatusPage(SWindow *pWnd)
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

SWindow *SPageMgr::GetStatusWindow(connection_t *c, STabPage *pPage)
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

void SPageMgr::InitUserAuthDialog(auth_param_t *param, UINT dialogId)
{
    WCHAR username[USER_PASS_LEN] = L"";
    WCHAR password[USER_PASS_LEN] = L"";
    param->c->dialogId = dialogId;
    auto *pPage = GetStatusPage(param->c);
    if (!pPage)
    {
        return ;
    }
    auto *pWnd = GetStatusWindow(param->c, pPage);
    if (!pWnd)
    {
        return ;
    }

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
        ForceForegroundWindow(m_pTab->GetHostHwnd());
    }
    else
    {
        SetForegroundWindow(m_pTab->GetHostHwnd());
    }

    pPage->SetUserData(reinterpret_cast<ULONG_PTR>(param->c));
    //pWnd->FindChildByName2<SButton>(L"btn_confirm")->EnableWindow(FALSE);
    pWnd->FindChildByName2<SButton>(L"btn_confirm")->GetEventSet()->subscribeEvent(EventCmd::EventID,Subscriber(&SPageMgr::OnButtonClick, this));
    pWnd->FindChildByName2<SButton>(L"btn_cancel")->GetEventSet()->subscribeEvent(EventCmd::EventID,Subscriber(&SPageMgr::OnButtonClick, this));
    pWnd->FindChildByName2<SCheckBox>(L"chk_savepass")->GetEventSet()->subscribeEvent(EventCmd::EventID,Subscriber(&SPageMgr::OnCheckBoxClick, this));
    pWnd->FindChildByName2<SButton>(L"btn_password")->GetEventSet()->subscribeEvent(EventCmd::EventID,Subscriber(&SPageMgr::OnButtonClick, this));
    pWnd->FindChildByName2<SEdit>(L"edt_username")->GetEventSet()->subscribeEvent(EventKeyDown::EventID,Subscriber(&SPageMgr::OnKeyDown, this));
    pWnd->FindChildByName2<SEdit>(L"edt_password")->GetEventSet()->subscribeEvent(EventKeyDown::EventID,Subscriber(&SPageMgr::OnKeyDown, this));
    if (dialogId == ID_DLG_AUTH_CHALLENGE)
    {
        pWnd->FindChildByName2<SButton>(L"btn_challenge")->GetEventSet()->subscribeEvent(EventCmd::EventID,Subscriber(&SPageMgr::OnButtonClick, this));
        pWnd->FindChildByName2<SEdit>(L"edt_challenge")->GetEventSet()->subscribeEvent(EventKeyDown::EventID,Subscriber(&SPageMgr::OnKeyDown, this));
    }
    pWnd->SetVisible(TRUE);
    ShowStatusPage(param->c, TRUE);
}

void SPageMgr::InitGenericPassDialog(auth_param_t *param, UINT dialogId)
{
    param->c->dialogId = dialogId;
    auto *pPage = GetStatusPage(param->c);
    if (!pPage)
    {
        return ;
    }
    auto *pWnd = GetStatusWindow(param->c, pPage);
    if (!pWnd)
    {
        return ;
    }

    WCHAR *wstr = Widen(param->str);
    if (!wstr)
    {
        ::WriteStatusLog(
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
        ::WriteStatusLog(param->c, L"GUI> ", L"Unknown password request", false);
        pWnd->FindChildByName2<SStatic>(L"txt_description")->SetWindowText(wstr);
    }
    free(wstr);

    if (param->c->state == resuming)
    {
        ForceForegroundWindow(m_pTab->GetHostHwnd());
    }
    else
    {
        SetForegroundWindow(m_pTab->GetHostHwnd());
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
    pWnd->FindChildByName2<SButton>(L"btn_confirm")->GetEventSet()->subscribeEvent(EventCmd::EventID,Subscriber(&SPageMgr::OnButtonClick, this));
    pWnd->FindChildByName2<SButton>(L"btn_cancel")->GetEventSet()->subscribeEvent(EventCmd::EventID,Subscriber(&SPageMgr::OnButtonClick, this));
    pWnd->FindChildByName2<SCheckBox>(L"chk_savepass")->GetEventSet()->subscribeEvent(EventCmd::EventID,Subscriber(&SPageMgr::OnCheckBoxClick, this));
    pWnd->FindChildByName2<SButton>(L"btn_password")->GetEventSet()->subscribeEvent(EventCmd::EventID,Subscriber(&SPageMgr::OnButtonClick, this));
    pWnd->FindChildByName2<SEdit>(L"edt_response")->GetEventSet()->subscribeEvent(EventKeyDown::EventID,Subscriber(&SPageMgr::OnKeyDown, this));
    pWnd->SetVisible(TRUE);
    ShowStatusPage(param->c, TRUE);
}

void SPageMgr::InitPrivKeyPassDialog(connection_t *c, UINT dialogId)
{
    WCHAR passphrase[KEY_PASS_LEN];
    c->dialogId = dialogId;
    auto *pPage = GetStatusPage(c);
    if (!pPage)
    {
        return ;
    }
    auto *pWnd = GetStatusWindow(c, pPage);
    if (!pWnd)
    {
        return ;
    }

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
        ForceForegroundWindow(m_pTab->GetHostHwnd());
    }
    else
    {
        SetForegroundWindow(m_pTab->GetHostHwnd());
    }

    pPage->SetUserData(reinterpret_cast<ULONG_PTR>(c));
    /* disable OK button by default - not disabled in resources */
    pWnd->FindChildByName2<SButton>(L"btn_confirm")->EnableWindow(FALSE);
    pWnd->FindChildByName2<SButton>(L"btn_confirm")->GetEventSet()->subscribeEvent(EventCmd::EventID,Subscriber(&SPageMgr::OnButtonClick, this));
    pWnd->FindChildByName2<SButton>(L"btn_cancel")->GetEventSet()->subscribeEvent(EventCmd::EventID,Subscriber(&SPageMgr::OnButtonClick, this));
    pWnd->FindChildByName2<SCheckBox>(L"chk_savepass")->GetEventSet()->subscribeEvent(EventCmd::EventID,Subscriber(&SPageMgr::OnCheckBoxClick, this));
    pWnd->FindChildByName2<SButton>(L"btn_passphrase")->GetEventSet()->subscribeEvent(EventCmd::EventID,Subscriber(&SPageMgr::OnButtonClick, this));
    pWnd->FindChildByName2<SEdit>(L"edt_passphrase")->GetEventSet()->subscribeEvent(EventKeyDown::EventID,Subscriber(&SPageMgr::OnKeyDown, this));
    pWnd->SetVisible(TRUE);
    ShowStatusPage(c, TRUE);
}

void SPageMgr::InitProxyAuthDialog(connection_t *c, UINT dialogId)
{
    c->dialogId = dialogId;
    auto *pPage = GetStatusPage(c);
    if (!pPage)
    {
        return ;
    }
    auto *pWnd = GetStatusWindow(c, pPage);
    if (!pWnd)
    {
        return ;
    }
    if (c->state == resuming)
    {
        ForceForegroundWindow(m_pTab->GetHostHwnd());
    }
    else
    {
        SetForegroundWindow(m_pTab->GetHostHwnd());
    }

    pPage->SetUserData(reinterpret_cast<ULONG_PTR>(c));
    pWnd->FindChildByName2<SButton>(L"btn_confirm")->EnableWindow(FALSE);
    pWnd->FindChildByName2<SButton>(L"btn_confirm")->GetEventSet()->subscribeEvent(EventCmd::EventID,Subscriber(&SPageMgr::OnButtonClick, this));
    pWnd->FindChildByName2<SButton>(L"btn_password")->GetEventSet()->subscribeEvent(EventCmd::EventID,Subscriber(&SPageMgr::OnButtonClick, this));
    pWnd->FindChildByName2<SEdit>(L"edt_username")->GetEventSet()->subscribeEvent(EventKeyDown::EventID,Subscriber(&SPageMgr::OnKeyDown, this));
    pWnd->FindChildByName2<SEdit>(L"edt_password")->GetEventSet()->subscribeEvent(EventKeyDown::EventID,Subscriber(&SPageMgr::OnKeyDown, this));
    pWnd->SetVisible(TRUE);
    ShowStatusPage(c, TRUE);
}

BOOL SPageMgr::OnCheckBoxClick(EventCmd *pEvt)
{
    auto pPage = GetStatusPage(dynamic_cast<SWindow *>(pEvt->Sender()));
    if (!pPage)
    {
        return FALSE;
    }
    connection_t *c = reinterpret_cast<connection_t *>(pPage->GetUserData());;
    SimulateButtonPress(c->hwndDlg, ID_CHK_SAVE_PASS);
    return  TRUE;
}

BOOL SPageMgr::OnButtonClick(EventCmd *pEvt)
{
    WCHAR username[USER_PASS_LEN] = L"";
    WCHAR password[USER_PASS_LEN] = L"";
    auto* pBtn =  dynamic_cast<SButton *>(pEvt->Sender());
    if (!pBtn)
    {
        return FALSE;
    }
    auto* pPage = GetStatusPage(pBtn);
    if (!pPage)
    {
        return FALSE;
    }
    connection_t *c = reinterpret_cast<connection_t *>(pPage->GetUserData());;
    if (wcscmp(pBtn->GetName(), L"btn_confirm") == 0)
    {
        auto* pWnd = GetStatusWindow(c, pPage);
        if (!pWnd)
        {
            return FALSE;
        }
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

BOOL SPageMgr::OnKeyDown(EventKeyDown *pEvt)
{
    auto *pPage =  GetStatusPage(dynamic_cast<SWindow *>(pEvt->Sender()));
    connection_t *c = reinterpret_cast<connection_t *>(pPage->GetUserData());
    STaskHelper::postTask(m_pTab->GetContainer(), this, &SPageMgr::CheckEditEmpty, c);
    return TRUE;
}

void SPageMgr::CheckEditEmpty(connection_t *c)
{
    BOOL bEnable = TRUE;
    auto* pWnd = GetStatusWindow(c);
    if (!pWnd)
    {
        return ;
    }
    switch (c->dialogId)
    {
        case ID_DLG_AUTH_CHALLENGE:
            bEnable = pWnd->FindChildByName2<SEdit>(L"edt_challenge")->GetWindowTextLength() > 0;
        case ID_DLG_AUTH:
            bEnable = bEnable && pWnd->FindChildByName2<SEdit>(L"edt_password")->GetWindowTextLength() > 0;
            bEnable = bEnable && pWnd->FindChildByName2<SEdit>(L"edt_username")->GetWindowTextLength() > 0;
            break;
        case ID_DLG_CHALLENGE_RESPONSE:
            bEnable = pWnd->FindChildByName2<SEdit>(L"edt_response")->GetWindowTextLength() > 0;
            break;
        case ID_DLG_PROXY_AUTH:
            bEnable = pWnd->FindChildByName2<SEdit>(L"edt_password")->GetWindowTextLength() > 0;
            bEnable = bEnable && pWnd->FindChildByName2<SEdit>(L"edt_username")->GetWindowTextLength() > 0;
            break;
        case ID_DLG_PASSPHRASE:
            bEnable = pWnd->FindChildByName2<SEdit>(L"edt_passphrase")->GetWindowTextLength() > 0;
            break;
        default:
            return ;
    }
    pWnd->FindChildByName2<SButton>(L"btn_confirm")->EnableWindow(bEnable);
    pWnd->FindChildByName2<SButton>(L"btn_confirm")->Invalidate();
}

BOOL SPageMgr::ChangePasswordVisibility(SButton *pBtn)
{
    SStringT attr;
    auto *pPage = GetStatusPage(pBtn);
    if (!pPage)
    {
        return FALSE;
    }
    connection_t *c = reinterpret_cast<connection_t *>(pPage->GetUserData());;
    auto *pWnd = GetStatusWindow(c, pPage);
    if (!pWnd)
    {
        return FALSE;
    }
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
    pEdit->GetAttribute(L"password", &attr);
    if (attr.Compare(L"1") == 0)
    {
        pBtn->SetAttribute(L"skin", L"img_eye", FALSE);
        pEdit->SetAttribute(L"password", L"0", FALSE);
    }
    else
    {
        pBtn->SetAttribute(L"skin", L"img_eye_stroke", FALSE);
        pEdit->SetAttribute(L"password", L"1", FALSE);
    }
    return TRUE;
}

void SPageMgr::SetWarningText(connection_t* c, LPCTSTR pszText)
{
    auto *pWnd = GetStatusWindow(c);
    if (!pWnd)
    {
        return;
    }
    pWnd->FindChildByName2<SStatic>(L"txt_warning")->SetWindowText(pszText);
}

void SPageMgr::SetWarningColor(connection_t* c, COLORREF clr)
{
    WCHAR szColor[8];
    wsprintf(szColor, L"#%02x%02x%02x", GetRValue(clr), GetGValue(clr), GetBValue(clr));
    auto *pWnd = GetStatusWindow(c);
    if (!pWnd)
    {
        return;
    }
    pWnd->FindChildByName2<SStatic>(L"txt_warning")->SetAttribute(L"colorText", szColor);
}

void SPageMgr::HandleMessage(mgmt_rtmsg_type msg_type, connection_t* c, char* msg, BOOL bPre)
{
    if (msg_type == log_)
    {
        return;
    }

    if (!m_pTab)
    {
        return;
    }
    SetImageState(c);

    auto pTree = m_pTab->FindChildByName2<STreeView>(L"tv_home");
    if (!pTree)
    {
        return;
    }
    auto pAdapter = dynamic_cast<STreeAdapter *>(pTree->GetAdapter());
    if (!pAdapter)
    {
        return;
    }
    pAdapter->NotifyStateChange();
}

void SPageMgr::WriteLogLine(connection_t *c, char *msg)
{
    time_t timestamp;
    wchar_t datetime[32];
    const SETTEXTEX ste = {ST_SELECTION, CP_UTF8 };
    auto* pPage = GetStatusPage(c);
    if (!pPage)
    {
        return;
    }
    auto* pLogWnd = pPage->FindChildByName2<SRichEdit>(L"log_viewer");
    if (!pLogWnd)
    {
        return;
    }

    char *flags = strchr(msg, ',');
    if (flags == NULL)
    {
        return;
    }
    flags++;

    char *message = strchr(flags, ',');
    if (message == NULL)
    {
        return;
    }
    message++;
    size_t flag_size = message - flags - 1; /* message is always > flags */

    /* Remove lines from log window if it is getting full */
    if (pLogWnd->SSendMessage(EM_GETLINECOUNT, 0, 0) > MAX_LOG_LINES)
    {
        int pos = pLogWnd->SSendMessage(EM_LINEINDEX, DEL_LOG_LINES, 0);
        pLogWnd->SSendMessage(EM_SETSEL, 0, pos);
        pLogWnd->SSendMessage(EM_REPLACESEL, FALSE, (LPARAM) _T(""));
    }

    timestamp = strtol(msg, NULL, 10);
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

    //if (text_clr != 0)
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
    pLogWnd->SSendMessage(EM_REPLACESEL, FALSE, (LPARAM) _T("\n"));

    /* scroll to the caret */
    //pLogWnd->SSendMessage(EM_SCROLLCARET, 0, 0);
    if (pLogWnd->SSendMessage(EM_GETLINECOUNT, 0, 0) > 20)
    {
        pLogWnd->SSendMessage(EM_SCROLL, SB_BOTTOM, 0);
    }
    else if (pLogWnd->SSendMessage(EM_GETLINECOUNT, 0, 0) > 10)
    {
        pLogWnd->SSendMessage(EM_SCROLL, SB_LINEDOWN, 0);
    }
}

void SPageMgr::WriteStatusLog(connection_t *c, LPCWSTR prefix, LPCWSTR msg)
{
    time_t now;
    wchar_t datetime[32];
    auto* pPage = GetStatusPage(c);
    if (!pPage)
    {
        return;
    }
    auto* pLogWnd = pPage->FindChildByName2<SRichEdit>(L"log_viewer");
    if (!pLogWnd)
    {
        return;
    }

    now = time(0);
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

    //if (text_clr != 0)
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

    /* scroll to the caret */
    if (pLogWnd->SSendMessage(EM_GETLINECOUNT, 0, 0) > 20)
    {
        pLogWnd->SSendMessage(EM_SCROLL, SB_BOTTOM, 0);
    }
    else if (pLogWnd->SSendMessage(EM_GETLINECOUNT, 0, 0) > 10)
    {
        pLogWnd->SSendMessage(EM_SCROLL, SB_LINEDOWN, 0);
    }
}

void SPageMgr::SetImageState(connection_t *c)
{
    auto* pPage = GetStatusPage(c);
    if (!pPage)
    {
        return;
    }

    switch (c->state)
    {
        case connected:
            pPage->FindChildByName(L"img_state")->SetAttribute(L"iconIndex", L"2");
            break;
        case connecting:
        case reconnecting:
        case disconnecting:
            pPage->FindChildByName(L"img_state")->SetAttribute(L"iconIndex", L"1");
            break;
        case disconnected:
            pPage->FindChildByName(L"img_state")->SetAttribute(L"iconIndex", L"0");
            break;
        case detached:
        case detaching:
        case onhold:
        case resuming:
        case suspended:
        default:
            pPage->FindChildByName(L"img_state")->SetAttribute(L"iconIndex", L"0");
            break;
    }
    pPage->Invalidate();
}