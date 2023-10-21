#pragma once
#include <helper/SAdapterBase.h>
#include <vector>
#include <iostream>
typedef struct times
{
	int Year;
	int Mon;
	int Day;
	int Hour;
	int Min;
	int Second;
}Times;

struct SListItemData
{
	int nType;
	std::string strID;
};

class SListAdapter : public SAdapterBase
{
public:
	SListAdapter();
	~SListAdapter(void);

public:
	void AddItem(const int& nType, const std::string& strID);

    void MoveItemToTop(const std::string& strID);
    void EnsureVisable(const std::string& strID);
    void SetCurSel(const std::string& strID);
    int GetItemIndexByID(const std::string& strID);
	void UpdateData(){notifyDataSetChanged();}
    BOOL OnButtonClick(EventCmd* pEvt)
    {
        SButton* pBtn = sobj_cast<SButton>(pEvt->Sender());
        int iItem = pBtn->GetRoot()->GetUserData();
        SMessageBox(NULL, SStringT().Format(_T("button of %d item was clicked"), iItem), _T("haha"), MB_OK);
        return true;
    }
protected:
	virtual void WINAPI getView(int position, SItemPanel* pItem, SXmlNode xmlTemplate);
	virtual int WINAPI getCount();
	virtual SStringT getColunName(int iCol) const;

private:
	Times stamp_to_standard(int stampTime);
	SStringW OperateTimestamp(const std::string& strTimestamp);

private:
	std::vector<SListItemData*> m_vecItemInfo;
};
