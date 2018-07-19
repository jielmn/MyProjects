// win32_1.cpp : 定义应用程序的入口点。
//

#ifdef _DEBUG
#include "vld.h"
#endif

#include "Windows.h"
#include "main.h"
#include "business.h"
#include "resource.h"
#include "AboutDlg.h"
#include "SettingDlg.h"  
#include "MyImage.h"
#include "LmnTelSvr.h"
#include <time.h>

class CDuiMenu : public DuiLib::WindowImplBase
{
protected:
	virtual ~CDuiMenu() {};        // 私有化析构函数，这样此对象只能通过new来生成，而不能直接定义变量。就保证了delete this不会出错
	DuiLib::CDuiString  m_strXMLPath;
	DuiLib::CControlUI * m_pOwner;

public:
	explicit CDuiMenu(LPCTSTR pszXMLPath, DuiLib::CControlUI * pOwner) : m_strXMLPath(pszXMLPath), m_pOwner(pOwner) {}
	virtual LPCTSTR    GetWindowClassName()const { return _T("CDuiMenu "); }
	virtual DuiLib::CDuiString GetSkinFolder() { return _T(""); }
	virtual DuiLib::CDuiString GetSkinFile() { return m_strXMLPath; }
	virtual void       OnFinalMessage(HWND hWnd) { delete this; }

	virtual LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		Close();
		bHandled = FALSE;
		return 0;
	}

	void Init(HWND hWndParent, POINT ptPos)
	{
		Create(hWndParent, _T("MenuWnd"), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
		::ClientToScreen(hWndParent, &ptPos);
		::SetWindowPos(*this, NULL, ptPos.x, ptPos.y, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
	}

	virtual void  Notify(DuiLib::TNotifyUI& msg) {
		if (msg.sType == "itemclick") {
			if (m_pOwner) {
				m_pOwner->GetManager()->SendNotify(m_pOwner, msg.pSender->GetName(), 0, 0, true);
				PostMessage(WM_CLOSE);
			}
			return;
		}
		WindowImplBase::Notify(msg);
	}

	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		return __super::HandleMessage(uMsg, wParam, lParam);
	}
};

CControlUI*  CDialogBuilderCallbackEx::CreateControl(LPCTSTR pstrClass) {
	if (0 == strcmp("MyImage", pstrClass)) {
		return new CMyImageUI(m_pManager);
	}

	return NULL;
}



void  CDuiFrameWnd::OnSetting() {
	CDuiString  strText;
	CSettingDlg * pSettingDlg = new CSettingDlg;

	pSettingDlg->Create(this->m_hWnd, _T("设置"), UI_WNDSTYLE_FRAME | WS_POPUP, NULL, 0, 0, 0, 0);
	pSettingDlg->CenterWindow();
	int ret = pSettingDlg->ShowModal();

	// 如果OK
	if (0 == ret) {
		for (int i = 0; i < MYCHART_COUNT; i++) {
			strText.Format("low alarm %d", i + 1);
			g_cfg->SetConfig( strText, g_dwLowTempAlarm[i]);

			strText.Format("high alarm %d", i + 1);
			g_cfg->SetConfig(strText, g_dwHighTempAlarm[i]);

			strText.Format("collect interval %d", i + 1);
			g_cfg->SetConfig(strText, g_dwCollectInterval[i]);

			strText.Format("min temperature degree %d", i + 1);
			g_cfg->SetConfig(strText, g_dwMinTemp[i]);

			strText.Format("com port %d", i + 1);
			g_cfg->SetConfig(strText, g_szComPort[i]);
		}
		
		g_cfg->Save();

		for (int i = 0; i < MYCHART_COUNT; i++) {
			if (m_ChartUi[i]) {
				m_ChartUi[i]->Invalidate_0();
			}
		}
		
	}

	delete pSettingDlg;
}

