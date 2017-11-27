// MainDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "TagsInventory.h"
#include "MainDlg.h"
#include "afxdialogex.h"

#define  TIMER_UPDATE_TAGS         1
#define  INTEVAL_UPDATE_TAGS       1000



class FindTagItem {
public:
	FindTagItem(TagItem * pItem) {
		memcpy(&m_item, pItem, sizeof(TagItem));
	}

	bool operator () (TagItem * pItem) {
		if ( *pItem == m_item ) {
			return true;
		}
		return false;
	}
private:
	TagItem    m_item;
};

class FindTagItemGt {
public:
	FindTagItemGt(TagItem * pItem) {
		memcpy(&m_item, pItem, sizeof(TagItem));
	}

	bool operator () (TagItem * pItem) {
		if (pItem->dwUidLen == m_item.dwUidLen) {
			if (memcmp(pItem->abyUid, m_item.abyUid, pItem->dwUidLen) > 0) {
				return true;
			}
			return false;
		}

		DWORD dwMinLen = pItem->dwUidLen < m_item.dwUidLen ? pItem->dwUidLen : m_item.dwUidLen;
		// 如果pItem比m_item大，找到了
		int ret = memcmp(pItem->abyUid, m_item.abyUid, dwMinLen);
		if (ret > 0) {
			return true;
		}
		else if (ret < 0) {
			return false;
		}
		else {
			if (pItem->dwUidLen > m_item.dwUidLen) {
				return true;
			}
		}
		return false;
	}
private:
	TagItem    m_item;
};

void CDuiFrameWnd1::InitWindow()
{
	char buf[8192];

	CLabelUI * pLogin = static_cast<CLabelUI*>(m_PaintManager.FindControl(_T("login_user")));
	if (pLogin) {
		pLogin->SetText(g_login_user.user_name);
	}

	CEditUI * pBatch = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("batch_id")));
	if (pBatch) {
		g_cfg_change->GetConfig("batch id", buf, sizeof(buf));
		pBatch->SetText( buf );
	}
}

void CDuiFrameWnd1::InsertTag(TagItem * pItem, int nIndex) {
	char buf[8192];
	CListUI* pList = static_cast<CListUI*>(m_PaintManager.FindControl(_T("list_data")));

	CListTextElementUI* pListElement = new CListTextElementUI;
	pList->AddAt(pListElement, nIndex);

	GetUid(buf, sizeof(buf), pItem->abyUid, pItem->dwUidLen, '-');
	pListElement->SetText(0, buf);
}

void  CDuiFrameWnd1::SaveBatchId() {
	CEditUI * pBatch = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("batch_id")));
	if (pBatch) {
		CDuiString strText = pBatch->GetText();
		g_cfg_change->SetConfig("batch id", strText);
	}
}

void CDuiFrameWnd1::SetCount(int nCount) {
	char buf[8192];
	CLabelUI * pLabel = static_cast<CLabelUI*>(m_PaintManager.FindControl(_T("lblCount")));
	if (pLabel) {
		sprintf(buf, "总数：%d", nCount);
		pLabel->SetText(buf);
	}
}

