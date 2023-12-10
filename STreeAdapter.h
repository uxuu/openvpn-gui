#pragma once

//#include "GlobalUnits.h"

#include <helper/SAdapterBase.h>
struct TreeItemData
{
	TreeItemData():bGroup(false){}
	INT32 iid;
	INT32 gid;			//组ID
	SStringT strImg ;	//图像
	SStringT strName;	//名称
	bool bGroup;
};
class STreeAdapter :public STreeAdapterBase<TreeItemData>
{
public:
public:
	STreeAdapter()
	{

        DbgPrintf(L"STreeAdapter");
		TreeItemData data;
		data.strName = L"新朋友";
		data.gid = 1;
		data.bGroup = true;

		HSTREEITEM hRoot = InsertItem(data);
		SetItemExpanded(hRoot, TRUE);

		data.bGroup = FALSE;
		data.strName = L"新的朋友";
		data.strImg = L"skin_personal";
		InsertItem(data, hRoot);
	}

	~STreeAdapter() {}

	virtual void getView(HSTREEITEM loc, SItemPanel* pItem, SXmlNode xmlTemplate)
	{
		ItemInfo & ii = m_tree.GetItemRef((HSTREEITEM)loc);
		int itemType = getViewType(loc);
        DbgPrintf(L"itemType=%d", itemType);
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
			pItem->FindChildByName(L"hr")->SetVisible(ii.data.gid != 1);
		}
		pItem->FindChildByName(L"name")->SetWindowText(ii.data.strName);
	}

	virtual int getViewType(HSTREEITEM hItem) const
	{
		ItemInfo & ii = m_tree.GetItemRef((HSTREEITEM)hItem);
		if (ii.data.bGroup) return 0;
		else return 1;
	}

	virtual int getViewTypeCount() const
	{
		return 2;
	}

};
