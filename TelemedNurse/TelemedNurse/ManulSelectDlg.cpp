#include <afx.h>   
#include "ManulSelectDlg.h"


CManulSelectWnd::CManulSelectWnd() {
	memset(m_TempData, 0, sizeof(m_TempData));
	m_tFirstTime = 0;
	m_nWeekIndex = 0;
}

void   CManulSelectWnd::OnItemSelected(const CDuiString & name) {
	int nComboIndex = 0;
	sscanf_s((const char *)name, "combo%d", &nComboIndex);
	nComboIndex--;

	CComboUI * pCombos[6] = { 0 };
	CDuiString str;

	for (int i = 0; i < 6; i++) {
		str.Format("combo%d", i + 1);
		pCombos[i] = (CComboUI *)m_PaintManager.FindControl((const char *)str);
	}

	// 拿到那天的温度数据
	CDateTimeUI * pDateTime = (CDateTimeUI *)m_PaintManager.FindControl("DateTime2");
	SYSTEMTIME st = pDateTime->GetTime();
	time_t tNewTime = ConvertDateTime(st);
	// 超出时间范围
	if (tNewTime < m_tFirstTime) {
		return;
	}

	int nDayIndex = (int)((tNewTime - m_tFirstTime) / (3600 * 24));
	// 超出时间范围
	if (nDayIndex >= (m_nWeekIndex + 1) * 7) {
		return;
	}
	TagData * pDay = m_TempData[nDayIndex];


	int nSel = pCombos[nComboIndex]->GetCurSel();
	CListLabelElementUI * pElement = (CListLabelElementUI *)pCombos[nComboIndex]->GetItemAt(nSel);
	TagData* pItem = (TagData*)pElement->GetTag();
	if (0 == pItem) {
		memset( &pDay[nComboIndex], 0, sizeof(TagData));
	}
	else {
		for (int i = 0; i < 6; i++) {
			if (i == nComboIndex) {
				continue;
			}

			if (0 == pCombos[i]) {
				continue;
			}

			int nTempSel = pCombos[i]->GetCurSel();
			CListLabelElementUI * pTmpElement = (CListLabelElementUI *)pCombos[i]->GetItemAt(nTempSel);
			TagData* pTmpItem = (TagData*)pTmpElement->GetTag();
			if (0 == pTmpItem) {
				continue;
			}

			// 时间重复
			if (pTmpItem->tTime == pItem->tTime) {
				pCombos[i]->SelectItem(0,false,false);
				memset(&pDay[i], 0, sizeof(TagData));
				break;
			}			
		}
		memcpy(&pDay[nComboIndex], pItem, sizeof(TagData));
	}
}

void   CManulSelectWnd::Notify(TNotifyUI& msg) {
	CDuiString name = msg.pSender->GetName();
	if (msg.sType == "itemselect") {
		OnItemSelected(name);
		return;
	}
	else if (msg.sType == "click") {
		if ( name == "btnManulSelectOk" )
		{
			m_sigOK.emit(m_TempData);
			::PostMessage(m_hWnd, WM_CLOSE, 0, 0);
			return;
		}
	}
	WindowImplBase::Notify(msg);
}

void   CManulSelectWnd::InitWindow() {
	int nDayCount = (m_nWeekIndex + 1) * 7;
	time_t  tLastDay = m_tFirstTime + 3600 * 24 * (nDayCount - 1);

	CDuiString  str;
	char szStart[256];
	char szEnd[256];
	ConvertDate(szStart, sizeof(szStart), &m_tFirstTime);
	ConvertDate(szEnd,   sizeof(szEnd),   &tLastDay);

	str.Format( "%s到%s", szStart, szEnd );

	CControlUI * pCtl = m_PaintManager.FindControl("lblRange");
	if (pCtl) {
		pCtl->SetText(str);
	}

	CDateTimeUI * pDateTime = (CDateTimeUI *)m_PaintManager.FindControl("DateTime2");
	if (pDateTime) {
		pDateTime->SetText(szStart);
		pDateTime->SetTime( &ConvertDateTime(m_tFirstTime) );
	}

	OnTimeChanged(m_tFirstTime);

	WindowImplBase::InitWindow();
}

