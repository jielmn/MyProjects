#ifdef _DEBUG
#include <vld.h>
#endif

#include "main.h"
#pragma comment(lib,"User32.lib")

// menu
class CDuiMenu : public WindowImplBase
{
protected:
	virtual ~CDuiMenu() {};        // ˽�л����������������˶���ֻ��ͨ��new�����ɣ�������ֱ�Ӷ���������ͱ�֤��delete this�������
	CDuiString  m_strXMLPath;
	CControlUI * m_pOwner;

public:
	explicit CDuiMenu(LPCTSTR pszXMLPath, CControlUI * pOwner) : m_strXMLPath(pszXMLPath), m_pOwner(pOwner) {}
	virtual LPCTSTR    GetWindowClassName()const { return _T("CDuiMenu "); }
	virtual CDuiString GetSkinFolder() { return _T(""); }
	virtual CDuiString GetSkinFile() { return m_strXMLPath; }
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

	virtual void  Notify(TNotifyUI& msg) {
		if (msg.sType == "itemclick") {
			if (m_pOwner) {
				m_pOwner->GetManager()->SendNotify(m_pOwner, msg.pSender->GetName(), 0, 0, true);
			}
			PostMessage(WM_CLOSE);
			return;
		}
		WindowImplBase::Notify(msg);
	}

	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		return __super::HandleMessage(uMsg, wParam, lParam);
	}
};












void CDuiFrameWnd::InitWindow() {
	m_tree = (CMyTreeUI * )m_PaintManager.FindControl("tree1"); 
	m_view = (CLabelUI* )m_PaintManager.FindControl("lblView");
	
	CMyTreeUI::Node* pCategoryNode = NULL;  
	CMyTreeUI::Node* pGameNode = NULL;
	CMyTreeUI::Node* pServerNode = NULL;
	CMyTreeUI::Node* pRoomNode = NULL;
	CMyTreeUI::Node* pRoomNode1 = NULL;

	CDuiString  strText;
	pCategoryNode = m_tree->AddNode(_T("{x 4}{i gameicons.png 18 3}{x 4}�Ƽ���Ϸ"));
	for (int i = 0; i < 2; ++i) 
	{
		strText.Format("{x 4}{i gameicons.png 18 10}{x 4}���˶����� %d", i + 1);
		pGameNode = m_tree->AddNode(strText, pCategoryNode, (void *)(i+1) );
		for (int j = 0; j < 2; ++j)
		{
			strText.Format("{x 4}{i gameicons.png 18 10}{x 4}���Է����� %d_%d", i+1, j + 1);
			pServerNode = m_tree->AddNode(strText, pGameNode, (void *)((i+1)*10+j+1) );
		}
	}
	m_tree->SelectItem(0); 

	WindowImplBase::InitWindow();      
}

void   CDuiFrameWnd::Notify(TNotifyUI& msg) {
	DuiLib::CDuiString name = msg.pSender->GetName();
	if ( msg.sType == _T("itemclick")) {
		if (m_tree->GetItemIndex(msg.pSender) != -1)
		{
			if (_tcscmp(msg.pSender->GetClass(), DUI_CTR_LISTLABELELEMENT) == 0) {
				CDuiString strText;
				CMyTreeUI::Node* node = (CMyTreeUI::Node*)msg.pSender->GetTag();
				strText.Format("%d", (int)node->data()._pUserData);
				m_view->SetText(strText);

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
	else if ( msg.sType == _T("menu") ) {
		if ( name == "tree1" )
		{
			POINT pt = { msg.ptMouse.x, msg.ptMouse.y };
			CDuiMenu *pMenu = new CDuiMenu(_T("menu.xml"), msg.pSender);
			pMenu->Init(*this, pt);
			pMenu->ShowWindow(TRUE);
		}
	}
	else if (msg.sType == "menu_delete") {
		int nIndex = m_tree->GetCurSel();
		if (nIndex >= 0) {
			m_tree->RemoveAt(nIndex);
		}		
		if (m_tree->GetCount() == 0) {
			m_view->SetText("");
		}
	}
	else if (msg.sType == "itemselect") {
		int nIndex = m_tree->GetCurSel();
		if (nIndex >= 0) {
			CControlUI * pControl = m_tree->GetItemAt(nIndex);
			CListLabelElementUI * pElement = (CListLabelElementUI *)pControl;
			CMyTreeUI::Node* node = (CMyTreeUI::Node*)pElement->GetTag();
			CDuiString strText;
			strText.Format("%d", (int)node->data()._pUserData);
			m_view->SetText(strText);
		}
		else {
			m_view->SetText("");
		}
	}
	else if ( msg.sType == "menu_add" ) {
		int nIndex = m_tree->GetCurSel();
		if (nIndex >= 0) {
			CListLabelElementUI * pElement = (CListLabelElementUI *)m_tree->GetItemAt(nIndex);
			CMyTreeUI::Node* node = (CMyTreeUI::Node*)pElement->GetTag();
			m_tree->AddNode("{x 4}{i gameicons.png 18 10}{x 4}�����ӵ�", node );
			m_tree->ExpandNode(node, true);
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