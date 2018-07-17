#include <time.h>
#include <assert.h>
#include "common.h"

ILog    * g_log = 0;
IConfig * g_cfg = 0;
HWND    g_hWnd;
//LmnToolkits::Thread *  g_thrd_db = 0;
LmnToolkits::Thread *  g_thrd_reader[MYCHART_COUNT];
LmnToolkits::Thread *  g_thrd_timer;
LmnToolkits::Thread *  g_thrd_background;

ReaderCmd  PREPARE_COMMAND;
ReaderCmd  READ_TAG_DATA_COMMAND;

DWORD   g_dwCollectInterval[MYCHART_COUNT];
DWORD   g_dwLowTempAlarm[MYCHART_COUNT];
DWORD   g_dwHighTempAlarm[MYCHART_COUNT];
DWORD   g_dwMinTemp[MYCHART_COUNT];
char    g_szComPort[MYCHART_COUNT][32];
char    g_szAlarmFilePath[MAX_ALARM_FILE_PATH];

DWORD SERIAL_PORT_SLEEP_TIME;


char * Time2String(char * szDest, DWORD dwDestSize, const time_t * t) {
	struct tm  tmp;
	localtime_s(&tmp, t);

	_snprintf_s(szDest, dwDestSize, dwDestSize, "%02d:%02d:%02d", tmp.tm_hour, tmp.tm_min, tmp.tm_sec);
	return szDest;
}

BOOL GetAllSerialPortName(std::vector<std::string> & vCom) {

	// TODO: 在此添加控件通知处理程序代码
	HKEY hKey = NULL;
	vCom.clear();

	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("HARDWARE\\DEVICEMAP\\SERIALCOMM"), 0, KEY_READ, &hKey) != ERROR_SUCCESS)
	{
		return FALSE;
	}

	char valuename[200], databuffer[200];
	DWORD valuenamebufferlength = 200, valuetype, databuddersize = 200;

	int i = 0;
	while (RegEnumValue(hKey, i++, valuename, &valuenamebufferlength, NULL, &valuetype, (BYTE*)databuffer, &databuddersize) != ERROR_NO_MORE_ITEMS)
	{
		std::string com_name = databuffer;
		vCom.push_back(com_name);

		databuddersize = 200;
		valuenamebufferlength = 200;
	}

	RegCloseKey(hKey);
	return TRUE;
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

int TransferReaderCmd(ReaderCmd & cmd, const char * szCmd) {
	if (0 == szCmd) {
		return -1;
	}

	SplitString split;
	split.SplitByBlankChars(szCmd);
	DWORD dwSize = split.Size();
	if (dwSize >= sizeof(cmd.abyCommand)) {
		return -1;
	}

	for (DWORD i = 0; i < dwSize; i++) {
		int  tmp = 0;
		sscanf_s(split[i], "%x", &tmp);
		cmd.abyCommand[i] = (BYTE)tmp;
	}
	cmd.dwCommandLength = dwSize;

	return 0;
}