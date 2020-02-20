// win32_1.cpp : 定义应用程序的入口点。
//

#ifdef _DEBUG
#include "vld.h"
#endif

#include "Windows.h"
#include "main.h"
#include "business.h"
#include "resource.h"
#include "MenuUI.h"
#include "AreaDlg.h"

CDuiFrameWnd::CDuiFrameWnd() {

}   

CDuiFrameWnd::~CDuiFrameWnd() { 
	  
}

void  CDuiFrameWnd::InitWindow() {
	g_data.m_hWnd = m_hWnd;

	m_tabs = static_cast<DuiLib::CTabLayoutUI*>(m_PaintManager.FindControl("switch"));

	// tab 1
	m_cmbHandReaderCom = static_cast<DuiLib::CComboUI*>(m_PaintManager.FindControl("cmbComPort_1"));
	m_cmbHandReaderAddr = static_cast<DuiLib::CComboUI*>(m_PaintManager.FindControl("cmbAddr_1"));
	m_cmbHandReaderChannel = static_cast<DuiLib::CComboUI*>(m_PaintManager.FindControl("cmbChannel_1"));
	m_btnHandReader_1 = static_cast<DuiLib::CButtonUI*>(m_PaintManager.FindControl("btnSetting_1"));
	m_btnHandReader_2 = static_cast<DuiLib::CButtonUI*>(m_PaintManager.FindControl("btnSetting_2"));
	m_edHandReaderSn = static_cast<DuiLib::CEditUI*>(m_PaintManager.FindControl("edtSN_1"));

	// tab 2
	m_cmbSurgencyReaderCom = static_cast<DuiLib::CComboUI*>(m_PaintManager.FindControl("cmbComPort_2"));
	m_cmbArea_1 = static_cast<DuiLib::CComboUI*>(m_PaintManager.FindControl("cmbArea_1"));
	m_cmbBed_1 = static_cast<DuiLib::CComboUI*>(m_PaintManager.FindControl("cmbBed_1"));
	m_cmbBed_2 = static_cast<DuiLib::CComboUI*>(m_PaintManager.FindControl("cmbBed_2"));
	m_edSurgencyReaderSn = static_cast<DuiLib::CEditUI*>(m_PaintManager.FindControl("edtSN_2"));
	m_btnSurgencyReader_1 = static_cast<DuiLib::CButtonUI*>(m_PaintManager.FindControl("btnSetting_3"));
	m_btnSurgencyReader_2 = static_cast<DuiLib::CButtonUI*>(m_PaintManager.FindControl("btnSetting_4"));

	// tab 3
	m_cmbReceiverCom = static_cast<DuiLib::CComboUI*>(m_PaintManager.FindControl("cmbComPort_3"));
	m_cmbArea_2 = static_cast<DuiLib::CComboUI*>(m_PaintManager.FindControl("cmbArea_2"));
	m_btnReceiver_1 = static_cast<DuiLib::CButtonUI*>(m_PaintManager.FindControl("btnSetting_5"));
	m_btnReceiver_2 = static_cast<DuiLib::CButtonUI*>(m_PaintManager.FindControl("btnSetting_6"));
	m_cmbChannel_a = static_cast<DuiLib::CComboUI*>(m_PaintManager.FindControl("cmbChannel_a"));
	//m_edtChannel_b = static_cast<DuiLib::CEditUI*>(m_PaintManager.FindControl("edtChannel_b"));
	//m_edtChannel_c = static_cast<DuiLib::CEditUI*>(m_PaintManager.FindControl("edtChannel_c"));

	// tab 4
	m_lstArea = static_cast<DuiLib::CListUI*>(m_PaintManager.FindControl("lstArea"));

	// progress
	m_progress = static_cast<CMyProgress*>(m_PaintManager.FindControl("progress"));

	CDuiString  strText;

#ifndef RESTRICTED_FLAG
	for ( int i = 1; i <= 255; i++ ) {
		strText.Format("%d", i);
		CListLabelElementUI * pElement = new CListLabelElementUI;
		pElement->SetText(strText);
		pElement->SetTag(i);
		m_cmbHandReaderAddr->Add(pElement); 
	}

	for (int i = 1; i <= 91; i++ ) {
		strText.Format("%d", i);
		CListLabelElementUI * pElement = new CListLabelElementUI;
		pElement->SetText(strText);
		pElement->SetTag(i);
		m_cmbHandReaderChannel->Add(pElement);
	}

#else
	for (int i = 1; i <= 96; i++) {
		strText.Format("%d", i);
		CListLabelElementUI * pElement = new CListLabelElementUI;
		pElement->SetText(strText);
		pElement->SetTag(i);
		m_cmbHandReaderAddr->Add(pElement);
	}

	for (int i = 1; i <= 91; i += 10 ) {
		int k = i;

#ifdef REMOVE_11_CHANNEL_FLAG
		if ( k == 11 ) {
			k = 12;
		}
#endif
		strText.Format("%d", k);
		CListLabelElementUI * pElement = new CListLabelElementUI;
		pElement->SetText(strText);
		pElement->SetTag(k);
		m_cmbHandReaderChannel->Add(pElement);

	}
#endif

	m_cmbHandReaderAddr->SelectItem(0);  
	m_cmbHandReaderChannel->SelectItem(0);     

	for (int i = 0; i < MAX_SURGENCY_BEDS_CNT; i++) {
		strText.Format("%d", i+1);
		CListLabelElementUI * pElement = new CListLabelElementUI;
		pElement->SetText(strText);
		pElement->SetTag(i);
		m_cmbBed_1->Add(pElement);		
	}
	m_cmbBed_1->SelectItem(0);

	for ( int i = 0; i < MAX_READERS_PER_BED; i++ ) {
		strText.Format("%c", 'A'+i);
		CListLabelElementUI * pElement = new CListLabelElementUI;
		pElement->SetText(strText);
		pElement->SetTag(i);
		m_cmbBed_2->Add(pElement);
	}
	m_cmbBed_2->SelectItem(0);

	std::vector<TArea *>::iterator it;
	for (it = g_vArea.begin(); it != g_vArea.end(); ++it) {
		TArea * pArea = *it;

		CListTextElementUI* pItem = new CListTextElementUI;
		m_lstArea->Add(pItem);

		strText.Format("%lu", pArea->dwAreaNo);
		pItem->SetText(0, strText);
		pItem->SetText(1, pArea->szAreaName);
		pItem->SetTag(pArea->dwAreaNo);
	}

	for (int i = 1; i <= 81; i += 20) {
		strText.Format("%d", i);
		CListLabelElementUI * pElement = new CListLabelElementUI;
		pElement->SetText(strText);
		pElement->SetTag(i);
		m_cmbChannel_a->Add(pElement);
	}
	m_cmbChannel_a->SelectItem(0);

	m_tabs->SelectItem(1);                

	OnDeviceChanged(0, 0);  
	OnAreasChanged();  

	WindowImplBase::InitWindow();     
}

