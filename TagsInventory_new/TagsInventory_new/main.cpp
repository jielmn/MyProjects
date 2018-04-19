// win32_1.cpp : 定义应用程序的入口点。
//
#include <afx.h>
#include "login.h"
#include "main.h"
#include "resource.h"
#include "Business.h"
#include "39BarCode.h"
#include "LmnString.h"

void  CDuiFrameWnd::InitWindow() {
	char buf[8192];

	PostMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0);

	m_lblDbStatus = (DuiLib::CLabelUI *)m_PaintManager.FindControl(DATABASE_STATUS_LABEL_ID);
	m_lblReaderStatus = (DuiLib::CLabelUI *)m_PaintManager.FindControl(READER_STATUS_LABEL_ID);
	m_lblUser = (DuiLib::CLabelUI *)m_PaintManager.FindControl(USER_LABEL_ID);
	m_tabs = static_cast<DuiLib::CTabLayoutUI*>(m_PaintManager.FindControl(_T(TABS_ID)));

	m_edtBatchId = static_cast<DuiLib::CEditUI*>(m_PaintManager.FindControl(_T(BATCH_ID_EDIT_ID)));
	m_edtPackageId = static_cast<DuiLib::CEditUI*>(m_PaintManager.FindControl(_T(PACKAGE_ID_EDIT_ID)));
	m_lblCountSmall = static_cast<DuiLib::CLabelUI*>(m_PaintManager.FindControl(_T(COUNT_SMALL_LABEL_ID)));

	// 小盘点的按钮
	m_btnStartSmall = static_cast<DuiLib::CButtonUI*>(m_PaintManager.FindControl(_T(START_SMALL_BUTTON_ID)));
	m_btnStopSmall = static_cast<DuiLib::CButtonUI*>(m_PaintManager.FindControl(_T(STOP_SMALL_BUTTON_ID)));
	m_btnSaveSmall = static_cast<DuiLib::CButtonUI*>(m_PaintManager.FindControl(_T(SAVE_SMALL_BUTTON_ID)));
	m_btnPrintSmall = static_cast<DuiLib::CButtonUI*>(m_PaintManager.FindControl(_T(PRINT_SMALL_BUTTON_ID)));
	m_lblInvSmallSaveRet = (DuiLib::CLabelUI *)m_PaintManager.FindControl(INV_SMALL_SAVE_LABEL_ID);

	// check
	m_lblCheckTagId = static_cast<DuiLib::CLabelUI*>(m_PaintManager.FindControl(_T(CHECK_TAG_ID_LABEL_ID)));
	m_lblCheckTagRet = static_cast<DuiLib::CLabelUI*>(m_PaintManager.FindControl(_T(CHECK_TAG_RET_LABEL_ID)));

	// 大盘点
	m_btnStartBig = static_cast<DuiLib::CButtonUI*>(m_PaintManager.FindControl(_T(START_BIG_BUTTON_ID)));
	m_btnStopBig = static_cast<DuiLib::CButtonUI*>(m_PaintManager.FindControl(_T(STOP_BIG_BUTTON_ID)));
	m_btnSaveBig = static_cast<DuiLib::CButtonUI*>(m_PaintManager.FindControl(_T(SAVE_BIG_BUTTON_ID)));
	m_btnPrintBig = static_cast<DuiLib::CButtonUI*>(m_PaintManager.FindControl(_T(PRINT_BIG_BUTTON_ID)));
	m_edtBigBatchId = static_cast<DuiLib::CEditUI*>(m_PaintManager.FindControl(_T(BIG_BATCH_ID_EDIT_ID)));
	m_edtBigPackageId = static_cast<DuiLib::CEditUI*>(m_PaintManager.FindControl(_T(BIG_PACKAGE_ID_EDIT_ID)));
	m_lblInvBigSaveRet = (DuiLib::CLabelUI *)m_PaintManager.FindControl(INV_BIG_SAVE_LABEL_ID);
	m_lblCountBig = static_cast<DuiLib::CLabelUI*>(m_PaintManager.FindControl(_T(COUNT_BIG_LABEL_ID)));	 
	m_lstInvBig = static_cast<DuiLib::CListUI*>(m_PaintManager.FindControl(_T(INV_BIG_LIST_ID)));	 
	m_edtManualSmallPkgId = static_cast<DuiLib::CEditUI*>(m_PaintManager.FindControl(_T(MANUAL_SMALL_PKG_ID_EDIT_ID)));
	m_btnManualSmallPkg = static_cast<DuiLib::CButtonUI*>(m_PaintManager.FindControl(_T(MANUAL_SMALL_PKG_ID_BUTTON_ID)));
	
	// 查询
	m_dtStart = static_cast<DuiLib::CDateTimeUI*>(m_PaintManager.FindControl(_T(QUERY_START_TIME_DATETIME_ID)));
	m_dtEnd = static_cast<DuiLib::CDateTimeUI*>(m_PaintManager.FindControl(_T(QUERY_END_TIME_DATETIME_ID)));
	m_edtQueryBatchId = static_cast<DuiLib::CEditUI*>(m_PaintManager.FindControl(_T(QUERY_BATCH_ID_EDIT_ID)));
	m_edtQueryOperator = static_cast<DuiLib::CEditUI*>(m_PaintManager.FindControl(_T(QUERY_OPERATOR_EDIT_ID)));
	m_cmbQueryType = static_cast<DuiLib::CComboUI*>(m_PaintManager.FindControl(_T(QUERY_TYPE_COMBO_ID)));
	m_lstQueryBig = static_cast<DuiLib::CListUI*>(m_PaintManager.FindControl(_T(QUERY_BIG_LIST_ID)));
	m_lstQuerySmall = static_cast<DuiLib::CListUI*>(m_PaintManager.FindControl(_T(QUERY_SMALL_LIST_ID)));
	m_lstQueryTags = static_cast<DuiLib::CListUI*>(m_PaintManager.FindControl(_T(QUERY_TAGS_LIST_ID)));
	 
	 

	CInvDatabase::DATABASE_STATUS eDbStatus     = CBusiness::GetInstance()->GetDbStatus();
	C601InvReader::READER_STATUS     eReaderStatus = CBusiness::GetInstance()->GetReaderStatus();

	if (eDbStatus == CInvDatabase::STATUS_OPEN) {
		SET_CONTROL_TEXT(m_lblDbStatus, DB_STATUS_OK_TEXT);
	}
	else {
		SET_CONTROL_TEXT(m_lblDbStatus, DB_STATUS_CLOSE_TEXT);
	}

	if (eReaderStatus == C601InvReader::STATUS_OPEN) {
		SET_CONTROL_TEXT(m_lblReaderStatus, READER_STATUS_OK_TEXT);
	}
	else {
		SET_CONTROL_TEXT(m_lblReaderStatus, READER_STATUS_CLOSE_TEXT);
	}

	g_cfg->GetConfig(LAST_BATCH_ID, buf, sizeof(buf), "");
	SET_CONTROL_TEXT(m_edtBatchId, buf);
	SET_CONTROL_TEXT(m_edtPackageId, "");
	SET_CONTROL_TEXT(m_lblCountSmall, "0");
	SET_CONTROL_TEXT(m_lblInvSmallSaveRet, "");
	SET_CONTROL_TEXT(m_lblCheckTagId, "");
	SET_CONTROL_TEXT(m_lblCheckTagRet, "");
	SET_CONTROL_TEXT(m_edtBigBatchId, buf);
	SET_CONTROL_TEXT(m_edtBigPackageId, "");
	SET_CONTROL_TEXT(m_lblInvBigSaveRet, "");
	SET_CONTROL_TEXT(m_lblCountBig, "0");
	SET_CONTROL_TEXT(m_edtManualSmallPkgId, "");
	SET_CONTROL_TEXT(m_edtQueryBatchId, "");
	SET_CONTROL_TEXT(m_edtQueryOperator, "");
	

	CString strText;
	strText = "欢迎您，";
	SET_CONTROL_TEXT(m_lblUser, strText + CBusiness::GetInstance()->GetUserName() );

	// 初始化小盘点
	InitInventorySmall();
	// 初始化大盘点
	InitInventoryBig();


	DWORD dwFontSize = 0;
	g_cfg->GetConfig("font size", dwFontSize, 28);
	m_font = CreateFont(
		(int)dwFontSize,               // nHeight
		0,                             // nWidth
		0,                             // nEscapement
		0,                             // nOrientation
		FW_BOLD,                       // nWeight
		FALSE,                         // bItalic
		FALSE,                         // bUnderline
		0,                             // cStrikeOut
		ANSI_CHARSET,                  // nCharSet
		OUT_DEFAULT_PRECIS,            // nOutPrecision
		CLIP_DEFAULT_PRECIS,           // nClipPrecision
		DEFAULT_QUALITY,               // nQuality
		DEFAULT_PITCH | FF_SWISS,      // nPitchAndFamily
		_T("黑体")
	);

	WindowImplBase::InitWindow();
}

