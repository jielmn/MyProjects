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
#include "GridUI.h"

#define   MAX_RICHEDIT_SIZE       512  
#define   FORMAT_TYPE_NIL         0
#define   FORMAT_TYPE_TEXT        1
#define   FORMAT_TYPE_GRIDS       2

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
	m_bOpend = FALSE;
	m_nFormatType = FORMAT_TYPE_NIL;
	m_nMaxItemsCnt = 0;
	m_nItemWidth = 0;
	m_nItemHeight = 0;
	m_bUtf8 = FALSE;
}

CDuiFrameWnd::~CDuiFrameWnd() {

}  
            
void  CDuiFrameWnd::InitWindow() {
	PostMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0);
	g_data.m_hWnd = m_hWnd;

	m_cmbComPorts = (CComboUI *)m_PaintManager.FindControl("cmComPort");
	m_cmbLuaFiles = static_cast<CComboUI *>(m_PaintManager.FindControl("cmLuaFile"));
	m_edDescription = static_cast<CEditUI *>(m_PaintManager.FindControl("etDescription"));
	m_params = static_cast<CMyTreeCfgUI *>(m_PaintManager.FindControl("params"));
	m_btnSend = static_cast<CButtonUI *>(m_PaintManager.FindControl("btnSend"));
	m_btnOpen = static_cast<CButtonUI *>(m_PaintManager.FindControl("btnOpen"));
	m_rich = static_cast<CRichEditUI *>(m_PaintManager.FindControl("rchData"));
	m_tabs = static_cast<CTabLayoutUI *>(m_PaintManager.FindControl("data"));
	m_layFormat = static_cast<CTileLayoutUI *>(m_PaintManager.FindControl("layGrids"));

	m_params->SetSelectedItemBkColor(0xFFFFFFFF);
	m_params->SetHotItemBkColor(0xFFFFFFFF); 

	m_layFormat->OnSize += MakeDelegate(this, &CDuiFrameWnd::OnGridsSize);

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
	else if (msg.sType == "click") {
		if (name == "btnSend") {
			OnSend();
		}
		else if (name == "btnOpen") {
			OnOpen();
		}
		else if (name == "btnClear") {
			OnClear();
		}
		else if (name == "btnClear1") {
			OnClear1(); 
		}
	}
	else if (msg.sType == "selectchanged") {
		if (name == "opn_raw_data" ) {
			m_tabs->SelectItem(0);
		}
		else if (name == "opn_format_data") {
			m_tabs->SelectItem(1);
		}
	}
	WindowImplBase::Notify(msg);
}

