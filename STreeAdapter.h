#pragma once
extern "C" {
#undef MAX_NAME
#include "options.h"
#include "openvpn_config.h"
#include "openvpn.h"

    extern options_t o;
#define USE_NESTED_CONFIG_MENU ((o.config_menu_view == CONFIG_VIEW_AUTO && o.num_configs > 25)   \
                                || (o.config_menu_view == CONFIG_VIEW_NESTED))
}
#include <string>
#include "StdAfx.h"
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
        }

        if (itemType == 1)
        {
            auto* pImg = pItem->FindChildByName(L"img_state");
            auto* pGif = pItem->FindChildByName(L"gif_state");
            auto* pBtnConn = pItem->FindChildByName2<SButton>(L"btn_conn");
            pBtnConn->GetRoot()->SetUserData(loc);
            pBtnConn->GetEventSet()->subscribeEvent(EventCmd::EventID, Subscriber(&STreeAdapter::OnButtonConnClick, this));
            auto* pBtnDisconn = pItem->FindChildByName2<SButton>(L"btn_disconn");
            pBtnDisconn->GetEventSet()->subscribeEvent(EventCmd::EventID, Subscriber(&STreeAdapter::OnButtonDisconnClick, this));
            auto* pBtnReconn = pItem->FindChildByName2<SButton>(L"btn_reconn");
            pBtnReconn->GetEventSet()->subscribeEvent(EventCmd::EventID, Subscriber(&STreeAdapter::OnButtonReconnClick, this));
            auto* pBtnOption = pItem->FindChildByName2<SButton>(L"btn_option");
            pBtnOption->GetEventSet()->subscribeEvent(EventCmd::EventID, Subscriber(&STreeAdapter::OnButtonOptionClick, this));
            auto* pBtnStatus = pItem->FindChildByName2<SButton>(L"btn_status");
            pBtnStatus->GetEventSet()->subscribeEvent(EventCmd::EventID, Subscriber(&STreeAdapter::OnButtonStatusClick, this));
            if (ii.data.c->state == connected)
            {
                pImg->SetAttribute(L"skin", L"skin_connected");
                pImg->SetVisible(TRUE);
                pGif->SetVisible(FALSE);
                FormatTime(ii.data.c->connected_since, buf);
                pItem->FindChildByName(L"txt_time")->SetWindowText(buf);
                pItem->FindChildByName(L"txt_ipaddr")->SetWindowText(ii.data.c->ip);
                FormatByte(ii.data.c->bytes_in, ii.data.c->bytes_out, buf);
                pItem->FindChildByName(L"txt_speed")->SetWindowText(buf);
                pBtnConn->SetVisible(FALSE);
                pBtnDisconn->SetVisible(TRUE);
                pBtnReconn->SetVisible(TRUE);
                pBtnOption->SetVisible(FALSE);
                pBtnStatus->SetVisible(TRUE);
            }
            else if (ii.data.c->state == connecting)
            {
                pGif->SetVisible(TRUE);
                pImg->SetVisible(FALSE);
                pBtnConn->SetVisible(FALSE);
                pBtnDisconn->SetVisible(TRUE);
                pBtnReconn->SetVisible(TRUE);
                pBtnOption->SetVisible(FALSE);
                pBtnStatus->SetVisible(FALSE);
            }
            else if (ii.data.c->state == disconnected)
            {
                pImg->SetAttribute(L"skin", L"skin_disconnected");
                pImg->SetVisible(TRUE);
                pGif->SetVisible(FALSE);
                pItem->FindChildByName(L"txt_time")->SetWindowText(L"");
                pItem->FindChildByName(L"txt_ipaddr")->SetWindowText(L"");
                pItem->FindChildByName(L"txt_speed")->SetWindowText(L"");
                pBtnConn->SetVisible(TRUE);
                pBtnDisconn->SetVisible(FALSE);
                pBtnReconn->SetVisible(FALSE);
                pBtnOption->SetVisible(TRUE);
                pBtnStatus->SetVisible(FALSE);
            }
        }
        else
        {
            pItem->FindChildByName(L"hr")->SetVisible(ii.data.gid != 0);
            pItem->SetUserData(loc);
            pItem->GetEventSet()->subscribeEvent(EventItemPanelDbclick::EventID, Subscriber(&STreeAdapter::OnItemPanelDbclick, this));
        }
        pItem->FindChildByName(L"txt_name")->SetWindowText(ii.data.strName);
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
            DeleteItem(GetFirstChildItem(STVI_ROOT), false);
        }
        notifyBranchChanged(STVI_ROOT);
    }

    void RefreshItems()
    {
        ItemData data;
        auto* groups = static_cast<HSTREEITEM*>(malloc(o.num_groups * sizeof(HSTREEITEM*)));
        if (USE_NESTED_CONFIG_MENU)
        {
            DeleteItems();
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
            for (connection_t* c = o.chead; c; c = c->next)
            {
                data.bGroup = FALSE;
                data.strName = c->config_name;
                data.c = c;
                InsertItem(data, groups[c->group]);
            }
            free(groups);
        }
        else
        {
            DeleteItems();
            for (connection_t* c = o.chead; c; c = c->next)
            {
                data.bGroup = FALSE;
                data.strName = c->config_name;
                data.c = c;
                InsertItem(data, STVI_ROOT);
            }
        }
        notifyBranchChanged(STVI_ROOT);
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
    BOOL OnButtonConnClick(EventCmd* pEvt)
    {
        auto* pBtn = sobj_cast<SButton>(pEvt->Sender());
        ItemInfo & ii = CSTree<SOUI::STreeAdapterBase<ItemData>::ItemInfo>::GetItemRef(pBtn->GetRoot()->GetUserData());
        StartOpenVPN(ii.data.c);
        return true;
    }
    BOOL OnButtonDisconnClick(EventCmd* pEvt)
    {
        auto* pBtn = sobj_cast<SButton>(pEvt->Sender());
        ItemInfo & ii = CSTree<SOUI::STreeAdapterBase<ItemData>::ItemInfo>::GetItemRef(pBtn->GetRoot()->GetUserData());
        StopOpenVPN(ii.data.c);
        return true;
    }
    BOOL OnButtonReconnClick(EventCmd* pEvt)
    {
        auto* pBtn = sobj_cast<SButton>(pEvt->Sender());
        ItemInfo & ii = CSTree<SOUI::STreeAdapterBase<ItemData>::ItemInfo>::GetItemRef(pBtn->GetRoot()->GetUserData());
        RestartOpenVPN(ii.data.c);
        return true;
    }
    BOOL OnButtonOptionClick(EventCmd* pEvt)
    {

        return true;
    }
    BOOL OnButtonStatusClick(EventCmd* pEvt)
    {

        return true;
    }
    BOOL OnItemPanelDbclick(EventItemPanelDbclick* pEvt)
    {
        auto* pItem = sobj_cast<SItemPanel>(pEvt->Sender());
        auto *pImg = pItem->FindChildByName2<SImageWnd>(L"img_expand");
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
protected:
    STreeView *m_treeView;
};
