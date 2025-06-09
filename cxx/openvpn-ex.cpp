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
    pMainDlg->ShowPage(nIndex);
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

void InitStatusPage(connection_t* c)
{
    auto *obj = TaskSingleton::getInstance();
    STaskHelper::post(pMainDlg->GetMsgLoop(), obj, &TaskSingleton::InitStatusPage, c);
}

void ReleaseStatusPage(connection_t* c)
{
    auto *obj = TaskSingleton::getInstance();
    STaskHelper::post(pMainDlg->GetMsgLoop(), obj, &TaskSingleton::ReleaseStatusPage, c);
}

void ShowStatusPage(connection_t* c, BOOL bShow)
{
    auto *obj = TaskSingleton::getInstance();
    STaskHelper::post(pMainDlg->GetMsgLoop(), obj, &TaskSingleton::ShowStatusPage, c, bShow);
}