void  CDuiFrameWnd::OnAbout() {
	CAboutDlg * pAboutDlg = new CAboutDlg;

	pAboutDlg->Create(this->m_hWnd, _T("关于"), UI_WNDSTYLE_FRAME | WS_POPUP, NULL, 0, 0, 0, 0);
	pAboutDlg->CenterWindow();
	int ret = pAboutDlg->ShowModal();

	// 如果OK
	if (0 == ret) {

	}

	delete pAboutDlg;
}

CControlUI* CALLBACK MY_FINDCONTROLPROC (CControlUI* pThis, LPVOID lpData) {
	const char * szName = (const char *)lpData;
	if ( 0 == strcmp(pThis->GetName(), szName) ) {
		return pThis;
	}
	return 0;
}

void  CDuiFrameWnd::OnDbClick() {
	POINT point;
	CDuiString strName;

	GetCursorPos(&point);
	::ScreenToClient(m_hWnd, &point);
	CControlUI * pFindControl = m_PaintManager.FindSubControlByPoint(m_layMain, point);

	while ( pFindControl ) {
		strName = pFindControl->GetName();
		if ( 0 == StrNiCmp((const char *)strName, "mychart", 7) ) {
			int nIndex = 0;
			sscanf(strName, "mychart_%d", &nIndex);
			assert( nIndex > 0 && nIndex <= MYCHART_COUNT );
			nIndex--;

			if ( m_chart_state == CHART_STATE_NORMAL ) {
				int nLayIndex = nIndex / MYCHART_PER_LAYER;
				
				for (int i = 0; i < MYCHART_COUNT; i++) {
					if (i != nIndex) {
						m_chart[i]->SetVisible(false);
					}					
				}

				for (int i = 0; i < LAY_LAYER_COUNT; i++) {
					if (i != nLayIndex) {
						m_layLayer[i]->SetVisible(false);
					}
				}       
				m_chart_state = CHART_STATE_EXPAND;

				CControlUI * pFlex = m_chart[nIndex]->FindControl( MY_FINDCONTROLPROC, "layFlex", 0);
				if ( pFlex) {  
					pFlex->SetAttribute("height", "150");
				}
			}
			else {
				for (int i = 0; i < LAY_LAYER_COUNT; i++) {
					m_layLayer[i]->SetVisible(true);
				}
				for (int i = 0; i < MYCHART_COUNT; i++) {
					m_chart[i]->SetVisible(true);
				}
				m_chart_state = CHART_STATE_NORMAL;

				CControlUI * pFlex = m_chart[nIndex]->FindControl(MY_FINDCONTROLPROC, "layFlex", 0);
				if (pFlex) {
					pFlex->SetAttribute("height", "72");
				}
			}
		}
		pFindControl = pFindControl->GetParent();
	}
}

void   CDuiFrameWnd::OnReaderStatus(WPARAM wParam, LPARAM lParam) {
	CTelemedReader::READER_STATUS eStatus = (CTelemedReader::READER_STATUS)wParam;
	int  nIndex = (int)lParam;
	assert(nIndex >= 0 && nIndex < MYCHART_COUNT);

	if (eStatus == CTelemedReader::STATUS_OPEN)
	{
		m_lblReaderStatus[nIndex]->SetText("读卡器连接OK");
		m_lblReaderComPort[nIndex]->SetText(CBusiness::GetInstance()->GetReaderComPort(nIndex));

		// 获取温度
		CBusiness::GetInstance()->ReadTagTempAsyn(nIndex);
	}
	else
	{
		m_lblReaderStatus[nIndex]->SetText("读卡器连接失败");
		m_lblReaderComPort[nIndex]->SetText("--");
	}
}

