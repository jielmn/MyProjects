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
	m_lblDevicesTitle = 0;
	m_lblBarTips = 0;
}

CDuiFrameWnd::~CDuiFrameWnd() {

}

void  CDuiFrameWnd::InitWindow() {
	g_data.m_hWnd = GetHWND();
	PostMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0);

	m_lstFloors = (CListUI *)m_PaintManager.FindControl("lstFloors");
	m_layDevices = (CTileLayoutUI *)m_PaintManager.FindControl("layDevices");
	m_lblDevicesTitle = (CLabelUI *)m_PaintManager.FindControl("lblDevicesTitle");
	m_lblBarTips = (CLabelUI *)m_PaintManager.FindControl("lblStatus");

	m_lblDevicesTitle->SetText("");

	m_layDevices->OnSize += MakeDelegate(this, &CDuiFrameWnd::OnLayoutDevicesSize);
   
	CBusiness::GetInstance()->GetAllFloorsAsyn();
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
		else if (strName == "btnDelFloor") {
			OnDelFloor();
		}
		else if (strName == "btnAddDevice") {
			OnAddDevice();
		}
		else if (strName == "btnDel") {
			CControlUI * pParent = msg.pSender->GetParent()->GetParent()->GetParent()->GetParent();
			OnDelDevice( (CDeviceUI *)pParent );
		}
	}
	WindowImplBase::Notify(msg);
}

LRESULT CDuiFrameWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if (WM_PAINT == uMsg) {
		
	}
	else if (UM_GET_ALL_FLOORS_RET == uMsg) {
		m_vFloors.clear();
		std::vector<int> * pvRet = (std::vector<int>*)wParam;
		std::copy(pvRet->begin(), pvRet->end(), std::back_inserter(m_vFloors));
		delete pvRet;

		std::sort(m_vFloors.begin(), m_vFloors.end(), sortInt);
		UpdateFloors(); 

		// 检查串口
		CheckDeviceCom();
	}
	else if (UM_GET_DEVICES_BY_FLOOR_RET == uMsg) {
		std::vector<DeviceItem*> * pvRet = (std::vector<DeviceItem*> *)wParam;
		int nFloor = lParam;
		UpdateDevicesByFloor(*pvRet, nFloor);
		ClearVector(*pvRet);
		delete pvRet;
	}
	else if (UM_DEL_DEVICE_RET == uMsg) {
		BOOL bRet = wParam;
		int  nDeviceId = lParam;

		if (bRet) {
			int nCnt = m_layDevices->GetCount();
			assert(nCnt > 0);
			if ( nCnt > 0 ) {
				nCnt--;
			}

			for ( int i = 0; i < nCnt; i++ ) {
				CDeviceUI * pDevice = (CDeviceUI *)m_layDevices->GetItemAt(i);
				if ( pDevice->GetDeviceId() == nDeviceId ) {
					m_layDevices->RemoveAt(i); 
					break;
				}
			}
		}
		else {
			MessageBox(GetHWND(), "删除设备失败!", "错误", 0);
		}
	}
	else if (UM_DEL_FLOOR_RET == uMsg) {
		BOOL  bRet   = wParam;
		int   nFloor = lParam;

		if ( bRet ) {
			std::vector<int>::iterator it = std::find(m_vFloors.begin(), m_vFloors.end(), nFloor);
			if ( it != m_vFloors.end() ) {
				m_vFloors.erase(it);
				UpdateFloors();
			}			
		}
		else {
			MessageBox(GetHWND(), "删除楼层失败!", "错误", 0);
		}
	}
	else if (uMsg == WM_DEVICECHANGE) {
		CheckDeviceCom();
	}
	else if (uMsg == UM_CHECK_COM_PORTS_RET) {
		std::vector<int> * pvRet = (std::vector<int> *)wParam;
		assert(pvRet);
		if (pvRet->size() == 0) {
			m_lblBarTips->SetText("没有打开任何接收基站");
		}
		else {
			std::vector<int>::iterator it;
			CDuiString strText = "打开了";
			int i = 0;
			for (it = pvRet->begin(); it != pvRet->end(); ++it, i++) {
				if (i > 0) {
					strText += ",";
				}
				CDuiString item;
				item.Format("com%d", *it);
				strText += item;
			}
			m_lblBarTips->SetText(strText);
		}
		if (pvRet) {
			delete pvRet;
		}
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

	UpdateDevices();
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

void  CDuiFrameWnd::OnDelFloor() {
	int nFloor = GetCurrentFloor();
	if (nFloor == 0) {
		return;
	}

	if ( IDYES == MessageBox(GetHWND(), "你确定要删除该楼层吗？", "删除楼层", 
		 MB_YESNO | MB_DEFBUTTON2) ) {
		CBusiness::GetInstance()->DeleteFloorAsyn(nFloor);
	}	
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
		m_lblDevicesTitle->SetText("实时数据显示(单位：℃)");
	}
	else {
		int  nFloor = m_lstFloors->GetItemAt(nSel)->GetTag();
		CBusiness::GetInstance()->GetDevicesByFloorAsyn(nFloor);
	}
}

