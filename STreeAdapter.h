#pragma once
extern "C" {
#undef MAX_NAME
#include "options.h"
#include "openvpn_config.h"

    extern options_t o;
}
#include "StdAfx.h"
#include <helper/SAdapterBase.h>
struct TreeItemData
{
    TreeItemData():bGroup(false){}
    INT32 gid;            //组ID
    SStringT strImg ;    //图像
    SStringT strName;    //名称
    bool bGroup;
    connection_t* c;
};
class STreeAdapter :public STreeAdapterBase<TreeItemData>
{
public:
public:
    STreeAdapter()
    {

    }

    ~STreeAdapter() {}

    virtual void getView(HSTREEITEM loc, SItemPanel* pItem, SXmlNode xmlTemplate)
    {
        ItemInfo & ii = m_tree.GetItemRef((HSTREEITEM)loc);
        int itemType = getViewType(loc);
        if (pItem->GetChildrenCount() == 0)
        {
            switch (itemType)
            {
            case 0:xmlTemplate = xmlTemplate.child(L"item_group");
                break;
            case 1:xmlTemplate = xmlTemplate.child(L"item_data");
                break;
            }
            pItem->InitFromXml(&xmlTemplate);
            if(itemType==0)
            {
                pItem->GetEventSet()->setMutedState(true);
            }
        }
        if(itemType == 1)
        {

        }
        else
        {
            pItem->FindChildByName(L"hr")->SetVisible(ii.data.gid != 0);
        }
        pItem->FindChildByName(L"name")->SetWindowText(ii.data.strName);
    }

    virtual int getViewType(HSTREEITEM hItem) const
    {
        ItemInfo & ii = m_tree.GetItemRef((HSTREEITEM)hItem);
        if (ii.data.bGroup) return 0;
        else return 1;
    }

    void DeleteAllItems()
    {
        while (HasChildren(STVI_ROOT))
        {
            DeleteItem(GetFirstChildItem(STVI_ROOT), true);
        }
    }

    void RefreshItems()
    {
        int i;
        HSTREEITEM* groups = (HSTREEITEM *)_alloca(o.num_groups * sizeof(HSTREEITEM *));
        DeleteAllItems();
        DbgPrintf(_T("o.num_groups=%d, o.num_configs=%d"), o.num_groups, o.num_configs);
        for (i = 0; i < o.num_groups; i++)
        {
            DbgPrintf(_T("o.groups[i].id=%d, o.groups[i].name=%s"), o.groups[i].id, o.groups[i].name);
            TreeItemData data;
            data.bGroup = true;
            data.gid = o.groups[i].id;
            data.strName = o.groups[i].name;
            groups[i] = InsertItem(data);
            SetItemExpanded(groups[i], TRUE);
        }
        for (connection_t* c = o.chead; c; c = c->next)
        {
            TreeItemData data;
            data.bGroup = FALSE;
            data.strName = c->config_name;
            data.strImg = L"skin_connected32";
            InsertItem(data, groups[c->group]);
        }
        notifyBranchChanged(STVI_ROOT);
    }

    virtual int getViewTypeCount() const
    {
        return 2;
    }

};
