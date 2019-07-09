// win32_1.cpp : 定义应用程序的入口点。
//

#ifdef _DEBUG
#include "vld.h"
#endif

#include "Windows.h"
#include "main.h"
#include "business.h"
#include "resource.h"
#include "NetDevice.h"

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
		CNetDevice *pCNetDevice = new CNetDevice();//创建自定义控件
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

			/*
			CLabelUI *pUserNameLabel = new CLabelUI;     //自定义控件增加一个标签
			if (pUserNameLabel != NULL)
			{
				pUserNameLabel->SetAttribute(_T("float"), _T("true"));
				pUserNameLabel->SetAttribute(_T("pos"), _T("8,60,80,80"));
				//pUserNameLabel->SetAttribute(_T("bkimage"), _T("SchoolNet/123.png"));
				pUserNameLabel->SetText(_T("drag&drop"));
				//pUserNameLabel->SetAttribute(_T("textcolor"), _T("#FF9FFF99"));
				//pUserNameLabel->SetMouseEnabled(false);
				pCNetDevice->Add(pUserNameLabel);
			}
			*/

			pVLNet->Add(pCNetDevice);
		}
	}
	WindowImplBase::InitWindow();
}

CControlUI * CDuiFrameWnd::CreateControl(LPCTSTR pstrClass) {
	return WindowImplBase::CreateControl(pstrClass);
}

void CDuiFrameWnd::Notify(TNotifyUI& msg) {
	WindowImplBase::Notify(msg);
}

LRESULT CDuiFrameWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	return WindowImplBase::HandleMessage(uMsg,wParam,lParam);
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


