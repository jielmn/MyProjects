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