LRESULT CDuiFrameWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if (uMsg == WM_DEVICECHANGE) {
		OnDeviceChanged(wParam, lParam);
	}
	else if (uMsg == UM_WRONG_LUA) {
		MessageBox(GetHWND(), "加载文件出现错误", "错误", 0);
	}
	else if (uMsg == UM_OPEN_COM_RET) {
		BOOL bRet = (BOOL)wParam;
		if (bRet) {
			m_btnOpen->SetText("关闭");
			m_bOpend = TRUE;
			m_cmbLuaFiles->SetEnabled(false);
			m_buf.Clear();
			m_buf_rch.Clear();
		}
	}
	else if (uMsg == UM_CLOSE_COM_RET) {
		BOOL bRet = (BOOL)wParam;
		if (bRet) {
			m_btnOpen->SetText("打开");
			m_bOpend = FALSE;
			m_cmbLuaFiles->SetEnabled(true); 
		}
	}
	else if (uMsg == UM_WRITE_COM_RET) {                        
		BOOL bRet = (BOOL)wParam;
		if (bRet) {
			m_btnSend->SetEnabled(true);
		}
		else {
			m_btnSend->SetEnabled(true);
			MessageBox(GetHWND(), "写串口数据失败", "错误", 0);     
		}
	}
	else if (uMsg == UM_READ_COM_RET) {
		BYTE * pData = (BYTE *)wParam;
		DWORD  dwSize = lParam;

		m_buf.Append(pData, dwSize);
		m_buf_rch.Append(pData, dwSize);

		if (pData) {
			delete[] pData;
		}

		if ( m_buf_rch.GetDataLength() > MAX_RICHEDIT_SIZE ) {
			m_buf_rch.SetReadPos( m_buf_rch.GetDataLength() - MAX_RICHEDIT_SIZE );
			m_buf_rch.Reform();
		}

		char buf[8192];
		DebugStream(buf, sizeof(buf), m_buf_rch.GetData(), m_buf_rch.GetDataLength());
		m_rich->SetText(buf);  

		lua_settop(m_L, 0);
		lua_getglobal(m_L, "receive");
		lua_pushlstring(m_L, (const char *)m_buf.GetData(), m_buf.GetDataLength());

		int ret = lua_pcall(m_L, 1, 5, 0);
		if (0 != ret) {
			size_t err_len = 0;
			const char * szErrDescription = lua_tolstring(m_L, -1, &err_len);
			MessageBox(GetHWND(), szErrDescription, "错误", 0);
			lua_settop(m_L, 0);
		}
		else {
			int nRet = lua_gettop(m_L);
			if (nRet != 5) {
				MessageBox(GetHWND(), "receive 函数返回的参数个数不是5", "错误", 0);
				lua_settop(m_L, 0);
			}
			else {
				int nTextColor = (int)lua_tonumber(m_L, -1);
				int nBkColor   = (int)lua_tonumber(m_L, -2);
				const char * szText = lua_tostring(m_L, -4);
				int nComsume = (int)lua_tonumber(m_L, -5);

				if (nComsume > 0) {
					m_buf.SetReadPos(nComsume);
					m_buf.Reform();

					if (m_nFormatType == FORMAT_TYPE_GRIDS) {
						std::vector<CMyData *> * pVKey = new std::vector<CMyData *>;
						lua_pop(m_L, 2);
						lua_pushnil(m_L);
						while (lua_next(m_L, -2) != 0) {
							int nType = lua_type(m_L, -1);
							if (nType == LUA_TNUMBER) {
								CMyData * pKey = new CMyData;
								pKey->m_nDataype = MYDATA_TYPE_INT;
								pKey->m_nData = (int)lua_tonumber(m_L, -1);
								pVKey->push_back(pKey);
							}
							else if (nType == LUA_TSTRING) {
								CMyData * pKey = new CMyData;
								pKey->m_nDataype = MYDATA_TYPE_STRING;
								pKey->m_strData = lua_tostring(m_L, -1);
								pVKey->push_back(pKey);
							}							
							lua_pop(m_L, 1); 
						}

						BOOL  bFoundItem = FALSE;
						CGridUI * pFoundItemUI = 0;

						if ( pVKey->size() > 0 ) {
							int nItemsCnt = m_layFormat->GetCount();
							for (int i = 0; i < nItemsCnt; i++) {
								CGridUI * pItemUI =  (CGridUI *)m_layFormat->GetItemAt(i);
								std::vector<CMyData *> * pVec = (std::vector<CMyData *> *)pItemUI->GetTag();
								if ( pVec && pVec->size() == pVKey->size() ) {
									DWORD j = 0;
									for ( j = 0; j < pVKey->size(); j++ ) {
										if ( pVKey->at(j)->m_nDataype == pVec->at(j)->m_nDataype ) {
											if ( pVKey->at(j)->m_nDataype == MYDATA_TYPE_INT ) {
												if (pVKey->at(j)->m_nData == pVec->at(j)->m_nData) {
													continue;
												}
												else {
													break;
												}
											}
											else if (pVKey->at(j)->m_nDataype == MYDATA_TYPE_STRING) {
												if ( pVKey->at(j)->m_strData == pVec->at(j)->m_strData ) {
													continue;
												}
												else {
													break;
												}
											}
											else {
												break;
											}
										}
										else {
											break;
										}
									}
									if (j >= pVKey->size()) {
										bFoundItem = TRUE;
										pFoundItemUI = pItemUI;
										break;
									}
								}
							}
						}
						
						if ( 0 == pFoundItemUI) {
							pFoundItemUI = new CGridUI;
							m_layFormat->Add(pFoundItemUI);
							pFoundItemUI->SetTag((UINT_PTR)pVKey); 
							pFoundItemUI->SetIndex(m_layFormat->GetCount());     
						}
						else {
							ClearVector(*pVKey);
							delete pVKey;
						}
				
						if (m_bUtf8) {
							char szTemp[1024];
							Utf8ToAnsi(szTemp, sizeof(szTemp), szText);
							pFoundItemUI->SetText(szTemp);
						}
						else {
							pFoundItemUI->SetText(szText);
						}
					}
					else if (m_nFormatType == FORMAT_TYPE_TEXT) {
						CTextUI * pItemUI = new CTextUI;
						int nItemsCnt = m_layFormat->GetCount();
						if (nItemsCnt >= m_nMaxItemsCnt) {
							m_layFormat->RemoveAt(0);
						}
						m_layFormat->Add(pItemUI);
						pItemUI->SetShowHtml(true);  
						if (m_bUtf8) {
							char szTemp[1024];
							Utf8ToAnsi(szTemp, sizeof(szTemp), szText);
							pItemUI->SetText(szTemp);
						}
						else {
							pItemUI->SetText(szText);  
						} 						
						pItemUI->SetFixedHeight(m_nItemHeight);
						pItemUI->SetFont(5);
						if (nBkColor != 0) {
							pItemUI->SetBkColor(nBkColor);
						}
						if (nTextColor == 0) { 
							pItemUI->SetTextColor(0xFF386382); 
						}
						else {
							pItemUI->SetTextColor(nTextColor);
						}
					}
				}
			}
		}

		
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
	m_btnSend->SetEnabled(false);

	lua_settop(m_L, 0);
	m_edDescription->SetText("");
	m_params->RemoveAll();

	const char * szDefault = "utf8=false;description=\"\";params={};nosend=false;function send(t) \n end\n formattype=nil; itemwidth=100; itemheight=30; maxitemscnt=10; function receive(t) \n end \n";
	luaL_loadstring(m_L, szDefault);
	int ret = lua_pcall(m_L, 0, LUA_MULTRET, 0);
	if (0 != ret) {
		return;
	}

	int nSel = m_cmbLuaFiles->GetCurSel();
	CDuiString strFile = m_cmbLuaFiles->GetItemAt(nSel)->GetText();
	char szFilePathName[256];
	SNPRINTF(szFilePathName, sizeof(szFilePathName), ".\\Lua\\%s", (const char *)strFile );

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

	lua_getglobal(m_L, "utf8");
	BOOL bUtf8 = lua_toboolean(m_L, -1); 
	m_bUtf8 = bUtf8;
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
	lua_settop(m_L, 0);

	lua_getglobal(m_L, "params");
	lua_pushnil(m_L);
	while ( lua_next(m_L, 1) != 0 ) {		

		int i = 0;
		char szTemp[2][1024];

		lua_pushnil(m_L);
		while (lua_next(m_L, 3) != 0 && i < 2) {
			size_t n = 0;
			const char * s = lua_tolstring(m_L, -1, &n);
			
			if (s) {
				if (bUtf8) {
					Utf8ToAnsi(szTemp[i], sizeof(szTemp[i]), s);
				}
				else {
					STRNCPY(szTemp[i], s, sizeof(szTemp[i]));
				}
			}
			lua_pop(m_L, 1);
			i++;			
		}

		CEditUI * pEdit = new CEditUI;
		pEdit->SetText(szTemp[1]);
		m_params->AddNode(szTemp[0], 0, 0, pEdit, 2, 0xFF386382, 2, 0xFF386382);

		lua_settop(m_L, 3);
		lua_pop(m_L, 1);
	}
	lua_settop(m_L, 0);
	int nParamCnt = m_params->GetCount();
	if (0 == nParamCnt) {
		m_params->AddNode("(无)", 0, 0, 0, 2, 0xFF386382, 2, 0xFF386382);
	}

	lua_getglobal(m_L, "nosend");
	BOOL bNoSend = lua_toboolean(m_L, 1);
	if (bNoSend)
		m_btnSend->SetEnabled(false);  
	else
		m_btnSend->SetEnabled(true);

	m_nFormatType = FORMAT_TYPE_NIL;

	lua_getglobal(m_L, "formattype");
	int nType = lua_type(m_L, -1);
	if (nType == LUA_TSTRING) {
		len = 0;
		const char * szType = lua_tolstring(m_L, -1, &len);
		if (szType) {
			if (0 == StrICmp(szType, "text")) {
				m_nFormatType = FORMAT_TYPE_TEXT;
			}
			else if (0 == StrICmp(szType, "grids")) {
				m_nFormatType = FORMAT_TYPE_GRIDS; 
			}
		}
	}
	lua_settop(m_L, 0);

	lua_getglobal(m_L, "maxitemscnt");
	m_nMaxItemsCnt = (int)lua_tonumber(m_L, -1);
	lua_settop(m_L, 0);

	m_nItemWidth  = 100;
	m_nItemHeight = 30;
	if (m_nFormatType != FORMAT_TYPE_NIL) {
		lua_getglobal(m_L, "itemwidth");
		m_nItemWidth = (int)lua_tonumber(m_L, -1);
		lua_settop(m_L, 0);

		lua_getglobal(m_L, "itemheight");
		m_nItemHeight = (int)lua_tonumber(m_L, -1);
		lua_settop(m_L, 0);
	}	

	int nCnt = m_layFormat->GetCount();
	for ( int i = 0; i < nCnt; i++ ) {
		CControlUI * pItemUI = m_layFormat->GetItemAt(i);
		void * pArg = (void *)pItemUI->GetTag();
		if ( pArg == 0 )
			continue;
		std::vector<CMyData * > * pVec = (std::vector<CMyData * > *)pArg;
		ClearVector(*pVec);
		delete pVec;
	}
	m_layFormat->RemoveAll();

	OnGridsSize(0);	
}