CControlUI * CDuiFrameWnd::CreateControl(LPCTSTR pstrClass) {
	if (0 == strcmp("reader_setting", pstrClass)) {
		DuiLib::CDialogBuilder builder;
		DuiLib::CControlUI * pUI = builder.Create(_T("reader_setting.xml"), (UINT)0, 0, &m_PaintManager);
		return pUI;
	}
	else if (0 == strcmp("reader_setting_1", pstrClass)) {
		DuiLib::CDialogBuilder builder;
		DuiLib::CControlUI * pUI = builder.Create(_T("reader_setting_1.xml"), (UINT)0, 0, &m_PaintManager);
		return pUI; 
	}
	else if (0 == strcmp("gw_setting", pstrClass)) {
		DuiLib::CDialogBuilder builder;
		DuiLib::CControlUI * pUI = builder.Create(_T("gw_setting.xml"), (UINT)0, 0, &m_PaintManager);
		return pUI;
	}
	else if (0 == strcmp("area_setting", pstrClass)) {
		DuiLib::CDialogBuilder builder;
		DuiLib::CControlUI * pUI = builder.Create(_T("area_setting.xml"), (UINT)0, 0, &m_PaintManager);
		return pUI;       
	}
	else if (0 == strcmp(pstrClass, "MyProgress")) {
		return new CMyProgress(&m_PaintManager, "progress_fore.png");
	}
	return WindowImplBase::CreateControl(pstrClass);
}

