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
    ItemInfo& ii = CSTree<STreeAdapter::ItemInfo>::GetItemRef((HSTREEITEM)loc);
    pItem->FindChildByName(L"txt_name")->SetWindowText(ii.data.strName);
    if (getViewType(loc) == 1)
    {
        SetButtonState(ii.data.c->state, pItem);
        SetImageState(ii.data.c->state, pItem);
        SetTextState(ii.data.c, pItem);
        SetButtonUserData(loc, pItem);
    }
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
            pItem->GetEventSet()->subscribeEvent(EventItemPanelDbclick::EventID,
                         Subscriber(&STreeAdapter::OnItemPanelDbclick, this));
            break;
        case 1:
            xmlTemplate = xmlTemplate.child(L"item_data");
            pItem->InitFromXml(&xmlTemplate);
            BindButtonEvent(loc, pItem);
            break;
        default:
            break;
    }
}


/**
 * @brief Binds button events to an item panel.
 * @param loc Handle to the tree item.
 * @param pItem Pointer to the item panel.
 */
void STreeAdapter::BindButtonEvent(HSTREEITEM loc, SItemPanel *pItem)
{
    for (int i = 0; i < _countof(btn_names); i++)
    {
        SButton* pBtn = pItem->FindChildByName2<SButton>(btn_names[i]);
        if (pBtn)
        {
            pBtn->GetEventSet()->subscribeEvent(EventCmd::EventID,
                             Subscriber(&STreeAdapter::OnButtonClick, this));
        }
    }
}

/**
 * @brief Sets the user data of buttons in an item panel.
 * @param loc Handle to the tree item.
 * @param pItem Pointer to the item panel.
 */
void STreeAdapter::SetButtonUserData(HSTREEITEM loc, SItemPanel *pItem)
{
    for (int i = 0; i < _countof(btn_names); i++)
    {
        SButton* pBtn = pItem->FindChildByName2<SButton>(btn_names[i]);
        if (pBtn)
        {
            pBtn->SetUserData(loc);
        }
    }
}

/**
 * @brief Handles button click events.
 * @param pEvt Pointer to the event data.
 * @return TRUE if the event was handled, FALSE otherwise.
 */
void STreeAdapter::SetButtonState(conn_state_t state, SItemPanel *pItem)
{
    switch (state)
    {
        case connected:
        case connecting:
        case reconnecting:
        case disconnecting:
            pItem->FindChildByName2<SButton>(btn_names[0])->SetVisible(FALSE);
            pItem->FindChildByName2<SButton>(btn_names[1])->SetVisible(TRUE);
            pItem->FindChildByName2<SButton>(btn_names[2])->SetVisible(TRUE);
            pItem->FindChildByName2<SButton>(btn_names[3])->SetVisible(FALSE);
            pItem->FindChildByName2<SButton>(btn_names[4])->SetVisible(TRUE);
            pItem->FindChildByName2<SButton>(btn_names[5])->SetVisible(TRUE);
            break;
        case disconnected:
            pItem->FindChildByName2<SButton>(btn_names[0])->SetVisible(TRUE);
            pItem->FindChildByName2<SButton>(btn_names[1])->SetVisible(FALSE);
            pItem->FindChildByName2<SButton>(btn_names[2])->SetVisible(FALSE);
            pItem->FindChildByName2<SButton>(btn_names[3])->SetVisible(TRUE);
            pItem->FindChildByName2<SButton>(btn_names[4])->SetVisible(FALSE);
            pItem->FindChildByName2<SButton>(btn_names[5])->SetVisible(FALSE);
            break;
        case detached:
        case detaching:
        case onhold:
        case resuming:
        case suspended:
        default:
            break;
    }
}

/**
 * @brief Sets the image state of an item panel based on the connection state.
 * @param state The connection state.
 * @param pItem Pointer to the item panel.
 */
void STreeAdapter::SetImageState(conn_state_t state, SItemPanel *pItem)
{
    switch (state)
    {
        case connected:
            pItem->FindChildByName(L"img_state")->SetVisible(TRUE);
            pItem->FindChildByName(L"img_state")->SetAttribute(L"skin", L"skin_connected");
            pItem->FindChildByName(L"gif_state")->SetVisible(FALSE);
            break;
        case connecting:
        case reconnecting:
        case disconnecting:
            pItem->FindChildByName(L"img_state")->SetVisible(FALSE);
            pItem->FindChildByName(L"gif_state")->SetVisible(TRUE);
            break;
        case disconnected:
            pItem->FindChildByName(L"img_state")->SetVisible(TRUE);
            pItem->FindChildByName(L"img_state")->SetAttribute(L"skin", L"skin_disconnected");
            pItem->FindChildByName(L"gif_state")->SetVisible(FALSE);
            break;
        case detached:
        case detaching:
        case onhold:
        case resuming:
        case suspended:
        default:
            break;
    }
}

