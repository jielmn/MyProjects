#include <time.h>
#include <locale>
#include "common.h"
#include "EditableButtonUI.h"


CGlobalData  g_data;

CControlUI*  CDialogBuilderCallbackEx::CreateControl(LPCTSTR pstrClass) {
	if (0 == strcmp(pstrClass, "EditableButton")) {
		return new CEditableButtonUI;
	}
	return NULL;
}

CWearItem::CWearItem() {
	memset(m_szDeviceId, 0, sizeof(m_szDeviceId));
	memset(m_szName, 0, sizeof(m_szName));
	m_nPose = 0;
	m_nHearbeat = 0;
	m_nOxy = 0;
	m_nTemp = 0;
}

CWearItem::~CWearItem() {

}

CWearItem & CWearItem::operator =(const CWearItem & item) {
	memcpy(m_szDeviceId, item.m_szDeviceId, sizeof(m_szDeviceId));
	m_nPose = item.m_nPose;

	if (item.m_nHearbeat > 0)
		m_nHearbeat = item.m_nHearbeat;

	if (item.m_nOxy > 0)
		m_nOxy = item.m_nOxy;
	
	if (item.m_nTemp > 0)
		m_nTemp = item.m_nTemp;     

	return *this;
}


// Linux g++ locale 名称: "zh_CN.utf"
// VC2010 locale 名称：    "Chinese"或者"Chinese_china"
#ifdef _MSC_VER
static const char *ZH_CN_LOCALE_STRING = "Chinese_china";
#else
static const char *ZH_CN_LOCALE_STRING = "zh_CN.utf8";
#endif
static const locale zh_CN_locale = locale(ZH_CN_LOCALE_STRING);
static const collate<char>& zh_CN_collate = use_facet<collate<char> >(zh_CN_locale);

int  CharacterCompare(const char * s1, const char * s2) {
	assert(s1 && s2);
	int len1 = strlen(s1);
	int len2 = strlen(s2);
	return zh_CN_collate.compare(s1, s1+len1, s2, s2+len2);
}

BOOL  IsWarningItem(CWearItem * pItem) {
	assert(pItem);

	int nHeartBeat = pItem->m_nHearbeat;
	int nOxy = pItem->m_nOxy;
	int nTemp = pItem->m_nTemp;

	if (nHeartBeat > 0) {
		if ((nHeartBeat >= g_data.m_nMaxHeartBeat) || (nHeartBeat <= g_data.m_nMinHeartBeat)) {
			return TRUE;
		}
	}

	if (nOxy > 0) {
		if (nOxy <= g_data.m_nMinOxy) {
			return TRUE;
		}
	}

	if (nTemp > 0) {
		if (nTemp <= g_data.m_nMinTemp || nTemp >= g_data.m_nMaxTemp) {
			return TRUE;
		}
	}

	return FALSE;
}

time_t  GetTodayZeroTime() {
	time_t now = time(0);

	struct tm tTmTime;
	localtime_s(&tTmTime, &now);

	tTmTime.tm_hour = 0;
	tTmTime.tm_min = 0;
	tTmTime.tm_sec = 0;

	return mktime(&tTmTime);
}

CGetDataRet::~CGetDataRet() {
	if (m_pvHeatBeat) {
		delete m_pvHeatBeat;
	}

	if (m_pvOxy) {
		delete m_pvOxy;
	}

	if (m_pvTemp) {
		delete m_pvTemp;
	}

}