void CDuiFrameWnd::Notify(TNotifyUI& msg) {
	DuiLib::CDuiString name = msg.pSender->GetName();

	if (msg.sType == _T("selectchanged"))
	{
		if (name == _T("opn_reader_setting")) {
			m_tabs->SelectItem(2);
		} 
		else if (name == _T("opn_reader_setting_1")) {
			m_tabs->SelectItem(3);
		}
		else if (name == _T("opn_gw_setting")) {
			m_tabs->SelectItem(1);
		}
		else if (name == _T("opn_area_setting")) {
			m_tabs->SelectItem(0);   
		}
	}
	else if (msg.sType == "click") {
		// 设置手持Reader
		if (name == "btnSetting_1") {
			OnSettingHandReader();
		}
		// 设置手持SN
		else if (name == "btnSetting_2") {
			OnSetHandReaderSn();
		}
		else if (name == "btnSetting_3") {
			OnSetSurgencyReader();
		}
		else if (name == "btnSetting_4") {
			OnSetSurgencyReaderSn();
		}
		else if (name == "btnSetting_5") {
			OnSetReceiverArea();
		}
		else if (name == "btnSetting_6") {
			OnSetReceiverChannel();
		}
	}
	else if (msg.sType == "menu")
	{
		if (name == "lstArea") {
			OnAreaListMenu(msg);
		}
	}
	else if (msg.sType == "menu_add_area") {
		OnAddArea();
	}
	else if (msg.sType == "menu_modify_area") {
		OnModifyArea();
	}
	else if (msg.sType == "menu_delete_area") {
		OnDeleteArea();
	}
	else if (msg.sType == "itemselect") {
		if (name == "cmbChannel_a") {
			OnChannelAChanged();
		}
	}
	WindowImplBase::Notify(msg);
}

LRESULT CDuiFrameWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if (uMsg == WM_DEVICECHANGE) {
		OnDeviceChanged(wParam, lParam);
	}
	else if (uMsg == UM_SETTING_HAND_READER_RET) {
		OnHandReaderSettingRet(wParam, lParam);
	}
	else if (uMsg == UM_SET_HAND_READER_SN_RET) {
		OnSetHandReaderSnRet(wParam, lParam);
	}
	else if (uMsg == UM_SET_RECIEVER_CHANNEL_RET) {
		OnSetReceiverChannelRet(wParam, lParam);
	}
	else if (uMsg == UM_SET_SURGENCY_READER_RET) {
		OnSetSurgencyReaderRet(wParam, lParam);
	}
	else if (uMsg == UM_SET_RECIEVER_AREA_RET) {
		OnSetReceiverAreaRet(wParam, lParam);
	}
	else if (uMsg == UM_SET_SURGENCY_READER_SN_RET) {
		OnSetSurgencyReaderSnRet(wParam, lParam);
	}
	return WindowImplBase::HandleMessage(uMsg,wParam,lParam);
}
             
void  CDuiFrameWnd::OnDeviceChanged(WPARAM wParm, LPARAM  lParam) {
	std::vector<std::string> vComPorts;
	EnumPortsWdm(vComPorts);

	SetComboCom(m_cmbHandReaderCom, vComPorts);
	SetComboCom(m_cmbSurgencyReaderCom, vComPorts);
	SetComboCom(m_cmbReceiverCom, vComPorts);                                 	
}

void  CDuiFrameWnd::SetBusy(BOOL bBusy /*= TRUE*/) {
	if (bBusy) {
		m_btnHandReader_1->SetEnabled(false);
		m_btnHandReader_2->SetEnabled(false);
		m_btnSurgencyReader_1->SetEnabled(false);
		m_btnSurgencyReader_2->SetEnabled(false);
		m_btnReceiver_1->SetEnabled(false);
		m_btnReceiver_2->SetEnabled(false);
		m_progress->SetVisible(true);
		m_progress->Start();
	}
	else {
		m_btnHandReader_1->SetEnabled(true);
		m_btnHandReader_2->SetEnabled(true);		
		m_btnSurgencyReader_1->SetEnabled(true);
		m_btnSurgencyReader_2->SetEnabled(true);
		m_btnReceiver_1->SetEnabled(true);
		m_btnReceiver_2->SetEnabled(true);
		m_progress->Stop();
		m_progress->SetVisible(false);
	}
}

void  CDuiFrameWnd::OnSettingHandReader() {
	int nSel = m_cmbHandReaderCom->GetCurSel();
	if (nSel < 0) {
		MessageBox(GetHWND(), "没有选中串口", "错误", 0);
		return;
	}

	CControlUI * pCtl = m_cmbHandReaderCom->GetItemAt(nSel);
	int nCom = (int)pCtl->GetTag();

	nSel = m_cmbHandReaderChannel->GetCurSel();
	pCtl = m_cmbHandReaderChannel->GetItemAt(nSel);
	BYTE byChannel = (BYTE)pCtl->GetTag();

	nSel = m_cmbHandReaderAddr->GetCurSel();
	pCtl = m_cmbHandReaderAddr->GetItemAt(nSel);
	BYTE byAddr = (BYTE)pCtl->GetTag();

	CBusiness::GetInstance()->SetHandReaderAsyn(byChannel, byAddr, nCom);
	SetBusy();
}

