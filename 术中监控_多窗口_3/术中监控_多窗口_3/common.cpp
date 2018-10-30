#include <time.h>
#include <stdio.h>   
#include <windows.h>  
#include <setupapi.h>
#include "common.h"
#include "MyImage.h"
#include "AlarmImage.h"

CGlobalData  g_data;
std::vector<TArea *>  g_vArea;
ARGB g_default_argb[MAX_READERS_PER_GRID] = { 0xFF00FF00,0xFF1b9375,0xFF00FFFF,0xFF51786C, 0xFFFFFF00, 0xFFCA5100, 0xFFFF00FF,
0xFFA5A852,0xFFCCCCCC };
LmnToolkits::Thread *  g_thrd_work = 0;
LmnToolkits::Thread *  g_thrd_launch = 0;

char * Time2String(char * szDest, DWORD dwDestSize, const time_t * t) {
	struct tm  tmp;
	localtime_s(&tmp, t);

	_snprintf_s(szDest, dwDestSize, dwDestSize, "%02d:%02d:%02d", tmp.tm_hour, tmp.tm_min, tmp.tm_sec);
	return szDest;
}

CControlUI*  CDialogBuilderCallbackEx::CreateControl(LPCTSTR pstrClass) {
	if (0 == strcmp(pstrClass, MYIMAGE_CLASS_NAME)) {
		return new CMyImageUI( CMyImageUI::TYPE_GRID );
	}
	else if (0 == strcmp(pstrClass, MYIMAGE_CLASS_NAME1)) {
		return new CMyImageUI( CMyImageUI::TYPE_MAX );
	}
	else if (0 == strcmp(pstrClass, ALARM_IMAGE_CLASS_NAME)) {
		return new CAlarmImageUI();
	}
	return NULL;
}

DuiLib::CControlUI* CALLBACK MY_FINDCONTROLPROC(DuiLib::CControlUI* pSubControl, LPVOID lpData) {
	if (pSubControl->GetName() == (const char *)lpData) {
		return pSubControl;
	}
	return 0;
}