void CDuiFrameWnd1::SetStatus(int nStatus) {
	if (nStatus == STATUS_CLOSE) {
		CControlUI * pCtrl = 0;

		pCtrl = m_PaintManager.FindControl(_T("new"));
		if (pCtrl) {
			pCtrl->SetEnabled(true);
		}

		pCtrl = m_PaintManager.FindControl(_T("save"));
		if (pCtrl) {
			pCtrl->SetEnabled(false);
		}

		pCtrl = m_PaintManager.FindControl(_T("twodimcode"));
		if (pCtrl) {
			pCtrl->SetEnabled(true);
		}

		pCtrl = m_PaintManager.FindControl(_T("query"));
		if (pCtrl) {
			pCtrl->SetEnabled(true);
		}

		pCtrl = m_PaintManager.FindControl(_T("lblStatusInv"));
		if (pCtrl) {
			pCtrl->SetText("");
		}
	}
	else if (nStatus == STATUS_OPEN) {
		CControlUI * pCtrl = 0;

		pCtrl = m_PaintManager.FindControl(_T("new"));
		if (pCtrl) {
			pCtrl->SetEnabled(true);
		}

		pCtrl = m_PaintManager.FindControl(_T("save"));
		if (pCtrl) {
			pCtrl->SetEnabled(true);
		}

		pCtrl = m_PaintManager.FindControl(_T("twodimcode"));
		if (pCtrl) {
			pCtrl->SetEnabled(false);
		}

		pCtrl = m_PaintManager.FindControl(_T("query"));
		if (pCtrl) {
			pCtrl->SetEnabled(true);
		}	

		pCtrl = m_PaintManager.FindControl(_T("lblStatusInv"));
		if (pCtrl) {
			pCtrl->SetText("正在盘点......");
		}		
	}
	else if (nStatus == STATUS_SAVING) {
		CControlUI * pCtrl = 0;

		pCtrl = m_PaintManager.FindControl(_T("new"));
		if (pCtrl) {
			pCtrl->SetEnabled(false);
		}

		pCtrl = m_PaintManager.FindControl(_T("save"));
		if (pCtrl) {
			pCtrl->SetEnabled(false);
		}

		pCtrl = m_PaintManager.FindControl(_T("twodimcode"));
		if (pCtrl) {
			pCtrl->SetEnabled(false);
		}

		pCtrl = m_PaintManager.FindControl(_T("query"));
		if (pCtrl) {
			pCtrl->SetEnabled(false);
		}

		pCtrl = m_PaintManager.FindControl(_T("lblStatusInv"));
		if (pCtrl) {
			pCtrl->SetText("正在保存......");
		}
	}
}

void CDuiFrameWnd1::ClearList() {
	CListUI* pList = static_cast<CListUI*>(m_PaintManager.FindControl(_T("list_data")));
	if (pList) {
		pList->RemoveAll();
	}
}

void CDuiFrameWnd1::SetControlText(const char * szControlName, CDuiString strText) {
	if (0 == szControlName) {
		return;
	}

	CControlUI * pCtrl = 0;
	pCtrl = m_PaintManager.FindControl(szControlName);
	if (0 == pCtrl) {
		return;
	}

	pCtrl->SetText(strText);
}

// CMainDlg 对话框
CDuiString  CDuiFrameWnd1::GetSkinFile() {
	//char buf[8192];
	//g_cfg->GetConfig("main xml file", buf, sizeof(buf), "main.xml");
	//return buf;

	return "main.xml";
}

void CDuiFrameWnd1::SetStatus(int nStatus, int nType) {
	CLabelUI * pStatus = 0;
	const char * szType = 0;

	if (STATUS_TYPE_DATABASE == nType) {
		pStatus = static_cast <CLabelUI*> (m_PaintManager.FindControl(_T("lblStatusDb")));
		szType = "数据库";
	}
	else {
		pStatus = static_cast <CLabelUI*> (m_PaintManager.FindControl(_T("lblStatusReader")));
		szType = "读卡器";
	}

	if (0 == pStatus) {
		return;
	}

	CString strText;
	if (STATUS_OPEN == nStatus) {
		strText.Format("%s连接OK", szType);
	}
	else {
		strText.Format("%s没有连接上", szType);
	}

	pStatus->SetText(strText);
}

void CDuiFrameWnd1::Notify(TNotifyUI& msg) {
	if (msg.sType == _T("click"))
	{
		CDuiString sendName = msg.pSender->GetName();
		// 保存
		if ( sendName == _T("save"))
		{
			::PostMessage(g_hDlg, UM_SAVE, 0, 0);
		}
		// 新建
		else if (sendName == _T("new") ) {
			::PostMessage(g_hDlg, UM_NEW, 0, 0);
		}
		// 打印二维码
		else if (sendName == _T("twodimcode")) {
			::PostMessage(g_hDlg, UM_TWO_DIMENSION_CODE, 0, 0);
		}
		else if (sendName == _T("query") ) {
			::PostMessage(g_hDlg, UM_QUERY, 0, 0);
		}
	}
}

CDuiString  CDuiFrameWnd1::GetControlText(const char * szControlName) {
	CDuiString strText;

	if (0 == szControlName) {
		return strText;
	}

	CControlUI * pCtrl = 0;
	pCtrl = m_PaintManager.FindControl(szControlName);
	if ( 0 == pCtrl ) {
		return strText;
	}

	return pCtrl->GetText();
}






