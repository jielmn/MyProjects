#ifdef _WIN32
#include "vld.h"
#endif

#include "main.h"
#include "LmnThread.h"
#include "Business.h"
#include "resource.h"
#include "LoginDlg.h"
#include "AgencyDlg.h"



// menu
class CDuiMenu : public DuiLib::WindowImplBase
{
protected:
	virtual ~CDuiMenu() {};        // ˽�л����������������˶���ֻ��ͨ��new�����ɣ�������ֱ�Ӷ���������ͱ�֤��delete this�������
	DuiLib::CDuiString  m_strXMLPath;
	DuiLib::CControlUI * m_pOwner;

public:
	explicit CDuiMenu(LPCTSTR pszXMLPath, DuiLib::CControlUI * pOwner) : m_strXMLPath(pszXMLPath), m_pOwner(pOwner) {}
	virtual LPCTSTR    GetWindowClassName()const { return _T("CDuiMenu "); }
	virtual DuiLib::CDuiString GetSkinFolder() { return _T(""); }
	virtual DuiLib::CDuiString GetSkinFile() { return m_strXMLPath; }
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

	virtual void  Notify(DuiLib::TNotifyUI& msg) {
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
	CBusiness::GetInstance()->m_sigStatusChange.connect(this, &CDuiFrameWnd::OnDbStatusChange);
	CBusiness::GetInstance()->m_sigGetAllAgency.connect(this, &CDuiFrameWnd::OnGetAllAgency);
	CBusiness::GetInstance()->m_sigDeleteAgency.connect(this, &CDuiFrameWnd::OnDeleteAgencyRet);
}

void  CDuiFrameWnd::InitWindow() {
	PostMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0);
	m_tabs = static_cast<DuiLib::CTabLayoutUI*>(m_PaintManager.FindControl("switch"));
	m_lblDbStatus = static_cast<DuiLib::CLabelUI*>(m_PaintManager.FindControl("lblDbStatus"));
	m_lstAgencies = static_cast<DuiLib::CListUI*>(m_PaintManager.FindControl("agency_list"));

	CInvoutDatabase::DATABASE_STATUS eStatus = CBusiness::GetInstance()->GetDbStatus();
	if (eStatus == CLmnOdbc::STATUS_OPEN) {
		m_lblDbStatus->SetText("���ݿ�����OK");
	}
	else {
		m_lblDbStatus->SetText("���ݿ�����ʧ��");     
	}

	CBusiness::GetInstance()->GetAllAgency();

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
		else if (name == "btnModifyAgency") {
			OnModifyAgency();
		}
		else if (name == "btnDelAgency") {
			OnDeleteAgency();
		}
	}
	else if (msg.sType == "itemactivate") {
		DuiLib::CControlUI * pParent = msg.pSender->GetParent();
		if ( pParent ) {
			pParent = pParent->GetParent();
			if (pParent) {
				if (pParent->GetName() == "agency_list") {
					OnModifyAgency();
				}
			}
		}

	} 
	else if (msg.sType == _T("menu"))
	{
		POINT pt = { msg.ptMouse.x, msg.ptMouse.y };
		DuiLib::CControlUI * pControl = m_PaintManager.FindControl(pt);
		if (0 == pControl) {
			return;
		}

		DuiLib::CDuiString sFindCtlClass = pControl->GetClass();
		if (sFindCtlClass == "ListTextElement") {
			DuiLib::CDuiString sListName = pControl->GetParent()->GetParent()->GetName();
			if (sListName == "agency_list") {
				CDuiMenu *pMenu = new CDuiMenu(_T("menu.xml"), msg.pSender);
				pMenu->Init(*this, pt);
				pMenu->ShowWindow(TRUE);
			}
		}
	}
	else if (msg.sType == "menu_modify_agent") {
		OnModifyAgency();
	}
	else if (msg.sType == "menu_delete_agent") {
		OnDeleteAgency();
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
      
// �����Զ�����Ϣ
LRESULT CDuiFrameWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	int ret = 0;
	if ( UM_GET_ALL_AGENCY_RET == uMsg ) {
		ret = wParam;
		std::vector<AgencyItem *> * pvRet = (std::vector<AgencyItem *> *)lParam;
		assert(pvRet);

		OnGetAllAgencyMsg(ret, *pvRet);

		if (pvRet) {
			ClearVector(*pvRet);
			delete pvRet;
		}
	}
	else if (UM_DB_STATUS == uMsg) {
		OnDbStatusChangeMsg( (CLmnOdbc::DATABASE_STATUS)wParam );
	}
	else if (UM_DELETE_AGENCY_RET == uMsg) {
		OnDeleteAgencyMsg(wParam, lParam);
	}
	return DuiLib::WindowImplBase::HandleMessage(uMsg, wParam, lParam);
}

void   CDuiFrameWnd::OnDbStatusChange(CLmnOdbc::DATABASE_STATUS eNewStatus) {
	::PostMessage(this->GetHWND(), UM_DB_STATUS, eNewStatus, 0);
}

void   CDuiFrameWnd::OnDbStatusChangeMsg(CLmnOdbc::DATABASE_STATUS eNewStatus) {
	if (eNewStatus == CLmnOdbc::STATUS_OPEN) {
		m_lblDbStatus->SetText("���ݿ�����OK");
	}
	else {
		m_lblDbStatus->SetText("���ݿ�����ʧ��");
	}
}

