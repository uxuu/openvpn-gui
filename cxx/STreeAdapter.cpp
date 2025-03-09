/**
 * @file STreeAdapter.cpp
 * @brief Implementation file for the STreeAdapter class.
 * @details This file contains the implementation of the STreeAdapter class, which provides functionality for managing tree views and handling item interactions.
 * @author UxGood <uxgood.org@gmail.com>
 * @date 2025-03-05
 */

#include "../stdafx.h"

#include "openvpn-ex.h"
#include "STreeAdapter.h"

STreeAdapter::STreeAdapter(STreeView* pTreeView)
{
    m_treeView = pTreeView;
}

STreeAdapter::~STreeAdapter()
{
    m_treeView = NULL;
}

void STreeAdapter::getView(HSTREEITEM loc, SItemPanel* pItem, SXmlNode xmlTemplate)
{
    InitItemByTemplate(loc, pItem, xmlTemplate);
    CRect rcItem = m_treeView->GetClientRect();
    pItem->SetAttribute(L"width", SStringT().Format(_T("%d"), rcItem.Width()));
}

int STreeAdapter::getViewType(HSTREEITEM hItem) CONST
{
    ItemInfo& ii = CSTree<SOUI::STreeAdapterBase<ItemData>::ItemInfo>::GetItemRef((HSTREEITEM)hItem);
    if (ii.data.bGroup)
        return 0;
    return 1;
}

void STreeAdapter::DeleteItems()
{
    m_treeView->LockUpdate();
    while (HasChildren(STVI_ROOT))
    {
        DeleteItem(GetLastChildItem(STVI_ROOT), false);
    }
    notifyBranchChanged(STVI_ROOT);
    m_treeView->UnlockUpdate();
}

void STreeAdapter::RefreshItems()
{
    bool nested = USE_NESTED_CONFIG_MENU;
    ItemData data;
    HSTREEITEM* groups = NULL;
    DeleteItems();
    m_treeView->LockUpdate();
    if (nested)
    {
        groups = static_cast<HSTREEITEM*>(malloc(o.num_groups * sizeof(HSTREEITEM*)));
        groups[0] = STVI_ROOT;
        for (int i = 1; i < o.num_groups; i++)
        {
            if (o.groups[i].active == 0)
                continue;
            data.bGroup = TRUE;
            data.gid = o.groups[i].id;
            data.strName = o.groups[i].name;
            groups[i] = InsertItem(data, groups[o.groups[i].parent]);
            SetItemExpanded(groups[i], TRUE);
        }
    }
    for (connection_t* c = o.chead; c; c = c->next)
    {
        data.bGroup = FALSE;
        data.strName = c->config_name;
        data.c = c;
        InsertItem(data, nested?groups[c->group]:STVI_ROOT);
    }
    notifyBranchChanged(STVI_ROOT);
    m_treeView->UnlockUpdate();
    if(groups) free(groups);
}

int STreeAdapter::getViewTypeCount() CONST
{
    return 2;
}

void STreeAdapter::InitItemByTemplate(HSTREEITEM loc, SItemPanel* pItem, SXmlNode xmlTemplate)
{
    if (pItem->GetChildrenCount() == 0)
    {
        switch (getViewType(loc))
        {
            case 0: xmlTemplate = xmlTemplate.child(L"item_group");
            break;
            case 1: xmlTemplate = xmlTemplate.child(L"item_data");
            break;
            default: break;
        }
        pItem->InitFromXml(&xmlTemplate);
    }
}