void   CDuiFrameWnd::OnReaderTemp(WPARAM wParam, LPARAM lParam) {
	int    ret = (int)wParam;
	DWORD  dwTemp = lParam & 0xFFFF;
	int    nIndex = lParam >> 16;
	
	assert(nIndex >= 0 && nIndex < MYCHART_COUNT);

	if (0 == ret) {
		OnTemperatureResult( nIndex, 0, dwTemp);
		// 获取温度
		CBusiness::GetInstance()->ReadTagTempAsyn( nIndex, g_dwCollectInterval[nIndex] * 1000 );
		m_nTimeNextMesure[nIndex] = g_dwCollectInterval[nIndex];
		m_tTimeNextMesure[nIndex] = time(0);
		ShowTimeLeft(nIndex);
	}
	// 获取温度失败
	else {
		OnTemperatureResult(nIndex, ret);
		m_nTimeNextMesure[nIndex] = -1;
		ShowTimeLeftError(nIndex);
	}
}

void   CDuiFrameWnd::ShowTimeLeft(int nIndex) {
	assert(nIndex >= 0 && nIndex < MYCHART_COUNT);

	if (m_nTimeNextMesure[nIndex] > 0) {
		CDuiString  strTimeLeft;
		time_t  now = time(0);
		int nElapsed = int(now - m_tTimeNextMesure[nIndex]);
		int nLeft = m_nTimeNextMesure[nIndex] - nElapsed;
		if (nLeft < 0) {
			nLeft = 0;
		}

		strTimeLeft.Format("%ld", nLeft);
		m_lblTimeLeft[nIndex]->SetText(strTimeLeft);
	}
	else {
		ShowTimeLeftError(nIndex);
	}
	
}

void   CDuiFrameWnd::ShowTimeLeftError(int nIndex) {
	assert(nIndex >= 0 && nIndex < MYCHART_COUNT);
	m_lblTimeLeft[nIndex]->SetText("--");
}

void   CDuiFrameWnd::OnTemperatureResult(int nIndex, int nRet, DWORD dwTemp /*= 0*/) {
	assert(nIndex >= 0 && nIndex < MYCHART_COUNT);

	if (0 != nRet) {
		m_lblTemperature[nIndex]->SetTextColor(0xFFFFFFFF);
		m_lblTemperature[nIndex]->SetText("--");
		return;
	}

	m_ChartUi[nIndex]->AddTemp(dwTemp);

	if (dwTemp < g_dwLowTempAlarm[nIndex]) {
		m_lblTemperature[nIndex]->SetTextColor(m_dwLowTempColor);
	}
	else if (dwTemp > g_dwHighTempAlarm[nIndex]) {
		m_lblTemperature[nIndex]->SetTextColor(m_dwHighTempColor);
	}
	else {
		m_lblTemperature[nIndex]->SetTextColor(m_dwNormalColor);
	}

	DuiLib::CDuiString  strTemp;
	strTemp.Format("%.2f", (dwTemp / 100.0));
	m_lblTemperature[nIndex]->SetText(strTemp);

	if ((int)dwTemp > m_nHighestTemp[nIndex]) {
		m_nHighestTemp[nIndex] = dwTemp;

		strTemp.Format("%.2f", (dwTemp / 100.0));
		m_lblHighTemperature[nIndex]->SetText(strTemp);
	}

	if ((int)dwTemp < m_nLowestTemp[nIndex]) {
		m_nLowestTemp[nIndex] = dwTemp;

		strTemp.Format("%.2f", (dwTemp / 100.0));
		m_lblLowTemperature[nIndex]->SetText(strTemp);
	}
}

void   CDuiFrameWnd::OnTimer(WPARAM wParam, LPARAM lParam) {
	if (TIME_LEFT_TIMER_ID == wParam) {
		for (int i = 0; i < MYCHART_COUNT; i++) {
			ShowTimeLeft(i);        
		}
	}
}

