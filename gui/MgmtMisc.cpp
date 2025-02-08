#include "stdafx.h"
#include "openvpnex.h"

#include "MainDlg.h"
#include "MgmtMisc.h"

extern "C" {
#include "proxy.h"
#include "save_pass.h"
#include "misc.h"
#include "localization.h"
#include "openvpn-gui-res.h"
    static mgmt_msg_func msg_handler[mgmt_rtmsg_type_max];
    void free_dynamic_cr(connection_t *c);
    BOOL parse_input_request(const char *msg, auth_param_t *param);
}
extern CMainDlg* pDlgMain;

MgmtMisc::MgmtMisc()
{
}

MgmtMisc::~MgmtMisc()
{
}

void MgmtMisc::OnReady(connection_t* c, char* msg)
{
    OnStateChange(c, msg);
}

void MgmtMisc::OnHold(connection_t* c, char* msg)
{
    OnStateChange(c, msg);
}

void MgmtMisc::OnLogLine(connection_t* c, char* msg)
{

}

void MgmtMisc::OnStateChange(connection_t* c, char* msg)
{
    DbgPrintf(_T("%s(%d): %hs"), _T(__FUNCTION__), __LINE__, msg);
    pDlgMain->RefreshTree();
    auto* pPage = GetPage(c);
    if (!pPage)
    {
        return;
    }
    auto* pImg = pPage->FindChildByName(L"img_state");
    auto* pGif = pPage->FindChildByName(L"gif_state");
    switch (c->state)
    {
    case connected:
        pImg->SetAttribute(L"skin", L"skin_connected");
        pImg->SetVisible(TRUE);
        pGif->SetVisible(FALSE);
        break;
    case connecting:
        pGif->SetVisible(TRUE);
        pImg->SetVisible(FALSE);
        break;
    case disconnected:
        pImg->SetAttribute(L"skin", L"skin_disconnected");
        pImg->SetVisible(TRUE);
        pGif->SetVisible(FALSE);
        break;
    default:
        break;
    }
}