void   CDuiFrameWnd::UpdateDevicesByFloor(std::vector<DeviceItem*> vRet, int nFloor) {
	int nSel = m_lstFloors->GetCurSel();
	if (nSel < 0) {
		m_layDevices->RemoveAll();
		m_lblDevicesTitle->SetText("实时数据显示(单位：℃)");
		return;
	}

	int nUiItemCnt = m_layDevices->GetCount();
	int nDataItemCnt = vRet.size();

	// 如果没有button，则添加button
	if (nUiItemCnt == 0) {
		CButtonUI * pBtn = new CButtonUI;
		pBtn->SetText("+");
		pBtn->SetFont(6);
		pBtn->SetBorderColor(0xFFC5C5C5);
		pBtn->SetTextColor(0xFFC5C5C5);
		pBtn->SetBorderSize(1);
		pBtn->SetName("btnAddDevice");
		m_layDevices->Add(pBtn);
	}
	else {
		nUiItemCnt--;
	}

	int nDiff = 0;
	if ( nUiItemCnt < nDataItemCnt ) {
		nDiff = nDataItemCnt - nUiItemCnt;
		for ( int i = 0; i < nDiff; i++ ) {
			CDeviceUI * pDevice = new CDeviceUI;
			m_layDevices->AddAt(pDevice, nUiItemCnt + i);
		}
	}
	else if (nUiItemCnt > nDataItemCnt) {
		nDiff = nUiItemCnt - nDataItemCnt;
		for (int i = 0; i < nDiff; i++) {
			m_layDevices->RemoveAt(0);
		}
	}
	nUiItemCnt = nDataItemCnt;

	CDuiString  strText;
	strText.Format("#%d 实时数据显示(单位：℃)", nFloor);
	m_lblDevicesTitle->SetText(strText);

	std::vector<DeviceItem*>::iterator it;

	for (int i = 0; i < nDataItemCnt; i++) {
		DeviceItem * pItem = vRet[i];
		CDeviceUI * pDevice = (CDeviceUI *)m_layDevices->GetItemAt(i);

		pDevice->SetAddress(pItem->szAddr);
		pDevice->SetDeviceId(pItem->nDeviceNo);
		pDevice->SetTemp(pItem->nTemp, pItem->time);         
	}
}

void  CDuiFrameWnd::OnAddDevice() {

	CAddDeviceDlg * pDlg = new CAddDeviceDlg;
	pDlg->m_nFloor = GetCurrentFloor();
	pDlg->Create(this->m_hWnd, _T("设置"), UI_WNDSTYLE_FRAME | WS_POPUP, NULL, 0, 0, 0, 0);
	pDlg->CenterWindow();
	int ret = pDlg->ShowModal();

	// 如果不是click ok
	if (0 != ret) {
		delete pDlg;
		return;
	}

	int nUiItemCnt = m_layDevices->GetCount();
	assert(nUiItemCnt > 0);
	// 如果没有button，则添加button
	if (nUiItemCnt == 0) {
		CButtonUI * pBtn = new CButtonUI;
		pBtn->SetText("+");
		pBtn->SetFont(6);
		pBtn->SetBorderColor(0xFFC5C5C5);
		pBtn->SetTextColor(0xFFC5C5C5);
		pBtn->SetBorderSize(1);
		pBtn->SetName("btnAddDevice");
		m_layDevices->Add(pBtn);
	}
	else {
		nUiItemCnt--;
	}


	CDeviceUI * pDevice = new CDeviceUI;
	m_layDevices->AddAt(pDevice, nUiItemCnt);

	pDevice->SetAddress(pDlg->m_szDeviceAddr);
	pDevice->SetDeviceId(pDlg->m_nDeviceId);

	delete pDlg;
}

void  CDuiFrameWnd::OnDelDevice(CDeviceUI * pDevice) {
	if ( IDYES == MessageBox(GetHWND(), "你确定要删除该设备吗？", "删除设备", MB_YESNO | MB_DEFBUTTON2)) {
		int nDeviceId = pDevice->GetDeviceId();
		CBusiness::GetInstance()->DeleteDeviceAsyn(nDeviceId);
	}
}

