// win32_1.cpp : 定义应用程序的入口点。
//

#ifdef _DEBUG
#include "vld.h"
#endif

#include "Windows.h"
#include "main.h"
#include "business.h"
#include "resource.h"
#include "MyProgress.h"
#include "AreaDlg.h"


class CDuiMenu : public WindowImplBase
{
protected:
	virtual ~CDuiMenu() {};        // 私有化析构函数，这样此对象只能通过new来生成，而不能直接定义变量。就保证了delete this不会出错
	CDuiString  m_strXMLPath;
	CControlUI * m_pOwner;

public:
	explicit CDuiMenu(LPCTSTR pszXMLPath, CControlUI * pOwner) : m_strXMLPath(pszXMLPath), m_pOwner(pOwner) {}
	virtual LPCTSTR    GetWindowClassName()const { return _T("CDuiMenu "); }
	virtual CDuiString GetSkinFolder() { return _T(""); }
	virtual CDuiString GetSkinFile() { return m_strXMLPath; }
	virtual void       OnFinalMessage(HWND hWnd) { delete this; }

	virtual LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		Close();
		bHandled = FALSE;
		return 0;
	}

	void Init(HWND hWndParent, POINT ptPos)
	{
		Create(hWndParent, _T("MenuWnd"), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
		::ClientToScreen(hWndParent, &ptPos);
		::SetWindowPos(*this, NULL, ptPos.x, ptPos.y, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
	}

	virtual void  Notify(TNotifyUI& msg) {
		if (msg.sType == "itemclick") {
			if (m_pOwner) {
				m_pOwner->GetManager()->SendNotify(m_pOwner, msg.pSender->GetName(), 0, 0, true);
			}
			return;
		}
		WindowImplBase::Notify(msg);
	}

	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		return __super::HandleMessage(uMsg, wParam, lParam);
	}
};




CDuiFrameWnd::CDuiFrameWnd() {
	m_bBusy = FALSE;
}

CDuiFrameWnd::~CDuiFrameWnd() {

}

void  CDuiFrameWnd::InitWindow() {
	g_hWnd = m_hWnd;
	PostMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0);

	m_tabs             = static_cast<DuiLib::CTabLayoutUI*>(m_PaintManager.FindControl("switch"));
	m_lblStatus        = static_cast<DuiLib::CLabelUI*>(m_PaintManager.FindControl("lblStatus"));
	m_reader_com_ports = static_cast<DuiLib::CComboUI*>(m_PaintManager.FindControl("cmbReaderComPort"));
	m_gw_com_ports     = static_cast<DuiLib::CComboUI*>(m_PaintManager.FindControl("cmGwComPort"));
	m_lstArea          = static_cast<DuiLib::CListUI*>(m_PaintManager.FindControl("lstArea"));
	m_cmbArea1         = static_cast<DuiLib::CComboUI*>(m_PaintManager.FindControl("cmbArea1"));
	m_cmbArea2         = static_cast<DuiLib::CComboUI*>(m_PaintManager.FindControl("cmbArea2"));
	m_btnSetting1      = static_cast<DuiLib::CButtonUI*>(m_PaintManager.FindControl("btnSetting_1"));
	m_btnSetting2      = static_cast<DuiLib::CButtonUI*>(m_PaintManager.FindControl("btnSetting_2"));
	m_btnSetting3      = static_cast<DuiLib::CButtonUI*>(m_PaintManager.FindControl("btnSetting_3"));
	m_edtBedNo         = static_cast<DuiLib::CEditUI*>(m_PaintManager.FindControl("edtBedNo"));
	m_edtSn            = static_cast<DuiLib::CEditUI*>(m_PaintManager.FindControl("edtSnNo"));
	m_progress         = (CMyProgress *)m_PaintManager.FindControl("progress");

	OnDeviceChanged(0, 0);

	DuiLib::CDuiString  strText;
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
	OnAreasChanged();

	WindowImplBase::InitWindow();
}

