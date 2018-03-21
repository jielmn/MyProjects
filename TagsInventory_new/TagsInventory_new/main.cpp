// win32_1.cpp : 定义应用程序的入口点。
//
#include <afx.h>
#include "login.h"
#include "main.h"
#include "resource.h"
#include "Business.h"


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

		
	CInvDatabase::DATABASE_STATUS eDbStatus     = CBusiness::GetInstance()->GetDbStatus();
	CInvReader::READER_STATUS     eReaderStatus = CBusiness::GetInstance()->GetReaderStatus();

	if (eDbStatus == CInvDatabase::STATUS_OPEN) {
		SET_CONTROL_TEXT(m_lblDbStatus, DB_STATUS_OK_TEXT);
	}
	else {
		SET_CONTROL_TEXT(m_lblDbStatus, DB_STATUS_CLOSE_TEXT);
	}

	if (eReaderStatus == CInvReader::STATUS_OPEN) {
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

	CString strText;
	strText = "欢迎您，";
	SET_CONTROL_TEXT(m_lblUser, strText + CBusiness::GetInstance()->GetUserName() );

	// 初始化小盘点
	InitInventorySmall();

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
			m_tabs->SelectItem(0);
		} else if (name == _T("Inventory_big")) {
			m_tabs->SelectItem(1);			
		} else if (name == _T("Query")) {
			m_tabs->SelectItem(2);
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
	}
	else if (msg.sType == "textchanged") {
		if (name == BATCH_ID_EDIT_ID) {
			strText = GET_CONTROL_TEXT(m_edtBatchId);
			g_cfg->SetConfig(LAST_BATCH_ID, (const char *)strText);
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

}

// 处理自定义信息
LRESULT CDuiFrameWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if ( uMsg == UM_INVENTORY_RESULT ) {
		TagItem * pItem = (TagItem *)wParam;

		OnInventorySmallRet(pItem);

		if (pItem) {
			delete pItem;
		}
		return 0;
	}
	else if (uMsg == UM_INV_SMALL_SAVE_RESULT) {
		int nError = (int)wParam;
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
	return WindowImplBase::HandleMessage(uMsg, wParam, lParam);
}

void CDuiFrameWnd::OnInventorySmallRet( const TagItem * pItem ) {
	if ( 0 == m_tabs || TABS_INDEX_INVENTORY_SMALL != m_tabs->GetCurSel() ) {
		return;
	}

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


