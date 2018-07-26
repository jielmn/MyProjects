#include <time.h>
#include "common.h"
#include "MyImage.h"

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
DWORD     g_dwMyImageTempTextOffsetX = 0;
DWORD     g_dwMyImageTempTextOffsetY = 0;
DWORD     g_dwCollectInterval[MAX_GRID_COUNT];
DWORD     g_dwMyImageMinTemp[MAX_GRID_COUNT];
DWORD     g_dwLowTempAlarm[MAX_GRID_COUNT];
DWORD     g_dwHighTempAlarm[MAX_GRID_COUNT];
BOOL      g_bAlarmOff = FALSE;   // 报警开关是否打开
char      g_szAlarmFilePath[MAX_ALARM_PATH_LENGTH];
char      g_szComPort[MAX_ALARM_PATH_LENGTH][MAX_COM_PORT_LENGTH];
CMySkin   g_skin;
BOOL      g_bAlarmVoiceOff = FALSE;
DWORD     g_dwSkinIndex = 0;

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

LRESULT CDuiMenu::OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	Close();
	bHandled = FALSE;
	return 0;
}

void CDuiMenu::Init(HWND hWndParent, POINT ptPos)
{
	Create(hWndParent, _T("MenuWnd"), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
	::ClientToScreen(hWndParent, &ptPos);
	::SetWindowPos(*this, NULL, ptPos.x, ptPos.y, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
}

void  CDuiMenu::Notify(DuiLib::TNotifyUI& msg) {
	if (msg.sType == "itemclick") {
		if (m_pOwner) {
			m_pOwner->GetManager()->SendNotify(m_pOwner, msg.pSender->GetName(), 0, 0, true);
			PostMessage(WM_CLOSE);
		}
		return;
	}
	WindowImplBase::Notify(msg);
}

LRESULT CDuiMenu::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return __super::HandleMessage(uMsg, wParam, lParam);
}


CControlUI*  CDialogBuilderCallbackEx::CreateControl(LPCTSTR pstrClass) {
	if (0 == strcmp(pstrClass, MYIMAGE_CLASS_NAME)) {
		return new CMyImageUI(m_pManager);
	}
	return NULL;
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