CControlUI * CDuiFrameWnd::CreateControl(LPCTSTR pstrClass) {
	if (0 == strcmp("reader_setting", pstrClass)) {
		DuiLib::CDialogBuilder builder;
		DuiLib::CControlUI * pUI = builder.Create(_T("reader_setting.xml"), (UINT)0, 0, &m_PaintManager);
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
	DuiLib::CDuiString strText;

	if (msg.sType == _T("selectchanged"))
	{
		if (name == _T("opn_reader_setting")) {
			m_tabs->SelectItem(0);
		}
		else if (name == _T("opn_gw_setting")) {
			m_tabs->SelectItem(1);
		}
		else if (name == _T("opn_area_setting")) {
			m_tabs->SelectItem(2);
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
	else if (msg.sType == "click") {
		if ( name == "btnSetting_3" ) {
			OnSettingGw();
		}
		else if (name == "btnSetting_1") {
			OnSettingReader1();
		}
		else if (name == "btnSetting_2") {
			OnSettingReader2();
		}
	}
	WindowImplBase::Notify(msg);
}

LRESULT CDuiFrameWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if (uMsg == WM_DEVICECHANGE) {
		OnDeviceChanged(wParam, lParam);
	}
	else if (uMsg == UM_SETTTING_GW_RET) {
		OnSettingGwRet(wParam, lParam);
	}
	else if (uMsg == UM_SETTTING_READER_RET) {
		OnSettingReaderRet1(wParam, lParam);
	}
	else if (uMsg == UM_SETTTING_SN_RET) {
		OnSettingSnRet(wParam, lParam);
	}
	return WindowImplBase::HandleMessage(uMsg,wParam,lParam);
}
 
void  CDuiFrameWnd::OnDeviceChanged(WPARAM wParm, LPARAM  lParam) {
	char szComPort[256] = {0};
	int nCount = GetCh340Count(szComPort, sizeof(szComPort));
	if ( nCount <= 0  ) {
		m_lblStatus->SetText("没有找到ch340串口");
	}
	else if (nCount > 1) {
		m_lblStatus->SetText("两个或两个以上的ch340串口");
	}
	else {
		m_lblStatus->SetText("");  
	}

	OnFreshComPort_Reader();
	OnFreshComPort_Gw();
}

void  CDuiFrameWnd::OnFreshComPort_Reader() {
	m_reader_com_ports->RemoveAll();

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
		m_reader_com_ports->Add(pElement);

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

	if (m_reader_com_ports->GetCount() > 0) {
		if (nFindeIndex >= 0) {
			m_reader_com_ports->SelectItem(nFindeIndex);
		}
		else {
			m_reader_com_ports->SelectItem(-1);
		}

	}
}

void  CDuiFrameWnd::OnFreshComPort_Gw() {
	m_gw_com_ports->RemoveAll();

	std::vector<std::string> vComPorts;
	EnumPortsWdm(vComPorts);

	BOOL  bFindReader = FALSE;
	int   nFindeIndex = -1;
	int   i = 0;
	std::vector<std::string>::iterator it;
	for (it = vComPorts.begin(); it != vComPorts.end(); it++, i++) {
		std::string & s = *it;

		CListLabelElementUI * pElement = new CListLabelElementUI;
		pElement->SetText(s.c_str());
		m_gw_com_ports->Add(pElement);

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

	if (m_gw_com_ports->GetCount() > 0) {
		if (nFindeIndex >= 0) {
			m_gw_com_ports->SelectItem(nFindeIndex);
		}
		else {
			m_gw_com_ports->SelectItem(-1);
		}
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
		MessageBox(GetHWND(), "没有选中要修改的信道", "错误", 0);
		return;
	}

	CListTextElementUI* pItem = (CListTextElementUI*)m_lstArea->GetItemAt(nSel);

	CAreaWnd * pAreaDlg = new CAreaWnd(FALSE);
	STRNCPY(pAreaDlg->m_tArea.szAreaName, pItem->GetText(1), sizeof(pAreaDlg->m_tArea.szAreaName));
	sscanf(pItem->GetText(0), "%lu", &pAreaDlg->m_tArea.dwAreaNo);
	pAreaDlg->Create(this->m_hWnd, _T("修改信道"), UI_WNDSTYLE_FRAME | WS_POPUP, NULL, 0, 0, 0, 0);
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
		MessageBox(GetHWND(), "没有选中要删除的信道", "错误", 0);
		return;
	}

	CListTextElementUI* pItem = (CListTextElementUI*)m_lstArea->GetItemAt(nSel);
	DWORD dwAreaId = pItem->GetTag();

	if (IDYES == MessageBox(GetHWND(), "确定要删除吗？", "删除", MB_YESNO | MB_DEFBUTTON2)) {
		std::vector<TArea *>::iterator it;
		for (it = g_vArea.begin(); it != g_vArea.end(); ++it) {
			TArea * pArea = *it;
			if (pArea->dwAreaNo == dwAreaId) {
				g_vArea.erase(it);
				delete pArea;
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

	m_cmbArea1->RemoveAll();
	m_cmbArea2->RemoveAll();

	for (it = g_vArea.begin(); it != g_vArea.end(); ++it) {
		TArea * pArea = *it;

		CListLabelElementUI * pElement = new CListLabelElementUI;
		strText.Format("%s (编号：%lu)", pArea->szAreaName, pArea->dwAreaNo);
		pElement->SetText(strText);
		pElement->SetTag(pArea->dwAreaNo);
		m_cmbArea1->Add(pElement);

		pElement = new CListLabelElementUI;
		pElement->SetText(strText);
		pElement->SetTag(pArea->dwAreaNo);
		m_cmbArea2->Add(pElement);
	}

	if (m_cmbArea1->GetCount() > 0) {
		m_cmbArea1->SelectItem(0);
	}

	if (m_cmbArea2->GetCount() > 0) {
		m_cmbArea2->SelectItem(0);
	}
}

void  CDuiFrameWnd::OnSettingGw() {
	if (m_bBusy) {
		MessageBox(GetHWND(), "正忙着配置或查询，请稍候", "忙碌", 0);
		return;
	}

	int nAreaNo = 0;
	int nPort = 0;
	DuiLib::CDuiString  strText;

	if (m_gw_com_ports->GetCurSel() < 0) {
		MessageBox(GetHWND(), "没有选中串口", "错误", 0);
		return;
	}
	nPort = m_gw_com_ports->GetItemAt(m_gw_com_ports->GetCurSel())->GetTag();

	int nSel = m_cmbArea2->GetCurSel();
	if (nSel < 0) {
		MessageBox(GetHWND(), "没有信道号。请先编辑信道", "错误", 0);
		return;
	}

	nAreaNo = m_cmbArea2->GetItemAt(nSel)->GetTag();

	CBusiness::GetInstance()->SettingGwAsyn(nAreaNo, nPort);
	SetBusy(TRUE);
}

void  CDuiFrameWnd::SetBusy(BOOL bBusy) {
	if (bBusy) {
		m_progress->SetVisible(true);
		m_progress->Start();
		m_bBusy = TRUE;

		m_btnSetting3->SetEnabled(false);
		m_btnSetting2->SetEnabled(false);
		m_btnSetting1->SetEnabled(false);
	}
	else {
		m_progress->Stop();
		m_progress->SetVisible(false);
		m_bBusy = FALSE;

		m_btnSetting3->SetEnabled(true);
		m_btnSetting2->SetEnabled(true);
		m_btnSetting1->SetEnabled(true);
	}
}

void  CDuiFrameWnd::OnSettingGwRet(WPARAM wParm, LPARAM  lParam) {
	int ret = (int)wParm;
	SetBusy(FALSE);

	if (ret != 0) {
		MessageBox(GetHWND(), "设置基站的过程中出错!", "错误", 0);
		return;
	}
	else {
		MessageBox(GetHWND(), "设置基站成功", "成功", 0);
	}
}

void  CDuiFrameWnd::OnSettingReader1() {
	if (m_bBusy) {
		MessageBox(GetHWND(), "正忙着配置或查询，请稍候", "忙碌", 0);
		return;
	}

	int nAreaNo = 0;
	int nBedNo = 0;
	int nPort = 0;
	DuiLib::CDuiString  strText;

	if (m_reader_com_ports->GetCurSel() < 0) {
		MessageBox(GetHWND(), "没有选中串口", "错误", 0);
		return;
	}
	nPort = m_reader_com_ports->GetItemAt(m_reader_com_ports->GetCurSel())->GetTag();

	int nSel = m_cmbArea1->GetCurSel();
	if (nSel < 0) {
		MessageBox(GetHWND(), "没有选中信道。请先编辑信道", "错误", 0);
		return;
	}

	nAreaNo = m_cmbArea1->GetItemAt(nSel)->GetTag();

	strText = m_edtBedNo->GetText();
	if (1 != sscanf(strText, "%d", &nBedNo)) {
		MessageBox(GetHWND(), "请输入地址", "错误", 0);
		return;
	}

	if (nBedNo <= 0) {
		MessageBox(GetHWND(), "地址必须是正整数", "错误", 0);
		return;
	}

	if (nBedNo > 200) {
		MessageBox(GetHWND(), "地址的范围是1到200", "错误", 0);
		return;
	}

	CBusiness::GetInstance()->SettingReaderAsyn(nAreaNo, nBedNo, nPort);

	SetBusy(TRUE);
}

void  CDuiFrameWnd::OnSettingReader2() {
	if (m_bBusy) {
		MessageBox(GetHWND(), "正忙着配置或查询，请稍候", "忙碌", 0);
		return;
	}

	int nSn = 0;
	int nPort = 0;
	DuiLib::CDuiString  strText;

	if (m_reader_com_ports->GetCurSel() < 0) {
		MessageBox(GetHWND(), "没有选中串口", "错误", 0);
		return;
	}
	nPort = m_reader_com_ports->GetItemAt(m_reader_com_ports->GetCurSel())->GetTag();

	strText = m_edtSn->GetText();
	if (1 != sscanf(strText, "%d", &nSn)) {
		MessageBox(GetHWND(), "请输入SN码", "错误", 0);
		return;
	}

	if (nSn <= 0) {
		MessageBox(GetHWND(), "SN码必须是正整数", "错误", 0);
		return;
	}

	if (nSn > 10000000) {
		MessageBox(GetHWND(), "SN码的范围是1到10000000", "错误", 0);
		return;
	}

	CBusiness::GetInstance()->SettingSnAsyn(nSn, nPort);

	SetBusy(TRUE);
}

void  CDuiFrameWnd::OnSettingReaderRet1(WPARAM wParm, LPARAM  lParam) {
	int ret = (int)wParm;
	SetBusy(FALSE);

	if (ret != 0) {
		MessageBox(GetHWND(), "设置Reader的过程中出错!", "错误", 0);
		return;
	}
	else {
		MessageBox(GetHWND(), "设置Reader成功", "成功", 0);
	}
}

void  CDuiFrameWnd::OnSettingSnRet(WPARAM wParm, LPARAM  lParam) {
	int ret = (int)wParm;
	SetBusy(FALSE);

	if (ret != 0) {
		MessageBox(GetHWND(), "设置Reader SN码的过程中出错!", "错误", 0);
		return;
	}
	else {
		MessageBox(GetHWND(), "设置Reader SN码成功", "成功", 0);
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


