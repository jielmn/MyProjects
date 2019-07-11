// win32_1.cpp : 定义应用程序的入口点。
//

#ifdef _DEBUG
#include "vld.h"
#endif

#include "Windows.h"
#include "main.h"
#include "business.h"
#include "resource.h"
#include "DragDropUI.h"

CDuiFrameWnd::CDuiFrameWnd() {

}

CDuiFrameWnd::~CDuiFrameWnd() {

}

void  CDuiFrameWnd::InitWindow() {
	CVerticalLayoutUI* pVLNet = static_cast<CVerticalLayoutUI*>(m_PaintManager.FindControl(_T("vlSchoolNet")));//需要绘制的背景布局
	if (NULL != pVLNet)
	{
		int _left = 205; 
		int _top = 47;
		CDragDropUI *pCNetDevice = new CDragDropUI();//创建自定义控件
		if (NULL != pCNetDevice)
		{
			pCNetDevice->SetFloat();                     //一定要设置为绝对定位，否则不能拖动
			SIZE leftTop = { _left,_top };
			pCNetDevice->SetFixedXY(leftTop);           // 自定义控件在背景布局的起始位置
			pCNetDevice->SetFixedWidth(80);
			pCNetDevice->SetFixedHeight(80);
			//pCNetDevice->SetBorderSize(1);
			//pCNetDevice->SetBorderColor(0xFF000000);
			pCNetDevice->SetAttribute(_T("bkimage"), _T("file='earth.png'"));
			pCNetDevice->SetName("dragdrop");

			pVLNet->Add(pCNetDevice);
		}
	}

	m_ForbidCtl = m_PaintManager.FindControl("layInvalid");
	WindowImplBase::InitWindow();
}

CControlUI * CDuiFrameWnd::CreateControl(LPCTSTR pstrClass) {
	return WindowImplBase::CreateControl(pstrClass);
}

void CDuiFrameWnd::Notify(TNotifyUI& msg) {
	if ( msg.sType == "CheckPos" ) {
		CDragDropUI * pDragDrop = (CDragDropUI *)msg.pSender;
		pDragDrop->m_bCheckPos = CheckPos(pDragDrop, msg.wParam);
	}
	WindowImplBase::Notify(msg);
}

LRESULT CDuiFrameWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	return WindowImplBase::HandleMessage(uMsg,wParam,lParam);
}

BOOL CDuiFrameWnd::CheckPos(CDragDropUI * pDragDrop, BOOL bSetPos /*= TRUE*/) {
	int width  = pDragDrop->GetWidth();
	int height = pDragDrop->GetHeight();
	CDuiRect rcParent = pDragDrop->GetParent()->GetPos();
	BOOL  bRet = TRUE;

	// 左边界、右边界
	if (pDragDrop->m_rcNewPos.left < rcParent.left) {
		// 左边完全出去
		if (pDragDrop->m_rcNewPos.left + width <= rcParent.left) {
			bRet = FALSE;
		}
		if (bSetPos)
			pDragDrop->m_rcNewPos.left = rcParent.left;
	}
	else if (pDragDrop->m_rcNewPos.right > rcParent.right) {
		// 右边完全出去
		if (pDragDrop->m_rcNewPos.right - rcParent.right >= width) {
			bRet = FALSE;
		}
		if (bSetPos)
			pDragDrop->m_rcNewPos.left = rcParent.right - width;
	}

	// 上边界、下边界
	if (pDragDrop->m_rcNewPos.top < rcParent.top) {
		// 上边完全出去
		if (pDragDrop->m_rcNewPos.top + height <= rcParent.top) {
			bRet = FALSE;
		}
		if (bSetPos)
			pDragDrop->m_rcNewPos.top = rcParent.top;
	}
	else if (pDragDrop->m_rcNewPos.bottom > rcParent.bottom) {
		// 下边完全出去
		if (pDragDrop->m_rcNewPos.bottom - rcParent.bottom >= height) {
			bRet = FALSE;
		}
		if (bSetPos)
			pDragDrop->m_rcNewPos.top = rcParent.bottom - height;
	}

	RECT rcForbit = m_ForbidCtl->GetPos();
	if (pDragDrop->m_rcNewPos.left >= rcForbit.left && pDragDrop->m_rcNewPos.right <= rcForbit.right
		&& pDragDrop->m_rcNewPos.top >= rcForbit.top && pDragDrop->m_rcNewPos.bottom <= rcForbit.bottom) {
		bRet = FALSE;
	} 

	if ( pDragDrop->m_rcNewPos.left < rcForbit.left && rcForbit.left < pDragDrop->m_rcNewPos.right ) {
		if (RectOverlap(pDragDrop->m_rcNewPos, rcForbit))
			if (bSetPos)
				pDragDrop->m_rcNewPos.left = rcForbit.left - width;		
	}
	else if (pDragDrop->m_rcNewPos.left < rcForbit.right && rcForbit.right < pDragDrop->m_rcNewPos.right ) {
		if (RectOverlap(pDragDrop->m_rcNewPos, rcForbit))
			if (bSetPos)
				pDragDrop->m_rcNewPos.left = rcForbit.right;
	}

	if (pDragDrop->m_rcNewPos.top < rcForbit.top && rcForbit.top < pDragDrop->m_rcNewPos.bottom) {
		if (RectOverlap(pDragDrop->m_rcNewPos, rcForbit))
			if (bSetPos)
				pDragDrop->m_rcNewPos.top = rcForbit.top - height;
	}
	else if (pDragDrop->m_rcNewPos.top < rcForbit.bottom && rcForbit.bottom < pDragDrop->m_rcNewPos.bottom ) {
		if (RectOverlap(pDragDrop->m_rcNewPos, rcForbit))
			if (bSetPos)
				pDragDrop->m_rcNewPos.top = rcForbit.bottom;
	}
	

	if (bSetPos) {
		pDragDrop->m_rcNewPos.right = pDragDrop->m_rcNewPos.left + width;
		pDragDrop->m_rcNewPos.bottom = pDragDrop->m_rcNewPos.top + height;
	}
	return bRet;
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