void  CDuiFrameWnd::OnHandReaderSettingRet(WPARAM wParm, LPARAM  lParam) {
	SetBusy(FALSE);

	if (0 == wParm) {
		MessageBox(GetHWND(), "设置成功", "设置", 0);
	}
	else {
		MessageBox(GetHWND(), "设置失败", "设置", 0);
	}
}

void  CDuiFrameWnd::OnSetHandReaderSn() {
	int nSel = m_cmbHandReaderCom->GetCurSel();
	if (nSel < 0) {
		MessageBox(GetHWND(), "没有选中串口", "错误", 0);
		return;
	}
	CControlUI * pCtl = m_cmbHandReaderCom->GetItemAt(nSel);
	int nCom = (int)pCtl->GetTag();

	CDuiString strText = m_edHandReaderSn->GetText();
	if (strText.GetLength() == 0) {
		MessageBox(GetHWND(), "请输入SN", "错误", 0);
		return;
	}

	DWORD  dwSn = 0;
	int ret = sscanf(strText, "%lu", &dwSn);
	if (0 == ret) {
		MessageBox(GetHWND(), "SN必须为数字", "错误", 0);
		return;
	}
	
	CBusiness::GetInstance()->SetHandReaderSnAsyn(dwSn, nCom);
	SetBusy();
}

void  CDuiFrameWnd::OnSetHandReaderSnRet(WPARAM wParm, LPARAM  lParam) {
	SetBusy(FALSE);

	if (0 == wParm) {
		MessageBox(GetHWND(), "设置SN成功", "设置", 0);
	}
	else {
		MessageBox(GetHWND(), "设置SN失败", "设置", 0);
	}
}

void  CDuiFrameWnd::OnAreaListMenu(TNotifyUI& msg) {
	POINT pt = { msg.ptMouse.x, msg.ptMouse.y };
	CDuiMenu *pMenu = new CDuiMenu(_T("menu_area.xml"), msg.pSender);
	pMenu->Init(*this, pt);
	pMenu->ShowWindow(TRUE);
}

void  CDuiFrameWnd::OnAddArea() {
	CAreaWnd * pAreaDlg = new CAreaWnd();
	pAreaDlg->Create(this->m_hWnd, _T("添加病区"), UI_WNDSTYLE_FRAME | WS_POPUP, NULL, 0, 0, 0, 0);
	pAreaDlg->CenterWindow();

	// 如果是确定按钮后关闭
	if (0 == pAreaDlg->ShowModal()) {
		DuiLib::CDuiString  strText;
		CListTextElementUI* pItem = new CListTextElementUI;
		m_lstArea->Add(pItem);

		strText.Format("%lu", pAreaDlg->m_tArea.dwAreaNo);
		pItem->SetText(0, strText);
		pItem->SetText(1, pAreaDlg->m_tArea.szAreaName);
		pItem->SetTag(pAreaDlg->m_tArea.dwAreaNo);

		OnAreasChanged();
	}
	delete pAreaDlg;
}

void  CDuiFrameWnd::OnModifyArea() {
	int nSel = m_lstArea->GetCurSel();
	if (nSel < 0) {
		MessageBox(GetHWND(), "没有选中要修改的病区", "错误", 0);
		return;
	}

	CListTextElementUI* pItem = (CListTextElementUI*)m_lstArea->GetItemAt(nSel);

	CAreaWnd * pAreaDlg = new CAreaWnd(FALSE);
	STRNCPY(pAreaDlg->m_tArea.szAreaName, pItem->GetText(1), sizeof(pAreaDlg->m_tArea.szAreaName));
	sscanf(pItem->GetText(0), "%lu", &pAreaDlg->m_tArea.dwAreaNo);
	pAreaDlg->Create(this->m_hWnd, _T("修改病区"), UI_WNDSTYLE_FRAME | WS_POPUP, NULL, 0, 0, 0, 0);
	pAreaDlg->CenterWindow();

	// 如果是确定按钮后关闭
	if (0 == pAreaDlg->ShowModal()) {
		pItem->SetText(1, pAreaDlg->m_tArea.szAreaName);

		OnAreasChanged();
	}

	delete pAreaDlg;
}