void   CDuiFrameWnd::OnUpdateScroll(WPARAM wParam, LPARAM lParam) {
	int nIndex = (int)wParam;
	assert(nIndex >= 0 && nIndex < MYCHART_COUNT);

	DuiLib::CVerticalLayoutUI * pParent = (DuiLib::CVerticalLayoutUI *)m_ChartUi[nIndex]->GetParent();
	SIZE tParentScrollPos = pParent->GetScrollPos();
	SIZE tParentScrollRange = pParent->GetScrollRange();
	pParent->SetScrollPos(tParentScrollRange);    
}




CDuiFrameWnd::CDuiFrameWnd() : m_callback(&m_PaintManager) {
	char buf[8192];
	g_cfg->GetConfig("low temperature color", buf, sizeof(buf), COLOR_LOW_TEMPERATURE);
	sscanf(buf, "0x%x", &m_dwLowTempColor);

	g_cfg->GetConfig("normal temperature color", buf, sizeof(buf), COLOR_NORMAL_TEMPERATURE);
	sscanf(buf, "0x%x", &m_dwNormalColor);

	g_cfg->GetConfig("high temperature color", buf, sizeof(buf), COLOR_HIGH_TEMPERATURE);
	sscanf(buf, "0x%x", &m_dwHighTempColor);

	for (int i = 0; i < MYCHART_COUNT; i++) {
		m_nLowestTemp[i] = 10000;
		m_nHighestTemp[i] = 0;
		m_nTimeNextMesure[i] = -1;
		m_tTimeNextMesure[i] = 0;
	}	
}

