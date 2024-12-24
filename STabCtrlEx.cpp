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
	return STabCtrl::InsertItem(m_xmlTemplate.root().first_child().child(L"page"), -1, TRUE);
}

void STabCtrlEx::OnInitFinished(IXmlNode *xmlNode)
{
	__super::OnInitFinished(xmlNode);
	m_xmlTemplate.root().append_copy(xmlNode->Child(KTemplate_Page, TRUE));
}

SNSEND