void MgmtMisc::OnPassword(connection_t* c, char* msg)
{
    auto* pPage = GetPage(c);
    PrintDebug(L"OnPassword with msg = %hs", msg);
    if (strncmp(msg, "Verification Failed", 19) == 0)
    {
        /* If the failure is due to dynamic challenge save the challenge string */
        char *chstr = strstr(msg, "CRV1:");

        free_dynamic_cr(c);
        if (chstr)
        {
            chstr += 5; /* beginning of dynamic CR string */

            /* Save the string for later processing during next Auth request */
            c->dynamic_cr = strdup(chstr);
            if (c->dynamic_cr && (chstr =  strstr(c->dynamic_cr, "']")) != NULL)
            {
                *chstr = '\0';
            }

            PrintDebug(L"Got dynamic challenge: <%hs>", c->dynamic_cr);
        }

        return;
    }

    if (strstr(msg, "'Auth'"))
    {
        char *chstr;
        auth_param_t *param = (auth_param_t *) calloc(1, sizeof(auth_param_t));

        if (!param)
        {
            ::WriteStatusLog(c, L"GUI> ", L"Error: Out of memory - ignoring user-auth request", false);
            return;
        }
        param->c = c;

        if (c->dynamic_cr)
        {
            if (!parse_dynamic_cr(c->dynamic_cr, param))
            {
                ::WriteStatusLog(c, L"GUI> ", L"Error parsing dynamic challenge string", FALSE);
                free_dynamic_cr(c);
                free_auth_param(param);
                return;
            }
            //LocalizedDialogBoxParamEx(ID_DLG_CHALLENGE_RESPONSE, c->hwndStatus, GenericPassDialogFunc, (LPARAM) param);
            auto* pWnd = pPage->FindChildByName2<SWindow>(L"wnd_response");
            LoadAuthDialogStatus(pWnd, param);
            free_dynamic_cr(c);
        }
        else if ( (chstr = strstr(msg, "SC:")) && strlen(chstr) > 5)
        {
            ULONG flags = strtoul(chstr+3, NULL, 10);
            param->flags |= (flags & 0x2) ? FLAG_CR_TYPE_CONCAT : FLAG_CR_TYPE_SCRV1;
            param->flags |= (flags & 0x1) ? FLAG_CR_ECHO : 0;
            param->str = strdup(chstr + 5);
            //LocalizedDialogBoxParamEx(ID_DLG_AUTH_CHALLENGE, c->hwndStatus, UserAuthDialogFunc, (LPARAM) param);
            auto* pWnd = pPage->FindChildByName2<SWindow>(L"wnd_auth");
            LoadAuthDialogStatus(pWnd, param);
        }
        else if (o.auth_pass_concat_otp)
        {
            param->flags |= FLAG_CR_ECHO | FLAG_CR_TYPE_CONCAT;
            //LocalizedDialogBoxParamEx(ID_DLG_AUTH_CHALLENGE, c->hwndStatus, UserAuthDialogFunc, (LPARAM) param);
            auto* pWnd = pPage->FindChildByName2<SWindow>(L"wnd_auth");
            LoadAuthDialogStatus(pWnd, param);
        }
        else
        {
            //LocalizedDialogBoxParamEx(ID_DLG_AUTH, c->hwndStatus, UserAuthDialogFunc, (LPARAM) param);
            auto* pWnd = pPage->FindChildByName2<SWindow>(L"wnd_auth");
            pWnd->FindChildByName2<SStatic>(L"txt_challenge")->GetParent()->SetVisible(FALSE);
            pWnd->FindChildByName2<SEdit>(L"edt_challenge")->GetParent()->SetVisible(FALSE);
            LoadAuthDialogStatus(pWnd, param);
        }
    }
    else if (strstr(msg, "'Private Key'"))
    {
        //LocalizedDialogBoxParamEx(ID_DLG_PASSPHRASE, c->hwndStatus, PrivKeyPassDialogFunc, (LPARAM) c);
        auto* pWnd = pPage->FindChildByName2<SWindow>(L"wnd_password");
        pWnd->SetVisible(TRUE);
    }
    else if (strstr(msg, "'HTTP Proxy'"))
    {
        QueryProxyAuth(c, http);
    }
    else if (strstr(msg, "'SOCKS Proxy'"))
    {
        QueryProxyAuth(c, socks);
    }
    /* All other password requests such as PKCS11 pin */
    else if (strncmp(msg, "Need '", 6) == 0)
    {
        auth_param_t *param = (auth_param_t *) calloc(1, sizeof(auth_param_t));

        if (!param)
        {
            ::WriteStatusLog(c, L"GUI> ", L"Error: Out of memory - ignoring user-auth request", false);
            return;
        }
        param->c = c;
        if (!parse_input_request(msg, param))
        {
            free_auth_param(param);
            return;
        }
        //LocalizedDialogBoxParamEx(ID_DLG_CHALLENGE_RESPONSE, c->hwndStatus, GenericPassDialogFunc, (LPARAM) param);
        auto* pWnd = pPage->FindChildByName2<SWindow>(L"wnd_response");
    }
}

void MgmtMisc::OnProxy(connection_t* c, char* msg)
{

}

void MgmtMisc::OnStop(connection_t* c, char* msg)
{
    OnStateChange(c, msg);
}

void MgmtMisc::OnNeedOk(connection_t* c, char* msg)
{

}

void MgmtMisc::OnNeedStr(connection_t* c, char* msg)
{

}

void MgmtMisc::OnEcho(connection_t* c, char* msg)
{

}

void MgmtMisc::OnByteCount(connection_t* c, char* msg)
{
    OnStateChange(c, msg);
}

void MgmtMisc::OnInfoMsg(connection_t* c, char* msg)
{

}

void MgmtMisc::OnTimeout(connection_t* c, char* msg)
{

}


void MgmtMisc::WriteStatusLog(connection_t *c, LPCWSTR prefix, LPCWSTR msg)
{

}

