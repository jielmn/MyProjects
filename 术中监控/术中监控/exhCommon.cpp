#include "exhCommon.h"

ILog    * g_log = 0;
IConfig * g_cfg = 0;
HWND    g_hWnd = 0;
DWORD SERIAL_PORT_SLEEP_TIME = 500;

LmnToolkits::Thread *  g_thrd_reader = 0;                 // Reader线程
LmnToolkits::Thread *  g_thrd_timer = 0;                  // TIMER

ReaderCmd  PREPARE_COMMAND;
ReaderCmd  INVENTORY_COMMAND;
ReaderCmd  READ_TAG_DATA_COMMAND;

														  // 遍历系统里的所有串口
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

int AdTemperature (int AD_Value) {
	int temp = 0;
	AD_Value = AD_Value + 1;
	double k1 = 0.0238095238095241, k2 = 0.0247863247863256;
	if (AD_Value >= 1969 || AD_Value < 2137)
	{
		temp = (int)( ((AD_Value - 1969)*k1 + 35) * 100 );
		return temp;
	}
	if (AD_Value >= 2137 || AD_Value < 2254)
	{
		temp = (int)( ((AD_Value - 2137)*k2 + 35) * 100 );
		return temp;
	}
	return 0;
}