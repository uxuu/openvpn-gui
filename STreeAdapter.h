#pragma once
extern "C" {
#undef MAX_NAME
#include "options.h"
#include "openvpn_config.h"

    extern options_t o;
}
#include <string>
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
    explicit STreeAdapter(STreeView *pTreeView)
    {
        m_treeView = pTreeView;

    }

    ~STreeAdapter() override
    {
        m_treeView = NULL;
    }

    STDMETHOD_(void, getView)(THIS_ HSTREEITEM loc, SItemPanel* pItem, SXmlNode xmlTemplate) override
    {
        WCHAR buf[MAX_NAME] = {0};
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
        if (itemType == 1)
        {
            if (ii.data.c->state == connected)
            {
                formatTime(ii.data.c->connected_since, buf);
                pItem->FindChildByName(L"time")->SetWindowText(buf);
                pItem->FindChildByName(L"ipaddr")->SetWindowText(ii.data.c->ip);
                formatByte(ii.data.c->bytes_in, ii.data.c->bytes_out, buf);
                pItem->FindChildByName(L"speed")->SetWindowText(buf);
            }
            else {
                pItem->FindChildByName(L"time")->SetWindowText(L" ");
                pItem->FindChildByName(L"ipaddr")->SetWindowText(L" ");
                pItem->FindChildByName(L"speed")->SetWindowText(L" ");
            }
        }
        else
        {
            pItem->FindChildByName(L"hr")->SetVisible(ii.data.gid != 0);
        }
        pItem->FindChildByName(L"name")->SetWindowText(ii.data.strName);
        CRect rcItem = m_treeView->GetClientRect();
        pItem->SetAttribute(L"width", std::to_wstring(rcItem.Width()).c_str());

    }

    STDMETHOD_(int, getViewType)(HSTREEITEM hItem) const override
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
        HSTREEITEM* groups = (HSTREEITEM *)malloc(o.num_groups * sizeof(HSTREEITEM *));
        DeleteAllItems();
        for (i = 0; i < o.num_groups; i++)
        {
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
            data.strImg = L"skin_connected";
            data.c = c;
            InsertItem(data, groups[c->group]);
        }
        notifyBranchChanged(STVI_ROOT);
        free(groups);
    }

    STDMETHOD_(int, getViewTypeCount)() const
    {
        return 2;
    }
protected:
    void formatTime(const time_t since, WCHAR* buf)
    {
        time_t now;
        time(&now);
        DWORD diff = difftime(now, since);
        if (diff > 24 * 3600)
        {
            swprintf(buf, L"%d %02d:%02d:%02d", diff / 24, diff / 3600 % 24, diff / 60 % 60, diff % 60);
        }
        else
        {
            swprintf(buf, L"%02d:%02d:%02d", diff / 3600, diff / 60 % 60, diff % 60);
        }

    }
    void formatByte(const long long byte_in, const long long byte_out, WCHAR* buf)
    {
        const char* suf[] = { "B", "KiB", "MiB", "GiB", "TiB", "PiB", "EiB", NULL };
        const char** s1 = suf;
        double x1 = byte_in;
        const char** s2 = suf;
        double x2 = byte_out;


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
        if (byte_in <= 1024 && byte_out <= 1024)
        {
            swprintf(buf, L"%I64uB/%I64uB", byte_in, byte_out);
        }
        if (byte_in <= 1024 && byte_out > 1024)
        {
            swprintf(buf, L"%I64uB/%.3f%hs", byte_in, x2, *s2);
        }
        if (byte_in > 1024 && byte_out < 1024)
        {
            swprintf(buf, L"%.3f%hs/%I64uB", x1, *s1, byte_out);
        }
        if (byte_in > 1024 && byte_out > 1024)
        {
            swprintf(buf, L"%.3f%hs/%.3f%hs", x1, *s1, x2, *s2);
        }
    }
protected:
    STreeView *m_treeView;
};