void  CDuiFrameWnd::OnSend() {
	if (!m_bOpend) {
		MessageBox(GetHWND(), "还没有打开串口", "错误", 0);
		return;
	}

	lua_settop(m_L, 0);
	lua_getglobal(m_L, "send");
	lua_newtable(m_L);

	int cnt = m_params->GetCount();
	for (int i = 0; i < cnt; i++) {
		CMyTreeCfgUI::ConfigValue value;
		m_params->GetConfigValue(i, value);
		lua_pushstring(m_L, value.m_strEdit);
		lua_rawseti(m_L, -2, i+1);
	}

	int ret = lua_pcall(m_L, 1, 1, 0);
	if (0 != ret) {
		size_t err_len = 0;
		const char * szErrDescription = lua_tolstring(m_L, -1, &err_len);
		MessageBox(GetHWND(), szErrDescription, "错误", 0);
		lua_settop(m_L, 0);
		return;
	}

	size_t len = 0;
	const char * pData = lua_tolstring(m_L, 1, &len);
	if ( len <= 0 ) {
		MessageBox(GetHWND(), "待发送的数据长度为0", "错误", 0);
		lua_settop(m_L, 0);
		return;
	}	

	CBusiness::GetInstance()->WriteComDataAsyn((const BYTE *)pData, len);
	m_btnSend->SetEnabled(false);
}