void MgmtMisc::InitStatusPage(connection_t* c)
{
    TCHAR pageName[16];
    _stprintf_s(pageName, _countof(pageName), _T("page_%08x"), c->id);
    auto* pTab = pDlgMain->FindChildByName2<STabCtrlEx>(L"tab_main");
    int nIndex = pTab->GetPageIndex(pageName, TRUE);
    if (nIndex < 0)
    {
        nIndex = pTab->InsertItem();
        pTab->SetItemTitle(nIndex, pageName);
    }
    //ShowPage(nIndex);
    auto* pPage = pTab->GetItem(nIndex);
    pPage->FindChildByName2<SWindow>(L"txt_name")->SetWindowText(c->config_name);
    auto* pWnd = pPage->FindChildByName2<SWindow>(L"wnd_auth");
    auto* pBtn = pWnd->FindChildByName2<SButton>(L"btn_confirm");
    pBtn->GetEventSet()->subscribeEvent(EventCmd::EventID,Subscriber(&MgmtMisc::AuthWindow_OnConfirmClick, this));
    pBtn = pWnd->FindChildByName2<SButton>(L"btn_cancel");
    pBtn->GetEventSet()->subscribeEvent(EventCmd::EventID,Subscriber(&MgmtMisc::AuthWindow_OnCancelClick, this));

    pWnd = pPage->FindChildByName2<SWindow>(L"wnd_auth2");
    pBtn = pWnd->FindChildByName2<SButton>(L"btn_confirm");
    //pBtn->GetEventSet()->subscribeEvent(EventCmd::EventID,Subscriber(&CMainDlg::OnAuth2Confirm, this));
    pBtn = pWnd->FindChildByName2<SButton>(L"btn_cancel");
    //pBtn->GetEventSet()->subscribeEvent(EventCmd::EventID,Subscriber(&CMainDlg::OnAuth2Cancel, this));
    pWnd = pPage->FindChildByName2<SWindow>(L"wnd_response");
    pBtn = pWnd->FindChildByName2<SButton>(L"btn_confirm");
    //pBtn->GetEventSet()->subscribeEvent(EventCmd::EventID,Subscriber(&CMainDlg::OnResponseConfirm, this));
    pBtn = pWnd->FindChildByName2<SButton>(L"btn_cancel");
    //pBtn->GetEventSet()->subscribeEvent(EventCmd::EventID,Subscriber(&CMainDlg::OnResponseCancel, this));
    pWnd = pPage->FindChildByName2<SWindow>(L"wnd_password");
    pBtn = pWnd->FindChildByName2<SButton>(L"btn_confirm");
    //pBtn->GetEventSet()->subscribeEvent(EventCmd::EventID,Subscriber(&CMainDlg::OnPassConfirm, this));
    pBtn = pWnd->FindChildByName2<SButton>(L"btn_cancel");
    //pBtn->GetEventSet()->subscribeEvent(EventCmd::EventID,Subscriber(&CMainDlg::OnPassCancel, this));
    pWnd = pPage->FindChildByName2<SWindow>(L"wnd_chpass");
    pBtn = pWnd->FindChildByName2<SButton>(L"btn_confirm");
    //pBtn->GetEventSet()->subscribeEvent(EventCmd::EventID,Subscriber(&CMainDlg::OnChpassConfirm, this));
    pBtn = pWnd->FindChildByName2<SButton>(L"btn_cancel");
    //pBtn->GetEventSet()->subscribeEvent(EventCmd::EventID,Subscriber(&CMainDlg::OnChpassCancel, this));
}

void MgmtMisc::ReleaseStatusPage(connection_t* c)
{
    TCHAR pageName[16];
    _stprintf_s(pageName, _countof(pageName), _T("page_%08x"), c->id);
    auto* pTab = pDlgMain->FindChildByName2<STabCtrlEx>(L"tab_main");
    int nIndex = pTab->GetPageIndex(pageName, TRUE);
    if (nIndex >= 0)
    {
        if (pTab->GetCurSel() == nIndex)
        {
            pDlgMain->ShowPage(_T("page_home"));
        }
        pTab->RemoveItem(nIndex);
    }
}

void MgmtMisc::ShowPage(connection_t* c, BOOL bShow)
{
    if (bShow)
    {
        TCHAR pageName[16];
        _stprintf_s(pageName, _countof(pageName), _T("page_%08x"), c->id);
        pDlgMain->ShowPage(pageName, TRUE);
    } else
    {
        pDlgMain->ShowPage(_T("page_home"), TRUE);
    }
}