void CDuiFrameWnd::CheckDeviceCom() {
	char szComPort[16];
	int nFindCount = GetCh340Count(szComPort, sizeof(szComPort));

	// 单个串口
	if (!g_data.m_bMultipleComport) {
		if (nFindCount > 1) {
			m_lblBarTips->SetText("存在多个USB-SERIAL CH340串口，请只连接一个发射器");
		}
		else if (0 == nFindCount) {
			m_lblBarTips->SetText("没有找到USB-SERIAL CH340串口，请连接发射器的USB线");
		}
		else {
			m_lblBarTips->SetText("");
		}
	}
	else {
		if (0 == nFindCount) {
			m_lblBarTips->SetText("没有找到USB-SERIAL CH340串口，请连接发射器的USB线");
		}
		else {
			m_lblBarTips->SetText("");
		}
	}

	CBusiness::GetInstance()->CheckLaunchAsyn();
}

int CDuiFrameWnd::GetCurrentFloor() {
	int nSel = m_lstFloors->GetCurSel();
	if ( nSel < 0 ) {
		return 0;
	}

	return m_lstFloors->GetItemAt(nSel)->GetTag();
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
   

 

CAddDeviceDlg::CAddDeviceDlg() {
	m_lblFloor = 0;
	m_edDeviceId = 0;
	m_edDeviceAddress = 0;

	m_nDeviceId = 0;
	memset( m_szDeviceAddr, 0, sizeof(m_szDeviceAddr)) ;
}

void  CAddDeviceDlg::Notify(DuiLib::TNotifyUI& msg) {
	CDuiString  strName = msg.pSender->GetName();
	if (msg.sType == "click") {
		if (strName == "btnOK") {
			OnBtnOk();
		}
	}
	WindowImplBase::Notify(msg);
}

void  CAddDeviceDlg::InitWindow() {
	m_lblFloor         = (CLabelUI *)m_PaintManager.FindControl("lblFloor");
	m_edDeviceId       = (CEditUI *)m_PaintManager.FindControl("edDeviceNo");
	m_edDeviceAddress  = (CEditUI *)m_PaintManager.FindControl("edAddress");

	CDuiString  strText;
	strText.Format("#%d", m_nFloor);
	m_lblFloor->SetText(strText);
	WindowImplBase::InitWindow();
}

LRESULT  CAddDeviceDlg::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if ( uMsg == UM_ADD_DEVICE_RET ) {
		BOOL  bRet = wParam;
		DeviceItem * pDevice = (DeviceItem *)lParam;

		if (bRet) {
			m_nDeviceId = pDevice->nDeviceNo;
			STRNCPY(m_szDeviceAddr, pDevice->szAddr, sizeof(m_szDeviceAddr));
			this->PostMessage(WM_CLOSE);
		}
		else {
			MessageBox(GetHWND(), "添加设备失败！", "错误", 0);
		}

		if (pDevice)
			delete pDevice;
	}
	return WindowImplBase::HandleMessage(uMsg, wParam, lParam);
}

DuiLib::CControlUI * CAddDeviceDlg::CreateControl(LPCTSTR pstrClass) {
	return WindowImplBase::CreateControl(pstrClass);
}

void  CAddDeviceDlg::OnBtnOk() {
	assert(m_nFloor != 0);

	CDuiString  strAddress;
	CDuiString  strDeviceId;
	int         nDeviceId = 0;

	strDeviceId = m_edDeviceId->GetText();
	strDeviceId.Trim();

	if ( 0 == strDeviceId.GetLength() ) {
		MessageBox(GetHWND(), "请输入设备编号!", "错误", 0);
		return;
	}

	if (8 != strDeviceId.GetLength()) {
		MessageBox(GetHWND(), "设备编号格式不对", "错误", 0);
		return;
	}

	strDeviceId.MakeLower();
	for ( int i = 0; i < strDeviceId.GetLength(); i++ ) {
		char ch = strDeviceId.GetAt(i);
		if ( !((ch >= '0' && ch <= '9') || (ch >= 'a' && ch <= 'z')) ) {
			MessageBox(GetHWND(), "设备编号格式不对", "错误", 0);
			return;
		}
	}

	nDeviceId = (int)strtoll(strDeviceId, 0, 16);

	strAddress = m_edDeviceAddress->GetText();
	strAddress.Trim();

	if (strAddress.GetLength() == 0) {
		MessageBox(GetHWND(), "请输入设备位置!", "错误", 0);
		return;
	} 

	CBusiness::GetInstance()->AddDeviceAsyn(GetHWND(), m_nFloor, nDeviceId, strAddress);
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


