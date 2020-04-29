// win32_1.cpp : 定义应用程序的入口点。
//

#ifdef _DEBUG
#include "vld.h"
#endif

#include "Windows.h"
#include "main.h"
#include "business.h"
#include "resource.h"
#include "LmnSerialPort.h"
#include "LmnTelSvr.h"
#include "LmnTemplates.h"

static lua_State* init_lua() {
	lua_State* L = luaL_newstate();  //创建Lua栈
	//lua_checkstack(L, 60);         //修改Lua栈大小为60，防止在C和Lua之间传递大数据时，崩溃
	luaL_openlibs(L);                //运行Lua虚拟机
	return L;
}

CDuiFrameWnd::CDuiFrameWnd() {
	m_cmbComPorts = 0;
	m_cmbLuaFiles = 0;
	m_cmbBaud = 0;
	m_chart = 0;
	m_bStartPaint = FALSE;
	m_bBusy = FALSE;
	m_bConnected = FALSE;

	m_L = init_lua();
	m_bCorrectLua = FALSE;

	memset(m_opChannels, 0, sizeof(m_opChannels));
	memset(m_Indicators, 0, sizeof(m_Indicators));
	m_opParams = 0;

	memset(m_lblChannelNames, 0, sizeof(m_lblChannelNames));
}

CDuiFrameWnd::~CDuiFrameWnd() {

}

void  CDuiFrameWnd::InitWindow() {
	g_data.m_hWnd = GetHWND();
	g_data.m_cursor_we = LoadCursor(NULL, IDC_SIZEWE);
	g_data.m_cursor_ns = LoadCursor(NULL, IDC_SIZENS);

	PostMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0);

	m_cmbComPorts = (CComboUI *)m_PaintManager.FindControl("cmComPort");
	m_cmbLuaFiles = static_cast<CComboUI *>(m_PaintManager.FindControl("cmLuaFile"));
	m_chart       = static_cast<CMyChartUI *>(m_PaintManager.FindControl("cstChart"));
	m_btnPaint    = static_cast<CButtonUI *>(m_PaintManager.FindControl("btnPaint"));
	m_cmbBaud     = static_cast<CComboUI *>(m_PaintManager.FindControl("cmBaud"));
	m_opParams    = static_cast<COptionUI *>(m_PaintManager.FindControl("opParams"));

	CDuiString  strText;
	for (int i = 0; i < MAX_CHANNEL_COUNT; i++) {
		strText.Format("opChannel_%d", i + 1);
		m_opChannels[i] = static_cast<COptionUI *>(m_PaintManager.FindControl(strText));

		strText.Format("indicator_%d", i + 1);
		m_Indicators[i] = m_PaintManager.FindControl(strText);
		m_Indicators[i]->SetBkColor(g_color[i+1]);

		strText.Format("lblChannelName_%d", i + 1);
		m_lblChannelNames[i] = static_cast<CLabelUI *>(m_PaintManager.FindControl(strText));
	} 

	int arrBauds[5] = { 9600,19200, 38400,57600,115200 };
	
	for (int i = 0; i < 5; i++) {
		CListLabelElementUI * pElement = new CListLabelElementUI;
		m_cmbBaud->Add(pElement);
		strText.Format("%d", arrBauds[i]);
		pElement->SetText(strText);
		pElement->SetTag(arrBauds[i]);
	}
	m_cmbBaud->SelectItem(0);

	InitCmbLuaFiles();
	OnDeviceChanged();

#ifdef _DEBUG
	m_chart->AddData(2, 100);
	m_chart->AddData(2, 200);
	m_chart->AddData(2, 250);
	m_chart->AddData(2, 300);

	m_chart->AddData(3, 1100);
	m_chart->AddData(3, 2100);
	m_chart->AddData(3, 2250);
	m_chart->AddData(3, 1000);	

	for (int i = 0; i < 100; i++) {
		m_chart->AddData(1, 200);
		m_chart->AddData(1, 300); 
		m_chart->AddData(1, 450);  
		m_chart->AddData(1, 2500);     

		m_chart->AddData(4, 100);
		m_chart->AddData(4, 500);
		m_chart->AddData(4, 300);
		m_chart->AddData(4, 600); 
	}
#endif

	WindowImplBase::InitWindow();
}

