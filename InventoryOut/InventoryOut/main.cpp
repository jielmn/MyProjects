#ifdef _WIN32
#include "vld.h"
#endif

#include "main.h"
#include "LmnThread.h"
#include "Business.h"
#include "resource.h"
#include "LoginDlg.h"
#include "AgencyDlg.h"
#include "TargetDlg.h"
#include "MyProgress.h"


// menu
class CDuiMenu : public DuiLib::WindowImplBase
{
protected:
	virtual ~CDuiMenu() {};        // 私有化析构函数，这样此对象只能通过new来生成，而不能直接定义变量。就保证了delete this不会出错
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



 


DuiLib::CControlUI* CDuiFrameWndBuildCallback::CreateControl(LPCTSTR pstrClass) {
	DuiLib::CDuiString  strText;
	strText = pstrClass;
	strText += ".xml";

	DuiLib::CDialogBuilder builder;
	DuiLib::CControlUI * pUI = builder.Create((const char *)strText, (UINT)0, 0, m_PaintManager);
	return pUI;
}





CDuiFrameWnd::CDuiFrameWnd() {
	m_nSmallCount = 0;
	m_nBigCount   = 0;
	m_bBusy = FALSE;

	CBusiness::GetInstance()->m_sigStatusChange.connect(this, &CDuiFrameWnd::OnDbStatusChange);
	CBusiness::GetInstance()->m_sigGetAllAgency.connect(this, &CDuiFrameWnd::OnGetAllAgency);
	CBusiness::GetInstance()->m_sigDeleteAgency.connect(this, &CDuiFrameWnd::OnDeleteAgencyRet);
	CBusiness::GetInstance()->m_sigTimer.connect(this, &CDuiFrameWnd::OnTimerRet);
	CBusiness::GetInstance()->m_sigSaveInvOutRet.connect(this, &CDuiFrameWnd::OnSaveInvOutRet);

	m_Callback.m_PaintManager = &m_PaintManager;
}

void  CDuiFrameWnd::InitWindow() {
	PostMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0);

	m_lblLoginUser = static_cast<DuiLib::CLabelUI*>(m_PaintManager.FindControl("lblUser"));

	m_tabs = static_cast<DuiLib::CTabLayoutUI*>(m_PaintManager.FindControl("switch"));
	m_lblDbStatus = static_cast<DuiLib::CLabelUI*>(m_PaintManager.FindControl("lblDbStatus"));
	m_lstAgencies = static_cast<DuiLib::CListUI*>(m_PaintManager.FindControl("agency_list"));

	m_optSales = static_cast<DuiLib::COptionUI*>(m_PaintManager.FindControl("optSales"));
	m_edTarget = static_cast<DuiLib::CEditUI*>(m_PaintManager.FindControl("edtInvTarget"));
	m_lstPackages = static_cast<DuiLib::CListUI*>(m_PaintManager.FindControl("inv_list"));
	m_lblSmallCnt = static_cast<DuiLib::CLabelUI*>(m_PaintManager.FindControl("lblSmallCount"));
	m_lblBigCnt = static_cast<DuiLib::CLabelUI*>(m_PaintManager.FindControl("lblBigCount"));
	m_edPackageId = static_cast<DuiLib::CEditUI*>(m_PaintManager.FindControl("edtPackageId"));
	m_btnInvOk = static_cast<DuiLib::CButtonUI*>(m_PaintManager.FindControl("btnInvOk"));

	m_tabs_query = static_cast<DuiLib::CTabLayoutUI*>(m_PaintManager.FindControl("query_switch"));

	m_progress = static_cast<CMyProgress *>(m_PaintManager.FindControl("progress"));

	CInvoutDatabase::DATABASE_STATUS eStatus = CBusiness::GetInstance()->GetDbStatus();
	if (eStatus == CLmnOdbc::STATUS_OPEN) {
		m_lblDbStatus->SetText("数据库连接OK");
	}
	else {
		m_lblDbStatus->SetText("数据库连接失败");     
	}

