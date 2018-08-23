#include <time.h>
#include "common.h"
#include "AlarmImage.h"

ILog    * g_log = 0;
IConfig * g_cfg = 0;
LmnToolkits::Thread *  g_thrd_db = 0;
HWND    g_hWnd = 0;
CGlobalData  g_data;

//char * Time2String(char * szDest, DWORD dwDestSize, const time_t * t) {
//	struct tm  tmp;
//	localtime_s(&tmp, t);
//
//	_snprintf_s(szDest, dwDestSize, dwDestSize, "%02d:%02d:%02d", tmp.tm_hour, tmp.tm_min, tmp.tm_sec);
//	return szDest;
//}

CControlUI*  CDialogBuilderCallbackEx::CreateControl(LPCTSTR pstrClass) {
	if (0 == strcmp(pstrClass, ALARM_IMAGE_CLASS_NAME)) {
		return new CAlarmImageUI(m_pManager);
	}
	return NULL;
}

DuiLib::CControlUI* CALLBACK MY_FINDCONTROLPROC(DuiLib::CControlUI* pSubControl, LPVOID lpData) {
	if (pSubControl->GetName() == (const char *)lpData) {
		return pSubControl;
	}
	return 0;
}