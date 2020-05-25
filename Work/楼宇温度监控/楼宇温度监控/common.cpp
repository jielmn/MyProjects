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