void  CDuiFrameWnd::InitWindow() {
	CDuiString strText;
	char buf[8192];

	g_hWnd = GetHWND();

	PostMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0);

	m_btnMenu = static_cast<DuiLib::CButtonUI*>(m_PaintManager.FindControl("menubtn"));

	m_layMain = static_cast<CVerticalLayoutUI*>(m_PaintManager.FindControl("layMain"));

	for (int i = 0; i < LAY_LAYER_COUNT; i++) {
		strText.Format("lay_%d", i + 1);
		m_layLayer[i] = static_cast<CHorizontalLayoutUI*>(m_PaintManager.FindControl(strText));
	}

	for (int i = 0; i < MYCHART_COUNT; i++) {
		strText.Format("mychart_%d", i+1);
		m_chart[i] = m_PaintManager.FindControl(strText);
	}
	m_chart_state = CHART_STATE_NORMAL; 

	for (int i = 0; i < MYCHART_COUNT; i++) {
		strText.Format("lblComStatus_%d", i + 1);
		m_lblReaderStatus[i] = static_cast<DuiLib::CLabelUI*>(m_PaintManager.FindControl(strText));
		m_lblReaderStatus[i]->SetText("读卡器连接失败");

		strText.Format("lblCom_%d", i + 1);
		m_lblReaderComPort[i] = static_cast<DuiLib::CLabelUI*>(m_PaintManager.FindControl(strText));
		m_lblReaderComPort[i]->SetText("--");
	}

	for (int i = 0; i < MYCHART_COUNT; i++) {
		strText.Format("lblCur_%d", i + 1);
		m_lblTemperature[i] = static_cast<DuiLib::CLabelUI*>(m_PaintManager.FindControl(strText));
		m_lblTemperature[i]->SetText("--");

		strText.Format("lblMax_%d", i + 1);
		m_lblHighTemperature[i] = static_cast<DuiLib::CLabelUI*>(m_PaintManager.FindControl(strText));
		m_lblHighTemperature[i]->SetText("--");

		strText.Format("lblMin_%d", i + 1);
		m_lblLowTemperature[i] = static_cast<DuiLib::CLabelUI*>(m_PaintManager.FindControl(strText));
		m_lblLowTemperature[i]->SetText("--");

		strText.Format("lblNextMeasure_%d", i + 1);
		m_lblTimeLeft[i] = static_cast<DuiLib::CLabelUI*>(m_PaintManager.FindControl(strText));
		m_lblTimeLeft[i]->SetText("--");			
	}

	for (int i = 0; i < MYCHART_COUNT; i++) {
		strText.Format("edtBed_%d", i + 1);
		m_edtBedNo[i] = static_cast<DuiLib::CEditUI*>(m_PaintManager.FindControl(strText));

		strText.Format("edtName_%d", i + 1);
		m_edtName[i] = static_cast<DuiLib::CEditUI*>(m_PaintManager.FindControl(strText));

		strText.Format("edtSex_%d", i + 1);
		m_edtSex[i] = static_cast<DuiLib::CEditUI*>(m_PaintManager.FindControl(strText));

		strText.Format("edtAge_%d", i + 1);
		m_edtAge[i] = static_cast<DuiLib::CEditUI*>(m_PaintManager.FindControl(strText));


		strText.Format("btnBed_%d", i + 1);
		m_btnBedNo[i] = static_cast<DuiLib::CButtonUI*>(m_PaintManager.FindControl(strText));

		strText.Format("btnName_%d", i + 1);
		m_btnName[i] = static_cast<DuiLib::CButtonUI*>(m_PaintManager.FindControl(strText));

		strText.Format("btnSex_%d", i + 1);
		m_btnSex[i] = static_cast<DuiLib::CButtonUI*>(m_PaintManager.FindControl(strText));

		strText.Format("btnAge_%d", i + 1);
		m_btnAge[i] = static_cast<DuiLib::CButtonUI*>(m_PaintManager.FindControl(strText));


		if ( m_edtBedNo[i] ) {
			m_edtBedNo[i]->SetVisible(false);
		}

		if (m_edtName[i]) {
			m_edtName[i]->SetVisible(false);
		}

		if (m_edtSex[i]) {
			m_edtSex[i]->SetVisible(false);
		}

		if (m_edtAge[i]) {
			m_edtAge[i]->SetVisible(false);
		}

		if (m_btnBedNo[i]) {
			strText.Format("last bed no %d", i + 1);
			g_cfg->GetConfig(strText, buf, sizeof(buf), "---");
			m_btnBedNo[i]->SetText(buf);
			m_btnBedNo[i]->SetVisible(true);
		}

		if (m_btnName[i]) {
			strText.Format("last name %d", i + 1);
			g_cfg->GetConfig(strText, buf, sizeof(buf), "--");
			m_btnName[i]->SetText(buf);
			m_btnName[i]->SetVisible(true);
		}

		if (m_btnSex[i]) {
			strText.Format("last sex %d", i + 1);
			g_cfg->GetConfig(strText, buf, sizeof(buf), "-");
			m_btnSex[i]->SetText(buf);
			m_btnSex[i]->SetVisible(true);
		}

		if (m_btnAge[i]) {
			strText.Format("last age %d", i + 1);
			g_cfg->GetConfig(strText, buf, sizeof(buf), "-");
			m_btnAge[i]->SetText(buf);
			m_btnAge[i]->SetVisible(true);
		}
	}

	for (int i = 0; i < MYCHART_COUNT; i++) {
		strText.Format("image_%d", i + 1);
		m_ChartUi[i] = static_cast<CMyImageUI*>(m_PaintManager.FindControl(strText));
		if (m_ChartUi[i]) {
			m_ChartUi[i]->m_nChartIndex = i;
		}		
	}

	SetTimer(m_hWnd, TIME_LEFT_TIMER_ID, TIME_LEFT_TIMER_INTEVAL, 0);
	WindowImplBase::InitWindow();
}

CControlUI * CDuiFrameWnd::CreateControl(LPCTSTR pstrClass) {
	CDialogBuilder builder;
	DuiLib::CDuiString strText;

	if (0 == strcmp(pstrClass, "MyChart_1") || 0 == strcmp(pstrClass, "MyChart_2") || 0 == strcmp(pstrClass, "MyChart_3") ||
		0 == strcmp(pstrClass, "MyChart_4") || 0 == strcmp(pstrClass, "MyChart_5") || 0 == strcmp(pstrClass, "MyChart_6")) {
		strText.Format("%s.xml", pstrClass);
		CControlUI * pUI = builder.Create((const char *)strText, (UINT)0, &m_callback, &m_PaintManager);
		return pUI;      
	}
	
	return WindowImplBase::CreateControl(pstrClass);                                                  
}

