#ifdef _DEBUG
#include <vld.h>
#endif

#include "main.h"
#pragma comment(lib,"User32.lib")

// menu
class CDuiMenu : public WindowImplBase
{
protected:
	virtual ~CDuiMenu() {};        // 私有化析构函数，这样此对象只能通过new来生成，而不能直接定义变量。就保证了delete this不会出错
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
	m_tree = (CMyTreeCfgUI *)m_PaintManager.FindControl("tree1");
	m_view = (CLabelUI*)m_PaintManager.FindControl("lblView");

	CMyTreeCfgUI::Node* pCategoryNode = NULL;
	CMyTreeCfgUI::Node* pGameNode = NULL;
	CMyTreeCfgUI::Node* pServerNode = NULL;
	CMyTreeCfgUI::Node* pRoomNode = NULL;
	CMyTreeCfgUI::Node* pRoomNode1 = NULL;

	CDuiString  strText;
	pCategoryNode = m_tree->AddNode(_T("参数设置"));
	for (int i = 0; i < 3; ++i)
	{
		strText.Format("配置选项 %d", i + 1);
		pGameNode = m_tree->AddNode(strText, pCategoryNode, (void *)(i + 1));

		for (int j = 0; j < 3; j++) {
			strText.Format("SUB配置选项 %d-%d", i+1, j + 1);

			if (j == 0) {
				CEditUI * pEdit = new CEditUI;
				pEdit->SetText("请输入");
				m_tree->AddNode(strText, pGameNode, (void *)((i + 1) * 10 + j), pEdit);
			}
			else if (j == 1) {
				CCheckBoxUI * pCheckBox = new CCheckBoxUI;
				//pCheckBox->SetText("请输入");
				pCheckBox->SetAttribute("normalimage", "file='checkbox_unchecked.png' dest='0,2,16,18'");
				pCheckBox->SetAttribute("selectedimage", "file='checkbox_checked.png' dest='0,2,16,18'");
				m_tree->AddNode(strText, pGameNode, (void *)((i + 1) * 10 + j), pCheckBox);
			}
			else {
				CComboUI * pCombo = new CComboUI;
				CListLabelElementUI * pElement = new CListLabelElementUI;
				pElement->SetText("123");
				pCombo->Add(pElement);

				pElement = new CListLabelElementUI;
				pElement->SetText("456");
				pCombo->Add(pElement);
				pCombo->SelectItem(0);

				pCombo->SetAttributeList("normalimage=\"file = 'Combo_nor.bmp' corner = '2,2,24,2'\" hotimage=\"file = 'Combo_over.bmp' corner = '2,2,24,2'\" pushedimage=\"file = 'Combo_over.bmp' corner = '2,2,24,2'\" textpadding=\"5, 1, 5, 1\" ");

				m_tree->AddNode(strText, pGameNode, (void *)((i + 1) * 10 + j), pCombo);
			}
		}
	}
	m_tree->SelectItem(0);

	WindowImplBase::InitWindow();
}

void   CDuiFrameWnd::Notify(TNotifyUI& msg) {
	DuiLib::CDuiString name = msg.pSender->GetName();
	if (msg.sType == "click") {
		int index = m_tree->GetItemIndex(msg.pSender);
		if (index != -1) {
			if (_tcscmp(msg.pSender->GetClass(), DUI_CTR_BUTTON) == 0) {
				CMyTreeCfgUI::Node* node = (CMyTreeCfgUI::Node*)msg.pSender->GetParent()->GetParent()->GetTag();
				m_tree->ExpandNode(node, !node->data()._expand);
			}
		}
	}
	else if (msg.sType == "itemclick") {
		int a = 100;
	}

	WindowImplBase::Notify(msg);
}

CControlUI * CDuiFrameWnd::CreateControl(LPCTSTR pstrClass) {
	if (0 == strcmp(pstrClass, "MyTree")) {
		return new CMyTreeCfgUI();
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