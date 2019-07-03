// win32_1.cpp : 定义应用程序的入口点。
//

#ifdef _DEBUG
#include "vld.h"
#endif

#include "Windows.h"
#include "main.h"
#include "business.h"
#include "resource.h"
         
CDuiFrameWnd::CDuiFrameWnd() {
	m_L = 0;
	m_edAddA = 0;
	m_edAddB = 0;
	m_edAddC = 0;
	m_lblAddErr = 0;
}

CDuiFrameWnd::~CDuiFrameWnd() {

}
   
void  CDuiFrameWnd::InitWindow() {
	m_L = init_lua();

	int ret = 0;
	ret = luaL_loadstring(m_L, "function add(a,b) return a + b end");
	assert(0 == ret);
	ret = lua_pcall(m_L, 0, 0, 0);
	assert(0 == ret);

	m_edAddA = static_cast<CEditUI *>(m_PaintManager.FindControl("edAddA"));
	m_edAddB = static_cast<CEditUI *>(m_PaintManager.FindControl("edAddB"));
	m_edAddC = static_cast<CEditUI *>(m_PaintManager.FindControl("edAddC"));
	m_lblAddErr = static_cast<CLabelUI *>(m_PaintManager.FindControl("lblAddErrMsg"));
	m_lblAddErr->SetText("");

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


