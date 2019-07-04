// win32_1.cpp : 定义应用程序的入口点。
//

#ifdef _DEBUG
#include "vld.h"
#endif

#include "Windows.h"
#include "main.h"
#include "business.h"
#include "resource.h"

#define  LUA_ADD_STRING		"function add(a,b) \n" \
							"  return a + b \n" \
							"end"

#define  LUA_TEMP_STRING    "require(\"bit\") \n" \
							"function temp(t) \n" \
							"  local a = bit.band(bit.rshift(t, 8), 0xff) \n" \
							"  local b = bit.band(t, 0xff) \n" \
							"  local c = string.char(0, 0, 0, 0, a, b, 0, 0) \n" \
							"  return c \n" \
							"end" 
         
CDuiFrameWnd::CDuiFrameWnd() {
	m_L = 0;
	m_edAddA = 0;
	m_edAddB = 0;
	m_edAddC = 0;
	m_lblAddErr = 0;
	m_edTemp = 0;
	m_edTempRet = 0;
	m_tooltip_temp = 0;
	m_lblTempErr = 0;
}

CDuiFrameWnd::~CDuiFrameWnd() {

}
   
void  CDuiFrameWnd::InitWindow() {
	CDuiString strText;

	m_L = init_lua();

	int ret = 0;
	ret = luaL_loadstring(m_L, LUA_ADD_STRING);
	assert(0 == ret);
	ret = lua_pcall(m_L, 0, 0, 0);
	assert(0 == ret);

	ret = luaL_loadstring(m_L, LUA_TEMP_STRING);
	assert(0 == ret);
	ret = lua_pcall(m_L, 0, 0, 0);
	assert(0 == ret);

	m_edAddA = static_cast<CEditUI *>(m_PaintManager.FindControl("edAddA"));
	m_edAddB = static_cast<CEditUI *>(m_PaintManager.FindControl("edAddB"));
	m_edAddC = static_cast<CEditUI *>(m_PaintManager.FindControl("edAddC"));
	m_lblAddErr = static_cast<CLabelUI *>(m_PaintManager.FindControl("lblAddErrMsg"));
	m_lblAddErr->SetText("");
	m_tooltip_add = m_PaintManager.FindControl("tooltip_add");
	strText.Format("lua code:\n----------------------\n%s", LUA_ADD_STRING);
	m_tooltip_add->SetToolTip(strText);   

	m_edTemp = static_cast<CEditUI *>(m_PaintManager.FindControl("edTemp"));
	m_edTempRet = static_cast<CEditUI *>(m_PaintManager.FindControl("edStream"));
	m_tooltip_temp = m_PaintManager.FindControl("tooltip_stream");
	strText.Format("lua code:\n----------------------\n%s", LUA_TEMP_STRING);
	m_tooltip_temp->SetToolTip(strText);
	m_lblTempErr = static_cast<CLabelUI *>(m_PaintManager.FindControl("lblTempErrMsg"));
	m_lblTempErr->SetText("");

	WindowImplBase::InitWindow();
}

CControlUI * CDuiFrameWnd::CreateControl(LPCTSTR pstrClass) {
	return WindowImplBase::CreateControl(pstrClass);
}

void CDuiFrameWnd::Notify(TNotifyUI& msg) {
	CDuiString  name = msg.pSender->GetName();
	if ( msg.sType == "textchanged" ) {
		if ( name == "edAddA" || name == "edAddB" ) {
			OnAdd();
		}
		else if ( name == "edTemp" ) {
			OnTemp();
		}
	}
	WindowImplBase::Notify(msg);
}

LRESULT CDuiFrameWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	return WindowImplBase::HandleMessage(uMsg,wParam,lParam);
}


                
void  CDuiFrameWnd::OnAdd() {
	CDuiString  strText;
	CDuiString  strTextA;
	CDuiString  strTextB;
	int a = 0, b = 0;
	int ret = 0;

	strTextA = m_edAddA->GetText();
	strTextB = m_edAddB->GetText();

	if ( strTextA.GetLength() == 0 && strTextB.GetLength() == 0 ) {
		m_edAddC->SetText("");
		return;
	}

	ret = sscanf(strTextA, " %d", &a);
	if (1 != ret) {
		m_lblAddErr->SetText("加数a不是数字!");
		return;
	}

	ret = sscanf(strTextB, " %d", &b);
	if (1 != ret) {
		m_lblAddErr->SetText("加数b不是数字!");
		return;
	}

	/*lua 处理*/
	lua_getglobal(m_L, "add");
	lua_pushinteger(m_L, a);
	lua_pushinteger(m_L, b);

	ret = lua_pcall(m_L, 2, 1, 0);
	if ( 0 != ret ) {
		m_lblAddErr->SetText("lua处理出错!");
		lua_settop(m_L, 0);
		return;
	}

	ret = (int)lua_tonumber(m_L, 1);
	lua_settop(m_L, 0);

	strText.Format("%d", ret);
	m_edAddC->SetText(strText);
	m_lblAddErr->SetText("");

}

void CDuiFrameWnd::OnTemp() {
	CDuiString  strText;
	int nTemp = 0;
	int ret = 0;

	strText = m_edTemp->GetText();
	if (strText.GetLength() == 0) {
		m_lblTempErr->SetText("");
		return;
	}

	ret = sscanf(strText, " %d", &nTemp);
	if (1 != ret) {
		m_lblTempErr->SetText("请输入一个温度值");
		return;
	}

	/*lua 处理*/
	lua_getglobal(m_L, "temp");
	lua_pushinteger(m_L, nTemp);

	ret = lua_pcall(m_L, 1, 1, 0);
	if (0 != ret) {
		m_lblTempErr->SetText("lua处理出错!");
		lua_settop(m_L, 0);
		return;
	}

	size_t len = 0;
	CDuiString str;
	strText = "";

	const char * p = lua_tolstring(m_L, 1, &len);
	for ( size_t i = 0; i < len; i++ ) {
		str.Format("%02X ", p[i]);
		strText += str;
	}
	m_edTempRet->SetText(strText);

	lua_settop(m_L, 0);

	int a = 100;
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