void CDuiFrameWnd::Notify(TNotifyUI& msg) {
	DuiLib::CDuiString name = msg.pSender->GetName();
	DuiLib::CDuiString strText;
	int nIndex = 0;

	if (msg.sType == "click") {
		if (name == "menubtn") {
			RECT r = m_btnMenu->GetPos();
			POINT pt = { r.left, r.bottom };
			CDuiMenu *pMenu = new CDuiMenu(_T("menu.xml"), msg.pSender);

			pMenu->Init(*this, pt);
			pMenu->ShowWindow(TRUE);
		}
		else if (0 == strncmp(name, "btnBed_", 7)) {
			sscanf(name.Mid(7), "%d", &nIndex);
			nIndex--;
			msg.pSender->SetVisible(false);

			if (m_edtBedNo[nIndex]) {
				m_edtBedNo[nIndex]->SetText(msg.pSender->GetText());
				m_edtBedNo[nIndex]->SetVisible(true);
				m_edtBedNo[nIndex]->SetFocus();
			}
		}
		else if ( 0 == strncmp(name, "btnName_", 8) ) {
			sscanf( name.Mid(8), "%d", &nIndex );
			nIndex--;
			msg.pSender->SetVisible(false);

			if ( m_edtName[nIndex] ) {
				m_edtName[nIndex]->SetText(msg.pSender->GetText());
				m_edtName[nIndex]->SetVisible(true);
				m_edtName[nIndex]->SetFocus();
			}
		}
		else if (0 == strncmp(name, "btnSex_", 7)) {
			sscanf(name.Mid(7), "%d", &nIndex);
			nIndex--;
			msg.pSender->SetVisible(false);

			if (m_edtSex[nIndex]) {
				m_edtSex[nIndex]->SetText(msg.pSender->GetText());
				m_edtSex[nIndex]->SetVisible(true);
				m_edtSex[nIndex]->SetFocus();
			}
		}
		else if (0 == strncmp(name, "btnAge_", 7)) {
			sscanf(name.Mid(7), "%d", &nIndex);
			nIndex--;
			msg.pSender->SetVisible(false);

			if (m_edtAge[nIndex]) {
				m_edtAge[nIndex]->SetText(msg.pSender->GetText());
				m_edtAge[nIndex]->SetVisible(true);
				m_edtAge[nIndex]->SetFocus();
			}
		}
	}	
	else if (msg.sType == "menu_setting") {
		OnSetting();
	}
	else if (msg.sType == "menu_about") {
		OnAbout();
	}
	else if (msg.sType == "killfocus") {
		if ( 0 == strncmp(name, "edtBed_", 7) ) {
			sscanf(name.Mid(7), "%d", &nIndex);
			nIndex--;

			strText.Format("last bed no %d", nIndex + 1);
			g_cfg->SetConfig( strText, msg.pSender->GetText());
			g_cfg->Save();

			msg.pSender->SetVisible(false);

			if (m_btnBedNo[nIndex]) {
				m_btnBedNo[nIndex]->SetText(msg.pSender->GetText());
				m_btnBedNo[nIndex]->SetVisible(true);
			}			
		}
		else if (0 == strncmp(name, "edtName_", 8)) {
			sscanf(name.Mid(8), "%d", &nIndex);
			nIndex--;

			strText.Format("last name %d", nIndex + 1);
			g_cfg->SetConfig(strText, msg.pSender->GetText());
			g_cfg->Save();

			msg.pSender->SetVisible(false);

			if (m_btnName[nIndex]) {
				m_btnName[nIndex]->SetText(msg.pSender->GetText());
				m_btnName[nIndex]->SetVisible(true);
			}
		}
		else if (0 == strncmp(name, "edtSex_", 7)) {
			sscanf(name.Mid(7), "%d", &nIndex);
			nIndex--;

			strText.Format("last sex %d", nIndex + 1);
			g_cfg->SetConfig(strText, msg.pSender->GetText());
			g_cfg->Save();

			msg.pSender->SetVisible(false);

			if (m_btnSex[nIndex]) {
				m_btnSex[nIndex]->SetText(msg.pSender->GetText());
				m_btnSex[nIndex]->SetVisible(true);
			}
		}
		else if (0 == strncmp(name, "edtAge_", 7)) {
			sscanf(name.Mid(7), "%d", &nIndex);
			nIndex--;

			strText.Format("last age %d", nIndex + 1);
			g_cfg->SetConfig(strText, msg.pSender->GetText());
			g_cfg->Save();

			msg.pSender->SetVisible(false);

			if (m_btnAge[nIndex]) {
				m_btnAge[nIndex]->SetText(msg.pSender->GetText());
				m_btnAge[nIndex]->SetVisible(true);
			}
		}


	}
	WindowImplBase::Notify(msg);
}