	m_lblLoginUser->SetText(CBusiness::GetInstance()->m_strLoginName);

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
		else if (name == "optSales" || name == "optAgent" ) {
			m_edTarget->SetText("");
		}
		else if (name == "optByTime") { 
			m_tabs_query->SelectItem(0);
		}
		else if (name == "optByBigPkg") {
			m_tabs_query->SelectItem(1);
		}
		else if (name == "optBySmallPkg") {
			m_tabs_query->SelectItem(2);
		}
		else if (name == "optByTag") {
			m_tabs_query->SelectItem(3);
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
		else if (name == "btnTarget") {
			OnSelectTarget();
		}
		else if (name == "btnClearList") {
			OnClearInvList();
		}
		else if ( name == "btnAddList" ) {
			OnAddPackage();
		}
		else if (name == "btnAddList") {
			OnAddPackage();
		}
		else if (name == "btnInvOk") {
			OnInvOk();
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
	if (0 == strcmp(pstrClass, "MyProgress")) {
		return new CMyProgress(&m_PaintManager, "progress_fore.png");  
	}

	DuiLib::CDuiString  strText;
	strText = pstrClass;
	strText += ".xml";

	DuiLib::CDialogBuilder builder;
	DuiLib::CControlUI * pUI = builder.Create((const char *)strText, (UINT)0, &m_Callback, &m_PaintManager);
	return pUI;

	//return DuiLib::WindowImplBase::CreateControl( pstrClass );
}
      
// 处理自定义信息
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
	else if (uMsg == WM_CHAR) {
		char ch = (char)wParam;
		if ( 0 == m_tabs->GetCurSel() ) {
			OnInvOutChar(ch);
		}
	}
	else if (uMsg == UM_TIMER_RET) {
		OnTimerMsg(wParam);
	}
	else if (uMsg == UM_SAVE_INV_OUT_RET) {
		OnSaveInvOutMsg(wParam);
	}
	return DuiLib::WindowImplBase::HandleMessage(uMsg, wParam, lParam);
}

void   CDuiFrameWnd::OnDbStatusChange(CLmnOdbc::DATABASE_STATUS eNewStatus) {
	::PostMessage(this->GetHWND(), UM_DB_STATUS, eNewStatus, 0);
}

void   CDuiFrameWnd::OnDbStatusChangeMsg(CLmnOdbc::DATABASE_STATUS eNewStatus) {
	if (eNewStatus == CLmnOdbc::STATUS_OPEN) {
		m_lblDbStatus->SetText("数据库连接OK");
	}
	else {
		m_lblDbStatus->SetText("数据库连接失败");
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
		::MessageBox(GetHWND(), GetErrorDescription(ret), "删除经销商", 0);
	}
}

void  CDuiFrameWnd::OnTimerRet(DWORD dwTimerId) {
	::PostMessage(GetHWND(), UM_TIMER_RET, dwTimerId, 0);
}

void  CDuiFrameWnd::OnTimerMsg(DWORD dwTimerId) {
	// 如果定时器时间到
	if (dwTimerId == INV_OUT_CHAR_TIMER) {
		OnInvOutBarCode(m_strInvOutBuf);
		m_strInvOutBuf = "";
	}
}

int CDuiFrameWnd::OnInvOutBarCode(const DuiLib::CDuiString & strBarCode) {
	// g_log->Output(ILog::LOG_SEVERITY_INFO, "received chars:%s\n", m_strInvBigBuf);

	// 检查格式(15位)
	// FACTORY CODE (2)  +   PRODUCT CODE (2)   +    批号(8, 例如20180301)  + 流水号(4, 例如0001, A001)

	char buf[8192];
	char  szProductId[64] = { 'E', 'T' };

	DWORD  dwFactoryLen = 2;
	DWORD  dwProductLen = 2;

	if (strBarCode.GetLength() != dwFactoryLen + dwProductLen + 8 + FLOW_NUM_LEN) {
		g_log->Output(ILog::LOG_SEVERITY_INFO, "received barcode:%s, length not correct!\n", strBarCode);
		return 1;
	}

	DuiLib::CDuiString strBatchId = strBarCode.Mid(dwFactoryLen + dwProductLen, 8);

	int nBatchId = 0;
	if (0 == sscanf(strBatchId.Mid(0, 4), "%d", &nBatchId)) {
		g_log->Output(ILog::LOG_SEVERITY_INFO, "received barcode:%s, format not correct!\n", strBarCode);
		return 2;
	}

	if (0 == sscanf(strBatchId.Mid(4, 2), "%d", &nBatchId)) {
		g_log->Output(ILog::LOG_SEVERITY_INFO, "received barcode:%s, format not correct!\n", strBarCode);
		return 2;
	}

	if (0 == sscanf(strBatchId.Mid(6), "%d", &nBatchId)) {
		g_log->Output(ILog::LOG_SEVERITY_INFO, "received barcode:%s, format not correct!\n", strBarCode);
		return 2;
	}

	DuiLib::CDuiString strFlowId = strBarCode.Mid(dwFactoryLen + dwProductLen + 8 + 1);
	int nFlowId = 0;
	if (0 == sscanf(strFlowId, "%d", &nFlowId)) {
		g_log->Output(ILog::LOG_SEVERITY_INFO, "received barcode:%s, format not correct!\n", strBarCode);
		return 2;
	}

	int nType = 0;
	char chType = strBarCode.GetAt(dwFactoryLen + dwProductLen + 8);
	if ((chType >= 'A' && chType <= 'Z') || (chType >= 'a' && chType <= 'z')) {
		nType = 1;
	}
	else if (chType >= '0' && chType <= '9') {
		nType = 0;
	}
	else {
		g_log->Output(ILog::LOG_SEVERITY_INFO, "received barcode:%s, format not correct!\n", strBarCode);
		return 3;
	}

	// g_log->Output(ILog::LOG_SEVERITY_INFO, "received barcode:%s, OK \n", strBarCode);      

	DuiLib::CDuiString  strText;
	int nCount = m_lstPackages->GetCount();
	for (int i = 0; i < nCount; i++) {
		DuiLib::CListTextElementUI* pListElement = (DuiLib::CListTextElementUI*)m_lstPackages->GetItemAt(i);
		strText = pListElement->GetText(1);

		// 已经存在相同的编号
		if ( StrICmp( strText, strBarCode ) == 0 ) {
			return 4;
		}
	}

	// 显示
	DuiLib::CListTextElementUI* pListElement = new DuiLib::CListTextElementUI;
	m_lstPackages->Add(pListElement);
	pListElement->SetText(0, (0 == nType ? "小包装" : "大包装") );	
	Str2Upper(strBarCode, buf, sizeof(buf));
	pListElement->SetText(1, buf);

	if (0 == nType) {
		m_nSmallCount++;
		strText.Format("%d", m_nSmallCount);
		m_lblSmallCnt->SetText(strText);
	}
	else {
		m_nBigCount++;
		strText.Format("%d", m_nBigCount);
		m_lblBigCnt->SetText(strText);
	}
	return 0;
}	


void  CDuiFrameWnd::OnSaveInvOutRet(int ret) {
	::PostMessage(GetHWND(), UM_SAVE_INV_OUT_RET, ret, 0);
}

void  CDuiFrameWnd::OnSaveInvOutMsg(int ret) {
	SetBusy(FALSE);

	if (0 == ret) {
		::MessageBox(GetHWND(), "出库记录保存成功", "出库记录保存", 0);
		ClearInvOut();
	}
	else {
		::MessageBox(GetHWND(), GetErrorDescription(ret), "出库记录保存", 0);
	}	
}









void CDuiFrameWnd::OnClearInvList() {
	m_lstPackages->RemoveAll();
	m_nSmallCount = 0;
	m_nBigCount = 0;

	m_lblSmallCnt->SetText("0");
	m_lblBigCnt->SetText("0");
}

// 添加条码
void CDuiFrameWnd::OnAddPackage() {
	DuiLib::CDuiString strText;
	strText = m_edPackageId->GetText();
	OnInvOutBarCode(strText);
}

// 
void CDuiFrameWnd::OnInvOk() {
	DuiLib::CDuiString  strText;
	strText = m_edTarget->GetText();
	if (strText.GetLength() == 0) {
		MessageBox(GetHWND(), "没有选择出库目标", "出库盘点", 0);
		return;
	}

	if ( m_lstPackages->GetCount() == 0) {
		MessageBox(GetHWND(), "出库列表为空", "出库盘点", 0);
		return;
	}

	int nPos = strText.Find(',');
	assert(nPos >= 0);

	DuiLib::CDuiString  strTargetId = strText.Mid(0, nPos);

	std::vector< DuiLib::CDuiString * > vBig;
	std::vector< DuiLib::CDuiString * > vSmall;

	int nCount = m_lstPackages->GetCount();
	for (int i = 0; i < nCount; i++) {
		DuiLib::CListTextElementUI* pListElement = (DuiLib::CListTextElementUI*)m_lstPackages->GetItemAt(i);
		assert(pListElement);
		DuiLib::CDuiString * pStr = new DuiLib::CDuiString;
		*pStr = pListElement->GetText(1);

		if ( 0 == strcmp(pListElement->GetText(0), "小包装") ) {			
			vSmall.push_back(pStr);
		}
		else {
			vBig.push_back(pStr);
		}
	}

	int nTargetType = 0;
	if (m_optSales->IsSelected()) {
		nTargetType = 0;
	}
	else {
		nTargetType = 1;
	}

	CBusiness::GetInstance()->SaveInvOutAsyn( nTargetType, strTargetId, vBig, vSmall );

	ClearVector(vBig);
	ClearVector(vSmall);

	SetBusy();
}

void  CDuiFrameWnd::ClearInvOut() {
	m_edTarget->SetText("");
	m_edPackageId->SetText("");
	OnClearInvList();
}

void  CDuiFrameWnd::SetBusy(BOOL bBusy /*= TRUE*/) {
	if (bBusy) {
		m_btnInvOk->SetEnabled(false);
		m_progress->SetVisible(true);
		m_progress->Start();
	}
	else {
		m_btnInvOk->SetEnabled(true);
		m_progress->Stop();
		m_progress->SetVisible(false);
	}
	m_bBusy = bBusy;
}

void  CDuiFrameWnd::OnQueryByTime() {

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

	pDlg->Create(this->m_hWnd, _T("新增经销商信息"), UI_WNDSTYLE_FRAME | WS_POPUP, NULL, 0, 0, 0, 0);
	pDlg->CenterWindow();
	int ret = pDlg->ShowModal();

	// 如果添加成功
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

	pDlg->Create(this->m_hWnd, _T("修改经销商信息"), UI_WNDSTYLE_FRAME | WS_POPUP, NULL, 0, 0, 0, 0);
	pDlg->CenterWindow();
	int ret = pDlg->ShowModal();

	// 如果修改成功
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

	if ( ::MessageBox(GetHWND(), "确定要删除吗？", "删除经销商", MB_YESNO | MB_DEFBUTTON2) == IDYES ) {
		CBusiness::GetInstance()->DeleteAgencyAsyn(pListElement->GetTag(),pListElement->GetText(0));
	}
}

void  CDuiFrameWnd::OnSelectTarget() {
	int nTargetType = 0;
	if ( m_optSales->IsSelected() ) {
		nTargetType = TARGET_TYPE_SALES;
	}
	else {
		nTargetType = TARGET_TYPE_AGENCIES;
	}

	CTargetDlg * pDlg = new CTargetDlg(nTargetType);

	pDlg->Create(this->m_hWnd, _T("目标"), UI_WNDSTYLE_FRAME | WS_POPUP, NULL, 0, 0, 0, 0);
	pDlg->CenterWindow();
	int ret = pDlg->ShowModal();

	DuiLib::CDuiString  strText;
	if (0 == ret) {
		strText = pDlg->m_strId;
		strText += ",";
		strText += pDlg->m_strName;

		m_edTarget->SetText(strText);
	}

	delete pDlg;                
}

// 盘点收到char
void CDuiFrameWnd::OnInvOutChar(char ch) {
	m_strInvOutBuf += ch;
	CBusiness::GetInstance()->SetTimerAsyn(INV_OUT_CHAR_TIMER, INV_OUT_CHAR_TIMER_INTEVAL);
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
	// 显示登录界面
	ret = loginFrame->ShowModal();
	delete loginFrame;

	// 如果登录成功
	if (0 == ret) {
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
	}

	CBusiness::GetInstance()->DeInit();
	delete CBusiness::GetInstance();
	LmnToolkits::ThreadManager::ReleaseInstance();

	return 0;
}   