#ifdef _WIN32
#include "vld.h"
#endif

#include "main.h"
#include "LmnThread.h"
#include "Business.h"
#include "resource.h"
#include "LoginDlg.h"
#include "AgencyDlg.h"

CDuiFrameWnd::CDuiFrameWnd() {
	CBusiness::GetInstance()->m_sigStatusChange.connect(this, &CDuiFrameWnd::OnDbStatusChange);
}

void  CDuiFrameWnd::InitWindow() {
	PostMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0);
	m_tabs = static_cast<DuiLib::CTabLayoutUI*>(m_PaintManager.FindControl("switch"));
	m_lblDbStatus = static_cast<DuiLib::CLabelUI*>(m_PaintManager.FindControl("lblDbStatus"));

	CInvoutDatabase::DATABASE_STATUS eStatus = CBusiness::GetInstance()->GetDbStatus();
	if (eStatus == CLmnOdbc::STATUS_OPEN) {
		m_lblDbStatus->SetText("数据库连接OK");
	}
	else {
		m_lblDbStatus->SetText("数据库连接失败");     
	}

	DuiLib::WindowImplBase::InitWindow();
}

void  CDuiFrameWnd::Notify(DuiLib::TNotifyUI& msg) {
	DuiLib::CDuiString name = msg.pSender->GetName();
	DuiLib::CDuiString strText;

	if (msg.sType == _T("selectchanged"))
	{
		if (name == _T("Inventory_out")) {
			m_tabs->SelectItem(0); 
		}
		else if (name == _T("Agency")) {
			m_tabs->SelectItem(1);
		}
		else if (name == _T("Query")) { 
			m_tabs->SelectItem(2);
		}
	}
	else if (msg.sType == "click") {
		if (name == "btnAddAgency") {
			OnAddAgency();
		}
	}

	DuiLib::WindowImplBase::Notify(msg);
}

DuiLib::CControlUI * CDuiFrameWnd::CreateControl(LPCTSTR pstrClass) {
	DuiLib::CDuiString  strText;
	strText = pstrClass;
	strText += ".xml";

	DuiLib::CDialogBuilder builder;
	DuiLib::CControlUI * pUI = builder.Create((const char *)strText, (UINT)0, 0, &m_PaintManager);
	return pUI;

	//return DuiLib::WindowImplBase::CreateControl( pstrClass );
}

// 处理自定义信息
LRESULT CDuiFrameWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	return DuiLib::WindowImplBase::HandleMessage(uMsg, wParam, lParam);
}

void   CDuiFrameWnd::OnDbStatusChange(CLmnOdbc::DATABASE_STATUS eNewStatus) {
	::PostMessage(this->GetHWND(), UM_DB_STATUS, eNewStatus, 0);
}

void   CDuiFrameWnd::OnDbStatusChangeMsg(CLmnOdbc::DATABASE_STATUS eNewStatus) {
	if (eNewStatus == CLmnOdbc::STATUS_OPEN) {
		m_lblDbStatus->SetText("数据库连接OK");
	}
	else {
		m_lblDbStatus->SetText("数据库连接失败");
	}
}

void  CDuiFrameWnd::OnAddAgency() {
	CAgencyWnd * pDlg = new CAgencyWnd;

	pDlg->Create(this->m_hWnd, _T("新增经销商信息"), UI_WNDSTYLE_FRAME | WS_POPUP, NULL, 0, 0, 0, 0);
	pDlg->CenterWindow();
	int ret = pDlg->ShowModal();

	// 如果添加成功
	if (0 == ret) {
		//DuiLib::CListTextElementUI* pListElement = new DuiLib::CListTextElementUI;
		//m_lstPatients->Add(pListElement);

		//AddPatientItem2List(pListElement, &pPatientDlg->m_tPatientInfo);
	}

	delete pDlg; 
}














int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	int ret = 0;
	LmnToolkits::ThreadManager::GetInstance();
	CBusiness::GetInstance()->Init();

	DuiLib::CPaintManagerUI::SetInstance(hInstance);
	HICON hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));


	CLoginWnd * loginFrame = new CLoginWnd;
	loginFrame->Create(NULL, _T("DUIWnd"), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
	loginFrame->CenterWindow();
	::SendMessage(loginFrame->GetHWND(), WM_SETICON, ICON_BIG, (LPARAM)hIcon);
	::SendMessage(loginFrame->GetHWND(), WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
	// 显示登录界面
	ret = loginFrame->ShowModal();
	delete loginFrame;

	// 如果登录成功
	if (0 == ret) {
		// 主界面	
		CDuiFrameWnd * duiFrame = new CDuiFrameWnd;
		duiFrame->Create(NULL, _T("DUIWnd"), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
		duiFrame->CenterWindow();

		// 设置icon
		::SendMessage(duiFrame->GetHWND(), WM_SETICON, ICON_BIG, (LPARAM)hIcon);
		::SendMessage(duiFrame->GetHWND(), WM_SETICON, ICON_SMALL, (LPARAM)hIcon);

		// show model
		duiFrame->ShowModal();
		delete duiFrame;
		duiFrame = 0;
	}

	CBusiness::GetInstance()->DeInit();
	delete CBusiness::GetInstance();
	LmnToolkits::ThreadManager::ReleaseInstance();

	return 0;
}