void  CDuiFrameWnd::OnDeleteArea() {
	int nSel = m_lstArea->GetCurSel();
	if (nSel < 0) {
		MessageBox(GetHWND(), "没有选中要删除的病区", "错误", 0);
		return;
	}

	CListTextElementUI* pItem = (CListTextElementUI*)m_lstArea->GetItemAt(nSel);
	DWORD dwAreaId = pItem->GetTag();

	if (IDYES == MessageBox(GetHWND(), "确定要删除吗？", "删除", MB_YESNO | MB_DEFBUTTON2)) {
		std::vector<TArea *>::iterator it;
		for (it = g_vArea.begin(); it != g_vArea.end(); ++it) {
			TArea * pArea = *it;
			if (pArea->dwAreaNo == dwAreaId) {
				delete pArea;
				g_vArea.erase(it);
				break;
			}
		}
		SaveAreas();
		m_lstArea->RemoveAt(nSel);

		OnAreasChanged();
	}
}

void  CDuiFrameWnd::OnAreasChanged() {
	DuiLib::CDuiString  strText;
	std::vector<TArea *>::iterator it;

	DuiLib::CComboUI *cmbAreas[2] = { m_cmbArea_1 , m_cmbArea_2 };

	for (int i = 0; i < 2; i++) {
		cmbAreas[i]->RemoveAll();
	}

	for (it = g_vArea.begin(); it != g_vArea.end(); ++it) {
		TArea * pArea = *it;
		CListLabelElementUI * pElement = 0;

		for (int i = 0; i < 2; i++) {
			pElement = new CListLabelElementUI;
			strText.Format("%s (编号：%lu)", pArea->szAreaName, pArea->dwAreaNo);
			pElement->SetText(strText);
			pElement->SetTag(pArea->dwAreaNo);
			cmbAreas[i]->Add(pElement);
		}
	}

	for (int i = 0; i < 2; i++) {
		if (cmbAreas[i]->GetCount() > 0) {
			cmbAreas[i]->SelectItem(0);
		}
	}
}

void  CDuiFrameWnd::OnChannelAChanged() {
	int nSel  = m_cmbChannel_a->GetCurSel();
	assert(nSel >= 0);

	CControlUI * pCtl = m_cmbChannel_a->GetItemAt(nSel);
	int nChannel = pCtl->GetTag();

	CDuiString  strText;

#ifdef REMOVE_11_CHANNEL_FLAG
	if (nChannel + 10 == 11) {
		strText.Format("%d", nChannel + 10 + 1);
		//m_edtChannel_b->SetText(strText);
	}
	else {
		strText.Format("%d", nChannel + 10);
		//m_edtChannel_b->SetText(strText);
	}	
#else
	strText.Format("%d", nChannel + 10);
	m_edtChannel_b->SetText(strText);
#endif

	//strText.Format("%d", nChannel + 20);
	//m_edtChannel_c->SetText(strText);
}

void  CDuiFrameWnd::OnSetReceiverChannel() {
	int nSel = m_cmbReceiverCom->GetCurSel();
	if (nSel < 0) {
		MessageBox(GetHWND(), "没有选中串口", "错误", 0);
		return;
	}
	CControlUI * pCtl = m_cmbReceiverCom->GetItemAt(nSel);
	int nCom = (int)pCtl->GetTag();

	nSel = m_cmbChannel_a->GetCurSel();
	assert(nSel >= 0);
	pCtl = m_cmbChannel_a->GetItemAt(nSel);
	BYTE byChannel_a = (BYTE)pCtl->GetTag();

	CDuiString strText;
	int nTmp = 0;

#ifdef REMOVE_11_CHANNEL_FLAG
	//strText = m_edtChannel_b->GetText();
	sscanf_s( strText, "%d", &nTmp );
	BYTE byChannel_b = (BYTE)nTmp;

	//strText = m_edtChannel_c->GetText();
	//sscanf_s(strText, "%d", &nTmp);
	//BYTE byChannel_c = (BYTE)nTmp;
	BYTE byChannel_c = 0;
#else
	BYTE byChannel_b = byChannel_a + 10;
	BYTE byChannel_c = byChannel_a + 20;
#endif

	CBusiness::GetInstance()->SetReceriverChannelAsyn( byChannel_a, byChannel_b, byChannel_c, nCom);
	SetBusy();
}

void  CDuiFrameWnd::OnSetReceiverChannelRet(WPARAM wParm, LPARAM  lParam) {
	SetBusy(FALSE);

	if (0 == wParm) {
		MessageBox(GetHWND(), "设置非连续测温模块成功", "设置", 0);
	}
	else {
		MessageBox(GetHWND(), "设置非连续测温模块失败", "设置", 0);
	}
}