DuiLib::CControlUI * CDuiFrameWnd::CreateControl(LPCTSTR pstrClass) {
	if (0 == strcmp("Inventory_small", pstrClass)) {
		DuiLib::CDialogBuilder builder;
		DuiLib::CControlUI * pUI = builder.Create(_T("Inventory_small.xml"), (UINT)0, 0, &m_PaintManager);
		return pUI;
	}
	else if (0 == strcmp("Inventory_big", pstrClass)) {
		DuiLib::CDialogBuilder builder;
		DuiLib::CControlUI * pUI = builder.Create(_T("Inventory_big.xml"), (UINT)0, 0, &m_PaintManager);
		return pUI;
	} else if (0 == strcmp("Query", pstrClass)) {
		DuiLib::CDialogBuilder builder;
		DuiLib::CControlUI * pUI = builder.Create(_T("Query.xml"), (UINT)0, 0, &m_PaintManager);
		return pUI;
	}
	else if (0 == strcmp("Check", pstrClass)) {
		DuiLib::CDialogBuilder builder;
		DuiLib::CControlUI * pUI = builder.Create(_T("Check.xml"), (UINT)0, 0, &m_PaintManager);
		return pUI;
	}
	return 0;
}

void CDuiFrameWnd::Notify(DuiLib::TNotifyUI& msg) {
	DuiLib::CDuiString name = msg.pSender->GetName();
	DuiLib::CDuiString strText;

	if (msg.sType == _T("selectchanged"))
	{
		if ( 0 == m_tabs ) {
			return;
		}

		if (name == _T("Inventory_small")) {
			m_tabs->SelectItem(TABS_INDEX_INVENTORY_SMALL);
		} else if (name == _T("Inventory_big")) {
			m_tabs->SelectItem(TABS_INDEX_INVENTORY_BIG);
		} else if (name == _T("Query")) {
			m_tabs->SelectItem(TABS_INDEX_INVENTORY_QUERY);
		} else if (name = "Check") {			
			m_tabs->SelectItem(TABS_INDEX_INVENTORY_CHECK);
			SET_CONTROL_TEXT(m_lblCheckTagId, "");
			SET_CONTROL_TEXT(m_lblCheckTagRet, "");
		}
	}
	else if (msg.sType == "click") {
		if (name == START_SMALL_BUTTON_ID) {
			StartInventorySmall();
		}
		else if (name == STOP_SMALL_BUTTON_ID) {
			StopInventorySmall();
		}
		else if (name == SAVE_SMALL_BUTTON_ID) {
			SaveInventorySmall();
		}
		else if (name == PRINT_SMALL_BUTTON_ID) {
			PrintInventorySmall();
		}
		else if (name == START_BIG_BUTTON_ID) {
			StartInventoryBig();
		}
		else if (name == STOP_BIG_BUTTON_ID) {
			StopInventoryBig();
		}
		else if (name == SAVE_BIG_BUTTON_ID) {
			SaveInventoryBig();
		}
		else if (name == PRINT_BIG_BUTTON_ID) {
			PrintInventoryBig();
		}
		else if ( name == MANUAL_SMALL_PKG_ID_BUTTON_ID ) {
			OnManualAddBarcode();
		} else if (name == QUERY_BUTTON_ID) {
			OnQuery();
		}
	}
	else if (msg.sType == "textchanged") {
		if (name == BATCH_ID_EDIT_ID) {
			strText = GET_CONTROL_TEXT(m_edtBatchId);
			g_cfg->SetConfig(LAST_BATCH_ID, (const char *)strText);
		}
		else if (name == BIG_BATCH_ID_EDIT_ID) {
			strText = GET_CONTROL_TEXT(m_edtBigBatchId);
			g_cfg->SetConfig(LAST_BATCH_ID, (const char *)strText);
		}
	}
	else if (msg.sType == "itemselect") {
		if (name == QUERY_BIG_LIST_ID) {
			OnQueryBigListChanged();
		}
		else if (name == QUERY_SMALL_LIST_ID) {
			OnQuerySmallListChanged();
		}
	}
	WindowImplBase::Notify(msg);
}

// 初始化小盘点
void  CDuiFrameWnd::InitInventorySmall() {
	if (m_btnStartSmall) {
		m_btnStartSmall->SetEnabled(true);
	}

	if ( m_btnStopSmall ) {
		m_btnStopSmall->SetEnabled(false);
	}

	if (m_btnSaveSmall) {
		m_btnSaveSmall->SetEnabled(false);
	}

	if (m_btnPrintSmall) {
		m_btnPrintSmall->SetEnabled(false);
	}	

	ClearVector(m_vInventorySmall);
	m_InventorySmallStatus = STATUS_CLOSE;
}