DuiLib::CControlUI* CALLBACK CS_FINDCONTROLPROC(DuiLib::CControlUI* pSubControl, LPVOID lpData) {
	if ( 0 == strcmp( pSubControl->GetClass(),DUI_CTR_LABEL )) {
		((CLabelUI *)pSubControl)->SetTextColor(g_data.m_skin[CMySkin::COMMON_TEXT]);
	}
	else if (0 == strcmp(pSubControl->GetClass(), DUI_CTR_BUTTON)) {
		((CButtonUI *)pSubControl)->SetTextColor(g_data.m_skin[CMySkin::COMMON_TEXT]);
	}
	else if (0 == strcmp(pSubControl->GetClass(), DUI_CTR_OPTION)) {
		((COptionUI *)pSubControl)->SetSelectedImage(g_data.m_skin.GetImageName(CMySkin::OPT_SELECTED));
		((COptionUI *)pSubControl)->SetNormalImage(g_data.m_skin.GetImageName(CMySkin::OPT_NOT_SELECTED));
	}
	return 0;
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

DWORD   GetMinTemp(DWORD  dwIndex) {
	switch (dwIndex)
	{
	case 0:
		return 20;
	case 1:
		return 24;
	case 2:
		return 28;
	case 3:
		return 32;
	default:
		break;
	}
	return 20;
}

DWORD   GetCollectInterval(DWORD dwIndex) {
	switch (dwIndex)
	{
	case 0:
		return 10;
	case 1:
		return 60;
	case 2:
		return 300;
	case 3:
		return 900;
	case 4:
		return 1800;
	case 5:
		return 3600;
	default:
		break;
	}
	return 10;
}

time_t  DateTime2String(const char * szDatetime) {
	struct tm  tmp;
	if (6 != sscanf_s(szDatetime, "%d-%d-%d %d:%d:%d", &tmp.tm_year, &tmp.tm_mon, &tmp.tm_mday, &tmp.tm_hour, &tmp.tm_min, &tmp.tm_sec)) {
		return 0;
	}

	tmp.tm_year -= 1900;
	tmp.tm_mon--;
	return mktime(&tmp);
}

CGraphicsRoundRectPath::CGraphicsRoundRectPath(void)
	:Gdiplus::GraphicsPath()
{

}

void CGraphicsRoundRectPath::AddRoundRect(INT x, INT y, INT width, INT height, INT cornerX, INT cornerY)
{
	INT elWid = 2 * cornerX;
	INT elHei = 2 * cornerY;

	AddArc(x, y, elWid, elHei, 180, 90); // 左上角圆弧
	AddLine(x + cornerX, y, x + width - cornerX, y); // 上边

	AddArc(x + width - elWid, y, elWid, elHei, 270, 90); // 右上角圆弧
	AddLine(x + width, y + cornerY, x + width, y + height - cornerY);// 右边

	AddArc(x + width - elWid, y + height - elHei, elWid, elHei, 0, 90); // 右下角圆弧
																		//AddLine(x + width - cornerX, y + height, x + cornerX, y + height); // 下边

	int x1 = x + width - cornerX;
	int y1 = y + height;
	int x2 = x + width / 2 + 10;
	int y2 = y1;
	AddLine(x1, y1, x2, y2);
	x1 = x2;
	y1 = y2;
	x2 = x + width / 2;
	y2 = y1 + 10;
	AddLine(x1, y1, x2, y2);
	x1 = x2;
	y1 = y2;
	x2 = x + width / 2 - 10;
	y2 = y + height;
	AddLine(x1, y1, x2, y2);


	AddArc(x, y + height - elHei, elWid, elHei, 90, 90);
	AddLine(x, y + cornerY, x, y + height - cornerY);
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

BOOL EnumPortsWdm(std::vector<std::string> & v)
{
	// Create a device information set that will be the container for 
	// the device interfaces.
	GUID *guidDev = (GUID*)&GUID_CLASS_COMPORT;
	HDEVINFO hDevInfo = INVALID_HANDLE_VALUE;
	SP_DEVICE_INTERFACE_DETAIL_DATA *pDetData = NULL;

	hDevInfo = SetupDiGetClassDevs(guidDev,
		NULL,
		NULL,
		DIGCF_PRESENT | DIGCF_DEVICEINTERFACE
	);

	if (INVALID_HANDLE_VALUE == hDevInfo)
	{
		return FALSE;
	}

	// Enumerate the serial ports
	BOOL bOk = TRUE;
	SP_DEVICE_INTERFACE_DATA ifcData;
	DWORD dwDetDataSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA) + 256;
	pDetData = (SP_DEVICE_INTERFACE_DETAIL_DATA*)new char[dwDetDataSize];
	if (!pDetData)
	{
		return FALSE;
	}
	// This is required, according to the documentation. Yes,
	// it's weird.
	ifcData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
	pDetData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
	for (DWORD ii = 0; bOk; ii++)
	{
		bOk = SetupDiEnumDeviceInterfaces(hDevInfo,
			NULL, guidDev, ii, &ifcData);
		if (bOk)
		{
			// Got a device. Get the details.
			SP_DEVINFO_DATA devdata = { sizeof(SP_DEVINFO_DATA) };
			bOk = SetupDiGetDeviceInterfaceDetail(hDevInfo,
				&ifcData, pDetData, dwDetDataSize, NULL, &devdata);
			if (bOk) {
				// Got a path to the device. Try to get some more info.
				CHAR fname[256] = { 0 };
				CHAR desc[256] = { 0 };
				BOOL bSuccess = SetupDiGetDeviceRegistryProperty(
					hDevInfo, &devdata, SPDRP_FRIENDLYNAME, NULL,
					(PBYTE)fname, sizeof(fname), NULL);
				bSuccess = bSuccess && SetupDiGetDeviceRegistryProperty(
					hDevInfo, &devdata, SPDRP_DEVICEDESC, NULL,
					(PBYTE)desc, sizeof(desc), NULL);
				BOOL bUsbDevice = FALSE;
				CHAR locinfo[256] = { 0 };
				if (SetupDiGetDeviceRegistryProperty(
					hDevInfo, &devdata, SPDRP_LOCATION_INFORMATION, NULL,
					(PBYTE)locinfo, sizeof(locinfo), NULL))
				{

					bUsbDevice = (strncmp(locinfo, "USB", 3) == 0);
				}
				if (bSuccess)
				{
					//printf("FriendlyName = %S\r\n", fname);
					//printf("Port Desc = %S\r\n", desc);

					v.push_back(fname);
				}

			}
			else
			{
				if (pDetData != NULL)
				{
					delete[](char*)pDetData;
				}
				if (hDevInfo != INVALID_HANDLE_VALUE)
				{
					SetupDiDestroyDeviceInfoList(hDevInfo);
				}
				return FALSE;
			}
		}
		else
		{
			DWORD err = GetLastError();
			if (err != ERROR_NO_MORE_ITEMS)
			{
				if (pDetData != NULL)
				{
					delete[](char*)pDetData;
				}
				if (hDevInfo != INVALID_HANDLE_VALUE)
				{
					SetupDiDestroyDeviceInfoList(hDevInfo);
				}
				return FALSE;
			}
		}
	}

	if (pDetData != NULL)
	{
		delete[](char*)pDetData;
	}
	if (hDevInfo != INVALID_HANDLE_VALUE)
	{
		SetupDiDestroyDeviceInfoList(hDevInfo);
	}

	return TRUE;
}

int  GetCh340Count(char * szComPort, DWORD dwComPortLen) {
	std::vector<std::string> vComPorts;
	EnumPortsWdm(vComPorts);

	char buf[8192];
	int nFindCount = 0;
	std::vector<std::string>::iterator it;
	for (it = vComPorts.begin(); it != vComPorts.end(); it++) {
		std::string & s = *it;
		Str2Lower(s.c_str(), buf, sizeof(buf));

		if (0 != strstr(buf, "usb-serial ch340")) {
			// 如果是第一次记录 comport
			if (0 == nFindCount) {
				int nComPort = 0;
				const char * pFind = strstr(buf, "com");
				while (pFind) {
					if (1 == sscanf(pFind + 3, "%d", &nComPort)) {
						break;
					}
					pFind = strstr(pFind + 3, "com");
				}
				assert(nComPort > 0);
				SNPRINTF(szComPort, dwComPortLen, "com%d", nComPort);
			}
			nFindCount++;
		}
	}
	return nFindCount;
}