void MgmtMisc::LoadAuthDialogStatus(SWindow *pWnd, auth_param_t *param)
{
    WCHAR username[USER_PASS_LEN] = L"";
    WCHAR password[USER_PASS_LEN] = L"";
    pWnd->SetVisible(TRUE);
    ShowPage(param->c, TRUE);
    if (param->str)
    {
        LPWSTR wstr = Widen(param->str);
        //HWND wnd_challenge = GetDlgItem(hwndDlg, ID_EDT_AUTH_CHALLENGE);
        auto *pEdit = pWnd->FindChildByName2<SEdit>(L"edt_challenge");

        if (!wstr)
        {
            ::WriteStatusLog(param->c, L"GUI> ", L"Error converting challenge string to widechar", false);
        }
        else
        {
            //SetDlgItemTextW(hwndDlg, ID_TXT_AUTH_CHALLENGE, wstr);
            pWnd->FindChildByName2<SStatic>(L"txt_challenge")->SetWindowText(wstr);
        }

        free(wstr);

        /* Set/Remove style ES_PASSWORD by SetWindowLong(GWL_STYLE) does nothing,
         * send EM_SETPASSWORDCHAR just works. */
        if (param->flags & FLAG_CR_ECHO)
        {
            //SendMessage(wnd_challenge, EM_SETPASSWORDCHAR, 0, 0);
            pEdit->SetAttribute(L"password", L"", TRUE);
        }

    }
    else if (param->flags & FLAG_CR_TYPE_CONCAT)
    {
        //SetDlgItemTextW(hwndDlg, ID_TXT_AUTH_CHALLENGE, LoadLocalizedString(IDS_NFO_OTP_PROMPT));
        pWnd->FindChildByName2<SStatic>(L"txt_challenge")->SetWindowText(LoadLocalizedString(IDS_NFO_OTP_PROMPT));

    }
    if (RecallUsername(param->c->config_name, username))
    {
        //SetDlgItemTextW(hwndDlg, ID_EDT_AUTH_USER, username);
        //SetFocus(GetDlgItem(hwndDlg, ID_EDT_AUTH_PASS));
        pWnd->FindChildByName2<SEdit>(L"edt_username")->SetWindowText(username);
        pWnd->FindChildByName2<SEdit>(L"edt_password")->SetFocus();
    }
    if (RecallAuthPass(param->c->config_name, password))
    {
        //SetDlgItemTextW(hwndDlg, ID_EDT_AUTH_PASS, password);
        pWnd->FindChildByName2<SEdit>(L"edt_password")->SetWindowText(password);
        if (username[0] != L'\0' && !(param->flags & (FLAG_CR_TYPE_SCRV1|FLAG_CR_TYPE_CONCAT))
            && password[0] != L'\0' && param->c->failed_auth_attempts == 0)
        {
            /* user/pass available and no challenge response needed: skip dialog
             * if silent_connection is on, else auto submit after a few seconds.
             * User can interrupt.
             */
            //SetFocus(GetDlgItem(hwndDlg, IDOK));
            pWnd->FindChildByName2<SButton>(L"btn_confirm")->SetFocus();
            UINT timeout = o.silent_connection ? 0 : 6; /* in seconds */
            //AutoCloseSetup(hwndDlg, IDOK, timeout, ID_TXT_WARNING, IDS_NFO_AUTO_CONNECT);
        }
        /* if auth failed, highlight password so that user can type over */
        else if (param->c->failed_auth_attempts)
        {
            //SendMessage(GetDlgItem(hwndDlg, ID_EDT_AUTH_PASS), EM_SETSEL, 0, MAKELONG(0, -1));
            pWnd->FindChildByName2<SEdit>(L"edt_password")->SetSel(0, -1,0);
            pWnd->FindChildByName2<SEdit>(L"edt_password")->SetFocus();
        }
        else if (param->flags & (FLAG_CR_TYPE_SCRV1|FLAG_CR_TYPE_CONCAT))
        {
            //SetFocus(GetDlgItem(hwndDlg, ID_EDT_AUTH_CHALLENGE));
            pWnd->FindChildByName2<SEdit>(L"edt_challenge")->SetFocus();
        }
        SecureZeroMemory(password, sizeof(password));
    }
    if (param->c->flags & FLAG_DISABLE_SAVE_PASS)
    {
        //ShowWindow(GetDlgItem(hwndDlg, ID_CHK_SAVE_PASS), SW_HIDE);
        pWnd->FindChildByName2<SCheckBox>(L"chk_savepass")->SetVisible(FALSE);
    }
    else if (param->c->flags & FLAG_SAVE_AUTH_PASS)
    {
        //Button_SetCheck(GetDlgItem(hwndDlg, ID_CHK_SAVE_PASS), BST_CHECKED);
        pWnd->FindChildByName2<SCheckBox>(L"chk_savepass")->SetCheck(TRUE);
    }

    //SetWindowText(hwndDlg, param->c->config_name);
    if (param->c->failed_auth_attempts > 0)
    {
        //SetDlgItemTextW(hwndDlg, ID_TXT_WARNING, LoadLocalizedString(IDS_NFO_AUTH_PASS_RETRY));
        pWnd->FindChildByName2<SStatic>(L"txt_warning")->SetWindowText(LoadLocalizedString(IDS_NFO_AUTH_PASS_RETRY));
    }

    if (param->c->state == resuming)
    {
        ForceForegroundWindow(pDlgMain->GetHwnd());
    }
    else
    {
        SetForegroundWindow(pDlgMain->GetHwnd());
    }
    //ResetPasswordReveal(GetDlgItem(hwndDlg, ID_EDT_AUTH_PASS),
    //                    GetDlgItem(hwndDlg, ID_PASSWORD_REVEAL), 0);
    pWnd->FindChildByName2<SButton>(L"btn_confirm")->SetUserData(reinterpret_cast<ULONG_PTR>(param));
    pWnd->FindChildByName2<SButton>(L"btn_cancel")->SetUserData(reinterpret_cast<ULONG_PTR>(param));
}