void  CDuiFrameWnd::OnSetSurgencyReader() {
	int nSel = m_cmbSurgencyReaderCom->GetCurSel();
	if (nSel < 0) {
		MessageBox(GetHWND(), "没有选中串口", "错误", 0);
		return;
	}
	CControlUI * pCtl = m_cmbSurgencyReaderCom->GetItemAt(nSel);
	int nCom = (int)pCtl->GetTag();

	nSel = m_cmbArea_1->GetCurSel();
	if (nSel < 0) {
		MessageBox(GetHWND(), "没有选中病区", "错误", 0);
		return;
	}
	pCtl = m_cmbArea_1->GetItemAt(nSel);
	BYTE byAreaNo = (BYTE)pCtl->GetTag();

	int nSel_1 = m_cmbBed_1->GetCurSel();
	int nSel_2 = m_cmbBed_2->GetCurSel();

	if (nSel_1 < 0 || nSel_2 < 0) {
		MessageBox(GetHWND(), "请选择床位号，例如2A, 3C等", "错误", 0);
		return;
	}

	WORD wBedNo = (WORD)(nSel_1 * MAX_READERS_PER_BED + nSel_2 + 1);

	CBusiness::GetInstance()->SetSurgencyReaderAsyn(wBedNo, byAreaNo, nCom);

	SetBusy();
}

void  CDuiFrameWnd::OnSetSurgencyReaderRet(WPARAM wParm, LPARAM  lParam) {
	SetBusy(FALSE);

	if (0 == wParm) {
		MessageBox(GetHWND(), "设置连续测温读卡器成功", "设置", 0);
	}
	else {
		MessageBox(GetHWND(), "设置连续测温读卡器失败", "设置", 0);
	}
}

void  CDuiFrameWnd::OnSetReceiverArea() {
	int nSel = m_cmbReceiverCom->GetCurSel();
	if (nSel < 0) {
		MessageBox(GetHWND(), "没有选中串口", "错误", 0);
		return;
	}
	CControlUI * pCtl = m_cmbReceiverCom->GetItemAt(nSel);
	int nCom = (int)pCtl->GetTag();

	nSel = m_cmbArea_2->GetCurSel();
	if (nSel < 0) {
		MessageBox(GetHWND(), "没有选中病区", "错误", 0);
		return;
	}
	pCtl = m_cmbArea_2->GetItemAt(nSel);
	BYTE byAreaNo = (BYTE)pCtl->GetTag();


	CBusiness::GetInstance()->SetReceriverAreaAsyn(byAreaNo, nCom);

	SetBusy();
}

void  CDuiFrameWnd::OnSetReceiverAreaRet(WPARAM wParm, LPARAM  lParam) {
	SetBusy(FALSE);

	if (0 == wParm) {
		MessageBox(GetHWND(), "设置连续测温模块成功", "设置", 0);
	}
	else {
		MessageBox(GetHWND(), "设置连续测温模块失败", "设置", 0);
	}
}

void  CDuiFrameWnd::OnSetSurgencyReaderSn() {
	int nSel = m_cmbSurgencyReaderCom->GetCurSel();
	if (nSel < 0) {
		MessageBox(GetHWND(), "没有选中串口", "错误", 0);
		return;
	}
	CControlUI * pCtl = m_cmbSurgencyReaderCom->GetItemAt(nSel);
	int nCom = (int)pCtl->GetTag();

	CDuiString strText = m_edSurgencyReaderSn->GetText();
	if (strText.GetLength() == 0) {
		MessageBox(GetHWND(), "请输入SN", "错误", 0);
		return;
	}

	DWORD  dwSn = 0;
	int ret = sscanf(strText, "%lu", &dwSn);
	if (0 == ret) {
		MessageBox(GetHWND(), "SN必须为数字", "错误", 0);
		return;
	}

	CBusiness::GetInstance()->SetSurgencyReaderSnAsyn(dwSn, nCom);
	SetBusy();
}

void  CDuiFrameWnd::OnSetSurgencyReaderSnRet(WPARAM wParm, LPARAM  lParam) {
	SetBusy(FALSE);

	if (0 == wParm) {
		MessageBox(GetHWND(), "设置连续测温读卡器SN成功", "设置", 0);
	}
	else {
		MessageBox(GetHWND(), "设置连续测温读卡器SN失败", "设置", 0);
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