CControlUI * CDuiFrameWnd::CreateControl(LPCTSTR pstrClass) {
	if (0 == strcmp(pstrClass, "MyChart")) {
		return new CMyChartUI;
	}
	else if (0 == strcmp(pstrClass, "MySlider")) {
		return new CMySliderUI;           
	}
	else if (0 == strcmp(pstrClass, "MyVSlider")) {
		return new CMyVSliderUI;  
	}
	return WindowImplBase::CreateControl(pstrClass);
}

void CDuiFrameWnd::Notify(TNotifyUI& msg) {
	CDuiString  strName = msg.pSender->GetName();

	if (msg.sType == "hslider_changed") {
		//JTelSvrPrint("hslider_changed: %d, %d", (int)msg.wParam, (int)msg.lParam);
		m_chart->SetPosAndLen(msg.wParam / 1000.0f, msg.lParam / 1000.0f);
	}
	else if (msg.sType == "vslider_changed") {
		//JTelSvrPrint("vslider_changed: %d, %d", (int)msg.wParam, (int)msg.lParam);
		m_chart->SetPosAndLen(msg.wParam / 1000.0f, msg.lParam / 1000.0f, FALSE);
	}
	else if ( msg.sType == "click" ) {
		if (strName == "btnPaint") {
			OnStartPaint();
		}
	}
	if (msg.sType == "itemselect") {
		if (strName == "cmLuaFile") {
			OnLuaFileSelected();
		}
	}
	else if (msg.sType == "selectchanged") {
		if (strName == "opChannel_1") {
			bool b = m_opChannels[0]->IsSelected();
			SetBit(g_data.m_dwChannels, 1, b);
			m_chart->Invalidate();
		}
		else if (strName == "opChannel_2") {
			bool b = m_opChannels[1]->IsSelected();
			SetBit(g_data.m_dwChannels, 2, b);
			m_chart->Invalidate();
		}
		else if (strName == "opChannel_3") {
			bool b = m_opChannels[2]->IsSelected();
			SetBit(g_data.m_dwChannels, 3, b);
			m_chart->Invalidate();
		}
		else if (strName == "opChannel_4") {
			bool b = m_opChannels[3]->IsSelected();
			SetBit(g_data.m_dwChannels, 4, b);
			m_chart->Invalidate();
		}
		else if (strName == "opParams") {
			g_data.m_bShowParams = m_opParams->IsSelected()?TRUE:FALSE;
			m_chart->Invalidate();
		}
	}
	WindowImplBase::Notify(msg);
}

LRESULT CDuiFrameWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if ( uMsg == UM_OPEN_COM_RET ) {
		OnOpenComRet(wParam);
	}
	else if (uMsg == UM_WRONG_LUA) {
		MessageBox(GetHWND(), "lua文件出错或缺少动态链接库文件", "错误", 0);
	}
	else if (uMsg == UM_COM_DATA) {
		const BYTE * pData = (const BYTE *)wParam;
		DWORD  dwLen = lParam;
		m_buf.Append(pData, dwLen);
		delete[] pData;

		OnReceive();		
	}
	else if ( uMsg == UM_CLOSE_COM_RET ) {
		OnCloseComRet();
	}
	return WindowImplBase::HandleMessage(uMsg,wParam,lParam);
}

void  CDuiFrameWnd::OnDeviceChanged() {
	m_cmbComPorts->RemoveAll();

	std::map<int, std::string> m;
	GetAllSerialPorts(m);

	BOOL  bFindReader = FALSE;
	int   nFindeIndex = -1;

	std::map<int, std::string>::iterator it;
	int i = 0;

	for (it = m.begin(); it != m.end(); it++, i++) {
		std::string & s = it->second;

		CListLabelElementUI * pElement = new CListLabelElementUI;
		pElement->SetText(s.c_str());
		m_cmbComPorts->Add(pElement);

		int nComPort = it->first;
		pElement->SetTag(nComPort);

		if (!bFindReader) {
			char tmp[256];
			Str2Lower(s.c_str(), tmp, sizeof(tmp));
			if (0 != strstr(tmp, "usb-serial ch340")) {
				bFindReader = TRUE;
				nFindeIndex = i;
			}
		}
	}

	if (m_cmbComPorts->GetCount() > 0) {
		if (nFindeIndex >= 0) {
			m_cmbComPorts->SelectItem(nFindeIndex);
		}
		else {
			m_cmbComPorts->SelectItem(0);
		}
	}
}