// 开始小盘点
void  CDuiFrameWnd::StartInventorySmall() {
	if ( m_InventorySmallStatus == STATUS_STOP ) {
		if (m_vInventorySmall.size() > 0) {
			if ( IDCANCEL == MessageBox(this->GetHWND(), MSG_BOX_NEW_INVENTORY_SMALL, CAPTION_NEW_INVENTORY_SMALL, MB_OKCANCEL | MB_DEFBUTTON2)) {
				return;
			}
		}
	}

	if (m_btnStartSmall) {
		m_btnStartSmall->SetEnabled(false);
	}

	if (m_btnStopSmall) {
		m_btnStopSmall->SetEnabled(true);
	}

	if (m_btnSaveSmall) {
		m_btnSaveSmall->SetEnabled(false);
	}

	if (m_btnPrintSmall) {
		m_btnPrintSmall->SetEnabled(false);
	}

	ClearVector(m_vInventorySmall);
	SET_CONTROL_TEXT(m_lblCountSmall, "0");

	SET_CONTROL_TEXT_COLOR(m_lblInvSmallSaveRet, NORMAL_COLOR);
	SET_CONTROL_TEXT(m_lblInvSmallSaveRet, "");
	SET_CONTROL_TEXT(m_edtPackageId, "");

	m_InventorySmallStatus = STATUS_START;
}

// 停止盘点
void  CDuiFrameWnd::StopInventorySmall() {
	if (m_btnStartSmall) {
		m_btnStartSmall->SetEnabled(true);
	}

	if (m_btnStopSmall) {
		m_btnStopSmall->SetEnabled(false);
	}

	if (m_btnSaveSmall) {
		if (m_vInventorySmall.size() > 0) {
			m_btnSaveSmall->SetEnabled(true);
		}
		else {
			m_btnSaveSmall->SetEnabled(false);
		}
	}

	if (m_btnPrintSmall) {
		m_btnPrintSmall->SetEnabled(false);
	}

	m_InventorySmallStatus = STATUS_STOP;
}

// 保存盘点
void  CDuiFrameWnd::SaveInventorySmall() {

	if ( CInvDatabase::STATUS_CLOSE == CBusiness::GetInstance()->GetDbStatus() ) {
		MessageBox( this->GetHWND(), MSG_BOX_DB_CLOSE, CAPTION_SAVE_INVENTORY_SMALL, 0 );
		return;
	}

	DuiLib::CDuiString str = GET_CONTROL_TEXT(m_edtBatchId);
	CString strBatchId = str;

	if (strBatchId.GetLength() == 0) {
		MessageBox( this->GetHWND(), MSG_BOX_BATCH_ID_EMPTY, CAPTION_SAVE_INVENTORY_SMALL, 0 );
		return;
	}

	// 201709001 或者20170901
	if (strBatchId.GetLength() != 8) {
		MessageBox(this->GetHWND(), MSG_BOX_WRONG_BATCH_ID_FORMAT, CAPTION_SAVE_INVENTORY_SMALL, 0 );
		return;
	}

	int nBatchId = 0;
	if (0 == sscanf(strBatchId.Mid(0, 4), "%d", &nBatchId)) {
		MessageBox(this->GetHWND(), MSG_BOX_WRONG_BATCH_ID_FORMAT, CAPTION_SAVE_INVENTORY_SMALL, 0);
		return;
	}

	if (0 == sscanf(strBatchId.Mid(4, 2), "%d", &nBatchId)) {
		MessageBox(this->GetHWND(), MSG_BOX_WRONG_BATCH_ID_FORMAT, CAPTION_SAVE_INVENTORY_SMALL, 0);
		return;
	}

	if (0 == sscanf(strBatchId.Mid(6), "%d", &nBatchId)) {
		MessageBox(this->GetHWND(), MSG_BOX_WRONG_BATCH_ID_FORMAT, CAPTION_SAVE_INVENTORY_SMALL, 0);
		return;
	}

	// 保存
	int ret = CBusiness::GetInstance()->InvSmallSaveAsyn(strBatchId, m_vInventorySmall);
	if (0 != ret) {
		return;
	}

	// 保存成功后的处理
	if (m_btnStartSmall) {
		m_btnStartSmall->SetEnabled(true);
	}

	if (m_btnStopSmall) {
		m_btnStopSmall->SetEnabled(false);
	}

	if (m_btnSaveSmall) {
		m_btnSaveSmall->SetEnabled(false);
	}

	if (m_btnPrintSmall) {
		m_btnPrintSmall->SetEnabled(false);
	}

	m_InventorySmallStatus = STATUS_SAVING;
}

// 打印条码
void  CDuiFrameWnd::PrintInventorySmall() {
	
	DWORD   dwPaperWidth = 0;
	DWORD   dwPaperLength = 0;
	DWORD   dwLeft = 0;
	DWORD   dwTop = 0;
	DWORD   dwPrintWidth = 0;
	DWORD   dwPrintHeight = 0;
	DWORD   dwTextHeight = 0;

	g_cfg->Reload();
	g_cfg->GetConfig("paper width", dwPaperWidth, 600);
	g_cfg->GetConfig("paper length", dwPaperLength, 170);
	g_cfg->GetConfig("paper left", dwLeft, 0);
	g_cfg->GetConfig("paper top", dwTop, 0);
	g_cfg->GetConfig("print width", dwPrintWidth, dwPaperWidth);
	g_cfg->GetConfig("print height", dwPrintHeight, dwPaperLength);
	g_cfg->GetConfig("bar code text height", dwTextHeight, 50);


	PRINTDLG printInfo;
	ZeroMemory(&printInfo, sizeof(printInfo));  //清空该结构     
	printInfo.lStructSize = sizeof(printInfo);
	printInfo.hwndOwner = 0;
	printInfo.hDevMode = 0;
	printInfo.hDevNames = 0;
	//这个是关键，PD_RETURNDC 如果不设这个标志，就拿不到hDC了      
	//            PD_RETURNDEFAULT 这个就是得到默认打印机，不需要弹设置对话框     
	//printInfo.Flags = PD_RETURNDC | PD_RETURNDEFAULT;   
	printInfo.Flags = PD_USEDEVMODECOPIESANDCOLLATE | PD_RETURNDC | PD_RETURNDEFAULT;
	printInfo.nCopies = 1;
	printInfo.nFromPage = 0xFFFF;
	printInfo.nToPage = 0xFFFF;
	printInfo.nMinPage = 1;
	printInfo.nMaxPage = 0xFFFF;

	//调用API拿出默认打印机     
	PrintDlg(&printInfo);
	//if (PrintDlg(&printInfo)==TRUE) 
	{
		LPDEVMODE lpDevMode = (LPDEVMODE)::GlobalLock(printInfo.hDevMode);

		if (lpDevMode) {
			lpDevMode->dmPaperSize = DMPAPER_USER;
			lpDevMode->dmFields = lpDevMode->dmFields | DM_PAPERSIZE | DM_PAPERLENGTH | DM_PAPERWIDTH;
			lpDevMode->dmPaperWidth = (short)dwPaperWidth;
			lpDevMode->dmPaperLength = (short)dwPaperLength;
			lpDevMode->dmOrientation = DMORIENT_PORTRAIT;
		}
		GlobalUnlock(printInfo.hDevMode);
		ResetDC(printInfo.hDC, lpDevMode);

		DOCINFO di;
		ZeroMemory(&di, sizeof(DOCINFO));
		di.cbSize = sizeof(DOCINFO);
		di.lpszDocName = _T("MyXPS");
		StartDoc(printInfo.hDC, &di);
		StartPage(printInfo.hDC);

		CDC *pDC = CDC::FromHandle(printInfo.hDC);
		pDC->SetMapMode(MM_ANISOTROPIC); //转换坐标映射方式

		DWORD dwMultiple = 0;
		g_cfg->GetConfig("multiple", dwMultiple);

		CSize size = CSize(dwPaperWidth, dwPaperLength);
		pDC->SetWindowExt(size);
		pDC->SetViewportExt(pDC->GetDeviceCaps(HORZRES), pDC->GetDeviceCaps(VERTRES));

		DuiLib::CDuiString strBatchId = GET_CONTROL_TEXT(m_edtPackageId);

		// 画条码
		DrawBarcode128(pDC->m_hDC, dwLeft, dwTop, dwPrintWidth, dwPrintHeight, (const char *)strBatchId, m_font, dwTextHeight, "S/N:");

		EndPage(printInfo.hDC);
		EndDoc(printInfo.hDC);

		// Delete DC when done.
		DeleteDC(printInfo.hDC);
	}
}