void  CDuiFrameWnd::OnGetAllAgency(int ret, const std::vector<AgencyItem *> & vRet ) {
	std::vector<AgencyItem *> * pvRet = new std::vector<AgencyItem *>;
	std::vector<AgencyItem *>::const_iterator it;
	for (it = vRet.begin(); it != vRet.end(); it++) {
		AgencyItem * pItem = *it;
		AgencyItem * pNewItem = new AgencyItem;
		memcpy(pNewItem, pItem, sizeof(AgencyItem));
		pvRet->push_back(pNewItem);
	}

	::PostMessage( GetHWND(), UM_GET_ALL_AGENCY_RET, ret, (LPARAM)pvRet);
}

void  CDuiFrameWnd::OnGetAllAgencyMsg(int ret, const std::vector<AgencyItem *> & vRet) {
	if (0 != ret) {
		return;
	}

	std::vector<AgencyItem *>::const_iterator it;
	for ( it = vRet.begin(); it != vRet.end(); it++) {
		AgencyItem * pItem = *it;

		DuiLib::CListTextElementUI* pListElement = new DuiLib::CListTextElementUI;
		m_lstAgencies->Add(pListElement);

		AddAgencyItem2List(pListElement, pItem);
	}
}

void  CDuiFrameWnd::OnDeleteAgencyRet(int ret, DWORD dwId) {
	::PostMessage(GetHWND(), UM_DELETE_AGENCY_RET, ret, dwId);
}

void  CDuiFrameWnd::OnDeleteAgencyMsg(int  ret, DWORD dwId) {
	if (0 == ret) {
		int nCnt = m_lstAgencies->GetCount();
		for (int i = 0; i < nCnt; i++) {
			DuiLib::CListTextElementUI* pListElement = (DuiLib::CListTextElementUI* )m_lstAgencies->GetItemAt(i);
			if (pListElement->GetTag() == dwId) {
				m_lstAgencies->Remove(pListElement);
				break;
			}
		}
	}
	else {
		::MessageBox(GetHWND(), GetErrorDescription(ret), "ɾ��������", 0);
	}
}





void  CDuiFrameWnd::AddAgencyItem2List(DuiLib::CListTextElementUI* pListElement, AgencyItem * pItem, BOOL bSetTag /*= TRUE*/) {
	pListElement->SetText(0, pItem->szId);
	pListElement->SetText(1, pItem->szName);
	pListElement->SetText(2, pItem->szProvince);    
	if (bSetTag) {
		pListElement->SetTag(pItem->dwId);
	}
}

void  CDuiFrameWnd::OnAddAgency() {
	CAgencyWnd * pDlg = new CAgencyWnd;

	pDlg->Create(this->m_hWnd, _T("������������Ϣ"), UI_WNDSTYLE_FRAME | WS_POPUP, NULL, 0, 0, 0, 0);
	pDlg->CenterWindow();
	int ret = pDlg->ShowModal();

	// �����ӳɹ�
	if (0 == ret) {
		DuiLib::CListTextElementUI* pListElement = new DuiLib::CListTextElementUI;
		m_lstAgencies->Add(pListElement);

		AddAgencyItem2List(pListElement, &pDlg->m_tAgency);
	}

	delete pDlg; 
}

void  CDuiFrameWnd::OnModifyAgency() {
	int nSelIndex = m_lstAgencies->GetCurSel();
	if (nSelIndex < 0) {
		return;
	}

	CAgencyWnd * pDlg = new CAgencyWnd(FALSE);

	DuiLib::CListTextElementUI* pListElement = (DuiLib::CListTextElementUI*)m_lstAgencies->GetItemAt(nSelIndex);
	STRNCPY(pDlg->m_tAgency.szId,         pListElement->GetText(0), sizeof(pDlg->m_tAgency.szId));
	STRNCPY(pDlg->m_tAgency.szName,       pListElement->GetText(1), sizeof(pDlg->m_tAgency.szName));
	STRNCPY(pDlg->m_tAgency.szProvince,   pListElement->GetText(2), sizeof(pDlg->m_tAgency.szProvince));
	pDlg->m_tAgency.dwId = pListElement->GetTag();

	pDlg->Create(this->m_hWnd, _T("�޸ľ�������Ϣ"), UI_WNDSTYLE_FRAME | WS_POPUP, NULL, 0, 0, 0, 0);
	pDlg->CenterWindow();
	int ret = pDlg->ShowModal();

	// ����޸ĳɹ�
	if (0 == ret) {
		pListElement->SetText(1, pDlg->m_tAgency.szName);
		pListElement->SetText(2, pDlg->m_tAgency.szProvince);
	}

	delete pDlg;
}

void  CDuiFrameWnd::OnDeleteAgency() {
	int nSelIndex = m_lstAgencies->GetCurSel();
	if (nSelIndex < 0) {
		return;
	}

	DuiLib::CListTextElementUI* pListElement = (DuiLib::CListTextElementUI*)m_lstAgencies->GetItemAt(nSelIndex);
	assert(pListElement);

	if ( ::MessageBox(GetHWND(), "ȷ��Ҫɾ����", "ɾ��������", MB_YESNO | MB_DEFBUTTON2) == IDYES ) {
		CBusiness::GetInstance()->DeleteAgencyAsyn(pListElement->GetTag());
	}
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
	// ��ʾ��¼����
	ret = loginFrame->ShowModal();
	delete loginFrame;

	// �����¼�ɹ�
	if (0 == ret) {
		// ������	
		CDuiFrameWnd * duiFrame = new CDuiFrameWnd;
		duiFrame->Create(NULL, _T("DUIWnd"), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
		duiFrame->CenterWindow();

		// ����icon
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