/**
 * @brief Sets the text state of an item panel based on the connection state.
 * @param c Pointer to the connection data.
 * @param pItem Pointer to the item panel.
 */
void STreeAdapter::SetTextState(connection_t* c, SItemPanel *pItem)
{
    WCHAR buf[MAX_NAME] = {0};
    switch (c->state)
    {
        case disconnecting:
        case connected:
        case connecting:
        case reconnecting:
            FormatTime(c->connected_since, buf);
            pItem->FindChildByName(L"txt_time")->SetWindowText(buf);
            FormatByte(c->bytes_in, c->bytes_out, buf);
            pItem->FindChildByName(L"txt_bytes")->SetWindowText(buf);
            pItem->FindChildByName(L"txt_ipaddr")->SetWindowText(c->ip);
            break;
        case disconnected:
            pItem->FindChildByName(L"txt_time")->SetWindowText(L"");
            pItem->FindChildByName(L"txt_ipaddr")->SetWindowText(L"");
            pItem->FindChildByName(L"txt_bytes")->SetWindowText(L"");
            break;
        case detached:
        case detaching:
        case onhold:
        case resuming:
        case suspended:
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
    ItemInfo& ii = CSTree<STreeAdapter::ItemInfo>::GetItemRef((HSTREEITEM)hItem);
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
    ItemData data;
    HSTREEITEM* groups = NULL;
    DeleteItems();
    m_treeView->LockUpdate();
    if (USE_NESTED_CONFIG_MENU)
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
        InsertItem(data, groups?groups[c->group]:STVI_ROOT);
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
    auto* pBtn = sobj_cast<SButton>(pEvt->Sender());
    HSTREEITEM loc = pBtn->GetUserData();
    ItemInfo& ii = CSTree<STreeAdapter::ItemInfo>::GetItemRef(loc);
    LPCWSTR btnName = pBtn->GetName();

    if (wcscmp(btnName, btn_names[0]) == 0)
    {
        StartOpenVPN(ii.data.c);
    }
    else if (wcscmp(btnName, btn_names[1]) == 0)
    {
        StopOpenVPN(ii.data.c);
    }
    else if (wcscmp(btnName, btn_names[2]) == 0)
    {
        RestartOpenVPN(ii.data.c);
    }
    else if (wcscmp(btnName, btn_names[3]) == 0)
    {
        // OptionOpenVPN(ii.data.c);
    }
    else if (wcscmp(btnName, btn_names[4]) == 0)
    {
        // StatusOpenVPN(ii.data.c);
    }
    else if (wcscmp(btnName, btn_names[5]) == 0)
    {
        // LogViewOpenVPN(ii.data.c);
    }

    return true;
}

/**
 * @brief Formats the time difference between two timestamps.
 * @param since The timestamp to compare with.
 * @param buf The buffer to store the formatted time.
 */
void STreeAdapter::FormatTime(const time_t since, WCHAR* buf)
{
    time_t now;
    time(&now);
    auto diff = static_cast<DWORD>(since?difftime(now, since):0);
    if (diff > 24 * 3600)
    {
        swprintf(buf, L"%d %02d:%02d:%02d", diff / 24, diff / 3600 % 24, diff / 60 % 60, diff % 60);
    }
    else
    {
        swprintf(buf, L"%02d:%02d:%02d", diff / 3600, diff / 60 % 60, diff % 60);
    }

}

/**
 * @brief Formats the byte count.
 * @param byte_in The number of bytes received.
 * @param byte_out The number of bytes sent.
 * @param buf The buffer to store the formatted byte count.
 */
void STreeAdapter::FormatByte(const unsigned long long byte_in, const unsigned long long byte_out, WCHAR* buf)
{
    const char* suf[] = { "B", "KiB", "MiB", "GiB", "TiB", "PiB", "EiB", nullptr };
    const char** s1 = suf;
    auto x1 = static_cast<double>(byte_in);
    const char** s2 = suf;
    auto x2 = static_cast<double>(byte_out);

    while (x1 > 1024 && *(s1 + 1))
    {
        x1 /= 1024.0;
        s1++;
    }

    while (x2 > 1024 && *(s2 + 1))
    {
        x2 /= 1024.0;
        s2++;
    }
    swprintf(buf, L"%.1f%hs/%.1f%hs", x1, *s1, x2, *s2);
}