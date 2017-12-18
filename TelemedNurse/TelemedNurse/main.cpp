//#include <windows.h>
#include <afxdb.h>   
#include <odbcinst.h>

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
			return;
		}
		WindowImplBase::Notify(msg);
	}

	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		return __super::HandleMessage(uMsg, wParam, lParam);
	}
};


class CDuiFrameWnd : public WindowImplBase, public sigslot::has_slots<>
{
public:
	virtual LPCTSTR    GetWindowClassName() const { return _T("DUIMainFrame"); }
	virtual CDuiString GetSkinFile() { return _T("main.xml"); }
	virtual CDuiString GetSkinFolder() { return _T("res"); }

	void ModifyPatient() {
		CListUI * pList = (CListUI *)m_PaintManager.FindControl("patients_list");
		if (0 == pList) {
			return;
		}

		int nIndex = pList->GetCurSel();
		if (nIndex < 0) {
			::MessageBox(m_hWnd, "没有选中病人信息", "修改病人信息", 0);
			return;
		}

		CListTextElementUI* pItem = (CListTextElementUI*)pList->GetItemAt(nIndex);
		char * szId = (char *)pItem->GetTag();
		PatientInfo tPatient;
		CBusiness::GetInstance()->GetPatient(szId, &tPatient);

		CPatientWnd * pPatientDlg = new CPatientWnd(FALSE);
		memcpy(&pPatientDlg->m_tPatientInfo, &tPatient, sizeof(PatientInfo));
		pPatientDlg->Create(this->m_hWnd, _T("修改病人信息"), UI_WNDSTYLE_FRAME | WS_POPUP, NULL, 0, 0, 0, 0);
		pPatientDlg->CenterWindow();
		pPatientDlg->ShowModal();
		delete pPatientDlg;
		return;
	}