LRESULT   CManulSelectWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if (WM_NOTIFY == uMsg) {
		NMHDR* pHeader = (NMHDR*)lParam;
		if (pHeader && pHeader->code == DTN_DATETIMECHANGE) {
			CDateTimeUI * pTempDataStartTime = (CDateTimeUI *)m_PaintManager.FindControl("DateTime2");
			if (pTempDataStartTime && pTempDataStartTime->GetNativeWindow() == pHeader->hwndFrom) {
				LPNMDATETIMECHANGE lpChage = (LPNMDATETIMECHANGE)lParam;
				time_t tStartTime = ConvertDateTime(lpChage->st);				
				OnTimeChanged(tStartTime);
			}
			return 0;
		}
	}
	return WindowImplBase::HandleMessage(uMsg, wParam, lParam);
}

void  CManulSelectWnd::OnTimeChanged(time_t & tNewTime) {
	CComboUI * pCombos[6] = { 0 };
	CDuiString str;

	for (int i = 0; i < 6; i++) {
		str.Format("combo%d",i + 1);
		pCombos[i] = (CComboUI *)m_PaintManager.FindControl((const char *)str);
		if (pCombos[i]) {
			pCombos[i]->RemoveAll();

			CListLabelElementUI * pElement = new CListLabelElementUI;
			pElement->SetText("无");
			pElement->SetTag(0);
			pCombos[i]->Add(pElement);
			pCombos[i]->SelectItem(0, false, false);
		}
	}

	// 如果超出时间范围
	if (tNewTime < m_tFirstTime) {
		return;
	}

	// 拿到那天的温度数据
	int nDayIndex = (int) ( (tNewTime - m_tFirstTime) / (3600 * 24) );
	// 如果超出范围
	if (nDayIndex >= (m_nWeekIndex+1) * 7) {
		return;
	}

	TagData * pDay = m_TempData[nDayIndex];

	std::vector<TagData*> v;
	std::vector<TagData*>::iterator it;
	for (it = m_pvTempetatureData->begin(); it != m_pvTempetatureData->end(); it++) {
		TagData* pItem = *it;
		if (pItem->tTime < tNewTime) {
			continue;
		}

		if (pItem->tTime - tNewTime >= 3600 * 24) {
			break;
		}

		v.push_back(pItem);
	}

	char szTime[256];
	for (int i = 0; i < 6; i++) {
		if ( 0 == pCombos[i] ) {
			continue;
		}

		int k = 0;
		for (it = v.begin(), k = 0; it != v.end(); it++, k++) {
			TagData* pItem = *it;

			struct tm  tmp;
			localtime_s(&tmp, &pItem->tTime);
			_snprintf_s(szTime, sizeof(szTime), _TRUNCATE, "%02d:%02d", tmp.tm_hour, tmp.tm_min );

			str.Format("%d 时间:%s 温度:%.2f", k+1, szTime, pItem->dwTemperature / 100.0);

			CListLabelElementUI * pElement = new CListLabelElementUI;
			pElement->SetText(str);
			pElement->SetTag( (UINT_PTR)pItem );
			pCombos[i]->Add(pElement);
		}

		TagData * pTmpDay = pDay + i;
		int nCnt = pCombos[i]->GetCount();

		for (int j = 1; j < nCnt; j++) {
			CListLabelElementUI * pElement = (CListLabelElementUI *)pCombos[i]->GetItemAt(j);
			TagData* pItem = (TagData*)pElement->GetTag();
			// 时间相同
			if (pItem->tTime == pTmpDay->tTime) {
				pCombos[i]->SelectItem(j, false, false);
				break;
			}
		}

	}
}