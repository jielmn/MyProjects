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

static lua_State* init_lua() {
	lua_State* L = luaL_newstate();  //创建Lua栈
									 //lua_checkstack(L, 60);//修改Lua栈大小为60，防止在C和Lua之间传递大数据时，崩溃
	luaL_openlibs(L);                //运行Lua虚拟机
	return L;
}

CDuiFrameWnd::CDuiFrameWnd() {
	m_cmbLuaFiles = 0;
	m_edDescription = 0;
	m_params = 0;
	m_btnSend = 0;
	m_L = init_lua();
}

CDuiFrameWnd::~CDuiFrameWnd() {

}

void  CDuiFrameWnd::InitWindow() {
	m_cmbComPorts = (CComboUI *)m_PaintManager.FindControl("cmComPort");
	m_cmbLuaFiles = static_cast<CComboUI *>(m_PaintManager.FindControl("cmLuaFile"));
	m_edDescription = static_cast<CEditUI *>(m_PaintManager.FindControl("etDescription"));
	m_params = static_cast<CMyTreeCfgUI *>(m_PaintManager.FindControl("params"));
	m_btnSend = static_cast<CButtonUI *>(m_PaintManager.FindControl("btnSend"));
	m_btnSend->SetEnabled(false);

	InitCmbLuaFiles();

	OnDeviceChanged(0,0);   
	WindowImplBase::InitWindow();       
}

CControlUI * CDuiFrameWnd::CreateControl(LPCTSTR pstrClass) {
	if ( 0 == strcmp( pstrClass, "MyTree" ) ) {
		return new CMyTreeCfgUI;       
	}
	return WindowImplBase::CreateControl(pstrClass);
}

void CDuiFrameWnd::Notify(TNotifyUI& msg) {
	CDuiString name = msg.pSender->GetName();
	if ( msg.sType == "itemselect" ) {
		if (name == "cmLuaFile") {
			OnLuaFileSelected();
		}
	}
	WindowImplBase::Notify(msg);
}

LRESULT CDuiFrameWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if (uMsg == WM_DEVICECHANGE) {
		OnDeviceChanged(wParam, lParam);
	}
	return WindowImplBase::HandleMessage(uMsg,wParam,lParam);
}

void  CDuiFrameWnd::OnDeviceChanged(WPARAM wParm, LPARAM  lParam) {

	m_cmbComPorts->RemoveAll();

	std::vector<std::string> vComPorts;
	EnumPortsWdm(vComPorts);

	BOOL  bFindReader = FALSE;
	int   nFindeIndex = -1;

	std::vector<std::string>::iterator it;
	int i = 0;
	for (it = vComPorts.begin(); it != vComPorts.end(); it++, i++) {
		std::string & s = *it;

		CListLabelElementUI * pElement = new CListLabelElementUI;
		pElement->SetText(s.c_str());
		m_cmbComPorts->Add(pElement);

		int nComPort = 0;
		const char * pFind = strstr(s.c_str(), "COM");
		while (pFind) {
			if (1 == sscanf(pFind + 3, "%d", &nComPort)) {
				pElement->SetTag(nComPort);
				break;
			}
			pFind = strstr(pFind + 3, "COM");
		}

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

		if ( !::FindNextFile(hFind, &FindData) ) {
			break;
		}
	} while (TRUE);

	FindClose(hFind);

	if (m_cmbLuaFiles->GetCount() > 0)
		m_cmbLuaFiles->SelectItem(0);
}

void  CDuiFrameWnd::OnLuaFileSelected() {
	if (!m_btnSend->IsEnabled())
		m_btnSend->SetEnabled(true);

	m_edDescription->SetText("");

	int nSel = m_cmbLuaFiles->GetCurSel();
	CDuiString strFile = m_cmbLuaFiles->GetItemAt(nSel)->GetText();
	char szFilePathName[256];
	SNPRINTF(szFilePathName, sizeof(szFilePathName), ".\\Lua\\%s", (const char *)strFile );

	int ret = luaL_loadfile(m_L, szFilePathName);
	if (0 != ret) {
		return;
	}

	ret = lua_pcall(m_L, 0, LUA_MULTRET, 0);
	if (0 != ret)
		return;

	lua_getglobal(m_L, "utf8");
	BOOL bUtf8 = lua_toboolean(m_L, -1); 
	lua_settop(m_L, 0);

	lua_getglobal(m_L, "description");
	size_t len = 0;
	const char * szDescription = lua_tolstring(m_L, -1, &len);
	if (szDescription) {
		if ( bUtf8 ) {
			char szTemp[1024];
			Utf8ToAnsi(szTemp, sizeof(szTemp), szDescription);
			m_edDescription->SetText(szTemp); 
		}
		else {
			m_edDescription->SetText(szDescription);
		}
	}
}




int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	LmnToolkits::ThreadManager::GetInstance();
	CBusiness::GetInstance()->Init();
	g_data.m_log->Output(ILog::LOG_SEVERITY_INFO, "main begin.\n");

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


