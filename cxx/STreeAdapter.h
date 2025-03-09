/**
 * @file STreeAdapter.h
 * @brief Header file for the STreeAdapter class, which provides functionality for managing tree views and handling item interactions.
 * @details This file defines the STreeAdapter class, a specialized adapter for tree views that handles item data and user interactions.
 * @author UxGood <uxgood.org@gmail.com>
 * @date 2025-03-05
 */

#pragma once
#include "options.h"
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
    STDMETHOD_(void, RefreshItems)(THIS_);
protected:
    STDMETHOD_(void, DeleteItems)(THIS_);
    STDMETHOD_(void, InitItemByTemplate)(THIS_ HSTREEITEM loc, SItemPanel* pItem, SXmlNode xmlTemplate);
protected:
    STreeView *m_treeView;
};