void  CDuiFrameWnd::InitInventoryBig() {
	SET_CONTROL_ENABLED(m_btnStartBig, true);
	SET_CONTROL_ENABLED(m_btnStopBig, false);
	SET_CONTROL_ENABLED(m_btnSaveBig, false);
	SET_CONTROL_ENABLED(m_btnPrintBig, false);
	SET_CONTROL_ENABLED(m_lstInvBig, false);
	SET_CONTROL_ENABLED(m_edtManualSmallPkgId, false);
	SET_CONTROL_ENABLED(m_btnManualSmallPkg, false);

	ClearVector(m_vInventoryBig);
	m_InventoryBigStatus = STATUS_CLOSE;
}

void  CDuiFrameWnd::StartInventoryBig() {
	if (m_InventoryBigStatus == STATUS_STOP) {
		if (m_vInventoryBig.size() > 0) {
			if (IDCANCEL == MessageBox(this->GetHWND(), MSG_BOX_NEW_INVENTORY_BIG, CAPTION_NEW_INVENTORY_BIG, MB_OKCANCEL | MB_DEFBUTTON2)) {
				return;
			}
		}
	}

	SET_CONTROL_ENABLED(m_btnStartBig, false);
	SET_CONTROL_ENABLED(m_btnStopBig, true);
	SET_CONTROL_ENABLED(m_btnSaveBig, false);
	SET_CONTROL_ENABLED(m_btnPrintBig, false);

	SET_CONTROL_ENABLED(m_lstInvBig, true);
	SET_CONTROL_ENABLED(m_edtManualSmallPkgId, true);
	SET_CONTROL_ENABLED(m_btnManualSmallPkg, true);

	ClearVector(m_vInventoryBig);
	SET_CONTROL_TEXT(m_lblCountBig, "0");
	if (m_lstInvBig) {
		m_lstInvBig->RemoveAll();
	}
	

	SET_CONTROL_TEXT_COLOR(m_lblInvBigSaveRet, NORMAL_COLOR);
	SET_CONTROL_TEXT(m_lblInvBigSaveRet, "");
	SET_CONTROL_TEXT(m_edtBigPackageId, "");

	m_InventoryBigStatus = STATUS_START;
}

void  CDuiFrameWnd::StopInventoryBig() {
	SET_CONTROL_ENABLED(m_btnStartBig, true);
	SET_CONTROL_ENABLED(m_btnStopBig, false);

	if (m_btnSaveBig) {
		if (m_vInventoryBig.size() > 0) {
			m_btnSaveBig->SetEnabled(true);
		}
		else {
			m_btnSaveBig->SetEnabled(false);
		}
	}
	SET_CONTROL_ENABLED(m_btnPrintBig, false);

	SET_CONTROL_ENABLED(m_lstInvBig, false);
	SET_CONTROL_ENABLED(m_edtManualSmallPkgId, false);
	SET_CONTROL_ENABLED(m_btnManualSmallPkg, false);

	m_InventoryBigStatus = STATUS_STOP;
}

void  CDuiFrameWnd::SaveInventoryBig() {
	if (CInvDatabase::STATUS_CLOSE == CBusiness::GetInstance()->GetDbStatus()) {
		MessageBox(this->GetHWND(), MSG_BOX_DB_CLOSE, CAPTION_SAVE_INVENTORY_BIG, 0);
		return;
	}

	DuiLib::CDuiString str = GET_CONTROL_TEXT(m_edtBatchId);
	CString strBatchId = str;

	if (strBatchId.GetLength() == 0) {
		MessageBox(this->GetHWND(), MSG_BOX_BATCH_ID_EMPTY, CAPTION_SAVE_INVENTORY_BIG, 0);
		return;
	}

	// 201709001 或者20170901
	if (strBatchId.GetLength() != 8) {
		MessageBox(this->GetHWND(), MSG_BOX_WRONG_BATCH_ID_FORMAT, CAPTION_SAVE_INVENTORY_BIG, 0);
		return;
	}

	int nBatchId = 0;
	if (0 == sscanf(strBatchId.Mid(0, 4), "%d", &nBatchId)) {
		MessageBox(this->GetHWND(), MSG_BOX_WRONG_BATCH_ID_FORMAT, CAPTION_SAVE_INVENTORY_BIG, 0);
		return;
	}

	if (0 == sscanf(strBatchId.Mid(4, 2), "%d", &nBatchId)) {
		MessageBox(this->GetHWND(), MSG_BOX_WRONG_BATCH_ID_FORMAT, CAPTION_SAVE_INVENTORY_BIG, 0);
		return;
	}

	if (0 == sscanf(strBatchId.Mid(6), "%d", &nBatchId)) {
		MessageBox(this->GetHWND(), MSG_BOX_WRONG_BATCH_ID_FORMAT, CAPTION_SAVE_INVENTORY_BIG, 0);
		return;
	}

	char  szFactoryId[64];
	char  szProductId[64];

	g_cfg->GetConfig(FACTORY_CODE, szFactoryId, sizeof(szFactoryId),"");
	g_cfg->GetConfig(PRODUCT_CODE, szProductId, sizeof(szProductId),"");

	DWORD  dwFactoryLen = strlen(szFactoryId);
	DWORD  dwProductLen = strlen(szProductId);

	std::vector<CString *>::iterator it;
	for (it = m_vInventoryBig.begin(); it != m_vInventoryBig.end(); it++) {
		CString * pItem = *it;
		if ( 0 != StrICmp(pItem->Mid( dwFactoryLen + dwProductLen, 8 ), strBatchId)) {
			MessageBox(this->GetHWND(), MSG_BOX_BATCH_NOT_MATCH, CAPTION_SAVE_INVENTORY_BIG, 0);			
			return;
		}
		if ( 0 != StrICmp(pItem->Mid(0, dwFactoryLen), szFactoryId) ) {
			MessageBox(this->GetHWND(), MSG_BOX_FACTORY_NOT_MATCH, CAPTION_SAVE_INVENTORY_BIG, 0);
			return;			
		}
		if (0 != StrICmp(pItem->Mid(dwFactoryLen, dwProductLen), szProductId)) {
			MessageBox(this->GetHWND(), MSG_BOX_PRODUCT_NOT_MATCH, CAPTION_SAVE_INVENTORY_BIG, 0);
			return;
		}
	}

	// 保存
	int ret = CBusiness::GetInstance()->InvBigSaveAsyn(strBatchId, m_vInventoryBig );
	if (0 != ret) {
		return;
	}

	// 保存成功后的处理
	SET_CONTROL_ENABLED(m_btnStartBig, true);
	SET_CONTROL_ENABLED(m_btnStopBig, false);
	SET_CONTROL_ENABLED(m_btnSaveBig, false);
	SET_CONTROL_ENABLED(m_btnPrintBig, false);

	m_InventoryBigStatus = STATUS_SAVING;
}

