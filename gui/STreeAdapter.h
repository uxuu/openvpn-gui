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

    STDMETHOD_(int, getViewType)(HSTREEITEM hItem) CONST OVERRIDE;
    void DeleteItems();
    void RefreshItems();
    STDMETHOD_(int, getViewTypeCount)() CONST OVERRIDE;
protected:
    void FormatTime(const time_t since, WCHAR* buf);
    void FormatByte(const unsigned long long byte_in, const unsigned long long byte_out, WCHAR* buf);
public:
    BOOL OnButtonConnClick(EventCmd* pEvt);
    BOOL OnButtonDisconnClick(EventCmd* pEvt);
    BOOL OnButtonReconnClick(EventCmd* pEvt);
    BOOL OnButtonOptionClick(EventCmd* pEvt);
    BOOL OnButtonStatusClick(EventCmd* pEvt);
    BOOL OnButtonLogviewClick(EventCmd* pEvt);
    BOOL OnItemPanelDbclick(EventItemPanelDbclick* pEvt);
protected:
    STreeView *m_treeView;
};
