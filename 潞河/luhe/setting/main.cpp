// win32_1.cpp : ����Ӧ�ó������ڵ㡣
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
	virtual ~CDuiMenu() {};        // ˽�л����������������˶���ֻ��ͨ��new�����ɣ�������ֱ�Ӷ���������ͱ�֤��delete this�������
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

}

CDuiFrameWnd::~CDuiFrameWnd() {

}

void  CDuiFrameWnd::InitWindow() {
	PostMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0);

	m_tabs             = static_cast<DuiLib::CTabLayoutUI*>(m_PaintManager.FindControl("switch"));
	m_lblStatus        = static_cast<DuiLib::CLabelUI*>(m_PaintManager.FindControl("lblStatus"));
	m_reader_com_ports = static_cast<DuiLib::CComboUI*>(m_PaintManager.FindControl("cmbReaderComPort"));
	m_gw_com_ports     = static_cast<DuiLib::CComboUI*>(m_PaintManager.FindControl("cmGwComPort"));
	m_lstArea          = static_cast<DuiLib::CListUI*>(m_PaintManager.FindControl("lstArea"));
	m_cmbArea1         = static_cast<DuiLib::CComboUI*>(m_PaintManager.FindControl("cmbArea1"));
	m_cmbArea2         = static_cast<DuiLib::CComboUI*>(m_PaintManager.FindControl("cmbArea2"));

	OnDeviceChanged(0, 0);
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
	WindowImplBase::Notify(msg);
}

LRESULT CDuiFrameWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if (uMsg == WM_DEVICECHANGE) {
		OnDeviceChanged(wParam, lParam);
	}
	return WindowImplBase::HandleMessage(uMsg,wParam,lParam);
}
 
void  CDuiFrameWnd::OnDeviceChanged(WPARAM wParm, LPARAM  lParam) {
	char szComPort[256] = {0};
	int nCount = GetCh340Count(szComPort, sizeof(szComPort));
	if ( nCount <= 0  ) {
		m_lblStatus->SetText("û���ҵ�ch340����");
	}
	else if (nCount > 1) {
		m_lblStatus->SetText("�������������ϵ�ch340����");
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
	pAreaDlg->Create(this->m_hWnd, _T("��Ӳ���"), UI_WNDSTYLE_FRAME | WS_POPUP, NULL, 0, 0, 0, 0);
	pAreaDlg->CenterWindow();

	// �����ȷ����ť��ر�
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

}

void  CDuiFrameWnd::OnDeleteArea() {

}

void  CDuiFrameWnd::OnAreasChanged() {
	DuiLib::CDuiString  strText;
	std::vector<TArea *>::iterator it;

	m_cmbArea1->RemoveAll();
	m_cmbArea2->RemoveAll();

	for (it = g_vArea.begin(); it != g_vArea.end(); ++it) {
		TArea * pArea = *it;

		CListLabelElementUI * pElement = new CListLabelElementUI;
		strText.Format("%s (��ţ�%lu)", pArea->szAreaName, pArea->dwAreaNo);
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


