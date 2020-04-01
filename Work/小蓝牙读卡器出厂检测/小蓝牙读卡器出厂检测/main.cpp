// win32_1.cpp : ����Ӧ�ó������ڵ㡣
//

#ifdef _DEBUG
#include "vld.h"
#endif

#include "Windows.h"
#include "main.h"
#include "UIMenu.h"
#include "LmnSerialPort.h"

#include "business.h"
#include "resource.h"

                                                             
CDuiFrameWnd::CDuiFrameWnd() {
	m_bComConnected = FALSE;
	m_bAutoTestStarted = FALSE;
	m_bBleConnected = FALSE;

	m_lblStatus   = 0;
	m_edReaderMac = 0;
	m_btnClearMac = 0;
	m_btnAutoTest = 0;
	m_rchInfo     = 0;
	m_btnTemp     = 0;
	m_opPass      = 0;
	m_opNotPass   = 0;

	for (int i = 0; i < 3; i++) {
		m_opErrReason[i] = 0;
	}

	m_btnSaveResult = 0;
}

CDuiFrameWnd::~CDuiFrameWnd() {

}

void  CDuiFrameWnd::InitWindow() {
	g_data.m_hWnd = GetHWND();

	m_lblStatus   = static_cast<DuiLib::CLabelUI*>(m_PaintManager.FindControl("lblStatus"));
	m_edReaderMac = static_cast<DuiLib::CEditUI*>(m_PaintManager.FindControl("edReaderMac"));
	m_btnClearMac = static_cast<DuiLib::CButtonUI*>(m_PaintManager.FindControl("btnClearMac"));
	m_btnAutoTest = static_cast<DuiLib::CButtonUI*>(m_PaintManager.FindControl("btnAutoTest"));
	m_rchInfo     = static_cast<DuiLib::CRichEditUI*>(m_PaintManager.FindControl("rchInfo"));
	m_btnTemp     = static_cast<DuiLib::CButtonUI*>(m_PaintManager.FindControl("btnTemp"));
	m_opPass      = static_cast<DuiLib::COptionUI*>(m_PaintManager.FindControl("radio1"));
	m_opNotPass   = static_cast<DuiLib::COptionUI*>(m_PaintManager.FindControl("radio2"));

	CDuiString  strText;
	for ( int i = 0; i < 3; i++ ) {
		strText.Format("option%d", i + 1);
		m_opErrReason[i] = static_cast<DuiLib::COptionUI*>(m_PaintManager.FindControl(strText));
	}

	m_btnSaveResult = static_cast<DuiLib::CButtonUI*>(m_PaintManager.FindControl("btnSaveResult"));
	
	CheckDevices(); 
	CheckControls();

	WindowImplBase::InitWindow();
}

CControlUI * CDuiFrameWnd::CreateControl(LPCTSTR pstrClass) {
	return WindowImplBase::CreateControl(pstrClass);
}

void CDuiFrameWnd::Notify(TNotifyUI& msg) {
	CDuiString  strName;
	strName = msg.pSender->GetName();

	if ( msg.sType == "click" ) {
		if (strName == "btnSetting") {
			RECT r = msg.pSender->GetPos();
			POINT point = { r.left, r.bottom };

			CMenuWnd* pMenu = new CMenuWnd(m_hWnd, msg.pSender);
			// CDuiPoint point = msg.ptMouse;
			ClientToScreen(m_hWnd, &point); 

			//ʹ����Դ��ʽ
			//STRINGorID xml(IDR_XML1);
			//pMenu->Init(NULL, xml, "xml", point);  

			//ʹ���ļ���ʽ
			STRINGorID xml("menusetting.xml"); 
			pMenu->Init(NULL, xml, 0, point); 
		}
		else if (strName == "btnAutoTest") {
			OnAutoTest();
		}
	}
	else if (msg.sType == "textchanged") {
		
	}
	WindowImplBase::Notify(msg);
}

