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