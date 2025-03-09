/**
 * @file STabCtrlEx.cpp
 * @brief Implementation file for the STabCtrlEx class.
 * @details This file contains the implementation of the STabCtrlEx class, which extends the STabCtrl class with XML template functionality.
 * @author UxGood <uxgood.org@gmail.com>
 * @date 2025-03-05
 */

#include "STabCtrlEx.h"
SNSBEGIN

const wchar_t * KTemplate_Page = L"pageTemplate";
STabCtrlEx::STabCtrlEx(void)
{

}

STabCtrlEx::~STabCtrlEx(void)
{

}

int STabCtrlEx::InsertItem(int iInsert/* = -1*/, BOOL bLoading/* = FALSE*/)
{
	int idx = STabCtrl::InsertItem(m_xmlTemplate.root().first_child().child(L"page"), iInsert, bLoading);
	auto pPage = GetItem(idx);
	pPage->GetRoot()->SDispatchMessage(UM_SETSCALE, GetScale(), 0);
	return idx;
}

int STabCtrlEx::InsertItem(LPCTSTR lpszTitle, int iInsert/* = -1*/, BOOL bLoading/* = FALSE*/)
{
    int idx = InsertItem(iInsert, bLoading);
    this->SetItemTitle(idx, lpszTitle);
    return idx;
}

void STabCtrlEx::OnInitFinished(IXmlNode *xmlNode)
{
	__super::OnInitFinished(xmlNode);
	m_xmlTemplate.root().append_copy(xmlNode->Child(KTemplate_Page, TRUE));
}

SNSEND