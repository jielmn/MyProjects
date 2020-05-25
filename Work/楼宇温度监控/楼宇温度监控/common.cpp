#include <time.h>
#include "common.h"
#include "MyControls.h"

CGlobalData  g_data;

CControlUI*  CDialogBuilderCallbackEx::CreateControl(LPCTSTR pstrClass) {
	if ( 0 == strcmp(pstrClass, "TempRule") ) {
		return new CTempRuleUI;
	}
	return NULL;
}

bool sortInt(int d1, int d2) {
	return d1 > d2;
}

CControlUI* CALLBACK FindControlByName(CControlUI* pSubControl, LPVOID pParam) {
	const char * name = (const char *)pParam;
	if (pSubControl->GetName() == name) {
		return pSubControl;
	}
	return 0;
}

DWORD GetIntFromDb(const char * szValue, int nDefault /*= 0*/) {
	DWORD dwValue = nDefault;
	if (szValue)
		sscanf_s(szValue, "%lu", &dwValue);
	return dwValue;
}

char * GetStrFromdDb(char * buf, DWORD dwSize, const char * szValue) {
	assert(dwSize > 0);
	if (szValue)
		STRNCPY(buf, szValue, dwSize);
	else if (dwSize > 0)
		buf[0] = '\0';
	return buf;
}