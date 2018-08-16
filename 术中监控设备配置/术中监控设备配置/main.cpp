// win32_1.cpp : 定义应用程序的入口点。
//

#ifdef _DEBUG
#include "vld.h"
#endif

#include "Windows.h"
#include "main.h"
#include "business.h"
#include "resource.h"
#include <locale.h>
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
	g_hWnd = GetHWND();
	this->PostMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0);

	m_tabs = static_cast<DuiLib::CTabLayoutUI*>(m_PaintManager.FindControl("switch"));
	m_reader_com_ports = static_cast<DuiLib::CComboUI*>(m_PaintManager.FindControl("cmbReaderComPort"));
	m_gw_com_ports = static_cast<DuiLib::CComboUI*>(m_PaintManager.FindControl("cmGwComPort"));

	m_cmbArea1 = static_cast<DuiLib::CComboUI*>(m_PaintManager.FindControl("cmbArea1"));
	m_edtBedNo = static_cast<DuiLib::CEditUI*>(m_PaintManager.FindControl("edtBedNo"));
	m_cmbArea2 = static_cast<DuiLib::CComboUI*>(m_PaintManager.FindControl("cmbArea2"));

	m_progress = (CMyProgress *)m_PaintManager.FindControl("progress");
	m_btnSetting1 = static_cast<DuiLib::CButtonUI*>(m_PaintManager.FindControl("btnSetting1"));
	m_btnSetting2 = static_cast<DuiLib::CButtonUI*>(m_PaintManager.FindControl("btnSetting2"));
	m_btnQuery2 = static_cast<DuiLib::CButtonUI*>(m_PaintManager.FindControl("btnQuery2"));

	m_edtQuery2Ret = static_cast<DuiLib::CEditUI*>(m_PaintManager.FindControl("edtAreaNo_Ret2"));
	m_lstArea = static_cast<DuiLib::CListUI*>(m_PaintManager.FindControl("lstArea"));

	OnFreshComPort_Reader();
	OnFreshComPort_Gw();    

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
		if (0 == m_tabs) {
			return;
		}

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
	else if ( msg.sType == "click" ) {
		if (name == "btnFresh1") {
			OnFreshComPort_Reader();
		}
		else if (name == "btnFresh2") {
			OnFreshComPort_Gw();
		}
		else if (name == "btnSetting1") {
			OnSettingReader();
		}
		else if (name == "btnSetting2" ) {
			OnSettingGw();
		}
		else if (name == "btnQuery2") {
			OnQueryGw();
		}
	}
	else if ( msg.sType == "menu" )
	{ 
		if (name == "lstArea" ) {
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
	WindowImplBase::Notify(msg);
}

LRESULT CDuiFrameWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if (uMsg == UM_SETTTING_READER_RET) {
		OnSettingReaderRet(wParam, lParam);
	}
	else if (uMsg == WM_DEVICECHANGE) {
		OnDeviceChanged(wParam, lParam);
	}
	else if (uMsg == UM_SETTTING_GW_RET) {
		OnSettingGwRet(wParam, lParam);
	}
	else if (uMsg == UM_QUERY_GW_RET) {
		OnQueryGwRet(wParam, lParam);
	}
	return WindowImplBase::HandleMessage(uMsg,wParam,lParam);
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
		while ( pFind ) {
			if ( 1 == sscanf(pFind + 3, "%d", &nComPort) ) {
				pElement->SetTag(nComPort); 
				break;
			}
			pFind = strstr(pFind + 3, "COM");
		}

		if (!bFindReader) {
			char tmp[256];
			Str2Lower(s.c_str(), tmp, sizeof(tmp) );
			if ( 0 != strstr(tmp, "usb-serial ch340") ) {
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
			m_reader_com_ports->SelectItem(0);
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
			m_gw_com_ports->SelectItem(0);
		}
	}
}

void  CDuiFrameWnd::OnSettingReader() {
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
		MessageBox(GetHWND(), "没有病区号。请先编辑病区", "错误", 0 );
		return;
	}

	nAreaNo = m_cmbArea1->GetItemAt(nSel)->GetTag();

	strText = m_edtBedNo->GetText();
	if (1 != sscanf(strText, "%d", &nBedNo)) {
		MessageBox(GetHWND(), "请输入床位号", "错误", 0);
		return;
	}

	if (nBedNo <= 0) {
		MessageBox(GetHWND(), "床位号必须是正整数", "错误", 0);
		return;
	}

	if (nBedNo > 200) {
		MessageBox(GetHWND(), "床号的范围是1到200", "错误", 0);
		return;
	}

	CBusiness::GetInstance()->SettingReaderAsyn(nAreaNo, nBedNo, nPort);

	m_btnSetting1->SetEnabled(false);
	SetBusy(TRUE);
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
		MessageBox(GetHWND(), "没有病区号。请先编辑病区", "错误", 0);
		return;
	}

	nAreaNo = m_cmbArea2->GetItemAt(nSel)->GetTag();

	CBusiness::GetInstance()->SettingGwAsyn(nAreaNo, nPort);

	m_btnSetting2->SetEnabled(false);
	SetBusy(TRUE);
}
 
