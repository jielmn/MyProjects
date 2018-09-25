#include <time.h>
#include "main.h"
#include "Business.h"
#include "resource.h"


CDuiFrameWnd::CDuiFrameWnd() {
	CBusiness::GetInstance()->m_sigReaderStatusChange.connect(this, &CDuiFrameWnd::OnReaderStatusChange);
	CBusiness::GetInstance()->m_sigSetReaderIdRet.connect(this, &CDuiFrameWnd::OnSetReaderIdRet);
	CBusiness::GetInstance()->m_sigSetReaderTimeRet.connect(this, &CDuiFrameWnd::OnSetReaderTimeRet);
	CBusiness::GetInstance()->m_sigSetReaderModeRet.connect(this, &CDuiFrameWnd::OnSetReaderModeRet);
	CBusiness::GetInstance()->m_sigClearReaderRet.connect(this, &CDuiFrameWnd::OnClearReaderRet);
	CBusiness::GetInstance()->m_sigGetReaderDataRet.connect(this, &CDuiFrameWnd::OnGetReaderDataRet);
	CBusiness::GetInstance()->m_sigSetReaderBlueToothRet.connect(this, &CDuiFrameWnd::OnSetReaderBlueToothRet);
}
  
void  CDuiFrameWnd::InitWindow() {
	PostMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0);
	//m_PaintManager.FindControl("maxbtn")->SetEnabled(false);
	DuiLib::CDuiString  strText;

	m_btnSetBluetooth = (DuiLib::CButtonUI *)m_PaintManager.FindControl("btnSetReaderBluetooth");
	m_btnSetId = (DuiLib::CButtonUI *)m_PaintManager.FindControl("btnSetReaderId");
	m_btnSetTime = (DuiLib::CButtonUI *)m_PaintManager.FindControl("btnSetReaderTime");
	m_btnSetMode = (DuiLib::CButtonUI *)m_PaintManager.FindControl("btnSetReaderMode");
	m_btnClear = (DuiLib::CButtonUI *)m_PaintManager.FindControl("btnClear");
	m_btnGetData = (DuiLib::CButtonUI *)m_PaintManager.FindControl("btnGetData");

	m_edVersion = (DuiLib::CEditUI *)m_PaintManager.FindControl("edReaderVersion");
	m_edId = (DuiLib::CEditUI *)m_PaintManager.FindControl("edReaderId");
	m_opNormalMode = (DuiLib::COptionUI *)m_PaintManager.FindControl("btnNormal");
	m_lstData = (DuiLib::CListUI *)m_PaintManager.FindControl("data_list");
	m_lblReaderStatus = (DuiLib::CLabelUI *)m_PaintManager.FindControl("lblReaderStatus");
	m_opBlueTooth = (DuiLib::COptionUI *)m_PaintManager.FindControl("opBluetooth");

	m_progress = (CMyProgress *)m_PaintManager.FindControl("progress");

	strText.Format("%lu", g_dwReaderVersion);
	m_edVersion->SetText(strText);
	DuiLib::WindowImplBase::InitWindow(); 
}

void  CDuiFrameWnd::Notify(DuiLib::TNotifyUI& msg) {
	DuiLib::CDuiString name = msg.pSender->GetName();
	DuiLib::CDuiString strText;
	if (msg.sType == "click") {
		if (name == "btnSetReaderId") {
			OnSetReaderId();
		}
		else if (name == "btnSetReaderTime") {
			OnSetReaderTime();
		}
		else if (name == "btnSetReaderMode") {
			OnSetReaderMode();
		}
		else if (name == "btnClear") {
			OnClearReader();
		}
		else if (name == "btnGetData") {
			OnGetData();
		}
		else if (name == "btnSetReaderBluetooth") {
			OnSetBluetooth();
		}
	}
	else if ( msg.sType == "textchanged") {
		if (name == "edReaderVersion") {
			strText = m_edVersion->GetText();
			g_cfg->SetConfig("reader version", (const char *)strText);
			g_cfg->Save();
		}
	}

	DuiLib::WindowImplBase::Notify(msg);
}

DuiLib::CControlUI * CDuiFrameWnd::CreateControl(LPCTSTR pstrClass) {
	if (0 == strcmp(pstrClass, "MyProgress")) {
		return new CMyProgress(&m_PaintManager, "progress_fore.png");
	}
	return DuiLib::WindowImplBase::CreateControl(pstrClass);
}

