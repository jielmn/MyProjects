#ifdef _DEBUG
#include <vld.h>
#endif

#include "main.h"
#include <time.h>
#pragma comment(lib,"User32.lib")

static char * Date2String(char * szDest, DWORD dwDestSize, const time_t * t) {
	struct tm  tmp;
	localtime_s(&tmp, t);

	_snprintf_s(szDest, dwDestSize, dwDestSize, "%02d-%02d-%02d", tmp.tm_year + 1900, tmp.tm_mon + 1, tmp.tm_mday);
	return szDest;
}

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
	m_tree->AddNode(_T("参数设置123"),0,0,0,1,0xFFFF0000); 

	CEditUI * pEdit = new CEditUI;
	pEdit->SetText("请输入123");
	m_tree->AddNode("100", 0, (void *)(100), pEdit);

	pCategoryNode = m_tree->AddNode(_T("参数设置"));
	for (int i = 0; i < 9; ++i)
	{
		strText.Format("配置选项 %d", i + 1);
		pGameNode = m_tree->AddNode(strText, pCategoryNode, (void *)(i + 1));

		for (int j = 0; j < 5; j++) {
			if (j == 0) {
				strText.Format("SUB配置选项 %d-%d", i + 1, j + 1);
			}
			else {
				strText.Format("%d-%d", i + 1, j + 1);
			}
			

			if (j == 0) {
				CEditUI * pEdit = new CEditUI;
				pEdit->SetText("请输入");
				m_tree->AddNode(strText, pGameNode, (void *)((i + 1) * 10 + j), pEdit,-1,0xFF000000,1,0xFFFF0000, 60);
			}
			else if (j == 1) {
				CCheckBoxUI * pCheckBox = new CCheckBoxUI;
				m_tree->AddNode(strText, pGameNode, (void *)((i + 1) * 10 + j), pCheckBox);
			}
			else if (j == 2) {
				CFileBrowseUI * pFileBrowse = new CFileBrowseUI;
				m_tree->AddNode(strText, pGameNode, (void *)((i + 1) * 10 + j), pFileBrowse);
			}
			else if ( j == 3 ) {
				CComboUI * pCombo = new CComboUI;
				CListLabelElementUI * pElement = new CListLabelElementUI;
				pElement->SetText("123");
				pCombo->Add(pElement);

				pElement = new CListLabelElementUI;
				pElement->SetText("456");
				pCombo->Add(pElement);
				pCombo->SelectItem(0);

				m_tree->AddNode(strText, pGameNode, (void *)((i + 1) * 10 + j), pCombo, -1, 0xFF0000FF,1,0xFF0000FF);
			}
			else {
				DuiLib::CDateTimeUI * pDate = new DuiLib::CDateTimeUI;
				m_tree->AddNode(strText, pGameNode, (void *)((i + 1) * 10 + j), pDate, -1, 0xFF000000, 1, 0xFFFF0000);
			}
		}
	}
	m_tree->SelectItem(0);	

	m_tree->SetTitle(2, "ABCDEF");
	m_tree->SetTitle(3, "UVWXYZ");
	m_tree->SetTitle(4, "1234");
	WindowImplBase::InitWindow();
}

void   CDuiFrameWnd::Notify(TNotifyUI& msg) {
	DuiLib::CDuiString name = msg.pSender->GetName();
	if (msg.sType == "itemselect") {
		int nIndex = m_tree->GetCurSel();
		if (nIndex >= 0) {
			CControlUI * pControl = m_tree->GetItemAt(nIndex);
			CListContainerElementUI * pElement = (CListContainerElementUI *)pControl;
			CMyTreeCfgUI::Node* node = (CMyTreeCfgUI::Node*)pElement->GetTag();
			CHorizontalLayoutUI * pLayout = (CHorizontalLayoutUI *)pElement->GetItemAt(0);
			if ( pLayout->GetCount() == 3 ) {
				CDuiString a = pLayout->GetItemAt(2)->GetClass();
				CDuiString b = pLayout->GetItemAt(2)->GetText();
			}

			CMyTreeCfgUI::ConfigValue  cfgValue;
			bool bGetCfg = m_tree->GetConfigValue(nIndex, cfgValue);

			CDuiString strText;
			if ( bGetCfg) {
				if (cfgValue.m_eConfigType == CMyTreeCfgUI::ConfigType_EDIT) {
					strText.Format("UserData:%d,edit:%s", (int)node->data()._pUserData, cfgValue.m_strEdit);
				}
				else if (cfgValue.m_eConfigType == CMyTreeCfgUI::ConfigType_COMBO) {
					strText.Format("UserData:%d,combo:%d", (int)node->data()._pUserData, cfgValue.m_nComboSel);
				} 
				else if (cfgValue.m_eConfigType == CMyTreeCfgUI::ConfigType_CHECKBOX) {
					strText.Format("UserData:%d,checkbox:%s", (int)node->data()._pUserData, cfgValue.m_bCheckbox?"checked":"unchecked" );
				}
				else if (cfgValue.m_eConfigType == CMyTreeCfgUI::ConfigType_FileBrowse) {
					strText.Format("UserData:%d,filebrowse:%s", (int)node->data()._pUserData, cfgValue.m_strEdit);
				}
				else if (cfgValue.m_eConfigType == CMyTreeCfgUI::ConfigType_DateTime) {
					char szDate[256];
					Date2String(szDate, sizeof(szDate), &cfgValue.m_time);
					strText.Format("UserData:%d,DateTime:%s", (int)node->data()._pUserData, szDate);
				}
				else {
					assert(FALSE);
				}
			}
			else {
				strText.Format("UserData:%d", (int)node->data()._pUserData);
			}
			
			m_view->SetText(strText);
		}
		else {
			m_view->SetText("");
		}
	}

	WindowImplBase::Notify(msg);
}

CControlUI * CDuiFrameWnd::CreateControl(LPCTSTR pstrClass) {
	if (0 == strcmp(pstrClass, "MyTree")) {
		return new CMyTreeCfgUI(120);
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