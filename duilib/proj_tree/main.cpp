#ifdef _DEBUG
#include <vld.h>
#endif

#include "main.h"


#pragma comment(lib,"User32.lib")

void CDuiFrameWnd::InitWindow() {
	m_tree = (CMyTreeUI * )m_PaintManager.FindControl("tree1"); 

	
	CMyTreeUI::Node* pCategoryNode = NULL;  
	CMyTreeUI::Node* pGameNode = NULL;
	CMyTreeUI::Node* pServerNode = NULL;
	CMyTreeUI::Node* pRoomNode = NULL;
	CMyTreeUI::Node* pRoomNode1 = NULL;
	pCategoryNode = m_tree->AddNode(_T("{x 4}{i gameicons.png 18 3}{x 4}�Ƽ���Ϸ"));
	for (int i = 0; i < 4; ++i) 
	{
		pGameNode = m_tree->AddNode(_T("{x 4}{i gameicons.png 18 10}{x 4}���˶�����"), pCategoryNode);
		for (int i = 0; i < 3; ++i)
		{
			pServerNode = m_tree->AddNode(_T("{x 4}{i gameicons.png 18 10}{x 4}���Է�����"), pGameNode);
			for (int i = 0; i < 3; ++i)
			{
				pRoomNode = m_tree->AddNode(_T("{x 4}{i gameicons.png 18 10}{x 4}���Է���"), pServerNode);
				for (int i = 0; i < 2; ++i)
				{
					pRoomNode1 = m_tree->AddNode(_T("{x 4}{i gameicons.png 18 10}{x 4}abc"), pRoomNode);
					for (int i = 0; i < 2; ++i)
					{
						m_tree->AddNode(_T("{x 4}{i gameicons.png 18 10}{x 4}xyz"), pRoomNode1);
					}
				}
			}
		}
	}


	WindowImplBase::InitWindow();      
}

void   CDuiFrameWnd::Notify(TNotifyUI& msg) {
	if ( msg.sType == _T("itemclick")) {
		if (m_tree->GetItemIndex(msg.pSender) != -1)
		{
			if (_tcscmp(msg.pSender->GetClass(), DUI_CTR_LISTLABELELEMENT) == 0) {

				CMyTreeUI::Node* node = (CMyTreeUI::Node*)msg.pSender->GetTag();
				POINT pt = { 0 };
				::GetCursorPos(&pt);
				::ScreenToClient(m_PaintManager.GetPaintWindow(), &pt);
				pt.x -= msg.pSender->GetX();
				SIZE sz = m_tree->GetExpanderSizeX(node);
				if (pt.x >= sz.cx && pt.x < sz.cy) 
					m_tree->ExpandNode(node, !node->data()._expand);
			}
		}
	}
	WindowImplBase::Notify(msg);
}

CControlUI * CDuiFrameWnd::CreateControl(LPCTSTR pstrClass) {
	if (0 == strcmp(pstrClass, "MyTree")) {
		return new CMyTreeUI();
	}
	return WindowImplBase::CreateControl(pstrClass);
}







int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	CPaintManagerUI::SetInstance(hInstance);

	CDuiFrameWnd duiFrame;
	duiFrame.Create(NULL, _T("DUIWnd"), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
	duiFrame.CenterWindow();
	duiFrame.ShowModal();
	return 0;
}