IMPLEMENT_DYNAMIC(CMainDlg, CDialogEx)

CMainDlg::CMainDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_MAIN, pParent)
{
	m_nStatus = -1;
	m_qrcode = 0;
	m_query_dlg = 0;
}

CMainDlg::~CMainDlg()
{
}

void CMainDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CMainDlg, CDialogEx)
	ON_MESSAGE(UM_TAG_ITEM, OnTagItemMessage)
	ON_MESSAGE(UM_NEW, OnNewMessage)
	ON_MESSAGE(UM_SAVE, OnSaveMessage)
	ON_MESSAGE(UM_TWO_DIMENSION_CODE, OnTwoDimCodeMessage)
	ON_MESSAGE(UM_DB_ERR_MSG, OnDbErrMessage)
	ON_MESSAGE(UM_SHOW_STATUS, OnStatusMessage)
	ON_MESSAGE(UM_QUERY, OnQueryMessage)
	ON_WM_DESTROY()
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CMainDlg 消息处理程序
BOOL CMainDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	g_hDlg = this->m_hWnd;

	CRect rect;
	GetClientRect(rect);
	m_duiFrame.Create(*this, _T("DUIWnd"), UI_WNDSTYLE_CHILD, 0, 0, 0, rect.Width(), rect.Height());
	m_duiFrame.ShowWindow(TRUE);

	DWORD dwWidth = 0, dwHeight = 0;
	DWORD dwCaptionHeight = 30;
	DWORD dwDlgBorder = 10;
	g_cfg->GetConfig("main width", dwWidth, 800);
	g_cfg->GetConfig("main height", dwHeight, 600);
	g_cfg->GetConfig("caption height", dwCaptionHeight, 30);
	g_cfg->GetConfig("dialog border", dwDlgBorder, 10);

	int screen_width = GetSystemMetrics(SM_CXSCREEN);
	int screen_height = GetSystemMetrics(SM_CYSCREEN);

	this->MoveWindow((screen_width - dwWidth) / 2, (screen_height - dwHeight) / 2, dwWidth, dwHeight);
	::MoveWindow(m_duiFrame.GetHWND(), 0, 0, dwWidth - 2 * dwDlgBorder, dwHeight - dwCaptionHeight - dwDlgBorder, TRUE);

	m_duiFrame.SetStatus(g_mydb->GetStatus(), STATUS_TYPE_DATABASE);
	m_duiFrame.SetStatus(g_myreader->GetStatus(), STATUS_TYPE_READER);

	m_brush.CreateSolidBrush(RGB(0, 0, 0));
	// 开始盘点
	StartInventory();
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}


LRESULT CMainDlg::OnTagItemMessage(WPARAM wParam, LPARAM lParam) {
	TagItem * pItem = (TagItem *)wParam;

	// 如果是用户登录id，则不处理
	if ( *pItem == g_login_user.user_tag_id ) {
		delete pItem;
		return 0;
	}

	// 关闭状态，不接受盘点
	if (m_nStatus == STATUS_CLOSE) {
		delete pItem;
		return 0;
	}

	std::vector<TagItem *>::iterator it = std::find_if(m_tag_items.begin(), m_tag_items.end(), FindTagItem(pItem));
	// 如果已经显示
	if (it != m_tag_items.end()) {
		delete pItem;
		return 0;
	}

	it = std::find_if(m_buf_tag_items.begin(), m_buf_tag_items.end(), FindTagItem(pItem));
	// 如果已经缓存
	if (it != m_buf_tag_items.end()) {
		delete pItem;
		return 0;
	}

	// 放在缓存中
	m_buf_tag_items.push_back(pItem);

	return 0;
}

void CMainDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
	m_duiFrame.SaveBatchId();

	ClearVector(m_tag_items);
	ClearVector(m_buf_tag_items);

	if (m_qrcode) {
		QRcode_free(m_qrcode);
		m_qrcode = 0;
	}
}