STabPage *MgmtMisc::GetPage(connection_t *c)
{
    TCHAR pageName[16];
    _stprintf_s(pageName, _countof(pageName), _T("page_%08x"), c->id);
    auto* pTab = pDlgMain->FindChildByName2<STabCtrlEx>(L"tab_main");
    return pTab->GetPage(pageName, TRUE);
}

/*
 * Generate a management command from user input and send it
 */
BOOL MgmtMisc::ManagementCommandFromInput(connection_t *c, LPCSTR fmt, SWindow *pWnd, LPCTSTR name)
{
    BOOL retval = FALSE;
    LPSTR input, cmd;
    int input_len, cmd_len;
    SStringA pStr;

    //GetDlgItemTextUtf8(hDlg, id, &input, &input_len);
    auto* pEdit = pWnd->FindChildByName2<SEdit>(name);
    input_len = pEdit->GetWindowTextU8(&pStr, TRUE);
    input = (LPSTR)malloc(input_len);
    strncpy(input, pStr.GetBuffer(), input_len);
    /* Escape input if needed */
    char *input_e = escape_string(input);
    if (!input_e)
    {
        goto out;
    }
    input = input_e;
    input_len = strlen(input);

    cmd_len = input_len + strlen(fmt);
    cmd = (LPSTR)malloc(cmd_len);
    if (cmd)
    {
        snprintf(cmd, cmd_len, fmt, input);
        retval = ManagementCommand(c, cmd, NULL, regular);
        free(cmd);
    }

    out:
        /* Clear buffers with potentially secret content */
        if (input_len)
        {
            memset(input, 'x', input_len);
            //SetDlgItemTextA(hDlg, id, input);
            pEdit->SetWindowTextU8(input);
        }

    return retval;
}

/*
 * Generate a management command from base64-encoded user inputs and send it
 */