void  CDuiFrameWnd::InitCmbLuaFiles() {
	WIN32_FIND_DATA FindData;
	HANDLE hFind;

	char szFilePathName[256];
	SNPRINTF(szFilePathName, sizeof(szFilePathName), ".\\Lua\\*.lua");

	hFind = FindFirstFile(szFilePathName, &FindData);
	if (hFind == INVALID_HANDLE_VALUE) {
		return;
	}

	do {
		CListLabelElementUI * pElement = new CListLabelElementUI;
		pElement->SetText(FindData.cFileName);
		m_cmbLuaFiles->Add(pElement);

		if (!::FindNextFile(hFind, &FindData)) {
			break;
		}
	} while (TRUE);

	FindClose(hFind);

	if (m_cmbLuaFiles->GetCount() > 0)
		m_cmbLuaFiles->SelectItem(0);
}

void  CDuiFrameWnd::OnStartPaint() {
	if ( !m_bStartPaint ) {
		int nSel = m_cmbComPorts->GetCurSel();
		if ( nSel < 0 ) {
			MessageBox(GetHWND(), "没有选择串口", "错误", 0);
			return;
		}

		int nCom = m_cmbComPorts->GetItemAt(nSel)->GetTag();
		nSel = m_cmbBaud->GetCurSel();
		int nBaud = m_cmbBaud->GetItemAt(nSel)->GetTag();

		nSel = m_cmbLuaFiles->GetCurSel();
		if (nSel < 0) {
			MessageBox(GetHWND(), "没有选择lua文件", "错误", 0);
			return;
		}

		if (!m_bCorrectLua) {
			MessageBox(GetHWND(), "lua文件出错或者缺少动态链接库", "错误", 0);
			return;
		}

		CBusiness::GetInstance()->OpenComAsyn(nCom, nBaud);
		SetBusy();
	}
	else {
		CBusiness::GetInstance()->CloseComAsyn();
		SetBusy();
	}
}

void  CDuiFrameWnd::SetBusy(BOOL bBusy /*= TRUE*/) {
	m_bBusy = bBusy;
	if ( m_bBusy ) {
		m_btnPaint->SetEnabled(false);
		m_cmbComPorts->SetEnabled(false);
		m_cmbBaud->SetEnabled(false);
		m_cmbLuaFiles->SetEnabled(false);
	}
	else {
		m_btnPaint->SetEnabled(true);
		if (m_bConnected) {
			m_cmbComPorts->SetEnabled(false);
			m_cmbBaud->SetEnabled(false);
			m_cmbLuaFiles->SetEnabled(false);
		}
		else {
			m_cmbComPorts->SetEnabled(true);
			m_cmbBaud->SetEnabled(true);
			m_cmbLuaFiles->SetEnabled(true);
		}
	}
}

void  CDuiFrameWnd::OnOpenComRet(BOOL bRet) {
	if ( bRet ) {
		m_bStartPaint = TRUE;
		m_bConnected = TRUE;
		m_btnPaint->SetText("停止绘图");
		SetBusy(FALSE);

		m_buf.Clear();
		m_chart->Clear();
		m_chart->Invalidate();
	}
	else {
		MessageBox(GetHWND(), "打开串口失败", "错误", 0);
		m_btnPaint->SetText("开始绘图");
		m_bStartPaint = FALSE;
		m_bConnected = FALSE;
		SetBusy(FALSE);
	}
}

void  CDuiFrameWnd::OnCloseComRet() {
	m_btnPaint->SetText("开始绘图");
	m_bStartPaint = FALSE;
	m_bConnected = FALSE;
	SetBusy(FALSE);
}

