
#ifndef __STABCTRLEX__H__
#define __STABCTRLEX__H__
#include <souistd.h>
#include <control/STabCtrl.h>

#include <tchar.h>
SNSBEGIN

class STabCtrlEx:public STabCtrl
{
	 DEF_SOBJECT(STabCtrl, L"tabctrlex")
public:
	STabCtrlEx(void);
	~STabCtrlEx(void) OVERRIDE;
	STDMETHOD_(int, InsertItem)(THIS_);
	STDMETHOD_(void, OnInitFinished)(THIS_ IXmlNode *xmlNode) OVERRIDE;
protected:
	SXmlDoc m_xmlTemplate;
};

SNSEND
#endif // __STABCTRLEX__H__