BOOL MgmtMisc::ManagementCommandFromInputBase64(connection_t *c, LPCSTR fmt, SWindow *pWnd, LPCTSTR name)
{
    BOOL retval = FALSE;
    LPSTR input, input_b64, cmd;
    int input_len, cmd_len;
    SStringA pStr;

    input_b64 = NULL;

    //GetDlgItemTextUtf8(hDlg, id, &input, &input_len);
    auto* pEdit = pWnd->FindChildByName2<SEdit>(name);
    input_len = pEdit->GetWindowTextU8(&pStr, TRUE);
    input = (LPSTR)malloc(input_len);
    strncpy(input, pStr.GetBuffer(), input_len);

    if (!Base64Encode(input, input_len, &input_b64))
    {
        goto out;
    }

    cmd_len = strlen(input_b64) + strlen(fmt);
    cmd = (LPSTR)malloc(cmd_len);
    if (cmd)
    {
        snprintf(cmd, cmd_len, fmt, input_b64);
        retval = ManagementCommand(c, cmd, NULL, regular);
        free(cmd);
    }

    out:
        /* Clear buffers with potentially secret content */
        if (input_b64)
        {
            memset(input_b64, 0, strlen(input_b64));
        }
    free(input_b64);

    if (input_len)
    {
        memset(input, 'x', input_len);
        //SetDlgItemTextA(hDlg, id, input);
        pEdit->SetWindowTextU8(input);
        free(input);
    }

    return retval;
}

/*
 * Generate a management command from double user inputs and send it
 */
BOOL MgmtMisc::ManagementCommandFromTwoInputsBase64(connection_t *c, LPCSTR fmt, SWindow *pWnd, LPCTSTR name, LPCTSTR name2)
{
    BOOL retval = FALSE;
    LPSTR input, input2, input_b64, input2_b64, cmd;
    int input_len, input2_len, cmd_len;
    SStringA pStr;

    input_b64 = NULL;
    input2_b64 = NULL;

    //GetDlgItemTextUtf8(hDlg, id, &input, &input_len);
    //GetDlgItemTextUtf8(hDlg, id2, &input2, &input2_len);
    auto* pEdit = pWnd->FindChildByName2<SEdit>(name);
    input_len = pEdit->GetWindowTextU8(&pStr, TRUE);
    input = (LPSTR)malloc(input_len);
    strncpy(input, pStr.GetBuffer(), input_len);
    auto* pEdit2 = pWnd->FindChildByName2<SEdit>(name2);
    input2_len = pEdit->GetWindowTextU8(&pStr, TRUE);
    input2 = (LPSTR)malloc(input2_len);
    strncpy(input2, pStr.GetBuffer(), input_len);

    if (!Base64Encode(input, input_len, &input_b64))
    {
        goto out;
    }
    if (!Base64Encode(input2, input2_len, &input2_b64))
    {
        goto out;
    }

    cmd_len = strlen(input_b64) + strlen(input2_b64) + strlen(fmt);
    cmd = (LPSTR)malloc(cmd_len);
    if (cmd)
    {
        snprintf(cmd, cmd_len, fmt, input_b64, input2_b64);
        retval = ManagementCommand(c, cmd, NULL, regular);
        free(cmd);
    }

    out:
        /* Clear buffers with potentially secret content */
        if (input_b64)
        {
            memset(input_b64, 0, strlen(input_b64));
        }
    if (input2_b64)
    {
        memset(input2_b64, 0, strlen(input2_b64));
    }
    free(input_b64);
    free(input2_b64);

    if (input_len)
    {
        memset(input, 'x', input_len);
        //SetDlgItemTextA(hDlg, id, input);
        pEdit->SetWindowTextU8(input);
        free(input);
    }
    if (input2_len)
    {
        memset(input2, 'x', input2_len);
        //SetDlgItemTextA(hDlg, id2, input2);
        pEdit2->SetWindowTextU8(input2);
        free(input2);
    }

    return retval;
}