void  CDuiFrameWnd::PrintInventoryBig() {
	DWORD   dwPaperWidth = 0;
	DWORD   dwPaperLength = 0;
	DWORD   dwLeft = 0;
	DWORD   dwTop = 0;
	DWORD   dwPrintWidth = 0;
	DWORD   dwPrintHeight = 0;
	DWORD   dwTextHeight = 0;

	g_cfg->Reload();
	g_cfg->GetConfig("paper width", dwPaperWidth, 600);
	g_cfg->GetConfig("paper length", dwPaperLength, 170);
	g_cfg->GetConfig("paper left big", dwLeft, 0);
	g_cfg->GetConfig("paper top", dwTop, 0);
	g_cfg->GetConfig("print width", dwPrintWidth, dwPaperWidth);
	g_cfg->GetConfig("print height", dwPrintHeight, dwPaperLength);
	g_cfg->GetConfig("bar code text height", dwTextHeight, 50);


	PRINTDLG printInfo;
	ZeroMemory(&printInfo, sizeof(printInfo));  //清空该结构     
	printInfo.lStructSize = sizeof(printInfo);
	printInfo.hwndOwner = 0;
	printInfo.hDevMode = 0;
	printInfo.hDevNames = 0;
	//这个是关键，PD_RETURNDC 如果不设这个标志，就拿不到hDC了      
	//            PD_RETURNDEFAULT 这个就是得到默认打印机，不需要弹设置对话框     
	//printInfo.Flags = PD_RETURNDC | PD_RETURNDEFAULT;   
	printInfo.Flags = PD_USEDEVMODECOPIESANDCOLLATE | PD_RETURNDC | PD_RETURNDEFAULT;
	printInfo.nCopies = 1;
	printInfo.nFromPage = 0xFFFF;
	printInfo.nToPage = 0xFFFF;
	printInfo.nMinPage = 1;
	printInfo.nMaxPage = 0xFFFF;

	//调用API拿出默认打印机     
	PrintDlg(&printInfo);
	//if (PrintDlg(&printInfo)==TRUE) 
	{
		LPDEVMODE lpDevMode = (LPDEVMODE)::GlobalLock(printInfo.hDevMode);

		if (lpDevMode) {
			lpDevMode->dmPaperSize = DMPAPER_USER;
			lpDevMode->dmFields = lpDevMode->dmFields | DM_PAPERSIZE | DM_PAPERLENGTH | DM_PAPERWIDTH;
			lpDevMode->dmPaperWidth = (short)dwPaperWidth;
			lpDevMode->dmPaperLength = (short)dwPaperLength;
			lpDevMode->dmOrientation = DMORIENT_PORTRAIT;
		}
		GlobalUnlock(printInfo.hDevMode);
		ResetDC(printInfo.hDC, lpDevMode);

		DOCINFO di;
		ZeroMemory(&di, sizeof(DOCINFO));
		di.cbSize = sizeof(DOCINFO);
		di.lpszDocName = _T("MyXPS");
		StartDoc(printInfo.hDC, &di);
		StartPage(printInfo.hDC);

		CDC *pDC = CDC::FromHandle(printInfo.hDC);
		pDC->SetMapMode(MM_ANISOTROPIC); //转换坐标映射方式

		CSize size = CSize(dwPaperWidth, dwPaperLength);
		pDC->SetWindowExt(size);
		pDC->SetViewportExt(pDC->GetDeviceCaps(HORZRES), pDC->GetDeviceCaps(VERTRES));

		DuiLib::CDuiString strBigBatchId = GET_CONTROL_TEXT(m_edtBigPackageId);

		// 画条码
		DrawBarcode128(pDC->m_hDC, dwLeft, dwTop, dwPrintWidth, dwPrintHeight, (const char *)strBigBatchId, m_font, dwTextHeight, "S/N:");

		EndPage(printInfo.hDC);
		EndDoc(printInfo.hDC);

		// Delete DC when done.
		DeleteDC(printInfo.hDC);
	}
}


