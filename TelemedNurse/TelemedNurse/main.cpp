#include <windows.h>
#include "resource.h"
#include "UIlib.h"
using namespace DuiLib;

#ifdef GetNextSibling
#undef GetNextSibling
#endif
#include "Xml2Chart.h"
#include "PatientDlg.h"
#include "Business.h"

#pragma comment(lib,"User32.lib")

class CTempChartUI : public CControlUI
{
public:
	CTempChartUI()
	{
		m_XmlChartFile = new CXml2ChartFile;		
		m_XmlChartFile->ReadXmlChartFile(CPaintManagerUI::GetInstancePath() + "res\\体温表设计.xml");
	}

	~CTempChartUI() {
		if (m_XmlChartFile) {
			delete m_XmlChartFile;
			m_XmlChartFile = 0;
		}
	}

	virtual void DoEvent(TEventUI& event) {
		CControlUI::DoEvent(event);
	}

	virtual bool DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl) {
		RECT r = this->GetPos();
		if ( m_XmlChartFile->m_ChartUI ) {
			int a = ( (r.right - r.left - 1) - 120 ) / 42;
			int b = ((1000 - 1) - 85 - 125 - 200) / 40;

			int right  = a * 42 + 120 + r.left + 1;
			int h = b * 40 + 85 + 125 + 200 + 1;

			m_XmlChartFile->m_ChartUI->SetRect( r.left, r.top, right, r.top + h );
			m_XmlChartFile->m_ChartUI->RecacluteLayout();
			DrawXml2ChartUI( hDC, m_XmlChartFile->m_ChartUI);
		}
		//return CControlUI::DoPaint(hDC, rcPaint, pStopControl);
		return true;
	}

	CXml2ChartFile *  m_XmlChartFile;
};

class CDialogBuilderCallbackEx : public IDialogBuilderCallback
{
public:
	CControlUI* CreateControl(LPCTSTR pstrClass)
	{
		if ( 0 == strcmp("ChartImage", pstrClass))
			return new CTempChartUI;
		return NULL;
	}
};

class CDuiFrameWnd : public WindowImplBase
{
public:
	virtual LPCTSTR    GetWindowClassName() const { return _T("DUIMainFrame"); }
	virtual CDuiString GetSkinFile() { return _T("main.xml"); }
	virtual CDuiString GetSkinFolder() { return _T("res"); }


	virtual void  Notify(TNotifyUI& msg) {
		CDuiString name = msg.pSender->GetName();
		if (msg.sType == _T("selectchanged"))
		{			
			CTabLayoutUI* pControl = static_cast<CTabLayoutUI*>(m_PaintManager.FindControl(_T("switch")));
			if (pControl)
			{
				if (name == _T("temperature"))
					pControl->SelectItem(0);
				else if (name == _T("patients_info"))
					pControl->SelectItem(1);
				return;
			}			
		}
		else if (msg.sType == _T("click") ) {
			if ( name == "btnAddPatient" ) {
				CPatientWnd * pPatientDlg = new CPatientWnd;
				pPatientDlg->Create( this->m_hWnd, _T("新增病人信息"), UI_WNDSTYLE_FRAME | WS_POPUP, NULL, 0, 0, 0, 0 );
				pPatientDlg->CenterWindow();
				pPatientDlg->ShowModal();
				delete pPatientDlg;
				return;
			}
		}
		WindowImplBase::Notify(msg);
	}

	virtual CControlUI * CreateControl(LPCTSTR pstrClass) {
		if ( 0 == strcmp("Temperature_data", pstrClass) ) {
			CDialogBuilder builder;
			CDialogBuilderCallbackEx cb;
			CControlUI * pUI = builder.Create(_T("temperature_data.xml"), (UINT)0, &cb, &m_PaintManager );
			return pUI;
		}
		else if (0 == strcmp("Patients", pstrClass) ) {
			CDialogBuilder builder;
			CDialogBuilderCallbackEx cb;
			CControlUI * pUI = builder.Create(_T("patients.xml"), (UINT)0, &cb, &m_PaintManager);
			return pUI;
		}
		return 0;
	}

	virtual void   InitWindow()
	{		
		WindowImplBase::InitWindow();
	}

};

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	CBusiness::GetInstance()->sigInitDb.emit();

	HICON hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));

	CPaintManagerUI::SetInstance(hInstance);

	CDuiFrameWnd duiFrame;
	duiFrame.Create(NULL, _T("DUIWnd"), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE, 0, 0, 800, 600);
	duiFrame.CenterWindow();

	HWND hWnd = duiFrame.GetHWND();
	::SendMessage(hWnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
	::SendMessage(hWnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
	
	duiFrame.ShowModal();
	delete duiFrame;

	CBusiness::GetInstance()->sigDeinitDb.emit();
	return 0;

}