void  CDuiFrameWnd::OnOpen() {

	if (!m_bOpend) {
		int nSel = m_cmbComPorts->GetCurSel();
		if (nSel < 0) {
			MessageBox(GetHWND(), "没有选中串口", "错误", 0);
			return;
		}

		CListLabelElementUI * pItem = (CListLabelElementUI *)m_cmbComPorts->GetItemAt(nSel);
		int nPort = pItem->GetTag();
		CBusiness::GetInstance()->OpenComPortAsyn(nPort);
	}
	else {
		CBusiness::GetInstance()->CloseComPortAsyn();
	}
}

void  CDuiFrameWnd::OnClear() {
	m_buf_rch.Clear();
	m_rich->SetText("");
}

void  CDuiFrameWnd::OnClear1() {
	int nCnt = m_layFormat->GetCount();
	for (int i = 0; i < nCnt; i++) {
		CControlUI * pItemUI = m_layFormat->GetItemAt(i);
		void * pArg = (void *)pItemUI->GetTag();
		if (pArg == 0)
			continue;
		std::vector<CMyData * > * pVec = (std::vector<CMyData * > *)pArg;
		ClearVector(*pVec);
		delete pVec;
	}
	m_layFormat->RemoveAll();
}

bool CDuiFrameWnd::OnGridsSize(void * pParam) {
	RECT r = m_layFormat->GetPos();
	int width = r.right - r.left;
	if (width <= 0)
		return true;

	if ( m_nFormatType == FORMAT_TYPE_TEXT ) {
		m_layFormat->SetFixedColumns(1);

		SIZE  s;
		s.cx = width;
		s.cy = m_nItemHeight;
		m_layFormat->SetItemSize(s);
	}
	else if (m_nFormatType == FORMAT_TYPE_GRIDS) {
		int nColumns = width / m_nItemWidth;
		if (nColumns <= 0)
			nColumns = 1;

		m_layFormat->SetFixedColumns(nColumns);

		SIZE  s;
		s.cx = width / nColumns;
		s.cy = m_nItemHeight;
		m_layFormat->SetItemSize(s);
	}

	return true;
}

void  CDuiFrameWnd::OnFinalMessage(HWND hWnd) {
	OnClear1();
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


