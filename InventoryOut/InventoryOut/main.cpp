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
#include "LmnExcel.h"

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
	CBusiness::GetInstance()->m_sigQueryByTime.connect(this, &CDuiFrameWnd::OnQueryByTimeRet);
	CBusiness::GetInstance()->m_sigQueryByBigPkg.connect(this, &CDuiFrameWnd::OnQueryByBigPkgRet);
	CBusiness::GetInstance()->m_sigQueryBySmallPkg.connect(this, &CDuiFrameWnd::OnQueryBySmallPkgRet);
	CBusiness::GetInstance()->m_sigQueryByTag.connect(this, &CDuiFrameWnd::OnQueryByTagRet);

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
	m_optSales_1 = static_cast<DuiLib::COptionUI*>(m_PaintManager.FindControl("optSales_1"));
	m_edTarget_1 = static_cast<DuiLib::CEditUI*>(m_PaintManager.FindControl("edtInvTarget_1"));
	m_btnQuery_1 = static_cast<DuiLib::CButtonUI*>(m_PaintManager.FindControl("btnQuery_1"));
	m_dateTimeStart = static_cast<DuiLib::CDateTimeUI*>(m_PaintManager.FindControl("DateTime1"));
	m_dateTimeEnd = static_cast<DuiLib::CDateTimeUI*>(m_PaintManager.FindControl("DateTime2"));
	m_lstQueryByTime = static_cast<DuiLib::CListUI*>(m_PaintManager.FindControl("query_list_1"));
	m_lblSmallCnt_1 = static_cast<DuiLib::CLabelUI*>(m_PaintManager.FindControl("lblSmallCount_1"));
	m_lblBigCnt_1 = static_cast<DuiLib::CLabelUI*>(m_PaintManager.FindControl("lblBigCount_1"));

	m_lstQueryByBigPkg = static_cast<DuiLib::CListUI*>(m_PaintManager.FindControl("query_list_2"));
	m_lstQueryByBigPkg_1 = static_cast<DuiLib::CListUI*>(m_PaintManager.FindControl("query_list_2_1"));
	m_btnQuery_2 = static_cast<DuiLib::CButtonUI*>(m_PaintManager.FindControl("btnQuery_2"));
	m_edBigPackageId = static_cast<DuiLib::CEditUI*>(m_PaintManager.FindControl("edtBigPackageId"));

	m_lstQueryBySmallPkg = static_cast<DuiLib::CListUI*>(m_PaintManager.FindControl("query_list_3"));
	m_btnQuery_3 = static_cast<DuiLib::CButtonUI*>(m_PaintManager.FindControl("btnQuery_3"));
	m_edSmallPackageId = static_cast<DuiLib::CEditUI*>(m_PaintManager.FindControl("edtSmallPackageId"));

	m_lstQueryByTag = static_cast<DuiLib::CListUI*>(m_PaintManager.FindControl("query_list_4"));
	m_btnQuery_4 = static_cast<DuiLib::CButtonUI*>(m_PaintManager.FindControl("btnQuery_4"));
	m_edTagId = static_cast<DuiLib::CEditUI*>(m_PaintManager.FindControl("edtTagId"));

	m_progress = static_cast<CMyProgress *>(m_PaintManager.FindControl("progress"));

	CInvoutDatabase::DATABASE_STATUS eStatus = CBusiness::GetInstance()->GetDbStatus();
	if (eStatus == CLmnOdbc::STATUS_OPEN) {
		m_lblDbStatus->SetText("数据库连接OK");
	}
	else {
		m_lblDbStatus->SetText("数据库连接失败");     
	}

	m_lblLoginUser->SetText(CBusiness::GetInstance()->m_strLoginName);

	m_lstQueryByBigPkg_1->SetVisible(false);

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
		else if (name == "optSales_1" || name == "optAgent_1") {
			m_edTarget_1->SetText("");
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
		else if (name == "btnQuery_1") {
			OnQueryByTime();
		}
		else if (name == "btnQuery_101") {
			OnExportExcel_1();
		}
		else if (name == "btnTarget_1") {
			OnSelectTarget_1();
		}
		else if ( name == "btnQuery_2" ) {
			OnQueryByBigPkg();
		}
		else if (name == "btnQuery_3") {
			OnQueryBySmallPkg();
		}
		else if (name == "btnQuery_4") {
			OnQueryByTag();
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
	else if (uMsg == UM_QUERY_BY_TIM_RET) {
		ret = wParam;
		std::vector<QueryByTimeItem*> * pvRet = (std::vector<QueryByTimeItem*> *)lParam;
		assert(pvRet);

		OnQueryByTimeMsg(ret, *pvRet);

		if (pvRet) {
			ClearVector(*pvRet);
			delete pvRet;
		}
	}
	else if (uMsg == UM_QUERY_BY_BIG_PKG_RET) {
		ret = wParam;
		void ** pParam = (void **)lParam;
		std::vector<PkgItem*> * pvBig = (std::vector<PkgItem*> *)pParam[0];
		std::vector<PkgItem*> * pvSmall = (std::vector<PkgItem*> *)pParam[1];
		assert(pvBig && pvSmall);

		OnQueryByBigPkgMsg(ret, *pvBig, *pvSmall);

		if (pvBig) {
			ClearVector(*pvBig);
			delete pvBig;
		}

		if (pvSmall) {
			ClearVector(*pvSmall);
			delete pvSmall;
		}
	}
	else if (uMsg == UM_QUERY_BY_SMALL_PKG_RET) {
		ret = wParam;
		std::vector<PkgItem*> * pvSmall = (std::vector<PkgItem*> *)lParam;
		assert(pvSmall);

		OnQueryBySmallPkgMsg(ret, *pvSmall);

		if (pvSmall) {
			ClearVector(*pvSmall);
			delete pvSmall;
		}
	}
	else if (uMsg == UM_QUERY_BY_TAG_RET) {
		ret = wParam;
		TagItem * pTag = (TagItem *)lParam;
		assert(pTag);

		OnQueryByTagMsg(ret, *pTag);

		if (pTag) {
			delete pTag;
		}
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
 
int CDuiFrameWnd::OnInvOutBarCode(const DuiLib::CDuiString & strBarCode_1) {
	// g_log->Output(ILog::LOG_SEVERITY_INFO, "received chars:%s\n", m_strInvBigBuf);

	// 检查格式(15位)
	// FACTORY CODE (2)  +   PRODUCT CODE (2)   +    批号(8, 例如20180301)  + 流水号(4, 例如0001, A001)

	char buf[8192];
	char  szProductId[64] = { 'E', 'T' };

	StrTrim( (const char*)strBarCode_1, buf, sizeof(buf) );
	DuiLib::CDuiString strBarCode = buf;

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

void  CDuiFrameWnd::OnQueryByTimeRet(int ret, const std::vector<QueryByTimeItem*> & vRet) {
	std::vector<QueryByTimeItem*> * pvRet = new std::vector<QueryByTimeItem*>;
	if ( ret == 0 ) {
		std::vector<QueryByTimeItem*>::const_iterator it;
		for (it = vRet.begin(); it != vRet.end(); it++) {
			QueryByTimeItem* pItem = *it;
			QueryByTimeItem* pNewItem = new QueryByTimeItem;
			memcpy(pNewItem, pItem, sizeof(QueryByTimeItem));
			pvRet->push_back(pNewItem);
		}
	}
	::PostMessage(GetHWND(), UM_QUERY_BY_TIM_RET, ret, (LPARAM)pvRet);
}

void  CDuiFrameWnd::OnQueryByTimeMsg(int ret, const std::vector<QueryByTimeItem*> & vRet) {
	SetBusy(FALSE);

	if (0 != ret) {
		::MessageBox(GetHWND(), GetErrorDescription(ret), "查询", 0);
		return;
	}
	
	char buf[8192];
	DuiLib::CDuiString strText;
	std::vector<QueryByTimeItem*>::const_iterator it;
	int nBigCnt = 0;
	int nSmallCnt = 0;
	int i = 0;

	for ( it = vRet.begin(), i = 0; it != vRet.end(); ++it, ++i ) {
		QueryByTimeItem* pItem = *it;
		DuiLib::CListTextElementUI* pListElement = new DuiLib::CListTextElementUI;
		m_lstQueryByTime->Add(pListElement);

		strText.Format("%d", i + 1);
		pListElement->SetText(0, strText);

		pListElement->SetText(1, PACKAGE_TYPE_BIG == pItem->m_nPackageType ? "大包装" : "小包装" );

		if (pItem->m_nPackageType == PACKAGE_TYPE_BIG) {
			nBigCnt++;
		} if (pItem->m_nPackageType == PACKAGE_TYPE_SMALL) {
			nSmallCnt++;
		}

		pListElement->SetText(2, pItem->m_szPackageId);

		pListElement->SetText(3, TARGET_TYPE_SALES == pItem->m_nTargetType ? "销售" : "经销商" );

		pListElement->SetText(4, pItem->m_szTargetName);

		pListElement->SetText(5, pItem->m_szOperatorName);

		DateTime2String(buf, sizeof(buf), &pItem->m_tOperatorTime);
		pListElement->SetText(6, buf);
	}
	   
	strText.Format("%d", nSmallCnt);
	m_lblSmallCnt_1->SetText(strText);

	strText.Format("%d", nBigCnt);
	m_lblBigCnt_1->SetText(strText); 

}

void  CDuiFrameWnd::OnQueryByBigPkgRet(int ret, const std::vector<PkgItem*> & vBig, const std::vector<PkgItem*> & vSmall) {
	void ** pParam = new void *[2];
	std::vector<PkgItem*> * pvBig   = new std::vector<PkgItem*>;
	std::vector<PkgItem*> * pvSmall = new std::vector<PkgItem*>;
	pParam[0] = pvBig;
	pParam[1] = pvSmall;

	std::vector<PkgItem*>::const_iterator it;
	for ( it = vBig.begin(); it != vBig.end(); it++ ) {
		PkgItem* pPkgItem = *it;
		PkgItem* pNewPkgItem = new PkgItem;
		memcpy(pNewPkgItem, pPkgItem, sizeof(PkgItem));
		pvBig->push_back(pNewPkgItem);
	}

	for (it = vSmall.begin(); it != vSmall.end(); it++) {
		PkgItem* pPkgItem = *it;
		PkgItem* pNewPkgItem = new PkgItem;
		memcpy(pNewPkgItem, pPkgItem, sizeof(PkgItem));
		pvSmall->push_back(pNewPkgItem);
	}

	::PostMessage( GetHWND(), UM_QUERY_BY_BIG_PKG_RET, ret, (LPARAM)pParam );
}

void  CDuiFrameWnd::OnQueryByBigPkgMsg(int ret, const std::vector<PkgItem*> & vBig, const std::vector<PkgItem*> & vSmall) {
	SetBusy(FALSE);

	if (0 != ret) {
		::MessageBox(GetHWND(), GetErrorDescription(ret), "查询", 0);
		return;
	}

	DuiLib::CDuiString  strText;
	char buf[8192];
	std::vector<PkgItem*>::const_iterator it;
	int i;

	for (it = vBig.begin(), i = 0; it != vBig.end(); it++, i++) {
		PkgItem* pItem = *it;
		DuiLib::CListTextElementUI* pListElement = new DuiLib::CListTextElementUI;
		m_lstQueryByBigPkg->Add(pListElement);

		strText.Format( "%d", i + 1 );
		pListElement->SetText(0, strText);

		pListElement->SetText(1, pItem->szId);
		if (pItem->dwStatus == PKG_STATUS_OUT) {
			pListElement->SetText(2, "已出库");
		}
		else if (pItem->dwStatus == PKG_STATUS_HALF_OUT) {
			pListElement->SetText(2, "半出库");
		}
		else {
			pListElement->SetText(2, "在库");
		}

		pListElement->SetText(3, pItem->szInOperator);

		DateTime2String( buf, sizeof(buf), &pItem->tInTime );
		pListElement->SetText( 4, buf );

		if (pItem->nOutTargetType != -1) {
			pListElement->SetText(5, TARGET_TYPE_SALES == pItem->nOutTargetType ? "销售" : "经销商");
		}
		
		pListElement->SetText(6, pItem->szOutTargetName );

		pListElement->SetText(7, pItem->szOutOperator);

		if (pItem->nOutTargetType != -1) {
			DateTime2String(buf, sizeof(buf), &pItem->tOutTime);
			pListElement->SetText(8, buf);
		}		
	}

	for (it = vSmall.begin(), i = 0; it != vSmall.end(); it++, i++) {
		PkgItem* pItem = *it;
		DuiLib::CListTextElementUI* pListElement = new DuiLib::CListTextElementUI;
		m_lstQueryByBigPkg_1->Add(pListElement);

		strText.Format("%d", i + 1);
		pListElement->SetText(0, strText);

		pListElement->SetText(1, pItem->szId);
		if (pItem->dwStatus == PKG_STATUS_OUT) {
			pListElement->SetText(2, "已出库");
		}
		else if (pItem->dwStatus == PKG_STATUS_HALF_OUT) {
			pListElement->SetText(2, "半出库");
		}
		else {
			pListElement->SetText(2, "在库");
		}

		pListElement->SetText(3, pItem->szInOperator);

		DateTime2String(buf, sizeof(buf), &pItem->tInTime);
		pListElement->SetText(4, buf);

		if (pItem->nOutTargetType != -1) {
			pListElement->SetText(5, TARGET_TYPE_SALES == pItem->nOutTargetType ? "销售" : "经销商");
		}

		pListElement->SetText(6, pItem->szOutTargetName);

		pListElement->SetText(7, pItem->szOutOperator);

		if (pItem->nOutTargetType != -1) {
			DateTime2String(buf, sizeof(buf), &pItem->tOutTime);
			pListElement->SetText(8, buf);
		}
	}

	if (vSmall.size() > 0) {
		m_lstQueryByBigPkg_1->SetVisible(true);
	}
}

void  CDuiFrameWnd::OnQueryBySmallPkgRet(int ret, const std::vector<PkgItem*> & vSmall) {
	std::vector<PkgItem*> * pvRet = new std::vector<PkgItem*>;
	std::vector<PkgItem*>::const_iterator it;
	for (it = vSmall.begin(); it != vSmall.end(); it++) {
		PkgItem* pPkgItem = *it;
		PkgItem* pNewPkgItem = new PkgItem;
		memcpy(pNewPkgItem, pPkgItem, sizeof(PkgItem));
		pvRet->push_back(pNewPkgItem);
	}

	::PostMessage(GetHWND(), UM_QUERY_BY_SMALL_PKG_RET, ret, (LPARAM)pvRet);
}

void  CDuiFrameWnd::OnQueryBySmallPkgMsg(int ret, const std::vector<PkgItem*> & vSmall) {
	SetBusy(FALSE);

	if (0 != ret) {
		::MessageBox(GetHWND(), GetErrorDescription(ret), "查询", 0);
		return;
	}

	DuiLib::CDuiString  strText;
	char buf[8192];
	std::vector<PkgItem*>::const_iterator it;
	int i;

	for (it = vSmall.begin(), i = 0; it != vSmall.end(); it++, i++) {
		PkgItem* pItem = *it;
		DuiLib::CListTextElementUI* pListElement = new DuiLib::CListTextElementUI;
		m_lstQueryBySmallPkg->Add(pListElement);

		strText.Format("%d", i + 1);
		pListElement->SetText(0, strText);

		pListElement->SetText(1, pItem->szParentPkgId);

		pListElement->SetText(2, pItem->szId);

		if (pItem->dwStatus == PKG_STATUS_OUT) {
			pListElement->SetText(3, "已出库");
		}
		else {
			pListElement->SetText(3, "在库");
		}

		pListElement->SetText(4, pItem->szInOperator);

		DateTime2String(buf, sizeof(buf), &pItem->tInTime);
		pListElement->SetText(5, buf);

		if (pItem->nOutTargetType != -1) {
			pListElement->SetText(6, TARGET_TYPE_SALES == pItem->nOutTargetType ? "销售" : "经销商");
		}

		pListElement->SetText(7, pItem->szOutTargetName);

		pListElement->SetText(8, pItem->szOutOperator);

		if (pItem->nOutTargetType != -1) {
			DateTime2String(buf, sizeof(buf), &pItem->tOutTime);
			pListElement->SetText(9, buf);
		}
	}
}

void  CDuiFrameWnd::OnQueryByTagRet(int ret, const TagItem & tTag) {
	TagItem * pTag = new TagItem;
	memcpy( pTag, &tTag, sizeof(TagItem) );
	::PostMessage( GetHWND(), UM_QUERY_BY_TAG_RET, ret, (LPARAM)pTag);
}

void  CDuiFrameWnd::OnQueryByTagMsg(int ret, const TagItem & tTag) {
	SetBusy(FALSE);

	if (0 != ret) {
		::MessageBox(GetHWND(), GetErrorDescription(ret), "查询", 0);
		return;
	}

	// 如果没有该Tag
	if ( '\0' == tTag.szTagId[0] ) {
		return;
	}

	char buf[8192];

	DuiLib::CListTextElementUI* pListElement = new DuiLib::CListTextElementUI;
	m_lstQueryByTag->Add(pListElement);
	
	pListElement->SetText(0, tTag.szBigPkgId);
	pListElement->SetText(1, tTag.szSmallPkgId);
	pListElement->SetText(2, tTag.szTagId);

	if (tTag.dwStatus == PKG_STATUS_OUT) {
		pListElement->SetText(3, "已出库");
	}
	else {
		pListElement->SetText(3, "在库");
	}

	pListElement->SetText(4, tTag.szInOperator);

	DateTime2String(buf, sizeof(buf), &tTag.tInTime);
	pListElement->SetText(5, buf);

	if (tTag.nOutTargetType != -1) {
		pListElement->SetText(6, TARGET_TYPE_SALES == tTag.nOutTargetType ? "销售" : "经销商");
	}

	pListElement->SetText(7, tTag.szOutTargetName);

	pListElement->SetText(8, tTag.szOutOperator);

	if (tTag.nOutTargetType != -1) {
		DateTime2String(buf, sizeof(buf), &tTag.tOutTime);
		pListElement->SetText(9, buf);
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
		m_btnQuery_1->SetEnabled(false);
		m_btnQuery_2->SetEnabled(false);
		m_btnQuery_3->SetEnabled(false);
		m_btnQuery_4->SetEnabled(false);
		m_progress->SetVisible(true);
		m_progress->Start();
	}
	else {
		m_btnInvOk->SetEnabled(true);
		m_btnQuery_1->SetEnabled(true); 
		m_btnQuery_2->SetEnabled(true);
		m_btnQuery_3->SetEnabled(true);
		m_btnQuery_4->SetEnabled(true);
		m_progress->Stop();
		m_progress->SetVisible(false);
	}
	m_bBusy = bBusy;
}

void  CDuiFrameWnd::OnQueryByTime() {
	SYSTEMTIME t1 = m_dateTimeStart->GetTime();
	SYSTEMTIME t2 = m_dateTimeEnd->GetTime();

	t1.wHour = 0;
	t1.wMinute = 0;
	t1.wSecond = 0;

	t2.wHour = 0;
	t2.wMinute = 0;
	t2.wSecond = 0;

	time_t tStartTime = SystemTime2Time(t1);
	time_t tEndTime = SystemTime2Time(t2);

	DuiLib::CDuiString  strText;
	DuiLib::CDuiString  strTargetId;
	int nTargetType = -1;

	strText = m_edTarget_1->GetText();
	if (strText.GetLength() > 0) {
		int nPos = strText.Find(',');
		assert(nPos >= 0);
		strTargetId = strText.Mid(0, nPos);

		if ( m_optSales_1->IsSelected() ) {
			nTargetType = 0;
		}
		else {
			nTargetType = 1;
		}
	}

	m_lstQueryByTime->RemoveAll();
	m_lblSmallCnt_1->SetText("0");
	m_lblBigCnt_1->SetText("0");
	SetBusy();
	CBusiness::GetInstance()->QueryByTimeAsyn(tStartTime, tEndTime, nTargetType, strTargetId); 
	
}

void  CDuiFrameWnd::OnSelectTarget_1() {
	int nTargetType = 0;
	if (m_optSales_1->IsSelected()) {
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

		m_edTarget_1->SetText(strText);
	}

	delete pDlg;
}

void  CDuiFrameWnd::OnExportExcel_1() {
	if ( !CExcelEx::IfExcelInstalled() ) {
		MessageBox(this->GetHWND(), "没有找到excel。请先安装excel", "导出", 0);
		return;
	}

	CExcelEx   excel;
	int nCnt = m_lstQueryByTime->GetCount();

	excel.WriteGrid(0, 0, "序号");
	excel.WriteGrid(0, 1, "包装类型");
	excel.WriteGrid(0, 2, "包装ID");
	excel.WriteGrid(0, 3, "出库目标类型");
	excel.WriteGrid(0, 4, "出库目标名称");
	excel.WriteGrid(0, 5, "操作人员");
	excel.WriteGrid(0, 6, "出库时间");

	for (int i = 0; i < nCnt; i++) {
		DuiLib::CListTextElementUI* pListElement = (DuiLib::CListTextElementUI*)m_lstQueryByTime->GetItemAt(i);
		for (int j = 0; j < 7; j++) {
			excel.WriteGrid(i + 1, j, pListElement->GetText(j) );
		}
	}

	try
	{
		excel.Save();		
	}
	catch (...) {

	}

	try {
		excel.Quit();
	}
	catch (...) {

	}
}

void CDuiFrameWnd::OnQueryByBigPkg() {
	DuiLib::CDuiString  strText;

	strText = m_edBigPackageId->GetText();

	m_lstQueryByBigPkg->RemoveAll();
	m_lstQueryByBigPkg_1->RemoveAll();
	m_lstQueryByBigPkg_1->SetVisible(false);

	char buf[256];
	Str2Upper( strText, buf, sizeof(buf) );

	SetBusy();
	CBusiness::GetInstance()->QueryByBigPkgAsyn(buf);
}

// 根据小包装查询
void CDuiFrameWnd::OnQueryBySmallPkg() {
	DuiLib::CDuiString  strText;

	strText = m_edSmallPackageId->GetText();

	m_lstQueryBySmallPkg->RemoveAll();

	char buf[256];
	Str2Upper(strText, buf, sizeof(buf));

	SetBusy();
	CBusiness::GetInstance()->QueryBySmallPkgAsyn(buf);
}

// 根据Tag查询
void CDuiFrameWnd::OnQueryByTag() {

	DuiLib::CDuiString  strText;

	strText = m_edTagId->GetText();	

	char buf[256];
	Str2Lower(strText, buf, sizeof(buf));
	StrTrim(buf);

	DWORD  dwLen = strlen(buf);
	if (dwLen == 0) {
		::MessageBox(GetHWND(), "请输入完整的Tag ID", "查询Tag", 0);
		return;
	}

	if ( dwLen < 16 ) {
		::MessageBox(GetHWND(), "Tag格式不对", "查询Tag", 0);
		return;
	}

	char szTagId[20] = {0};
	int nStatus = 0;  // 0, 第一个字符，1, 第二个字符，2，间隔符号
	int nIndex = 0;

	for ( DWORD i = 0; i < dwLen; i++ ) {
		if ( 0 == nStatus || 1 == nStatus ) {
			if ( (buf[i] >= '0' && buf[i] <= '9') || (buf[i] >= 'a' && buf[i] <= 'f') ) {

				// 如果id过长
				if (nIndex >= 16) {
					::MessageBox(GetHWND(), "Tag格式不对", "查询Tag", 0);
					return;
				}

				szTagId[nIndex] = buf[i];
				nIndex++;
				nStatus++;
			}
			// 如果是空格，忽略
			else if ( buf[i] == ' ' ) {

			}
			else {
				::MessageBox(GetHWND(), "Tag格式不对", "查询Tag", 0);
				return;
			}
		}
		else {
			assert(nStatus == 2);
			// 如果没有间隔
			if ( (buf[i] >= '0' && buf[i] <= '9') || (buf[i] >= 'a' && buf[i] <= 'f') ) {

				// 如果id过长
				if (nIndex >= 16) {
					::MessageBox(GetHWND(), "Tag格式不对", "查询Tag", 0);
					return;
				}

				szTagId[nIndex] = buf[i];
				nIndex++;
				nStatus = 1;
			}
			// 如果是空格，忽略
			else if (buf[i] == ' ') {

			}
			// 如果是间隔符号
			else if (buf[i] == '-' || buf[i] == ':') {
				nStatus = 0;
			}
			else {
				::MessageBox(GetHWND(), "Tag格式不对", "查询Tag", 0);
				return;
			}
		}
	}

	if ( nIndex != 16 ) {
		::MessageBox(GetHWND(), "Tag格式不对", "查询Tag", 0);
		return;
	}

	m_lstQueryByTag->RemoveAll();

	SetBusy();
	CBusiness::GetInstance()->QueryByTagAsyn(szTagId);

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