LRESULT CDuiFrameWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if ( uMsg == UM_COM_STATUS ) {
		int nCom  = (int)wParam;
		BOOL bRet = (BOOL)lParam;
		CDuiString strText;

		if (nCom > 0) {
			if (bRet) {
				strText.Format("�򿪴���com%d�ɹ�", nCom);
				m_bComConnected = TRUE;
				m_bAutoTestStarted = FALSE;
			}
			else {
				strText.Format("�򿪴���com%dʧ�ܣ�", nCom);
				m_bComConnected = FALSE;
			}
		}
		else if (nCom == 0) {
			strText.Format("û�д�����ģ�鴮��");     
			m_bComConnected = FALSE;
		}
		else {
			strText.Format("�ҵ��������ģ�飬��ȷ��ֻ��һ��");
			m_bComConnected = FALSE;
		}

		m_lblStatus->SetText(strText);
		CheckControls();
	}
	else if (WM_DEVICECHANGE == uMsg) {
		CheckDevices();
	}
	return WindowImplBase::HandleMessage(uMsg,wParam,lParam);
}

void CDuiFrameWnd::CheckDevices() {	
	CBusiness::GetInstance()->CheckDevicesAsyn();
}

void  CDuiFrameWnd::CheckControls() {
	// ���û����������ģ��Ĵ���
	if (!m_bComConnected) {
		m_edReaderMac->SetEnabled(false);
		m_edReaderMac->SetBkColor(0xFFF1F1F1);
		m_btnClearMac->SetEnabled(false);
		m_btnAutoTest->SetEnabled(false);
		m_btnTemp->SetEnabled(false);
		m_opPass->SetEnabled(false);
		m_opNotPass->SetEnabled(false);
		for (int i = 0; i < 3; i++) {
			m_opErrReason[i]->SetEnabled(false);
		}
		m_btnSaveResult->SetEnabled(false);
	}
	else {
		// �Զ����Կ�ʼ��
		if (m_bAutoTestStarted) {
			m_edReaderMac->SetEnabled(false);
			m_edReaderMac->SetBkColor(0xFFF1F1F1);
			m_btnClearMac->SetEnabled(false);
			m_btnAutoTest->SetEnabled(true);
			m_btnAutoTest->SetText("ֹͣ����");
			m_btnTemp->SetEnabled(true);
			m_opPass->SetEnabled(true);
			m_opNotPass->SetEnabled(true);
			for (int i = 0; i < 3; i++) {
				m_opErrReason[i]->SetEnabled(true);
			}
			m_btnSaveResult->SetEnabled(true);
		}
		else {
			m_edReaderMac->SetEnabled(true);
			m_edReaderMac->SetBkColor(0xFFFFFFFF);
			m_btnClearMac->SetEnabled(true);
			m_btnAutoTest->SetEnabled(true);
			m_btnAutoTest->SetText("�Զ�����");
			m_btnTemp->SetEnabled(false);
			m_opPass->SetEnabled(false);
			m_opNotPass->SetEnabled(false);
			for (int i = 0; i < 3; i++) {
				m_opErrReason[i]->SetEnabled(false);
			}
			m_btnSaveResult->SetEnabled(false);
		}
	}
}

void   CDuiFrameWnd::OnAutoTest() {
	//if (m_bAutoTestStarted) {
	//	m_bAutoTestStarted = FALSE;
	//}
	//else {
	//	m_bAutoTestStarted = TRUE;
	//}
	//CheckControls();

	if (!m_bComConnected) {
		MessageBox(GetHWND(), "û�д�����ģ��Ĵ���", "����", 0);
		return;
	}

	// �����û�п�ʼ����
	if (!m_bAutoTestStarted) {
		CDuiString strMac = m_edReaderMac->GetText();
		strMac.Trim();

		if ( strMac.GetLength() != 12 ) {
			MessageBox(GetHWND(), "С����Reader��Mac��ַ��ʽ������ȷ��ʽΪxxxxxxxxxxxx", "����", 0);
			return;
		}

		for ( int i = 0; i < 12; i++ ) {
			char ch = Char2Lower(strMac.GetAt(i));
			if (!((ch >= '0' && ch <= '9') || (ch >= 'a' && ch <= 'f'))) {
				MessageBox(GetHWND(), "С����Reader��Mac��ַ��ʽ������ȷ��ʽΪxxxxxxxxxxxx", "����", 0);
				return;
			}
		}

		CBusiness::GetInstance()->StartAutoTestAsyn( strMac );
	}
	else {
		CBusiness::GetInstance()->StopAutoTestAsyn();
	}
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

	// ����icon
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