// 处理自定义信息
LRESULT CDuiFrameWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	int nError = 0;
	char buf[8192];

	if ( uMsg == UM_INVENTORY_RESULT ) {
		TagItem * pItem = (TagItem *)wParam;

		if (m_tabs) {
			if (TABS_INDEX_INVENTORY_SMALL == m_tabs->GetCurSel()) {
				OnInventorySmallRet(pItem);
			}
			else if (TABS_INDEX_INVENTORY_CHECK == m_tabs->GetCurSel()) {
				OnInventoryCheckRet(pItem);
			}
		}		
			
		if (pItem) {
			delete pItem;
		}
		return 0;
	}
	else if (uMsg == UM_INV_SMALL_SAVE_RESULT) {
		nError = (int)wParam;
		CString * pPackageId = (CString *)lParam;

		if (0 == nError) {
			SET_CONTROL_TEXT_COLOR(m_lblInvSmallSaveRet, NORMAL_COLOR);
			SET_CONTROL_TEXT(m_lblInvSmallSaveRet, INV_SMALL_SAVE_RET_OK);
			SET_CONTROL_TEXT(m_edtPackageId, *pPackageId );

			if (m_btnPrintSmall) {
				m_btnPrintSmall->SetEnabled(true);
			}
		}
		else if (INV_ERR_DB_NOT_UNIQUE == nError) {
			SET_CONTROL_TEXT_COLOR(m_lblInvSmallSaveRet, ERROR_COLOR);
			SET_CONTROL_TEXT(m_lblInvSmallSaveRet, INV_SMALL_SAVE_RET_SAME_TAG);
		}
		else {
			SET_CONTROL_TEXT_COLOR(m_lblInvSmallSaveRet, ERROR_COLOR);
			SET_CONTROL_TEXT(m_lblInvSmallSaveRet, INV_SMALL_SAVE_RET_ERROR);
		}
		m_InventorySmallStatus = STATUS_SAVED;

		if (pPackageId) {
			delete pPackageId;
		}
	}
	else if (uMsg == UM_CHECK_TAG_RESULT) {
		nError = (int)wParam;
		TagItem * pItem = (TagItem *)lParam;

		GetUid(buf, sizeof(buf), pItem->abyUid, pItem->dwUidLen, '-');
		SET_CONTROL_TEXT(m_lblCheckTagId, buf);

		if (0 == nError) {
			SET_CONTROL_TEXT_COLOR(m_lblCheckTagRet, NORMAL_COLOR);
			SET_CONTROL_TEXT(m_lblCheckTagRet, CHECK_TAG_RET_OK);
		}
		// 如果是断开连接
		else if (INV_ERR_DB_CLOSE == nError) {
			SET_CONTROL_TEXT_COLOR(m_lblCheckTagRet, NORMAL_COLOR);
			SET_CONTROL_TEXT(m_lblCheckTagRet, "");
		}
		else {
			SET_CONTROL_TEXT_COLOR(m_lblCheckTagRet, ERROR_COLOR);
			SET_CONTROL_TEXT(m_lblCheckTagRet, CHECK_TAG_RET_ERROR);
		}

		if (pItem) {
			delete pItem;
		}
	}
	else if ( uMsg == WM_CHAR ) {
		char ch = (char)wParam;
		//g_log->Output(ILog::LOG_SEVERITY_INFO, "char:%c,%d\n", ch,(int)ch);

		if (m_tabs) {
			if (TABS_INDEX_INVENTORY_BIG == m_tabs->GetCurSel() ) {
				OnInvBigChar( ch );
			}
		}

	}
	else if (uMsg == UM_TIMER) {
		DWORD dwTimerId = (DWORD)wParam;

		// 如果定时器时间到
		if (dwTimerId == INV_BIG_CHAR_TIMER) {			
			OnInvBigBarCode(m_strInvBigBuf);
			m_strInvBigBuf = "";
		}
	}
	else if (uMsg == UM_INV_BIG_SAVE_RESULT) {
		nError = (int)wParam;
		CString * pPackageId = (CString *)lParam;
		CString  strText;

		if (0 == nError) {
			SET_CONTROL_TEXT_COLOR(m_lblInvBigSaveRet, NORMAL_COLOR);
			SET_CONTROL_TEXT(m_lblInvBigSaveRet, INV_BIG_SAVE_RET_OK);
			SET_CONTROL_TEXT(m_edtBigPackageId, *pPackageId);

			SET_CONTROL_ENABLED(m_btnPrintBig, true);
		} 
		else if (INV_ERR_DB_SMALL_PKG_IN_USE == nError) {
			SET_CONTROL_TEXT_COLOR(m_lblInvBigSaveRet, ERROR_COLOR);
			strText.Format("小包装 %s 已经打包到大包装", *pPackageId);
			SET_CONTROL_TEXT(m_lblInvBigSaveRet, strText);
		}
		else if (INV_ERR_DB_SMALL_PKG_NOT_FOUND == nError) {
			SET_CONTROL_TEXT_COLOR(m_lblInvBigSaveRet, ERROR_COLOR);
			strText.Format("小包装 %s 不存在", *pPackageId);
			SET_CONTROL_TEXT(m_lblInvBigSaveRet, strText);
		}
		else {
			SET_CONTROL_TEXT_COLOR(m_lblInvBigSaveRet, ERROR_COLOR);
			SET_CONTROL_TEXT(m_lblInvBigSaveRet, INV_BIG_SAVE_RET_ERROR);
		}
		m_InventoryBigStatus = STATUS_SAVED;

		if (pPackageId) {
			delete pPackageId;
		}
	}
	else if (uMsg == UM_SHOW_READER_STATUS) {
		C601InvReader::READER_STATUS eStatus = (C601InvReader::READER_STATUS)wParam;
		if (eStatus == C601InvReader::STATUS_OPEN) {
			SET_CONTROL_TEXT(m_lblReaderStatus, READER_STATUS_OK_TEXT);
		}
		else {
			SET_CONTROL_TEXT(m_lblReaderStatus, READER_STATUS_CLOSE_TEXT);
		}
		return 0;
	}
	else if ( uMsg == UM_SHOW_DB_STATUS) {
		CInvDatabase::DATABASE_STATUS eStatus = (CInvDatabase::DATABASE_STATUS)wParam;
		if (eStatus == CInvDatabase::STATUS_OPEN) {
			SET_CONTROL_TEXT(m_lblDbStatus, DB_STATUS_OK_TEXT);
		}
		else {
			SET_CONTROL_TEXT(m_lblDbStatus, DB_STATUS_CLOSE_TEXT);
		}
		return 0;
	}
	else if (uMsg == UM_QUERY_RESULT) {
		nError = (int)wParam;
		QueryResult * pRet = (QueryResult *)lParam;

		OnQueryResult(pRet);

		if ( pRet ) {
			if (pRet->pvRet) {
				ClearVector(*pRet->pvRet);
				delete pRet->pvRet;
			}			
			delete pRet;
		}
	}
	else if (uMsg == UM_QUERY_SMALL_RESULT) {
		nError = (int)wParam;
		std::vector<QuerySmallResultItem*> * pVec = (std::vector<QuerySmallResultItem*> *)lParam;

		OnQuerySmallResult(nError, pVec);

		if (pVec) {
			ClearVector(*pVec);
			delete pVec;
		}
	}
	else if (uMsg == UM_QUERY_BIG_RESULT) {
		nError = (int)wParam;
		std::vector<QueryResultItem*> * pVec = (std::vector<QueryResultItem*> *)lParam;

		OnQueryBigResult(nError, pVec);

		if (pVec) {
			ClearVector(*pVec);
			delete pVec;
		}
	}
	return WindowImplBase::HandleMessage(uMsg, wParam, lParam);
}

