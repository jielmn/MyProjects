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
	m_lstFloors = 0;
	m_layDevices = 0;
	m_pHighlightDevice = 0;
}

CDuiFrameWnd::~CDuiFrameWnd() {

}

void  CDuiFrameWnd::InitWindow() {
	g_data.m_hWnd = GetHWND();
	PostMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0);

	m_lstFloors = (CListUI *)m_PaintManager.FindControl("lstFloors");
	m_layDevices = (CTileLayoutUI *)m_PaintManager.FindControl("layDevices");

#ifdef _DEBUG
	//for (int i = 0; i < 10; i++) {
	//	AddFloor(i + 1);  
	//}

	//for (int i = 0; i < 10; i++) {
	//	CDeviceUI * pDevice = new CDeviceUI;
	//	m_layDevices->Add(pDevice);
	//}

	//CButtonUI * pBtn = new CButtonUI;
	//pBtn->SetText("+");
	//pBtn->SetFont(6); 
	//pBtn->SetBorderColor(0xFFC5C5C5);
	//pBtn->SetTextColor(0xFFC5C5C5);  
	//pBtn->SetBorderSize(1);
	//m_layDevices->Add(pBtn);
#endif

	m_layDevices->OnSize += MakeDelegate(this, &CDuiFrameWnd::OnLayoutDevicesSize);
   

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
	CDuiString  strName = msg.pSender->GetName();

	if ( msg.sType == "itemselect" ) {
		if ( msg.pSender == m_lstFloors ) {
			OnFloorSelected(msg.lParam);
		}
	}
	else if (msg.sType == "device_selected") {
		OnDeviceHighlight( (CDeviceUI *)msg.pSender );
	}
	else if (msg.sType == "device_unselected") {
		OnDeviceUnHighlight( (CDeviceUI *)msg.pSender );
	}
	else if (msg.sType == "device_dbclick") {
		
	}
	else if (msg.sType == "click") {
		if (strName == "btnAddFloor") {
			OnAddFloor();
		}
	}
	WindowImplBase::Notify(msg);
}

LRESULT CDuiFrameWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if (WM_PAINT == uMsg) {
		
	}
	return WindowImplBase::HandleMessage(uMsg,wParam,lParam);
}

