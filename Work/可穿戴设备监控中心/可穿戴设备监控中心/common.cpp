#include <time.h>
#include "common.h"
#include "EditableButtonUI.h"

CGlobalData  g_data;

CControlUI*  CDialogBuilderCallbackEx::CreateControl(LPCTSTR pstrClass) {
	if (0 == strcmp(pstrClass, "EditableButton")) {
		return new CEditableButtonUI;
	}
	return NULL;
}