// 处理自定义信息
LRESULT CDuiFrameWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if (uMsg == UM_READER_STATUS) {
		CTelemedReader::TELEMED_READER_STATUS eNewStatus = (CTelemedReader::TELEMED_READER_STATUS)wParam;
		OnReaderStatusChangeMsg(eNewStatus);
	}
	else if (uMsg == UM_READER_ID_RET) {
		OnSetReaderIdRetMsg(wParam);
	}
	else if (uMsg == UM_READER_TIME_RET) {
		OnSetReaderTimeRetMsg(wParam);
	}
	else if (uMsg == UM_READER_MODE_RET) {
		OnSetReaderModeRetMsg(wParam);
	}
	else if (uMsg == UM_CLEAR_READER_RET) {
		OnClearReaderRetMsg(wParam);
	}
	else if (uMsg == UM_READER_BLUE_TOOTH_RET) {
		OnSetReaderBlueToothRetMsg(wParam);
	}
	else if (uMsg == UM_GET_READER_DATA_RET) {
		int ret = wParam;
		std::vector<TempItem* > * pvRet = (std::vector<TempItem* > *)lParam;

		OnGetReaderDataRetMsg(ret, pvRet);

		if (pvRet) {
			ClearVector(*pvRet);
			delete pvRet;
		}
	}
	return DuiLib::WindowImplBase::HandleMessage(uMsg, wParam, lParam);
}

void  CDuiFrameWnd::SetBusy(BOOL bBusy /*= TRUE*/) {
	if ( bBusy ) {
		m_btnSetId->SetEnabled( false );
		m_btnSetTime->SetEnabled(false);
		m_btnSetMode->SetEnabled(false);
		m_btnClear->SetEnabled(false);
		m_btnGetData->SetEnabled(false);
		m_btnSetBluetooth->SetEnabled(false);

		m_progress->SetVisible(true);
		m_progress->Start();
	}
	else {
		m_btnSetId->SetEnabled(true);
		m_btnSetTime->SetEnabled(true);
		m_btnSetMode->SetEnabled(true);
		m_btnClear->SetEnabled(true);
		m_btnGetData->SetEnabled(true);
		m_btnSetBluetooth->SetEnabled(true);

		m_progress->Stop();
		m_progress->SetVisible(false);
	}
}
       
void  CDuiFrameWnd::OnReaderStatusChange(CTelemedReader::TELEMED_READER_STATUS eNewStatus) {
	::PostMessage(GetHWND(), UM_READER_STATUS, (WPARAM)eNewStatus, 0);
}

void  CDuiFrameWnd::OnReaderStatusChangeMsg(CTelemedReader::TELEMED_READER_STATUS eNewStatus) {
	if (CTelemedReader::STATUS_CLOSE == eNewStatus) {
		m_lblReaderStatus->SetText("Reader连接失败");
	}
	else if (CTelemedReader::STATUS_OPEN == eNewStatus) {
		m_lblReaderStatus->SetText("Reader连接OK");
	}
	else {
		assert(0);
	}
}

void  CDuiFrameWnd::OnSetReaderIdRet(int ret) {
	::PostMessage(GetHWND(), UM_READER_ID_RET, (WPARAM)ret, 0);	
}

void  CDuiFrameWnd::OnSetReaderIdRetMsg(int ret) {
	
	SetBusy(FALSE); 

	if (0 == ret) {
		MessageBox(GetHWND(), "设置Reader ID成功", "设置Reader ID", 0);
	}
	else {
		MessageBox(GetHWND(), GetErrorDescription( ret ), "设置Reader ID", 0);
	}	
 	
}

void  CDuiFrameWnd::OnSetReaderTimeRet(int ret) {
	::PostMessage(GetHWND(), UM_READER_TIME_RET, (WPARAM)ret, 0);
}

void  CDuiFrameWnd::OnSetReaderTimeRetMsg(int ret) {
	SetBusy(FALSE);

	if (0 == ret) {
		MessageBox(GetHWND(), "设置Reader Time成功", "设置Reader Time", 0);
	}
	else {
		MessageBox(GetHWND(), GetErrorDescription(ret), "设置Reader Time", 0);
	}
}

void  CDuiFrameWnd::OnSetReaderBlueToothRet(int ret) {
	::PostMessage(GetHWND(), UM_READER_BLUE_TOOTH_RET, (WPARAM)ret, 0);
}

void  CDuiFrameWnd::OnSetReaderBlueToothRetMsg(int ret) {
	SetBusy(FALSE);

	if (0 == ret) {
		MessageBox(GetHWND(), "设置Reader 蓝牙成功", "设置Reader 蓝牙", 0);
	}
	else {
		MessageBox(GetHWND(), GetErrorDescription(ret), "设置Reader 蓝牙", 0);
	}
}

void  CDuiFrameWnd::OnSetReaderModeRet(int ret) {
	::PostMessage(GetHWND(), UM_READER_MODE_RET, (WPARAM)ret, 0);
}

void  CDuiFrameWnd::OnSetReaderModeRetMsg(int ret) {
	SetBusy(FALSE);

	if (0 == ret) {
		MessageBox(GetHWND(), "设置Reader Mode成功", "设置Reader Mode", 0);
	}
	else {
		MessageBox(GetHWND(), GetErrorDescription(ret), "设置Reader Mode", 0);
	}
}

void  CDuiFrameWnd::OnClearReaderRet(int ret) {
	::PostMessage(GetHWND(), UM_CLEAR_READER_RET, (WPARAM)ret, 0);
}