void  CDuiFrameWnd::OnLuaFileSelected() {
	m_bCorrectLua = FALSE;

	lua_settop(m_L, 0);
	const char * szDefault = "function receive(t) \n end \n";
	luaL_loadstring(m_L, szDefault);
	int ret = lua_pcall(m_L, 0, LUA_MULTRET, 0);
	if (0 != ret) {
		return;
	}

	int nSel = m_cmbLuaFiles->GetCurSel();
	CDuiString strFile = m_cmbLuaFiles->GetItemAt(nSel)->GetText();
	char szFilePathName[256];
	SNPRINTF(szFilePathName, sizeof(szFilePathName), ".\\Lua\\%s", (const char *)strFile);

	ret = luaL_loadfile(m_L, szFilePathName);
	if (0 != ret) {
		this->PostMessage(UM_WRONG_LUA);
		return;
	}

	ret = lua_pcall(m_L, 0, LUA_MULTRET, 0);
	if (0 != ret) {
		this->PostMessage(UM_WRONG_LUA);
		return;
	}

	CDuiString strText;
	for ( int i = 0; i < MAX_CHANNEL_COUNT; i++ ) {
		strText.Format("通道%d", i + 1);
		m_lblChannelNames[i]->SetText(strText);
	}

	lua_getglobal(m_L, "channels");
	lua_pushnil(m_L);
	while (lua_next(m_L, 1) != 0) {
		lua_pushnil(m_L);

		int  nChannel = 0;
		char szChannelName[256] = {0};

		while (lua_next(m_L, 3) != 0) {
			int nSubType = lua_type(m_L, -1);
			if (nSubType == LUA_TNUMBER) {
				nChannel = (int)lua_tonumber(m_L, -1);
			}
			else if (nSubType == LUA_TSTRING ) {
				const char * szName = lua_tolstring(m_L, -1, 0);
				STRNCPY(szChannelName, szName, sizeof(szChannelName));
			}
			lua_pop(m_L, 1);
		}

		if ( nChannel > 0 && nChannel <= MAX_CHANNEL_COUNT && szChannelName[0] != '\0' ) {
			m_lblChannelNames[nChannel-1]->SetText(szChannelName);
		}

		lua_pop(m_L, 1);
	}
	lua_settop(m_L, 0);

	m_bCorrectLua = TRUE;
}

void  CDuiFrameWnd::OnReceive() {
	BOOL  bNeedUpdate = FALSE;

	do 
	{
		lua_settop(m_L, 0);
		lua_getglobal(m_L, "receive");
		lua_pushlstring(m_L, (const char *)m_buf.GetData(), m_buf.GetDataLength());

		int ret = lua_pcall(m_L, 1, 2, 0);
		if (0 != ret) {
			size_t err_len = 0;
			const char * szErrDescription = lua_tolstring(m_L, -1, &err_len);
			MessageBox(GetHWND(), szErrDescription, "错误", 0);
			break;
		}

		int nRet = lua_gettop(m_L);
		if (nRet != 2) {
			MessageBox(GetHWND(), "receive 函数返回的参数个数不是2", "错误", 0);
			break;
		}

		int nType = lua_type(m_L, -2);
		if (nType != LUA_TNUMBER) {
			MessageBox(GetHWND(), "receive 函数返回的参数格式错误", "错误", 0);
			break;
		}

		int nComsume = (int)lua_tonumber(m_L, -2);
		if (nComsume <= 0) {
			break;
		}
		m_buf.SetReadPos(m_buf.GetReadPos() + nComsume);
		m_buf.Reform();

		nType = lua_type(m_L, -1);
		if (nType != LUA_TTABLE) {
			MessageBox(GetHWND(), "receive 函数返回的参数格式错误", "错误", 0);
			break;
		}

		lua_pushnil(m_L);
		while (lua_next(m_L, -2) != 0) {
			nType = lua_type(m_L, -1);
			if (nType == LUA_TTABLE) {
				lua_pushnil(m_L);
				int nIndex = 0;
				int nChannel = 0;
				int nValue = 0;
				while (lua_next(m_L, -2) != 0) {
					int nSubType = lua_type(m_L, -1);
					if (nSubType == LUA_TNUMBER) {
						int n = (int)lua_tonumber(m_L, -1);
						if (nIndex == 0) {
							nChannel = n;
						}
						else if (nIndex == 1) {
							nValue = n;
						}
						nIndex++;
					}
					lua_pop(m_L, 1);
				}
				m_chart->AddData(nChannel, nValue);
				bNeedUpdate = TRUE;
			}
			lua_pop(m_L, 1);
		}

	} while ( TRUE );

	
	if (bNeedUpdate) {
		m_chart->Invalidate();
	}
}
        



int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	DWORD  dwPort = 2020;
	JTelSvrStart((unsigned short)dwPort, 3);

	LmnToolkits::ThreadManager::GetInstance();
	CBusiness::GetInstance()->Init();
	g_data.m_log->Output(ILog::LOG_SEVERITY_INFO, "main begin.\n");

	CoInitialize(NULL);

	ULONG_PTR gdiplusToken;
	GdiplusStartupInput gdiplusStartupInput;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

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

	g_data.m_log->Output(ILog::LOG_SEVERITY_INFO, "main close.\n");

	CBusiness::GetInstance()->DeInit();
	delete CBusiness::GetInstance();
	LmnToolkits::ThreadManager::ReleaseInstance();

	return 0;
}