void  CMainDlg::StartInventory() {
	if (m_tag_items.size() > 0) {
		if (IDNO == MessageBox("当前的盘点没有保存，是否还要重新盘点？", MSG_BOX_CAPTION, MB_YESNO | MB_DEFBUTTON2 )) {
			return;
		}
	}
	m_nStatus = STATUS_OPEN;
	m_duiFrame.SetStatus(m_nStatus);

	ClearVector(m_tag_items);
	ClearVector(m_buf_tag_items);
	m_duiFrame.ClearList();

	m_duiFrame.SetControlText("package_id", "");
	m_duiFrame.SetCount(0);
	SetTimer(TIMER_UPDATE_TAGS, INTEVAL_UPDATE_TAGS, 0);
}

void CMainDlg::OnTimer(UINT_PTR nIDEvent) {
	if (nIDEvent == TIMER_UPDATE_TAGS) {
		std::vector<TagItem *>::iterator it;
		if (0 == m_buf_tag_items.size()) {
			return;
		}

		for (it = m_buf_tag_items.begin(); it != m_buf_tag_items.end(); it++) {
			TagItem * pItem = *it;

			std::vector<TagItem *>::iterator ij;
			ij = std::find_if(m_tag_items.begin(), m_tag_items.end(), FindTagItemGt(pItem));

			int nIndex = std::distance(m_tag_items.begin(), ij);
			m_tag_items.insert(ij, pItem);
			m_duiFrame.InsertTag(pItem, nIndex);
		}
		m_duiFrame.SetCount(m_tag_items.size());
		m_buf_tag_items.clear();
	}
}

LRESULT CMainDlg::OnNewMessage(WPARAM wParam, LPARAM lParam) {
	StartInventory();
	return 0;
}

LRESULT CMainDlg::OnSaveMessage(WPARAM wParam, LPARAM lParam) {
	if (m_tag_items.size() == 0) {
		MessageBox("盘点总数为0", MSG_BOX_CAPTION);
		return 0;
	}

	if (STATUS_CLOSE == g_mydb->GetStatus()) {
		MessageBox("数据库没有连接上", MSG_BOX_CAPTION);
		return 0;
	}
	
	CDuiString str = m_duiFrame.GetControlText("batch_id");
	CString strBatchId = str;

	if (strBatchId.GetLength() == 0) {
		MessageBox("批号不能为空!");
		return 0;
	}

	// 201709001 或者20170901
	if (strBatchId.GetLength() != 8) {
		MessageBox("批号格式不对!必须是年+月+号码，例如\"20170906\"");
		return 0;
	}

	int nBatchId = 0;
	if (0 == sscanf(strBatchId.Mid(0, 4), "%d", &nBatchId)) {
		MessageBox("批号格式不对!必须是年+月+号码，例如\"20170906\"");
		return 0;
	}

	if (0 == sscanf(strBatchId.Mid(4, 2), "%d", &nBatchId)) {
		MessageBox("批号格式不对!必须是年+月+号码，例如\"20170906\"");
		return 0;
	}

	if (0 == sscanf(strBatchId.Mid(6), "%d", &nBatchId)) {
		MessageBox("批号格式不对!必须是年+月+号码，例如\"20170906\"");
		return 0;
	}

	CSaveInvParam * pParam = new CSaveInvParam;
	pParam->m_strBatchId = strBatchId;

	std::vector<TagItem *>::iterator it ;
	for ( it = m_tag_items.begin(); it != m_tag_items.end(); it++ ) {
		TagItem * pItem = *it;
		TagItem * pNewItem = new TagItem;
		memcpy(pNewItem, pItem, sizeof(TagItem));
		pParam->m_items.push_back(pNewItem);
	}

	g_thrd_db->PostMessage(g_handler_db, MSG_SAVE_INVENTORY, pParam);
	
	ClearVector(m_buf_tag_items);
	KillTimer(TIMER_UPDATE_TAGS);

	m_nStatus = STATUS_SAVING;
	m_duiFrame.SetStatus(m_nStatus);
	return 0;
}

