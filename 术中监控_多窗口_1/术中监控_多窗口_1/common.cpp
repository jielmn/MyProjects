#include <time.h>
#include "common.h"

ILog    * g_log = 0;
IConfig * g_cfg = 0;
//LmnToolkits::Thread *  g_thrd_db = 0;
DWORD     g_dwLayoutColumns = 0;
DWORD     g_dwLayoutRows = 0;
DWORD     g_dwTimeUnitWidth = 0;
DWORD     g_dwMaxPointsCount = 0;
DWORD     g_dwMyImageLeftBlank = 0;
DWORD     g_dwMyImageRightBlank = 0;
DWORD     g_dwMyImageTimeTextOffsetX = 0;
DWORD     g_dwMyImageTimeTextOffsetY = 0;
DWORD     g_dwCollectInterval[MAX_GRID_COUNT];
DWORD     g_dwMyImageMinTemp[MAX_GRID_COUNT];
DWORD     g_dwLowTempAlarm[MAX_GRID_COUNT];
DWORD     g_dwHighTempAlarm[MAX_GRID_COUNT];
BOOL      g_bAlarmOff = FALSE;   // 报警开关是否打开
char      g_szAlarmFilePath[MAX_ALARM_PATH_LENGTH];
char      g_szComPort[MAX_ALARM_PATH_LENGTH][MAX_COM_PORT_LENGTH];
CMySkin   g_skin;

char * Time2String(char * szDest, DWORD dwDestSize, const time_t * t) {
	struct tm  tmp;
	localtime_s(&tmp, t);

	_snprintf_s(szDest, dwDestSize, dwDestSize, "%02d:%02d:%02d", tmp.tm_hour, tmp.tm_min, tmp.tm_sec);
	return szDest;
}

DuiLib::CControlUI* CALLBACK MY_FINDCONTROLPROC(DuiLib::CControlUI* pSubControl, LPVOID lpData) {
	if (pSubControl->GetName() == (const char *)lpData) {
		return pSubControl;
	}
	return 0;
}

char * GetDefaultAlarmFile(char * szDefaultFile, DWORD dwSize) {
	char buf[8192];

	GetModuleFileName(0, buf, sizeof(buf));
	const char * pStr = strrchr(buf, '\\');
	assert(pStr);
	DWORD  dwTemp = pStr - buf;
	buf[dwTemp] = '\0';

	SNPRINTF(szDefaultFile, dwSize, "%s%s", buf, DEFAULT_ALARM_FILE_PATH);
	return szDefaultFile;
}