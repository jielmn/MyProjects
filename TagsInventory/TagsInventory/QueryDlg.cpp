// QueryDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "TagsInventory.h"
#include "QueryDlg.h"
#include "afxdialogex.h"

#define  STATUS_IDLE      0
#define  STATUS_BUSY      1


void CDuiFrameWnd2::Notify(TNotifyUI& msg) {
	if (msg.sType == _T("click"))
	{
		CDuiString sendName = msg.pSender->GetName();
		// 保存
		if (sendName == _T("query"))
		{
			CListUI* pList = static_cast<CListUI*>(m_PaintManager.FindControl(_T("result")));
			if (pList == 0) {
				DbErrMsg * pDbErrMsg = new DbErrMsg;
				memset(pDbErrMsg, 0, sizeof(DbErrMsg));

				pDbErrMsg->dwErrId = 1;
				strncpy(pDbErrMsg->szErrDescription, "duilib库的bug", sizeof(pDbErrMsg->szErrDescription) - 1);
				pDbErrMsg->bDisconnected = FALSE;
				::PostMessage(g_hQueryDlg, UM_DB_ERR_MSG, (WPARAM)pDbErrMsg, 0);
				return;
			}
			pList->RemoveAll();

			
			CDateTimeUI * pDateCtrl = static_cast<CDateTimeUI*>(m_PaintManager.FindControl(_T("DateTime1")));
			SYSTEMTIME startTime = pDateCtrl->GetTime();

			pDateCtrl = static_cast<CDateTimeUI*>(m_PaintManager.FindControl(_T("DateTime2")));
			SYSTEMTIME endTime = pDateCtrl->GetTime();

			CTime  s1(startTime);
			CTime  s2(endTime);

			CString  strStart, strEnd;
			strStart = s1.Format("%Y-%m-%d 00:00:00");
			strEnd   = s2.Format("%Y-%m-%d 23:59:59");

			CEditUI * pOperator = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("operator")));

			CQueryParam * pParam = new CQueryParam( strStart, strEnd, pOperator->GetText() );
			::PostMessage(g_hQueryDlg, UM_QUERY, (WPARAM)pParam, 0);

			SetStatus(STATUS_BUSY);
		}		
	}
}

void CDuiFrameWnd2::InsertItem(QueryResult * pItem) {
	char buf[8192];
	CListUI* pList = static_cast<CListUI*>(m_PaintManager.FindControl(_T("result")));

	CListTextElementUI* pListElement = new CListTextElementUI;
	pList->Add(pListElement);

	pListElement->SetText(0, GetNum( buf, sizeof(buf),pItem->dwId) );
	pListElement->SetText(2, GetOperateType( buf, sizeof(buf), pItem->nProcType) );
	pListElement->SetText(4, pItem->szOperator);
	pListElement->SetText(6, pItem->szProcTime);
	pListElement->SetText(8, GetNum(buf, sizeof(buf), pItem->dwTagsCnt));
	pListElement->SetText(10, pItem->szBatchId);
}

void CDuiFrameWnd2::SetStatus(int nStatus) {
	if (STATUS_BUSY == nStatus) {
		CControlUI * pBtn = m_PaintManager.FindControl(_T("query"));
		if (pBtn) {
			pBtn->SetEnabled(false);
		}
	}
	else if (STATUS_IDLE == nStatus) {
		CControlUI * pBtn = m_PaintManager.FindControl(_T("query"));
		if (pBtn) {
			pBtn->SetEnabled(true);
		}
	}
}


// CQueryDlg 对话框

IMPLEMENT_DYNAMIC(CQueryDlg, CDialogEx)

CQueryDlg::CQueryDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_QUERY, pParent)
{

}

CQueryDlg::~CQueryDlg()
{
}

void CQueryDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CQueryDlg, CDialogEx)
	ON_WM_DESTROY()
	ON_MESSAGE(UM_QUERY, OnQueryMessage)
	ON_MESSAGE(UM_DB_ERR_MSG, OnDbErrMessage)
END_MESSAGE_MAP()


// CQueryDlg 消息处理程序
BOOL CQueryDlg::OnInitDialog() {
	CDialogEx::OnInitDialog();

	g_hQueryDlg = this->m_hWnd;

	CRect rect;
	GetClientRect(rect);
	m_duiFrame.Create(*this, _T("DUIWnd"), UI_WNDSTYLE_CHILD, 0, 0, 0, rect.Width(), rect.Height());
	m_duiFrame.ShowWindow(TRUE);

	DWORD dwWidth = 0, dwHeight = 0;
	DWORD dwCaptionHeight = 30;
	DWORD dwDlgBorder = 10;

	g_cfg->GetConfig("query width", dwWidth, 800);
	g_cfg->GetConfig("query height", dwHeight, 600);
	g_cfg->GetConfig("caption height", dwCaptionHeight, 30);
	g_cfg->GetConfig("dialog border", dwDlgBorder, 10);

	int screen_width = GetSystemMetrics(SM_CXSCREEN);
	int screen_height = GetSystemMetrics(SM_CYSCREEN);

	this->MoveWindow((screen_width - dwWidth) / 2, (screen_height - dwHeight) / 2, dwWidth, dwHeight);
	::MoveWindow(m_duiFrame.GetHWND(), 0, 0, dwWidth - 2 * dwDlgBorder, dwHeight - dwCaptionHeight - dwDlgBorder, TRUE);

	return TRUE;
}

void CQueryDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
	g_hQueryDlg = 0;
}

LRESULT CQueryDlg::OnQueryMessage(WPARAM wParam, LPARAM lParam) {
	CQueryParam * pParam = (CQueryParam *)wParam;
	g_thrd_db->PostMessage(g_handler_db, MSG_QUERY, pParam);
	return 0;
}

LRESULT CQueryDlg::OnDbErrMessage(WPARAM wParam, LPARAM lParam) {
	DbErrMsg * pDbErrMsg = (DbErrMsg *)wParam;

	if (0 == pDbErrMsg->dwErrId) {
		std::vector<QueryResult *> * pvRet = (std::vector<QueryResult *> *)pDbErrMsg->pParam;
		if (pvRet) {
			std::vector<QueryResult *>::iterator it;
			for (it = pvRet->begin(); it != pvRet->end(); it++) {
				QueryResult * pItem = *it;
				m_duiFrame.InsertItem(pItem);
			}
		}

	}
	else {
		MessageBox(pDbErrMsg->szErrDescription, MSG_BOX_CAPTION);
		if (pDbErrMsg->bDisconnected) {
			g_thrd_db->PostDelayMessage(RECONNECT_DB_TIME, g_handler_db, MSG_RECONNECT_DB);
		}
	}
	
	std::vector<QueryResult *> * pvRet = (std::vector<QueryResult *> *)pDbErrMsg->pParam;
	if (pvRet) {
		ClearVector(*pvRet);
		delete pvRet;
	}
	delete pDbErrMsg;

	m_duiFrame.SetStatus(STATUS_IDLE);
	return 0;
}