void CDuiFrameWnd::AddFloor(int nFloor) {
	CDuiString  strText;

	if ( nFloor >= 0 ) {
		strText.Format("%3d楼", nFloor);
	}
	else if (nFloor < 0) {
		strText.Format("地下%3d楼", -nFloor);
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
	pLabel->SetName("lblFloor");
	pLabel->SetFixedWidth(200);
	pLabel->SetFont(2);
	pLabel->SetTextColor(0xFF447AA1);
	pLabel->SetText(strText);
	pLayout->Add(pLabel);

	CVerticalLayoutUI * pVLayout = new CVerticalLayoutUI;
	CControlUI * pHolder = new CControlUI;
	pVLayout->Add(pHolder);

	CControlUI * pWarning = new CControlUI;
	pWarning->SetFixedWidth(20);
	pWarning->SetFixedHeight(20);
	//pWarning->SetBkImage("warning.png"); 
	pVLayout->Add(pWarning);

	pHolder = new CControlUI;
	pVLayout->Add(pHolder);

	pLayout->Add(pVLayout);      
}

void  CDuiFrameWnd::SetFloor(CListContainerElementUI * pItem, int nFloor) {
	CLabelUI * pLblFloor = (CLabelUI *)pItem->FindControl(FindControlByName, "lblFloor",0);
	if ( pLblFloor ) {
		CDuiString strText;
		if (nFloor >= 0) {
			strText.Format("%d楼", nFloor);
		}
		else if (nFloor < 0) {
			strText.Format("地下%d楼", -nFloor);
		}
		pLblFloor->SetText(strText);
	}
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

bool CDuiFrameWnd::OnLayoutDevicesSize(void * pParam) {
	RECT rcDevices = m_layDevices->GetClientPos();
	int  nDevicesWidth  = rcDevices.right - rcDevices.left;
	int  nDevicesHeight = rcDevices.bottom - rcDevices.top;	

	int nColumns = nDevicesWidth / CDeviceUI::FIXED_WIDTH;
	if (nColumns < 1) {
		nColumns = 1;
	}

	m_layDevices->SetFixedColumns(nColumns);

	SIZE  sz = { 0 };
	sz.cx = nDevicesWidth / nColumns;
	sz.cy = CDeviceUI::FIXED_HEIGHT;
	m_layDevices->SetItemSize(sz);

	return true;      
}

void CDuiFrameWnd::OnDeviceHighlight(CDeviceUI * pDevice) {
	if (m_pHighlightDevice) {
		m_pHighlightDevice->SetHighlight(FALSE);
	}

	pDevice->SetHighlight(TRUE);
	m_pHighlightDevice = pDevice;
}

void CDuiFrameWnd::OnDeviceUnHighlight(CDeviceUI * pDevice) {
	pDevice->SetHighlight(FALSE);
	if (pDevice == m_pHighlightDevice) {
		m_pHighlightDevice = 0;
	}	
}

void CDuiFrameWnd::OnAddFloor() {
	RECT rect;
	::GetWindowRect(GetHWND(), &rect);

	CAddFloorDlg * pDlg = new CAddFloorDlg;
	pDlg->Create(this->m_hWnd, _T("设置"), UI_WNDSTYLE_FRAME | WS_POPUP, NULL, 0, 0, 0, 0);
	pDlg->CenterWindow();
	::MoveWindow(pDlg->GetHWND(), rect.left + 50, rect.top + 100, 300, 200, TRUE);
	int ret = pDlg->ShowModal();

	// 如果不是click ok
	if (0 != ret) {
		delete pDlg;
		return;   
	}

	m_vFloors.push_back(pDlg->m_nFloor);
	std::sort(m_vFloors.begin(), m_vFloors.end(), sortInt);
	UpdateFloors();

	delete pDlg;
}

void CDuiFrameWnd::UpdateFloors() {
	int nCnt   = (int)m_vFloors.size();
	int nUiCnt = m_lstFloors->GetCount();
	int nDiff = 0;

	int nOldSelNo = 0;
	int nOldSel = m_lstFloors->GetCurSel();
	if (nOldSel >= 0) {
		nOldSelNo = m_lstFloors->GetItemAt(nOldSel)->GetTag();
	}

	if (nCnt > nUiCnt) {
		nDiff = nCnt - nUiCnt;
		for (int i = 0; i < nDiff; i++) {
			AddFloor(0);
		}
	}
	else if (nCnt < nUiCnt) {
		nDiff = nUiCnt - nCnt;
		for (int i = 0; i < nDiff; i++) {
			m_lstFloors->RemoveAt(0);
		}
	}

	CDuiString strText;
	for (int i = 0; i < nCnt; i++) {
		int    nFloor  = m_vFloors[i];

		CListContainerElementUI * pItem = (CListContainerElementUI *)m_lstFloors->GetItemAt(i);
		SetFloor(pItem, nFloor);
		pItem->SetTag(nFloor);
	}

	if (nOldSelNo != 0) {
		int i = 0;
		for (i = 0; i < nCnt; i++) {
			CListContainerElementUI * pItem = (CListContainerElementUI *)m_lstFloors->GetItemAt(i);
			if (pItem->GetTag() == nOldSelNo) {
				m_lstFloors->SelectItem(i);
				break;
			}
		}

		// 如果没有找到
		if (i >= nCnt) {
			if (nCnt > 0) {
				m_lstFloors->SelectItem(0);
			}
		}
	}
	else {
		if (nCnt > 0) {
			m_lstFloors->SelectItem(0);
		}
	}

	UpdateDevices();
}

void CDuiFrameWnd::UpdateDevices() {
	int nSel = m_lstFloors->GetCurSel();
	if ( nSel < 0 ) {
		m_layDevices->RemoveAll();	
	}
	else {
		int  nFloor = m_lstFloors->GetItemAt(nSel)->GetTag();
		//CBusiness::GetInstance()->GetRoomDataAsyn(dwNo);
	}
}






CAddFloorDlg::CAddFloorDlg() {
	m_edFloor = 0;
	m_opUnderGround = 0;
	m_nFloor = 0;
}

void  CAddFloorDlg::Notify(DuiLib::TNotifyUI& msg) {
	CDuiString  strName = msg.pSender->GetName();
	if (msg.sType == "click") {
		if (strName == "btnOK") {
			OnBtnOk();
		}
	}
	WindowImplBase::Notify(msg);
}

void  CAddFloorDlg::OnBtnOk() {
	BOOL bUnderGround = m_opUnderGround->IsSelected() ? TRUE : FALSE;

	CDuiString strText = m_edFloor->GetText();
	int nFloor = 0;
	sscanf(strText, "%d", &nFloor);
	if (nFloor <= 0) {
		MessageBox(GetHWND(), "楼层输入有误", "错误", 0);
		return;
	}

	if (bUnderGround) {
		nFloor = -nFloor;
	}

	CBusiness::GetInstance()->AddFloorAsyn(GetHWND(),nFloor);
}

void  CAddFloorDlg::InitWindow() {
	m_edFloor = (CEditUI * )m_PaintManager.FindControl("edtFloor");
	m_opUnderGround = (COptionUI *)m_PaintManager.FindControl("radio2");
	WindowImplBase::InitWindow();
}

LRESULT  CAddFloorDlg::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if ( uMsg == UM_ADD_FLOOR_RET ) {
		BOOL bRet = (BOOL)wParam;
		if (bRet) {
			m_nFloor = lParam;			
			this->PostMessage(WM_CLOSE);
		}
		else {
			MessageBox(GetHWND(), "添加楼层失败！", "错误", 0);
		}
	}
	return WindowImplBase::HandleMessage(uMsg, wParam, lParam);
}

DuiLib::CControlUI * CAddFloorDlg::CreateControl(LPCTSTR pstrClass) {
	return 0;
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


