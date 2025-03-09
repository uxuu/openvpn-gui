/**
 * @file STabCtrlEx.h
 * @brief Header file for the STabCtrlEx class, which extends the STabCtrl class.
 * @details This file defines the STabCtrlEx class, a specialized tab control that includes XML template functionality.
 * @author UxGood <uxgood.org@gmail.com>
 * @date 2025-03-05
 */

#ifndef __STABCTRLEX__H__
#define __STABCTRLEX__H__
#include <souistd.h>
#include <control/STabCtrl.h>

#include <tchar.h>
SNSBEGIN

/**
 * @class STabCtrlEx
 * @brief Extended tab control class with XML template support.
 * @details This class inherits from STabCtrl and adds functionality for handling XML templates.
 */
class STabCtrlEx:public STabCtrl
{
	 DEF_SOBJECT(STabCtrl, L"tabctrlex")
public:
	STabCtrlEx(void);
	~STabCtrlEx(void) OVERRIDE;
	STDMETHOD_(int, InsertItem)(THIS_ int iInsert = -1, BOOL bLoading = FALSE);
	STDMETHOD_(int, InsertItem)(THIS_ LPCTSTR lpszTitle, int iInsert = -1, BOOL bLoading = FALSE);
	STDMETHOD_(void, OnInitFinished)(THIS_ IXmlNode *xmlNode) OVERRIDE;
protected:
	SXmlDoc m_xmlTemplate; ///< XML document template used by the tab control.
};

SNSEND
#endif // __STABCTRLEX__H__