LRESULT CDuiFrameWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if (uMsg == WM_LBUTTONDBLCLK) {
		OnDbClick();
	}	
	else if (uMsg == UM_SHOW_READER_STATUS) {
		OnReaderStatus(wParam, lParam);		
	}
	else if (uMsg == UM_SHOW_READ_TAG_TEMP_RET) {
		OnReaderTemp(wParam, lParam);
	}
	else if (uMsg == WM_TIMER) {
		OnTimer(wParam, lParam);
	}
	else if (uMsg == UM_UPDATE_SCROLL) {
		OnUpdateScroll(wParam, lParam);		
	}
	return DuiLib::WindowImplBase::HandleMessage(uMsg, wParam, lParam);
}
  







int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	HANDLE handle = ::CreateMutex(NULL, FALSE, "surgery_surveil_1");//handle为声明的HANDLE类型的全局变量 
	if ( GetLastError() == ERROR_ALREADY_EXISTS ) {
		HWND hWnd = ::FindWindow("DUIMainFrame_surgery", 0);
		::ShowWindow(hWnd, SW_SHOWMAXIMIZED);
		::InvalidateRect(hWnd, 0, TRUE);
		return 0;
	}

	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR pGdiToken;
	GdiplusStartup(&pGdiToken, &gdiplusStartupInput, NULL);//初始化GDI+

	LmnToolkits::ThreadManager::GetInstance();
	CBusiness::GetInstance()->Init();
	g_log->Output(ILog::LOG_SEVERITY_INFO, "begin.\n");

	DWORD  dwPort = 0;
	g_cfg->GetConfig("telnet port", dwPort, 1135);
	JTelSvrStart((unsigned short)dwPort);

	CPaintManagerUI::SetInstance(hInstance);
	HICON hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));

	CDuiFrameWnd * duiFrame = new CDuiFrameWnd;
	duiFrame->Create(NULL, _T("DUIWnd"), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
	duiFrame->CenterWindow();  

	// 设置icon
	::SendMessage(duiFrame->GetHWND(), WM_SETICON, ICON_BIG, (LPARAM)hIcon);
	::SendMessage(duiFrame->GetHWND(), WM_SETICON, ICON_SMALL, (LPARAM)hIcon);

	duiFrame->ShowModal();
	delete duiFrame;

	g_log->Output(ILog::LOG_SEVERITY_INFO, "close.\n");         

	CBusiness::GetInstance()->DeInit();
	delete CBusiness::GetInstance();
	LmnToolkits::ThreadManager::ReleaseInstance(); 

	GdiplusShutdown(pGdiToken);//关闭GDI+
	JTelSvrStop();

	return 0;
}