void CMainDlg::DrawQrImg(CDC * pDc ) {
	int  dwOffsetX = 0;
	int  dwOffsetY = 0;

	g_cfg_change->Deinit();
	g_cfg_change->Init("last.cfg");

	DWORD dwPaperXOffset = 0;
	DWORD dwPaperYOffset = 0;
	DWORD dwBold = 1;

	g_cfg_change->GetConfig("paper x offset", dwPaperXOffset, 0);
	g_cfg_change->GetConfig("paper y offset", dwPaperYOffset, 0);
	g_cfg->GetConfig("2 dimension thread bold", dwBold, 1);

	int nOffsetX = 0, nOffsetY = 0;
	DWORD   dwPaperWidth = 0;
	DWORD   dwPaperLength = 0;
	g_cfg->GetConfig("paper width",  dwPaperWidth, 500);
	g_cfg->GetConfig("paper length", dwPaperLength, 620);

	nOffsetX = (int)( dwPaperWidth  - dwBold * 25) / 2;
	nOffsetY = (int)( dwPaperLength - dwBold * 25) / 2;

	nOffsetX += (int)dwPaperXOffset;
	nOffsetY += (int)dwPaperYOffset;

	if ( 0 == m_qrcode ) {
		return;
	}

	for (int i = 0; i < 25; i++)
	{
		for (int j = 0; j < 25; j++)
		{
			if (m_qrcode->data[i * 25 + j] & 0x01)
			{
				CRect rect(i*dwBold + nOffsetX, j*dwBold + nOffsetY, (i + 1)*dwBold + nOffsetX, (j + 1)*dwBold + nOffsetY);
				pDc->FillRect(rect, &m_brush);
			}
		}
	}

}

LRESULT CMainDlg::OnTwoDimCodeMessage(WPARAM wParam, LPARAM lParam) {

	// TODO: 在此添加控件通知处理程序代码
	DWORD   dwPaperWidth = 0;
	DWORD   dwPaperLength = 0;

	g_cfg->GetConfig("paper width", dwPaperWidth, 500);
	g_cfg->GetConfig("paper length", dwPaperLength, 620);

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

		DrawQrImg(pDC);

		EndPage(printInfo.hDC);
		EndDoc(printInfo.hDC);

		// Delete DC when done.
		DeleteDC(printInfo.hDC);

		//OnOK();
	}
	return 0;
}

LRESULT CMainDlg::OnDbErrMessage(WPARAM wParam, LPARAM lParam) {
	DbErrMsg * pDbErrMsg = (DbErrMsg *)wParam;
	if (0 == pDbErrMsg->dwErrId) {
		MessageBox("保存成功!", MSG_BOX_CAPTION);

		if ( m_nStatus == STATUS_SAVING) {
			m_nStatus = STATUS_CLOSE;
			m_duiFrame.SetStatus(m_nStatus);
			m_duiFrame.SetControlText("package_id", pDbErrMsg->szBatchId);

			ClearVector(m_tag_items);
			ClearVector(m_buf_tag_items);

			if (m_qrcode) {
				QRcode_free(m_qrcode);
				m_qrcode = 0;
			}
			m_qrcode = QRcode_encodeString(pDbErrMsg->szBatchId, 2, QR_ECLEVEL_L, QR_MODE_8, 0);
		}		
	}
	else {
		MessageBox(pDbErrMsg->szErrDescription, MSG_BOX_CAPTION);

		if ( m_nStatus == STATUS_SAVING ) {
			m_nStatus = STATUS_OPEN;
			m_duiFrame.SetStatus(m_nStatus);
			SetTimer(TIMER_UPDATE_TAGS, INTEVAL_UPDATE_TAGS, 0);
		}

		if (pDbErrMsg->bDisconnected) {
			g_thrd_db->PostDelayMessage(RECONNECT_DB_TIME, g_handler_db, MSG_RECONNECT_DB);
			m_duiFrame.SetStatus(STATUS_CLOSE, STATUS_TYPE_DATABASE);
		}
	}
	delete pDbErrMsg;
	return 0;
}

LRESULT CMainDlg::OnStatusMessage(WPARAM wParam, LPARAM lParam) {
	m_duiFrame.SetStatus(lParam, wParam);
	return 0;
}

LRESULT CMainDlg::OnQueryMessage(WPARAM wParam, LPARAM lParam) {
	m_query_dlg = new CQueryDlg;
	m_query_dlg->DoModal(); 
	delete m_query_dlg;
	m_query_dlg = 0;
	return 0;
}