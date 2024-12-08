#pragma once
extern "C" {
#undef MAX_NAME
#include "options.h"
#include "openvpn_config.h"
#include "openvpn.h"

    extern options_t o;
}
#include <string>
#include "StdAfx.h"
#include <helper/SAdapterBase.h>
struct ItemData
{
    INT32 gid{};
    SStringT strImg ;
    SStringT strName;
    bool bGroup{};
    connection_t* c{};
};
class STreeAdapter final :public STreeAdapterBase<ItemData>
{

public:
    explicit STreeAdapter(STreeView *pTreeView)
    {
        m_treeView = pTreeView;
    }

    ~STreeAdapter() override
    {
        m_treeView = nullptr;
    }

    STDMETHOD_(void, getView)(THIS_ HSTREEITEM loc, SItemPanel* pItem, SXmlNode xmlTemplate) override
    {
        WCHAR buf[MAX_NAME] = {0};
        ItemInfo& ii = CSTree<SOUI::STreeAdapterBase<ItemData>::ItemInfo>::GetItemRef((HSTREEITEM)loc);
        int itemType = getViewType(loc);
        if (pItem->GetChildrenCount() == 0)
        {
            switch (itemType)
            {
            case 0:xmlTemplate = xmlTemplate.child(L"item_group");
                break;
            case 1:xmlTemplate = xmlTemplate.child(L"item_data");
                break;
            default:break;
            }
            pItem->InitFromXml(&xmlTemplate);
            if(itemType==0)
            {
                pItem->GetEventSet()->setMutedState(true);
            }
        }

        if (itemType == 1)
        {
            auto* pBtn = pItem->FindChildByName2<SImageButton>(L"btn_state");
            pBtn->GetRoot()->SetUserData(loc);
            pBtn->GetEventSet()->subscribeEvent(EventCmd::EventID, Subscriber(&STreeAdapter::OnButtonClick, this));
            if (ii.data.c->state == connected)
            {
                FormatTime(ii.data.c->connected_since, buf);
                pItem->FindChildByName(L"time")->SetWindowText(buf);
                pItem->FindChildByName(L"ipaddr")->SetWindowText(ii.data.c->ip);
                FormatByte(ii.data.c->bytes_in, ii.data.c->bytes_out, buf);
                pItem->FindChildByName(L"speed")->SetWindowText(buf);
                pBtn->SetAttribute(L"skin", L"skin_connected");

            }
            else if (ii.data.c->state == connecting)
            {
                pBtn->SetAttribute(L"skin", L"skin_connecting");
            }
            else if (ii.data.c->state == disconnected)
            {
                pItem->FindChildByName(L"time")->SetWindowText(L" ");
                pItem->FindChildByName(L"ipaddr")->SetWindowText(L" ");
                pItem->FindChildByName(L"speed")->SetWindowText(L" ");
                pBtn->SetAttribute(L"skin", L"skin_disconnected");
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
        ItemInfo & ii = CSTree<SOUI::STreeAdapterBase<ItemData>::ItemInfo>::GetItemRef((HSTREEITEM)hItem);
        if (ii.data.bGroup) return 0;
        else return 1;
    }

    void DeleteItems()
    {
        while (HasChildren(STVI_ROOT))
        {
            DeleteItem(GetFirstChildItem(STVI_ROOT), true);
        }
    }

    void RefreshItems()
    {
        ItemData data;
        auto* groups = static_cast<HSTREEITEM*>(malloc(o.num_groups * sizeof(HSTREEITEM*)));
        DeleteItems();
        for (int i = 1; i < o.num_groups; i++)
        {
            data.bGroup = TRUE;
            data.gid = o.groups[i].id;
            data.strName = o.groups[i].name;
            groups[i] = InsertItem(data);
            SetItemExpanded(groups[i], TRUE);
        }
        for (connection_t* c = o.chead; c; c = c->next)
        {
            data.bGroup = FALSE;
            data.strName = c->config_name;
            data.strImg = L"skin_connected";
            data.c = c;
            InsertItem(data, groups[c->group]);
        }
        notifyBranchChanged(STVI_ROOT);
        free(groups);
    }

    STDMETHOD_(int, getViewTypeCount)() const override
    {
        return 2;
    }
protected:
    void FormatTime(const time_t since, WCHAR* buf)
    {
        time_t now;
        time(&now);
        auto diff = static_cast<DWORD>(difftime(now, since));
        if (diff > 24 * 3600)
        {
            swprintf(buf, L"%d %02d:%02d:%02d", diff / 24, diff / 3600 % 24, diff / 60 % 60, diff % 60);
        }
        else
        {
            swprintf(buf, L"%02d:%02d:%02d", diff / 3600, diff / 60 % 60, diff % 60);
        }

    }
    void FormatByte(const unsigned long long byte_in, const unsigned long long byte_out, WCHAR* buf)
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
public:
    BOOL OnButtonClick(EventCmd* pEvt)
    {
        auto* pBtn = sobj_cast<SImageButton>(pEvt->Sender());
        ItemInfo & ii = CSTree<SOUI::STreeAdapterBase<ItemData>::ItemInfo>::GetItemRef((HSTREEITEM)pBtn->GetRoot()->GetUserData());
        if (ii.data.c->state == connected)
        {
            StopOpenVPN(ii.data.c);
        }
        else if (ii.data.c->state == disconnected)
        {
            StartOpenVPN(ii.data.c);
        }
        return true;
    }
protected:
    STreeView *m_treeView;
};
