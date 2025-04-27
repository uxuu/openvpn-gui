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

/**
 * @brief Constructor for the STreeAdapter class.
 * @param pTreeView Pointer to the associated tree view.
 */
STreeAdapter::STreeAdapter(STreeView* pTreeView)
{
    m_treeView = pTreeView;
}

/**
 * @brief Destructor for the STreeAdapter class.
 */
STreeAdapter::~STreeAdapter()
{
    m_treeView = NULL;
}

/**
 * @brief Retrieves and initializes a view for a specific tree item.
 * @param loc Handle to the tree item.
 * @param pItem Pointer to the item panel.
 * @param xmlTemplate XML template for initializing the item.
 */
void STreeAdapter::getView(HSTREEITEM loc, SItemPanel* pItem, SXmlNode xmlTemplate)
{
    if (pItem->GetChildrenCount() == 0)
    {
        InitItemByTemplate(loc, pItem, xmlTemplate);
    }
    CRect rcItem = m_treeView->GetClientRect();
    pItem->SetAttribute(L"width", SStringT().Format(_T("%d"), rcItem.Width()));
}

/**
 * @brief Initializes an item panel using a specified XML template.
 * @param loc Handle to the tree item.
 * @param pItem Pointer to the item panel.
 * @param xmlTemplate XML template for the item.
 */
void STreeAdapter::InitItemByTemplate(HSTREEITEM loc, SItemPanel *pItem, SXmlNode xmlTemplate)
{
    switch (getViewType(loc))
    {
        case 0:
            xmlTemplate = xmlTemplate.child(L"item_group");
            pItem->InitFromXml(&xmlTemplate);
            pItem->SetUserData(loc);
            pItem->GetEventSet()->subscribeEvent(EventItemPanelDbclick::EventID,
                         Subscriber(&STreeAdapter::OnItemPanelDbclick, this));
            break;
        case 1:
            xmlTemplate = xmlTemplate.child(L"item_data");
            pItem->InitFromXml(&xmlTemplate);

            break;
        default:
            break;
    }
}

/**
 * @brief Determines the view type of a tree item.
 * @param hItem Handle to the tree item.
 * @return An integer representing the view type (0 for group, 1 for data).
 */
int STreeAdapter::getViewType(HSTREEITEM hItem) CONST
{
    ItemInfo& ii = CSTree<SOUI::STreeAdapterBase<ItemData>::ItemInfo>::GetItemRef((HSTREEITEM)hItem);
    if (ii.data.bGroup)
        return 0;
    return 1;
}

/**
 * @brief Deletes all items in the tree view.
 */
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

/**
 * @brief Refreshes the items in the tree view by reloading data.
 */
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

/**
 * @brief Gets the total number of view types supported by the adapter.
 * @return The number of view types.
 */
int STreeAdapter::getViewTypeCount() CONST
{
    return 2;
}

/**
 * @brief Handles the double-click event on an item panel.
 * @param pEvt Pointer to the event object.
 * @return TRUE if the event is handled, FALSE otherwise.
 */
BOOL STreeAdapter::OnItemPanelDbclick(EventItemPanelDbclick* pEvt)
{
    auto* pItem = sobj_cast<SItemPanel>(pEvt->Sender());
    auto* pImg = pItem->FindChildByName2<SImageWnd>(L"img_expand");
    if (this->IsItemExpanded(pItem->GetUserData()))
    {
        pImg->SetAttribute(L"iconIndex", L"0");
    }
    else
    {
        pImg->SetAttribute(L"iconIndex", L"1");
    }
    this->ExpandItem(pItem->GetUserData(), TVC_TOGGLE);
    return true;
}

/**
 * @brief Handles button click events.
 * @param pEvt Pointer to the event object.
 * @return TRUE if the event is handled, FALSE otherwise.
 */
BOOL STreeAdapter::OnButtonClick(EventCmd* pEvt)
{
    return true;
}

