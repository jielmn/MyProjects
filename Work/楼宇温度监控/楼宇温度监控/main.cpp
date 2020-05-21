// win32_1.cpp : ����Ӧ�ó������ڵ㡣
//

#ifdef _DEBUG
#include "vld.h"
#endif

#include "Windows.h"
#include "main.h"
#include "business.h"
#include "resource.h"

CDuiFrameWnd::CDuiFrameWnd() {
	m_lstFloors = 0;
}

CDuiFrameWnd::~CDuiFrameWnd() {

}

void  CDuiFrameWnd::InitWindow() {
	g_data.m_hWnd = GetHWND();
	PostMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0);

	m_lstFloors = (CListUI *)m_PaintManager.FindControl("lstFloors");

#ifdef _DEBUG
	for (int i = 0; i < 10; i++) {
		AddFloor(i + 1);  
	}
#endif
   

	WindowImplBase::InitWindow();
}

CControlUI * CDuiFrameWnd::CreateControl(LPCTSTR pstrClass) {
	DuiLib::CDialogBuilder builder;
	DuiLib::CDuiString  strText;
	DuiLib::CControlUI * pUI = 0;

	if (0 == strcmp(pstrClass, "Floors")) {
		strText.Format("%s.xml", pstrClass);
		pUI = builder.Create((const char *)strText, (UINT)0, this, &m_PaintManager);
		return pUI;
	}
	else if (0 == strcmp(pstrClass, "Devices")) {
		strText.Format("%s.xml", pstrClass);
		pUI = builder.Create((const char *)strText, (UINT)0, this, &m_PaintManager);
		return pUI;                    
	}

	return WindowImplBase::CreateControl(pstrClass);
}

void CDuiFrameWnd::Notify(TNotifyUI& msg) {
	if ( msg.sType == "itemselect" ) {
		if ( msg.pSender == m_lstFloors ) {
			OnFloorSelected(msg.lParam);
		}
	}
	WindowImplBase::Notify(msg);
}

LRESULT CDuiFrameWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if (WM_PAINT == uMsg) {
		int a = 100; 
	}
	return WindowImplBase::HandleMessage(uMsg,wParam,lParam);
}

void CDuiFrameWnd::AddFloor(int nFloor) {
	CDuiString  strText;

	if (nFloor == 0)
		return;

	if ( nFloor > 0 ) {
		strText.Format("%3d¥", nFloor);
	}
	else if (nFloor < 0) {
		strText.Format("����%3d¥", -nFloor);
	}

	CListContainerElementUI * pListContainerElement = 0;

	pListContainerElement = new CListContainerElementUI;
	pListContainerElement->SetFixedHeight(40);      

	m_lstFloors->Add(pListContainerElement);

	CHorizontalLayoutUI * pLayout = new CHorizontalLayoutUI;
	RECT rcInset = { 20 };
	pLayout->SetInset(rcInset);
	pListContainerElement->Add(pLayout);

	CLabelUI * pLabel = new CLabelUI;
	pLabel->SetFixedWidth(200);
	pLabel->SetFont(2);
	pLabel->SetTextColor(0xFF447AA1);
	pLabel->SetText(strText);
	pLayout->Add(pLabel);
}

CControlUI* CALLBACK FindFloorLabel (CControlUI* pControl, LPVOID) {
	if ( 0 == _tcscmp(pControl->GetClass(), DUI_CTR_LABEL) ) {
		return pControl;
	}
	return 0;
}

void CDuiFrameWnd::OnFloorSelected(int nOldSelected) {
	int nSel = m_lstFloors->GetCurSel();
	if (nSel < 0)
		return;

	CListContainerElementUI * pListContainerElement = (CListContainerElementUI *)m_lstFloors->GetItemAt(nSel);
	CLabelUI * pLabel =  (CLabelUI *)pListContainerElement->FindControl(FindFloorLabel, 0, 0);
	pLabel->SetTextColor(0xFFFFFFFF);

	if ( nOldSelected >= 0 ) {
		pListContainerElement = (CListContainerElementUI *)m_lstFloors->GetItemAt(nOldSelected);
		pLabel = (CLabelUI *)pListContainerElement->FindControl(FindFloorLabel, 0, 0);
		pLabel->SetTextColor(0xFF447AA1);
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

	// ����icon
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