	void DeletePatient() {
		CListUI * pList = (CListUI *)m_PaintManager.FindControl("patients_list");
		if (0 == pList) {
			return;
		}

		int nIndex = pList->GetCurSel();
		if (nIndex < 0) {
			::MessageBox(m_hWnd, "没有选中病人信息", "删除病人信息", 0);
			return;
		}

		CListTextElementUI* pItem = (CListTextElementUI*)pList->GetItemAt(nIndex);
		char * szId = (char *)pItem->GetTag();

		if (::MessageBox(m_hWnd, "确定要删除吗？", "删除病人信息", MB_YESNO) == IDYES) {
			int ret = CBusiness::GetInstance()->DeletePatient(szId);
			if (0 == ret) {
				PatientInfo tInfo;
				strncpy_s(tInfo.szId, szId, sizeof(tInfo.szId));
				CBusiness::GetInstance()->sigAMPatientRet.emit(&tInfo, OPERATION_DELETE);
			}
		}
	}


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
			else if ( name == "btnModifyPatient" ) {
				ModifyPatient();
				return;
			}
			else if (name == "btnDelPatient") {
				DeletePatient();
				return;
			}			
			else if ( name == "import_excel" ) {
				OPENFILENAME ofn = { 0 };
				TCHAR strFilename[MAX_PATH] = { 0 };//用于接收文件名  
				ofn.lStructSize = sizeof(OPENFILENAME);//结构体大小  
				ofn.hwndOwner = NULL;//拥有着窗口句柄，为NULL表示对话框是非模态的，实际应用中一般都要有这个句柄  
				ofn.lpstrFilter = TEXT("Excel Flie(*.xls)\0*.xls\0所有文件\0*.*\0\0");//设置过滤  
				ofn.nFilterIndex = 1;//过滤器索引  
				ofn.lpstrFile = strFilename;//接收返回的文件名，注意第一个字符需要为NULL  
				ofn.nMaxFile = sizeof(strFilename);//缓冲区长度  
				//ofn.lpstrInitialDir = NULL;//初始目录为默认  
				ofn.lpstrInitialDir = CPaintManagerUI::GetInstancePath();
				ofn.lpstrTitle = TEXT("请选择一个文件");//使用系统默认标题留空即可  
				ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;//文件、目录必须存在，隐藏只读选项  
				if (GetOpenFileName(&ofn))
				{
					int ret = CBusiness::GetInstance()->ImportExcel(strFilename);
					if (0 == ret) 
						::MessageBox(m_hWnd, "导入Excel成功！", "导入Excel", 0);
					else
						::MessageBox(m_hWnd, GetErrDescription((GlobalErrorNo)ret), "导入Excel", 0);
				}
				return;
			}
		}
		else if (msg.sType == "itemselect") {
			if (name == "patients_list_short") {
				CControlUI * pBlock0 = m_PaintManager.FindControl("VerticalLayout_Temp");
				CControlUI * pBlock1 = m_PaintManager.FindControl("VerticalLayout_Temp_none");
				if (pBlock0) {
					pBlock0->SetVisible(false);
				}
				if (pBlock1) {
					pBlock1->SetVisible(true);
				}
				return;
			}			
		}
		else if (msg.sType == "itemactivate") {
			ModifyPatient();
			return;
		}
		else if (msg.sType == _T("menu"))
		{
			POINT pt = { msg.ptMouse.x, msg.ptMouse.y };
			CControlUI * pControl = m_PaintManager.FindControl(pt);
			if ( 0 == pControl) {
				return;
			}

			CDuiString sFindCtlClass = pControl->GetClass();
			if ( sFindCtlClass == "ListTextElement") {
				CDuiString sListName  = pControl->GetParent()->GetParent()->GetName();
				if (sListName == "patients_list") {
					CDuiMenu *pMenu = new CDuiMenu(_T("menu.xml"), msg.pSender);
					pMenu->Init(*this, pt);
					pMenu->ShowWindow(TRUE);
				}
			}
			return;
		}
		else if (msg.sType == "menu_modify_patient") {
			ModifyPatient();
			return;
		}
		else if (msg.sType == "menu_delete_patient") {
			DeletePatient();
			return;
		}
		//TRACE("msgtype = %s\n", (const char *)msg.sType);
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
		SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0);

		char buf[8192];
		std::vector<PatientInfo *> v;
		CBusiness::GetInstance()->GetAllPatients(v);

		CListUI* pListShort = static_cast<CListUI*>(m_PaintManager.FindControl(_T("patients_list_short")));
		CListUI* pList      = static_cast<CListUI*>(m_PaintManager.FindControl(_T("patients_list")));

		if (pListShort) {
			std::vector<PatientInfo *>::iterator it;
			for (it = v.begin(); it != v.end(); ++it) {
				PatientInfo * pInfo = *it;

				CListTextElementUI* pListElement = new CListTextElementUI;
				pListShort->Add(pListElement);
				pListElement->SetText(0, pInfo->szId);
				pListElement->SetText(1, pInfo->szName);
				pListElement->SetText(2, pInfo->szBedNo);

				char * szId = new char[sizeof(pInfo->szId)];
				strncpy_s(szId, sizeof(pInfo->szId),  pInfo->szId, sizeof(pInfo->szId) );
				pListElement->SetTag((UINT_PTR)szId);
			}
		}

		if (pList) {
			std::vector<PatientInfo *>::iterator it;
			for (it = v.begin(); it != v.end(); ++it) {
				PatientInfo * pInfo = *it;

				CListTextElementUI* pListElement = new CListTextElementUI;
				pList->Add(pListElement);
				pListElement->SetText(0, pInfo->szId);
				pListElement->SetText(1, pInfo->szInNo);
				pListElement->SetText(2, pInfo->szName);
				pListElement->SetText(3, GetSex(pInfo->bMale) );
				pListElement->SetText(4, GetInt( buf, sizeof(buf), pInfo->nAge ) );
				pListElement->SetText(5, pInfo->szOffice);
				pListElement->SetText(6, pInfo->szBedNo);
				pListElement->SetText(7, ConvertDate( buf, sizeof(buf), &pInfo->tInDate ) );
				pListElement->SetText(8, pInfo->szCardNo);

				char * szId = new char[sizeof(pInfo->szId)];
				strcpy_s(szId, sizeof(pInfo->szId), pInfo->szId);
				pListElement->SetTag((UINT_PTR)szId);
			}
		}

		ClearVector(v);
		WindowImplBase::InitWindow();
	}

	void OnPatientEvent(PatientInfo * pInfo, OperationType eType ) {
		char buf[8192];
		CListUI* pListShort = static_cast<CListUI*>(m_PaintManager.FindControl(_T("patients_list_short")));
		CListUI* pList = static_cast<CListUI*>(m_PaintManager.FindControl(_T("patients_list")));

		if (eType == OPERATION_ADD) {
			if (pListShort) {
				CListTextElementUI* pListElement = new CListTextElementUI;
				pListShort->Add(pListElement);
				pListElement->SetText(0, pInfo->szId);
				pListElement->SetText(1, pInfo->szName);
				pListElement->SetText(2, pInfo->szBedNo);

				char * szId = new char[sizeof(pInfo->szId)];
				strcpy_s(szId, sizeof(pInfo->szId), pInfo->szId);
				pListElement->SetTag((UINT_PTR)szId);
			}

			if (pList) {
				CListTextElementUI* pListElement = new CListTextElementUI;
				pList->Add(pListElement);
				pListElement->SetText(0, pInfo->szId);
				pListElement->SetText(1, pInfo->szInNo);
				pListElement->SetText(2, pInfo->szName);
				pListElement->SetText(3, GetSex(pInfo->bMale));
				pListElement->SetText(4, GetInt(buf, sizeof(buf), pInfo->nAge));
				pListElement->SetText(5, pInfo->szOffice);
				pListElement->SetText(6, pInfo->szBedNo);
				pListElement->SetText(7, ConvertDate(buf, sizeof(buf), &pInfo->tInDate));
				pListElement->SetText(8, pInfo->szCardNo);

				char * szId = new char[sizeof(pInfo->szId)];
				strcpy_s(szId, sizeof(pInfo->szId), pInfo->szId);
				pListElement->SetTag((UINT_PTR)szId);
			}
		}
		else if (eType == OPERATION_MODIFY) {
			if (pListShort) {
				int n = pListShort->GetCount();
				for (int i = 0; i < n; i++) {
					CListTextElementUI* pItem = (CListTextElementUI*)pListShort->GetItemAt(i);
					char * szId = (char *)pItem->GetTag();
					if (0 == strcmp(szId, pInfo->szId)) {
						pItem->SetText(1, pInfo->szName);
						pItem->SetText(2, pInfo->szBedNo);
						break;
					}
				}
			}

			if (pList) {
				int n = pList->GetCount();
				for (int i = 0; i < n; i++) {
					CListTextElementUI* pItem = (CListTextElementUI*)pList->GetItemAt(i);
					char * szId = (char *)pItem->GetTag();

					if (0 == strcmp(szId, pInfo->szId)) {
						pItem->SetText(1, pInfo->szInNo);
						pItem->SetText(2, pInfo->szName);
						pItem->SetText(3, GetSex(pInfo->bMale));
						pItem->SetText(4, GetInt(buf, sizeof(buf), pInfo->nAge));
						pItem->SetText(5, pInfo->szOffice);
						pItem->SetText(6, pInfo->szBedNo);
						pItem->SetText(7, ConvertDate(buf, sizeof(buf), &pInfo->tInDate));
						pItem->SetText(8, pInfo->szCardNo);
						break;
					}
				}
			}
		}
		else if (eType == OPERATION_DELETE) {
			if (pListShort) {
				int n = pListShort->GetCount();
				for (int i = 0; i < n; i++) {
					CListTextElementUI* pItem = (CListTextElementUI*)pListShort->GetItemAt(i);
					char * szId = (char *)pItem->GetTag();
					if (0 == strcmp(szId, pInfo->szId)) {
						pListShort->RemoveAt(i);
						break;
					}
				}
			}

			if (pList) {
				int n = pList->GetCount();
				for (int i = 0; i < n; i++) {
					CListTextElementUI* pItem = (CListTextElementUI*)pList->GetItemAt(i);
					char * szId = (char *)pItem->GetTag();

					if (0 == strcmp(szId, pInfo->szId)) {
						pList->RemoveAt(i);
						break;
					}
				}
			}
		}
		
	}

};

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	int ret = 0;
	CBusiness::GetInstance()->sigInit.emit(&ret);
	if (0 != ret) {
		::MessageBox(0, GetErrDescription((GlobalErrorNo)ret), "初始化数据库错误", 0);
		return -1;
	}

	HICON hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));

	CPaintManagerUI::SetInstance(hInstance);

	CDuiFrameWnd duiFrame;
	CBusiness::GetInstance()->sigAMPatientRet.connect(&duiFrame, &CDuiFrameWnd::OnPatientEvent);

	duiFrame.Create(NULL, _T("DUIWnd"), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE, 0, 0, 800, 600);
	duiFrame.CenterWindow();

	HWND hWnd = duiFrame.GetHWND();
	::SendMessage(hWnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
	::SendMessage(hWnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
	
	duiFrame.ShowModal();
	delete duiFrame;

	CBusiness::GetInstance()->sigDeinit.emit(&ret);
	return 0;

}
