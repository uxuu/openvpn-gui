#include "StdAfx.h"
#include "SListAdapter.h"
#include <sstream>

SListAdapter::SListAdapter()
{
}

SListAdapter::~SListAdapter(void)
{
}

void SListAdapter::getView(int position, SItemPanel* pItem, SXmlNode xmlTemplate)
{
	if (0 == pItem->GetChildrenCount())
	{
		pItem->InitFromXml(&xmlTemplate);
	}

	size_t sPos = static_cast<size_t>(position);
	if (sPos >= m_vecItemInfo.size())
		return;

    SButton* pBtn = pItem->FindChildByName2<SButton>(L"btn_test");
    pBtn->SetWindowText(SStringT().Format(_T("button %d"), position));
    pBtn->GetRoot()->SetUserData(position);
    pBtn->GetEventSet()->subscribeEvent(EventCmd::EventID, Subscriber(&SListAdapter::OnButtonClick, this));

	SListItemData* pInfo = m_vecItemInfo[sPos];
	if (NULL == pInfo)
		return;

	SImageWnd* pItemFace = pItem->FindChildByName2<SImageWnd>(L"cfg_face");
	SASSERT(pItemFace);
	SStringW sstrFace;
	switch (pInfo->nType)
	{
	case 0://filehelper
		sstrFace = L"skin_filehelper";
		break;
	case 1://personal
		sstrFace = L"skin_personal";
		break;
	case 2://group
		sstrFace = L"skin_group";
		break;
	case 3:
		sstrFace = L"skin_dyh";
		break;
	case 4:
		sstrFace = L"skin_news";
		break;
	case 5:
		sstrFace = L"skin_gzh";
		break;
	default:
		break;
	}
	pItemFace->SetAttribute(L"skin", sstrFace, TRUE);

	//name
	SStatic* pItemName = pItem->FindChildByName2<SStatic>(L"cfg_name");
	SASSERT(pItemName);
	SStringW sstrName;
	switch (pInfo->nType)
	{
	case 0://filehelper
		sstrName = L"filehelper";
		break;
	case 1://personal
		{
			std::string strName = "personal";

			sstrName = S_CA2W(strName.c_str());
		}
		break;
	case 2://group
		{
			std::string strName = "group";
			sstrName = S_CA2W(strName.c_str());
		}
		break;
	default:
		break;
	}
	pItemName->SetWindowText(sstrName);

	SStatic* pItemContent = pItem->FindChildByName2<SStatic>(L"cfg_content");
	SASSERT(pItemContent);
	pItemContent->SetWindowText(L"SOUI_IMDemo");

	SStatic* pItemTime = pItem->FindChildByName2<SStatic>(L"cfg_time");
	SASSERT(pItemTime);
	pItemTime->SetWindowText(L"2018/12/17");
}

int SListAdapter::getCount()
{
	return static_cast<int>(m_vecItemInfo.size());
}

SStringT SListAdapter::getColunName(int iCol) const
{
	return L"col_nick";
}

void SListAdapter::AddItem(const int& nType, const std::string& strID)
{
	SListItemData* pItemData = new SListItemData;
	pItemData->nType = nType;
	pItemData->strID = strID;

	m_vecItemInfo.push_back(pItemData);
	notifyDataSetChanged();
}

void SListAdapter::MoveItemToTop(const std::string& strID)
{
	int nIndex = GetItemIndexByID(strID);
	if (-1 != nIndex)
	{
		size_t sIndex = static_cast<size_t>(nIndex);
		if(sIndex < 0 || sIndex >= m_vecItemInfo.size())
			return ;
		SListItemData* pItemData = m_vecItemInfo[nIndex];
		if (pItemData)
		{
			m_vecItemInfo.erase(m_vecItemInfo.begin() + sIndex);
			m_vecItemInfo.insert(m_vecItemInfo.begin(), pItemData);
		}
	}
}

int SListAdapter::GetItemIndexByID(const std::string& strID)
{
	int nIndex = -1;
	for (int i = 0; i < m_vecItemInfo.size(); i++)
	{
		if (m_vecItemInfo[i]->strID == strID)
		{
			nIndex = i;
			break;
		}
	}
	return nIndex;
}
SStringW SListAdapter::OperateTimestamp(const std::string& strTimestamp)
{
	std::ostringstream os;
	Times t = stamp_to_standard(atoi(strTimestamp.c_str()));

	time_t ttMorning = time(NULL);
	struct tm* tmMorning = localtime(&ttMorning);
	tmMorning->tm_hour = 0;
	tmMorning->tm_min = 0;
	tmMorning->tm_sec = 0;

	unsigned int MorningTimestamp = (unsigned int)mktime(tmMorning);
	unsigned int SundayTimestamp = MorningTimestamp - tmMorning->tm_wday * (24*60*60);

	unsigned int nTimestamp = atoi(strTimestamp.c_str());
	if (nTimestamp >= MorningTimestamp)
	{
		os.str("");
		if (t.Hour > 12){
			if (t.Min >= 10)
				os<<"下午 "<<t.Hour<<":"<<t.Min;
			else
				os<<"下午 "<<t.Hour<<":0"<<t.Min;
		}
		else{
			if (t.Min >= 10)
				os<<"上午 "<<t.Hour<<":"<<t.Min;
			else
				os<<"上午 "<<t.Hour<<":0"<<t.Min;
		}

		std::string strTime = os.str();
		SStringW sstrTime = S_CA2W(strTime.c_str());
		return sstrTime;
	}
	else if (nTimestamp < MorningTimestamp && nTimestamp >= MorningTimestamp - 86400){
		return L"昨天";
	}
	else{
		SStringW sstrResult = L"";
		if (nTimestamp < SundayTimestamp)
			sstrResult.Format(L"%d/%d/%d", t.Year, t.Mon, t.Day);
		else
		{
			int elapsed_time = (int)difftime(nTimestamp, SundayTimestamp);
			int elapsed_day = elapsed_time/(24*60*60);
			switch (elapsed_day)
			{
			case 0:
				sstrResult = L"星期日";
				break;
			case 1:
				sstrResult = L"星期一";
				break;
			case 2:
				sstrResult = L"星期二";
				break;
			case 3:
				sstrResult = L"星期三";
				break;
			case 4:
				sstrResult = L"星期四";
				break;
			case 5:
				sstrResult = L"星期五";
				break;
			case 6:
				sstrResult = L"星期六";
				break;
			default:
				sstrResult.Format(L"%d/%d/%d", t.Year, t.Mon, t.Day);
				break;
			}
		}
		return sstrResult;
	}
	return L"";
}

Times SListAdapter::stamp_to_standard(int stampTime)
{
	time_t tick = (time_t)stampTime;
	struct tm tm;
	char s[100];
	Times standard;
	tm = *localtime(&tick);
	strftime(s, sizeof(s), "%Y-%m-%d %H:%M:%S", &tm);

	standard.Year = atoi(s);
	standard.Mon = atoi(s+5);
	standard.Day = atoi(s+8);
	standard.Hour = atoi(s+11);
	standard.Min = atoi(s+14);
	standard.Second = atoi(s+17);

	return standard;
}
