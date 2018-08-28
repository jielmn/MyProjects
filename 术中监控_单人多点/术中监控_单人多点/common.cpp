#include <time.h>
#include "common.h"
#include "AlarmImage.h"
#include "MyImage.h"

ILog    * g_log = 0;
IConfig * g_cfg = 0;
IConfig * g_cfg_area = 0;
//LmnToolkits::Thread *  g_thrd_db = 0;
LmnToolkits::Thread *  g_thrd_work = 0;
HWND    g_hWnd = 0;
CGlobalData  g_data;
ARGB g_default_argb[MAX_READERS_COUNT] = { 0xFF00FF00,0xFF1b9375,0xFF00FFFF,0xFF51786C, 0xFFFFFF00, 0xFFCA5100, 0xFFFF00FF,
										   0xFFA5A852,0xFFCCCCCC };
std::vector<TArea *>  g_vArea;
DuiLib::CEditUI * g_edRemark = 0;

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

time_t  DateTime2String (const char * szDatetime) {
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

	AddArc(x, y, elWid, elHei, 180, 90); // ◊Û…œΩ«‘≤ª°
	AddLine(x + cornerX, y, x + width - cornerX, y); // …œ±ﬂ

	AddArc(x + width - elWid, y, elWid, elHei, 270, 90); // ”“…œΩ«‘≤ª°
	AddLine(x + width, y + cornerY, x + width, y + height - cornerY);// ”“±ﬂ

	AddArc(x + width - elWid, y + height - elHei, elWid, elHei, 0, 90); // ”“œ¬Ω«‘≤ª°
																		//AddLine(x + width - cornerX, y + height, x + cornerX, y + height); // œ¬±ﬂ

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

void  OnEdtRemarkKillFocus(CControlUI * pUiImage) {
	DuiLib::CDuiString  strRemark = g_edRemark->GetText();
	g_edRemark->SetText("");
	g_edRemark->SetVisible(false);
	g_data.m_bAutoScroll = TRUE;

	CMyImageUI * pImage = (CMyImageUI *)pUiImage;
	pImage->SetRemark(strRemark);
}

char * Date2String_1(char * szDest, DWORD dwDestSize, const time_t * t) {
	struct tm  tmp;
	localtime_s(&tmp, t);

	_snprintf_s(szDest, dwDestSize, dwDestSize, "%04dƒÍ%02d‘¬%02d»’%02d ±%02d∑÷%02d√Î", tmp.tm_year + 1900, tmp.tm_mon + 1, tmp.tm_mday, tmp.tm_hour, tmp.tm_min, tmp.tm_sec);
	return szDest;
}

char * Time2String(char * szDest, DWORD dwDestSize, const time_t * t) {
	struct tm  tmp;
	localtime_s(&tmp, t);

	_snprintf_s(szDest, dwDestSize, dwDestSize, "%02d:%02d:%02d", tmp.tm_hour, tmp.tm_min, tmp.tm_sec);
	return szDest;
}