void  CDuiFrameWnd::OnClearReaderRetMsg(int ret) {
	SetBusy(FALSE);

	if (0 == ret) {
		MessageBox(GetHWND(), "清除Reader数据成功", "清除Reader数据", 0);
	}
	else {
		MessageBox(GetHWND(), GetErrorDescription(ret), "清除Reader数据", 0);
	}
}

void  CDuiFrameWnd::OnGetReaderDataRet(int ret, const std::vector<TempItem* > & vRet) {
	if (0 != ret) {
		::PostMessage(GetHWND(), UM_GET_READER_DATA_RET, (WPARAM)ret, 0);
	}
	else {
		std::vector<TempItem* > * pvRet = new std::vector<TempItem* >;

		std::vector<TempItem* >::const_iterator it;
		for (it = vRet.begin(); it != vRet.end(); it++) {
			TempItem* pItem = *it;
			TempItem* pNewItem = new TempItem;
			memcpy(pNewItem, pItem, sizeof(TempItem));
			pvRet->push_back(pNewItem);
		}

		::PostMessage(GetHWND(), UM_GET_READER_DATA_RET, (WPARAM)ret, (LPARAM)pvRet );
	}
}

void  CDuiFrameWnd::OnGetReaderDataRetMsg(int ret, const std::vector<TempItem* > * pvRet ) {
	SetBusy(FALSE);

	if (0 != ret) {
		MessageBox(GetHWND(), GetErrorDescription(ret), "获取Reader数据", 0);
		return;
	}

	assert(pvRet);
	std::vector<TempItem* >::const_iterator it;
	int i = 0;
	DuiLib::CDuiString  strText;
	char buf[8192];
	 
	for (it = pvRet->begin(); it != pvRet->end(); it++, i++) {
		TempItem* pItem = *it;

		DuiLib::CListTextElementUI* pListElement = new DuiLib::CListTextElementUI;
		m_lstData->Add(pListElement);

		strText.Format("%d", i + 1);
		pListElement->SetText(0, strText);

		pListElement->SetText(1, DateTime2Str(buf, sizeof(buf), &pItem->tTime) );

		pListElement->SetText(2, GetUid(buf, sizeof(buf), pItem->tTagId.abyUid, pItem->tTagId.dwLen, '-' ) );

		strText.Format("%.2f", pItem->dwTemp / 100.0f );
		pListElement->SetText(3, strText);

		pListElement->SetText(4, GetUid(buf, sizeof(buf), pItem->tCardId.abyUid, pItem->tCardId.dwLen, '-'));
	}

	MessageBox(GetHWND(), "获取Reader数据成功", "获取Reader数据", 0);
}





void  CDuiFrameWnd::OnSetReaderId() {
	DuiLib::CDuiString  strText;
	int  nId = 0;
	int  nVersion = 0;

	strText = m_edId->GetText();
	if (1 != sscanf(strText, "%d", &nId)) {
		::MessageBox(GetHWND(), "Reader ID格式不对，请输入数字(0~65535)", "设置Reader ID", 0);
		return;
	}

	if (nId < 0 || nId > 65535) {
		::MessageBox(GetHWND(), "Reader ID格式不对，请输入数字(0~65535)", "设置Reader ID", 0);
		return;
	}

	strText = m_edVersion->GetText();
	if (1 != sscanf(strText, "%d", &nVersion)) {
		::MessageBox(GetHWND(), "Reader 版本号格式不对，请输入数字(0~65535)", "设置Reader 版本号", 0);
		return;
	}

	if (nVersion < 0 || nVersion > 256) {
		::MessageBox(GetHWND(), "Reader 版本号格式不对，请输入数字(0~65535)", "设置Reader 版本号", 0);
		return;
	}
	     
	CBusiness::GetInstance()->SetReaderIdAsyn(nId, nVersion);
	SetBusy(TRUE);
}

void  CDuiFrameWnd::OnSetReaderTime() {
	time_t now = time(0);
	CBusiness::GetInstance()->SetReaderTimeAsyn(now);
	SetBusy(TRUE);
}

void  CDuiFrameWnd::OnSetReaderMode() {
	int nModeIndex = 0;

	if ( !m_opNormalMode->IsSelected() ) {
		nModeIndex = 1;
	}

	CBusiness::GetInstance()->SetReaderModeAsyn(nModeIndex);
	SetBusy(TRUE);
}

void  CDuiFrameWnd::OnClearReader() {
	CBusiness::GetInstance()->ClearReaderAsyn();
	SetBusy(TRUE);
}

void  CDuiFrameWnd::OnGetData() {
	m_lstData->RemoveAll();

	CBusiness::GetInstance()->GetReaderDataAsyn();
	SetBusy(TRUE);
}

void  CDuiFrameWnd::OnSetBluetooth() {
	BOOL b = m_opBlueTooth->IsSelected() ? TRUE : FALSE ;
	CBusiness::GetInstance()->SetReaderBluetoothAsyn(b);
	SetBusy(TRUE);
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

	CBusiness::GetInstance()->DeInit();
	delete CBusiness::GetInstance();
	LmnToolkits::ThreadManager::ReleaseInstance(); 

	return 0; 
}