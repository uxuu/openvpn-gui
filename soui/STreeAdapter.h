/**
 * @file STreeAdapter.h
 * @brief Header file for the STreeAdapter class, which provides functionality for managing tree views and handling item interactions.
 * @details This file defines the STreeAdapter class, a specialized adapter for tree views that handles item data and user interactions.
 * @author UxGood <uxgood.org@gmail.com>
 * @date 2025-03-05
 */

#pragma once

#include <helper/SAdapterBase.h>

struct ItemData
{
    INT32 gid{};
    bool bGroup{};
    SStringT strImg ;
    SStringT strName;
    connection_t* c{};
};
class STreeAdapter final :public STreeAdapterBase<ItemData>
{

public:
    explicit STreeAdapter(STreeView *pTreeView);
    ~STreeAdapter() OVERRIDE;
    STDMETHOD_(void, getView)(THIS_ HSTREEITEM loc, SItemPanel* pItem, SXmlNode xmlTemplate) OVERRIDE;
    STDMETHOD_(int, getViewType)(THIS_ HSTREEITEM hItem) CONST OVERRIDE;
    STDMETHOD_(int, getViewTypeCount)() CONST OVERRIDE;
    STDMETHOD_(void, NotifyStateChange)(THIS_);
protected:
    STDMETHOD_(void, InitItemByTemplate)(THIS_ HSTREEITEM loc, SItemPanel* pItem, SXmlNode xmlTemplate);
    STDMETHOD_(void, BindButtonEvent)(THIS_ SItemPanel* pItem);
    STDMETHOD_(void, SetButtonUserData)(THIS_ HSTREEITEM loc, SItemPanel* pItem);
    STDMETHOD_(void, SetButtonState)(THIS_ conn_state_t state, SItemPanel *pItem);
    STDMETHOD_(void, SetImageState)(THIS_ conn_state_t state, SItemPanel *pItem);
    STDMETHOD_(void, SetTextState)(THIS_ connection_t* c, SItemPanel* pItem);

protected:
    STDMETHOD_(void, FormatTime)(THIS_ const time_t since, WCHAR* buf);
    STDMETHOD_(void, FormatByte)(THIS_ const unsigned long long byte_in, const unsigned long long byte_out, WCHAR* buf);
public:
    BOOL OnItemPanelDbclick(EventItemPanelDbclick* pEvt);
    BOOL OnButtonClick(EventCmd* pEvt);
protected:
    STreeView *m_treeView;
    const WCHAR btn_names[6][12] = {L"btn_start", L"btn_stop", L"btn_restart", L"btn_option", L"btn_status", L"btn_logview"};
};
