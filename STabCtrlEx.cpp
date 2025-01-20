#include "STabCtrlEx.h"
SNSBEGIN

const wchar_t * KTemplate_Page = L"pageTemplate";
STabCtrlEx::STabCtrlEx(void)
{

}

STabCtrlEx::~STabCtrlEx(void)
{

}

int STabCtrlEx::InsertItem()
{
	int idx = STabCtrl::InsertItem(m_xmlTemplate.root().first_child().child(L"page"), -1, TRUE);
	auto pPage = GetItem(idx);
	pPage->GetRoot()->SDispatchMessage(UM_SETSCALE, GetScale(), 0);
	return idx;
}

void STabCtrlEx::OnInitFinished(IXmlNode *xmlNode)
{
	__super::OnInitFinished(xmlNode);
	m_xmlTemplate.root().append_copy(xmlNode->Child(KTemplate_Page, TRUE));
}

SNSEND