BOOL MgmtMisc::AuthWindow_OnConfirmClick(EventCmd* pEvt)
{
    WCHAR username[USER_PASS_LEN] = L"";
    WCHAR password[USER_PASS_LEN] = L"";
    auto* pBtn =  static_cast<SButton*>(pEvt->Sender());
    auth_param_t *param = reinterpret_cast<auth_param_t *>(pBtn->GetUserData());
    auto* pPage = GetPage(param->c);
    auto* pWnd = pPage->FindChildByName2<SWindow>(L"wnd_auth");
    pWnd->FindChildByName2<SEdit>(L"edt_username")->GetWindowTextW(username, _countof(username), TRUE);
    //if (GetDlgItemTextW(hwndDlg, ID_EDT_AUTH_USER, username, _countof(username)))
    if (pWnd->FindChildByName2<SEdit>(L"edt_username")->GetWindowTextW(username, _countof(username), TRUE))
    {
        if (!validate_input(username, L"\n"))
        {
            //show_error_tip(GetDlgItem(hwndDlg, ID_EDT_AUTH_USER), LoadLocalizedString(IDS_ERR_INVALID_USERNAME_INPUT));
            return 0;
        }
        SaveUsername(param->c->config_name, username);
    }
    //if (GetDlgItemTextW(hwndDlg, ID_EDT_AUTH_PASS, password, _countof(password)))
    if (pWnd->FindChildByName2<SEdit>(L"edt_password")->GetWindowTextW(password, _countof(password), TRUE))
    {
        if (!validate_input(password, L"\n"))
        {
            //show_error_tip(GetDlgItem(hwndDlg, ID_EDT_AUTH_PASS), LoadLocalizedString(IDS_ERR_INVALID_PASSWORD_INPUT));
            SecureZeroMemory(password, sizeof(password));
            return 0;
        }
        if (param->c->flags & FLAG_SAVE_AUTH_PASS && wcslen(password) )
        {
            SaveAuthPass(param->c->config_name, password);
        }
        if (param->flags & FLAG_CR_TYPE_CONCAT)
        {
            pWnd->FindChildByName2<SEdit>(L"edt_challenge")->GetWindowText(password + wcslen(password), _countof(password)-wcslen(password), TRUE);
            pWnd->FindChildByName2<SEdit>(L"edt_password")->SetWindowTextW(password);
        //     GetDlgItemTextW(hwndDlg, ID_EDT_AUTH_CHALLENGE, password + wcslen(password), _countof(password)-wcslen(password));
        //     SetDlgItemTextW(hwndDlg, ID_EDT_AUTH_PASS, password);
             /* erase potentially secret contents in the response text box */
            memset(password, L'x', wcslen(password));
        //     SetDlgItemTextW(hwndDlg, ID_EDT_AUTH_CHALLENGE, password);
            pWnd->FindChildByName2<SEdit>(L"edt_challenge")->SetWindowTextW(password);
        }

        SecureZeroMemory(password, sizeof(password));
    }
    //ManagementCommandFromInput(c, "username \"Auth\" \"%s\"", hwndDlg, ID_EDT_AUTH_USER);
    ManagementCommandFromInput(param->c, "username \"Auth\" \"%s\"", pWnd, _T("edt_username"));

    if (param->flags & FLAG_CR_TYPE_SCRV1)
    {
    //     ManagementCommandFromTwoInputsBase64(param->c, "password \"Auth\" \"SCRV1:%s:%s\"", hwndDlg, ID_EDT_AUTH_PASS, ID_EDT_AUTH_CHALLENGE);
        ManagementCommandFromTwoInputsBase64(param->c, "password \"Auth\" \"SCRV1:%s:%s\"", pWnd, _T("edt_password"), _T("edt_challenge"));
    }
    else
    {
    //     ManagementCommandFromInput(param->c, "password \"Auth\" \"%s\"", hwndDlg, ID_EDT_AUTH_PASS);
        ManagementCommandFromInput(param->c, "password \"Auth\" \"%s\"", pWnd, _T("edt_password"));
    }
    // EndDialog(hwndDlg, LOWORD(wParam));
    ShowPage(param->c, FALSE);
    pWnd->SetVisible(FALSE);
    return TRUE;
}

BOOL MgmtMisc::AuthWindow_OnCancelClick(EventCmd* pEvt)
{
    auto* pBtn =  static_cast<SButton*>(pEvt->Sender());
    auth_param_t *param = reinterpret_cast<auth_param_t *>(pBtn->GetUserData());
    auto* pPage = GetPage(param->c);
    auto* pWnd = pPage->FindChildByName2<SWindow>(L"wnd_auth");
    StopOpenVPN(param->c);
    pWnd->SetVisible(FALSE);
    return TRUE;
}
