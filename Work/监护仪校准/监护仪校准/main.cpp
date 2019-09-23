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
	m_nCurColumns = 0;
	m_layMain = 0;
	memset(m_layColumns, 0, sizeof(m_layColumns));
	memset(m_temp_items, 0, sizeof(m_temp_items));
}

CDuiFrameWnd::~CDuiFrameWnd() {

}

void  CDuiFrameWnd::InitWindow() {
	PostMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0);

	m_cmbComPorts = static_cast<DuiLib::CComboUI*>(m_PaintManager.FindControl("cmComPort"));
	m_cmbMachineType = static_cast<DuiLib::CComboUI*>(m_PaintManager.FindControl("cmMachine"));
	m_cmbFiles = static_cast<DuiLib::CComboUI*>(m_PaintManager.FindControl("cmFiles"));
	m_layMain = static_cast<DuiLib::CHorizontalLayoutUI*>(m_PaintManager.FindControl("layMain"));

	for ( int i = 0; i < MAX_COLUMNS_CNT; i++ ) {
		m_layColumns[i] = new CVerticalLayoutUI;
		m_layColumns[i]->SetVisible(false);
		m_layMain->Add(m_layColumns[i]);
	}
	m_layMain->OnSize += MakeDelegate(this, &CDuiFrameWnd::OnMainSize);

	for (int i = 0; i < MAX_TEMP_ITEMS_CNT; i++) {
		m_temp_items[i] = new CTempItemUI;
		m_temp_items[i]->SetTemp(FIRST_TEMP + i * 10);
	}

	for (int i = MachineType_MR; i <= MachineType_GE2; i++) {
		CListLabelElementUI * pElement = new CListLabelElementUI;
		pElement->SetText(GetMachineType((MachineType)i));
		pElement->SetTag(i);
		m_cmbMachineType->Add(pElement);
	}
	m_cmbMachineType->SelectItem(0,false,false);

	CListLabelElementUI * pElement = new CListLabelElementUI;
	pElement->SetText("标准");
	m_cmbFiles->Add(pElement);
	m_cmbFiles->SelectItem(0);

	CheckDevice();     
	WindowImplBase::InitWindow();
}
 
CControlUI * CDuiFrameWnd::CreateControl(LPCTSTR pstrClass) {
	return WindowImplBase::CreateControl(pstrClass);
}

void CDuiFrameWnd::Notify(TNotifyUI& msg) {
	CDuiString  name = msg.pSender->GetName();
	if ( msg.sType == "windowinit" ) {
		OnWindowInit();
	}
	else if (msg.sType == "itemselect") {
		if (name == "cmFiles") {
			OnFileChanged();
		}
		else if (name == "cmMachine") {
			OnMachineChanged();
		}
	}
	WindowImplBase::Notify(msg);                
}

LRESULT CDuiFrameWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	return WindowImplBase::HandleMessage(uMsg,wParam,lParam);
}

void  CDuiFrameWnd::OnWindowInit() {
	
}

LRESULT CDuiFrameWnd::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
	return WindowImplBase::OnSize(uMsg, wParam, lParam, bHandled);
}

bool CDuiFrameWnd::OnMainSize(void * pParam) {
	RECT rcMain = m_layMain->GetPos();
	int nWidth = rcMain.right - rcMain.left - 2;
	int nColumn = nWidth / CTempItemUI::WIDTH;
	if (nColumn > MAX_COLUMNS_CNT)
		nColumn = MAX_COLUMNS_CNT;
	else if (nColumn <= 0)
		nColumn = 1;

	// 如果列数不变
	if (m_nCurColumns == nColumn)
		return true;

	for (int i = 0; i < MAX_COLUMNS_CNT; i++) {
		int nCnt = m_layColumns[i]->GetCount();
		for (int j = 0; j < nCnt; j++) {
			m_layColumns[i]->RemoveAt( 0, true );
		}
		m_layColumns[i]->SetVisible(false);
	}
	
	int nRows = (MAX_TEMP_ITEMS_CNT - 1) / nColumn + 1;
	int nResume = nRows % 10;
	// 如果不是整数10行
	if (0 != nResume ) {
		nRows += 10 - nResume;
	}
	// 重新计算列数
	nColumn = ( MAX_TEMP_ITEMS_CNT - 1 ) / nRows + 1;
	m_nCurColumns = nColumn;

	int nItemIndex = 0;
	for ( int i = 0; i < m_nCurColumns; i++ ) {
		m_layColumns[i]->SetVisible(true);   
		for ( int j = nRows * i; j < (i + 1) * nRows && nItemIndex < MAX_TEMP_ITEMS_CNT; j++, nItemIndex++ ) {
			m_layColumns[i]->Add(m_temp_items[j]);
		}
	}

	m_layMain->SetFixedHeight(nRows * CTempItemUI::HEIGHT + 2); 
	return true;
}   

// 检查硬件(串口)
void  CDuiFrameWnd::CheckDevice() {
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
			m_cmbComPorts->SelectItem(nFindeIndex,false,false);
		}
		else {
			m_cmbComPorts->SelectItem(0,false,false);
		}
	}
}

void  CDuiFrameWnd::OnFileChanged() {
	int nFileSel     = m_cmbFiles->GetCurSel();
	int nMachineSel  = m_cmbMachineType->GetCurSel();
	assert(nFileSel >= 0 && nMachineSel >= 0);
	int nMachine = m_cmbMachineType->GetItemAt(nMachineSel)->GetTag();

	// 如果是标准类型
	if (nFileSel == 0) {
		for (int i = 0; i < MAX_TEMP_ITEMS_CNT; i++) {
			m_temp_items[i]->SetDutyCycle( g_data.m_standard_items[nMachine][i].m_nDutyCycle );
		}
	}
	// 不是标准类型
	else {

	}
}

void  CDuiFrameWnd::OnMachineChanged() {
	m_cmbFiles->SelectItem(0, false, false);
	OnFileChanged();
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