void CDuiFrameWnd::OnInventorySmallRet( const TagItem * pItem ) {

	// 如果不是开始状态
	if (m_InventorySmallStatus != STATUS_START) {
		return;
	}

	// 如果是用户登录的卡片
	if (CBusiness::GetInstance()->IfUserId(pItem)) {
		return;
	}

	int ret = 0;
	int nFindIndex = -1;

	if (m_vInventorySmall.size() > 0) {
		// 用二分法
		int begin  = 0;
		int end    = m_vInventorySmall.size() - 1;
		int middle = 0;		

		do {
			middle = (begin + end) / 2;
			TagItem * pInvItem = m_vInventorySmall.at(middle);
			ret = CompTag(pItem, pInvItem);

			if (ret < 0) {
				if ( middle <= begin ) {
					nFindIndex = middle;
					break;
				}
				end = middle - 1;
			}
			else if (ret > 0) {
				if (middle >= end) {
					nFindIndex = middle;
					break;
				}
				begin = middle + 1;
			}
			// 刚好找到
			else {
				nFindIndex = middle;
				break;
			}
		} while (true);

		// 如果找到
		if ( 0 == ret ) {
			return;
		}
	}

	
	TagItem * pNewItem = new TagItem;
	if (0 == pNewItem) {
		g_log->Output(ILog::LOG_SEVERITY_ERROR, "no memory!\n");
		return;
	}
	memcpy(pNewItem, pItem, sizeof(TagItem));

	//m_vInventorySmall.push_back(pNewItem);
	if (nFindIndex >= 0) {
		assert(ret != 0);
		if ( ret < 0 ) {
			m_vInventorySmall.insert(m_vInventorySmall.begin() + nFindIndex, pNewItem);
		}
		else {
			m_vInventorySmall.insert(m_vInventorySmall.begin() + nFindIndex + 1, pNewItem);
		}
	}
	else {
		m_vInventorySmall.push_back(pNewItem);
	}

	DuiLib::CDuiString strText;
	strText.Format( "%lu", m_vInventorySmall.size());

	SET_CONTROL_TEXT(m_lblCountSmall, strText);
}


void  CDuiFrameWnd::OnInventoryCheckRet(const TagItem * pItem) {
	static DWORD  dwLastTick = 0;

	DWORD dwCur = LmnGetTickCount();
	// 不要发送太频繁
	if (dwCur - dwLastTick < 1000) {
		return;
	}
	dwLastTick = dwCur;

	CBusiness::GetInstance()->CheckTagAsyn(pItem);
}

// 大盘点收到char
void   CDuiFrameWnd::OnInvBigChar(char ch) {
	if (m_InventoryBigStatus == STATUS_START) {
		m_strInvBigBuf += ch;
		CBusiness::GetInstance()->SetTimer(INV_BIG_CHAR_TIMER, INV_BIG_CHAR_TIMER_INTEVAL);
	}
}


int CDuiFrameWnd::OnInvBigBarCode( const CString & strBarCode ) {
	// g_log->Output(ILog::LOG_SEVERITY_INFO, "received chars:%s\n", m_strInvBigBuf);

	// 检查格式(15位)
	// FACTORY CODE   +   PRODUCT CODE   +    批号(20180301)  + 流水号(001)

	char  szFactoryId[64] = { 0 };
	char  szProductId[64] = { 0 };

	g_cfg->GetConfig(FACTORY_CODE, szFactoryId, sizeof(szFactoryId), "");
	g_cfg->GetConfig(PRODUCT_CODE, szProductId, sizeof(szProductId), "");

	DWORD  dwFactoryLen = strlen(szFactoryId);
	DWORD  dwProductLen = strlen(szProductId);

	if (strBarCode.GetLength() != dwFactoryLen + dwProductLen + 8 + FLOW_NUM_LEN) {
		g_log->Output(ILog::LOG_SEVERITY_INFO, "received barcode:%s, length not correct!\n", strBarCode);
		return 1;
	}

	CString strBatchId = strBarCode.Mid(dwFactoryLen + dwProductLen, 8);

	int nBatchId = 0;
	if (0 == sscanf(strBatchId.Mid(0, 4), "%d", &nBatchId)) {
		g_log->Output(ILog::LOG_SEVERITY_INFO, "received barcode:%s, format correct!\n", strBarCode);
		return 2;
	}

	if (0 == sscanf(strBatchId.Mid(4, 2), "%d", &nBatchId)) {
		g_log->Output(ILog::LOG_SEVERITY_INFO, "received barcode:%s, format correct!\n", strBarCode);
		return 2;
	}

	if (0 == sscanf(strBatchId.Mid(6), "%d", &nBatchId)) {
		g_log->Output(ILog::LOG_SEVERITY_INFO, "received barcode:%s, format correct!\n", strBarCode);
		return 2;
	}

	CString strFlowId = strBarCode.Mid(dwFactoryLen + dwProductLen + 8);
	int nFlowId = 0;
	if (0 == sscanf( strFlowId, "%d", &nFlowId)) {
		g_log->Output(ILog::LOG_SEVERITY_INFO, "received barcode:%s, format correct!\n", strBarCode);
		return 2;
	}

	// g_log->Output(ILog::LOG_SEVERITY_INFO, "received barcode:%s, OK \n", strBarCode);

	std::vector<CString *>::iterator it;
	for (it = m_vInventoryBig.begin(); it != m_vInventoryBig.end(); it++) {
		CString * pItem = *it;
		// 已经存在相同的编号
		if ( 0 == StrICmp(*pItem, strBarCode)) {
			return 3;
		}
	}

	CString * pNewBarcode = new CString( strBarCode );
	m_vInventoryBig.push_back(pNewBarcode);

	// 显示
	if (m_lstInvBig) {
		DuiLib::CListTextElementUI* pListElement = new DuiLib::CListTextElementUI;
		m_lstInvBig->Add(pListElement);
		pListElement->SetText(INDEX_INV_BIG_PACKAGE_ID, strBarCode);
	}

	DuiLib::CDuiString  strText;
	strText.Format("%d", m_vInventoryBig.size());
	SET_CONTROL_TEXT(m_lblCountBig, strText);
	
	return 0;
}


void CDuiFrameWnd::OnManualAddBarcode() {
	char buf[8192];
	if (m_InventoryBigStatus == STATUS_START) {
		CString strPkgId = (const char *)GET_CONTROL_TEXT(m_edtManualSmallPkgId);
		Str2Upper( strPkgId, buf, sizeof(buf) );
		strPkgId = buf;
		int ret = OnInvBigBarCode(strPkgId);
		if (0 == ret) {
			SET_CONTROL_TEXT(m_edtManualSmallPkgId, "");
		}
	}
}


void CDuiFrameWnd::OnQuery() {
	if (0 == m_dtStart || 0 == m_dtEnd || 0 == m_edtQueryBatchId || 0 == m_edtQueryOperator || 0 == m_cmbQueryType || m_lstQuerySmall == 0 || m_lstQueryBig == 0 || m_lstQueryTags == 0) {
		return;
	}

	//m_lstQuerySmall->RemoveAll();
	//m_lstQueryBig->RemoveAll();
	//m_lstQueryTags->RemoveAll();

	SYSTEMTIME st = m_dtStart->GetTime();
	time_t tStartTime = ConvertDateTime(st);

	st = m_dtEnd->GetTime();
	time_t tEndTime = ConvertDateTime(st);

	DuiLib::CDuiString strBatchId = GET_CONTROL_TEXT(m_edtQueryBatchId);
	DuiLib::CDuiString strOperator = GET_CONTROL_TEXT(m_edtQueryOperator);
	int nType = m_cmbQueryType->GetCurSel();

	CBusiness::GetInstance()->QueryAsyn(tStartTime, tEndTime, strBatchId, strOperator, nType);
}