void  CDuiFrameWnd::SetBusy(BOOL bBusy) {
	if (bBusy) {
		m_progress->SetVisible(true);
		m_progress->Start();
		m_bBusy = TRUE;
	}
	else {
		m_progress->Stop();
		m_progress->SetVisible(false);
		m_bBusy = FALSE;
	}
}

void  CDuiFrameWnd::OnQueryGw() {
	if (m_bBusy) {
		MessageBox(GetHWND(), "正忙着配置或查询，请稍候", "忙碌", 0);
		return;
	}

	int nPort = 0;
	if (m_gw_com_ports->GetCurSel() < 0) {
		MessageBox(GetHWND(), "没有选中串口", "错误", 0);
		return;
	}
	nPort = m_gw_com_ports->GetItemAt(m_gw_com_ports->GetCurSel())->GetTag();

	CBusiness::GetInstance()->QueryGwAsyn(nPort);

	m_edtQuery2Ret->SetText("");
	m_btnQuery2->SetEnabled(false);
	SetBusy(TRUE);
}

void  CDuiFrameWnd::OnAddArea() {
	CAreaWnd * pAreaDlg = new CAreaWnd();
	pAreaDlg->Create(this->m_hWnd, _T("添加病区"), UI_WNDSTYLE_FRAME | WS_POPUP, NULL, 0, 0, 0, 0);
	pAreaDlg->CenterWindow();

	// 如果是确定按钮后关闭
	if ( 0 == pAreaDlg->ShowModal() ) {
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
	STRNCPY( pAreaDlg->m_tArea.szAreaName, pItem->GetText(1), sizeof(pAreaDlg->m_tArea.szAreaName) );
	sscanf( pItem->GetText(0), "%lu", &pAreaDlg->m_tArea.dwAreaNo);
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

	if ( IDYES == MessageBox(GetHWND(), "确定要删除吗？", "删除", MB_YESNO | MB_DEFBUTTON2) ) {
		std::vector<TArea *>::iterator it;
		for (it = g_vArea.begin(); it != g_vArea.end(); ++it) {
			TArea * pArea = *it;
			if (pArea->dwAreaNo == dwAreaId) {
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

void  CDuiFrameWnd::OnSettingReaderRet(WPARAM wParm, LPARAM  lParam) {
	int ret = (int)wParm;
	SetBusy(FALSE);
	m_btnSetting1->SetEnabled(true);

	if (ret != 0) {
		MessageBox(GetHWND(), "设置Reader的过程中出错!", "错误", 0);
		return;
	}
}
      
void  CDuiFrameWnd::OnSettingGwRet(WPARAM wParm, LPARAM  lParam) {
	int ret = (int)wParm;
	SetBusy(FALSE);
	m_btnSetting2->SetEnabled(true);

	if (ret != 0) {
		MessageBox(GetHWND(), "设置发射器的过程中出错!", "错误", 0);
		return;
	}
	else {
		MessageBox(GetHWND(), "设置发射器成功", "成功", 0);
	}
}

void  CDuiFrameWnd::OnQueryGwRet(WPARAM wParm, LPARAM  lParam) {
	int ret = (int)wParm;
	SetBusy(FALSE);
	m_btnQuery2->SetEnabled(true);

	if (ret != 0) {
		MessageBox(GetHWND(), "获取发射器房间号的过程中出错!", "错误", 0);
		return;
	}
	else {
		DuiLib::CDuiString strText;
		strText.Format("%d", (int)lParam);
		m_edtQuery2Ret->SetText(strText);
		MessageBox(GetHWND(), "获取发射器房间号成功", "成功", 0);
	}
}

void  CDuiFrameWnd::OnDeviceChanged(WPARAM wParm, LPARAM  lParam) {
	if (m_bBusy) {
		return;
	}

	OnFreshComPort_Reader();
	OnFreshComPort_Gw();
}

void  CDuiFrameWnd::OnAreaListMenu(TNotifyUI& msg) {
	POINT pt = { msg.ptMouse.x, msg.ptMouse.y };
	CDuiMenu *pMenu = new CDuiMenu(_T("menu.xml"), msg.pSender);
	pMenu->Init(*this, pt);
	pMenu->ShowWindow(TRUE);
}

                         
    
        
    


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{	
	LmnToolkits::ThreadManager::GetInstance();
	CBusiness::GetInstance()->Init();
	g_log->Output(ILog::LOG_SEVERITY_INFO, "main begin.\n");

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

	g_log->Output(ILog::LOG_SEVERITY_INFO, "main close.\n");

	CBusiness::GetInstance()->DeInit();
	delete CBusiness::GetInstance();
	LmnToolkits::ThreadManager::ReleaseInstance();

	return 0;
}
       