// 查询结果
void CDuiFrameWnd::OnQueryResult(const QueryResult * pRet) {
	DuiLib::CListUI * pListUi = 0;

	if (m_lstQueryBig)
		m_lstQueryBig->RemoveAll();

	if (m_lstQuerySmall)
		m_lstQuerySmall->RemoveAll();

	if (m_lstQueryTags)
		m_lstQueryTags->RemoveAll();

	// 小包装
	if ( pRet->nQueryType == 0 ) {
		pListUi = m_lstQuerySmall;
	}
	// 大包装
	else {
		pListUi = m_lstQueryBig;
	}

	if (0 == pListUi) {
		return;
	}

	std::vector<QueryResultItem*>::const_iterator it;
	const std::vector<QueryResultItem*> & v = *pRet->pvRet;

	int i = 0;
	CString strText;
	for (it = v.begin(),i=0; it != v.end(); it++,i++) {
		QueryResultItem* pItem = *it;

		DuiLib::CListTextElementUI* pListElement = new DuiLib::CListTextElementUI;
		pListUi->Add(pListElement);

		strText.Format("%d", i + 1);
		pListElement->SetText(0, strText);
		pListElement->SetText(1,pItem->szBatchId );
		pListElement->SetText(2, pItem->szOperator);
		pListElement->SetText(3, pItem->szProcTime);

		// 小包装
		if (pRet->nQueryType == 0) {
			pListElement->SetText( 4, pItem->dwParentId == 0 ? NO_TEXT : YES_TEXT);
		}

		pListElement->SetTag(pItem->dwId);
	}

	if (pListUi->GetCount() > 0) {
		pListUi->SelectItem(0, false, true);
	}
	
}

void  CDuiFrameWnd::OnQueryBigListChanged() {
	if (m_lstQueryBig == 0) {
		return;
	}

	int nIndex = m_lstQueryBig->GetCurSel();
	DuiLib::CListTextElementUI* pItem = (DuiLib::CListTextElementUI*)m_lstQueryBig->GetItemAt(nIndex);
	DWORD dwId = (DWORD)pItem->GetTag();
	CBusiness::GetInstance()->QueryBigAsyn(dwId);
}

void  CDuiFrameWnd::OnQuerySmallListChanged() {
	if (m_lstQuerySmall == 0) {
		return;
	}

	int nIndex = m_lstQuerySmall->GetCurSel();
	DuiLib::CListTextElementUI* pItem = (DuiLib::CListTextElementUI*)m_lstQuerySmall->GetItemAt(nIndex);
	DWORD dwId = (DWORD)pItem->GetTag();
	CBusiness::GetInstance()->QuerySmallAsyn(dwId);
}


// 查询小包装结果
void CDuiFrameWnd::OnQuerySmallResult(int nError, const std::vector<QuerySmallResultItem*> * pVec) {

	if (0 == m_lstQueryTags) {
		return;
	}

	m_lstQueryTags->RemoveAll();

	if (pVec == 0) {
		return;
	}

	char buf[8192];

	std::vector<QuerySmallResultItem*>::const_iterator it;
	int i = 0;
	CString strText;
	for (it = pVec->begin(),i=0; it != pVec->end(); it++,i++) {
		QuerySmallResultItem* pItem = *it;

		DuiLib::CListTextElementUI* pListElement = new DuiLib::CListTextElementUI;
		m_lstQueryTags->Add(pListElement);

		strText.Format("%d", i + 1);
		pListElement->SetText(0, strText);

		GetUid(buf, sizeof(buf), pItem->item.abyUid, pItem->item.dwUidLen, '-');
		pListElement->SetText(1, buf);
	}

	if (m_lstQueryTags->GetCount() > 0) {
		m_lstQueryTags->SelectItem(0, false, false);
	}
}


void CDuiFrameWnd::OnQueryBigResult(int nError, const std::vector<QueryResultItem*> * pVec) {
	if (0 == m_lstQuerySmall) {
		return;
	}

	m_lstQuerySmall->RemoveAll();

	if (pVec == 0) {
		return;
	}

	std::vector<QueryResultItem*>::const_iterator it;
	const std::vector<QueryResultItem*> & v = *pVec;

	int i = 0;
	CString strText;
	for (it = v.begin(), i = 0; it != v.end(); it++, i++) {
		QueryResultItem* pItem = *it;

		DuiLib::CListTextElementUI* pListElement = new DuiLib::CListTextElementUI;
		m_lstQuerySmall->Add(pListElement);

		strText.Format("%d", i + 1);
		pListElement->SetText(0, strText);
		pListElement->SetText(1, pItem->szBatchId);
		pListElement->SetText(2, pItem->szOperator);
		pListElement->SetText(3, pItem->szProcTime);
		pListElement->SetText(4, pItem->dwParentId == 0 ? NO_TEXT : YES_TEXT);

		pListElement->SetTag(pItem->dwId);
	}

	if (m_lstQuerySmall->GetCount() > 0) {
		m_lstQuerySmall->SelectItem(0, false, true);
	}
}








int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	int ret = 0;
	LmnToolkits::ThreadManager::GetInstance();
	CBusiness::GetInstance()->sigInit.emit(&ret);

	DuiLib::CPaintManagerUI::SetInstance(hInstance);
	HICON hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
	
	CLoginWnd loginFrame;
	loginFrame.Create(NULL, _T("DUIWnd"), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
	loginFrame.CenterWindow();
	g_hWnd = loginFrame.GetHWND();
	::SendMessage(loginFrame.GetHWND(), WM_SETICON, ICON_BIG, (LPARAM)hIcon);
	::SendMessage(loginFrame.GetHWND(), WM_SETICON, ICON_SMALL, (LPARAM)hIcon);

	// 连接数据库
	CBusiness::GetInstance()->ReconnectDatabaseAsyn();
	// 连接Reader
	CBusiness::GetInstance()->ReconnectReaderAsyn();

	// 显示登录界面
	loginFrame.ShowModal();

	// 如果登录成功
	if ( CBusiness::GetInstance()->IfLogined() ) {
		// 主界面	
		CDuiFrameWnd duiFrame;
		duiFrame.Create(NULL, _T("DUIWnd"), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
		duiFrame.CenterWindow();
		g_hWnd = duiFrame.GetHWND();
		::SendMessage(duiFrame.GetHWND(), WM_SETICON, ICON_BIG, (LPARAM)hIcon);
		::SendMessage(duiFrame.GetHWND(), WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
		duiFrame.ShowModal();
	}

	CBusiness::GetInstance()->sigDeinit.emit(&ret);
	delete CBusiness::GetInstance();
	LmnToolkits::ThreadManager::ReleaseInstance();

	return 0;
}


