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
#include "LmnContainer.h"
#include "LmnConfig.h"
#include "LmnLog.h"
#include "LmnThread.h"
#include "BindingReader.h"
#include "UiMessage.h"
#include "SerialPort.h"

#pragma comment(lib,"User32.lib")


HWND    g_hWnd = 0;
BOOL    g_bUseUtf8 = FALSE;

#define  INDEX_PATIENT_ID            0
#define  INDEX_PATIENT_INNO          1
#define  INDEX_PATIENT_NAME          2
#define  INDEX_PATIENT_SEX           3
#define  INDEX_PATIENT_AGE           4
#define  INDEX_PATIENT_OFFICE        5
#define  INDEX_PATIENT_BEDNO         6
#define  INDEX_PATIENT_INDATE        7
#define  INDEX_PATIENT_CARDNO        8
#define  INDEX_PATIENT_TAGS          9

#define  SHORT_INDEX_PATIENT_ID            0
#define  SHORT_INDEX_PATIENT_NAME          1
#define  SHORT_INDEX_PATIENT_BEDNO         2

class CMyProgress : public CProgressUI
{
public:
	CMyProgress(CPaintManagerUI *p, CDuiString sForeImage) :m_pManager(p), m_nPos(0), m_sForeImage(sForeImage) {
	}
	~CMyProgress() {}

	void DoEvent(TEventUI& event) {
		if (event.Type == UIEVENT_TIMER && event.wParam == 10)
		{
			const int MARGIN = 3;
			const int STEP = 4;
			CDuiString imageProp;
			const int PROGRESS_WIDTH = 36;
			const int PROGRESS_HEIGHT = 7;
			const int HORIZONTAL_MARGIN = 3;

			int width = this->GetWidth();
			int height = this->GetHeight();

			width -= 2 * HORIZONTAL_MARGIN;

			if (m_nPos < PROGRESS_WIDTH) {
				imageProp.Format("file='%s' source='%d,%d,%d,%d' dest='%d,%d,%d,%d'", (const char *)m_sForeImage,
					PROGRESS_WIDTH - m_nPos, 0, PROGRESS_WIDTH, PROGRESS_HEIGHT, HORIZONTAL_MARGIN,
					MARGIN, HORIZONTAL_MARGIN + m_nPos, height - MARGIN);
			}
			else if (m_nPos > width) {
				imageProp.Format("file='%s' source='%d,%d,%d,%d' dest='%d,%d,%d,%d'", (const char *)m_sForeImage,
					0, 0, PROGRESS_WIDTH + width - m_nPos, PROGRESS_HEIGHT,
					m_nPos - PROGRESS_WIDTH + HORIZONTAL_MARGIN, MARGIN, width + HORIZONTAL_MARGIN, height - MARGIN);
			}
			else {
				imageProp.Format("file='%s' source='%d,%d,%d,%d' dest='%d,%d,%d,%d'", (const char *)m_sForeImage, 0, 0, PROGRESS_WIDTH, PROGRESS_HEIGHT,
					m_nPos - PROGRESS_WIDTH + HORIZONTAL_MARGIN, MARGIN, m_nPos + HORIZONTAL_MARGIN, height - MARGIN);
			}

			this->SetForeImage(imageProp);

			m_nPos += STEP;
			if (m_nPos > width + PROGRESS_WIDTH) {
				m_nPos = 0;
			}
			return;
		}

		CProgressUI::DoEvent(event);
	}

	void Stop() {
		if (m_pManager) {
			m_pManager->KillTimer(this, 10);
		}
	}

	void Start() {
		if (m_pManager) {
			m_pManager->SetTimer(this, 10, 50);
		}
		m_nPos = 0;
	}
private:
	CPaintManagerUI * m_pManager;
	int               m_nPos;
	CDuiString        m_sForeImage;
};



class CTempChartUI : public CControlUI
{
public:
	CTempChartUI()
	{
		m_XmlChartFile = new CXml2ChartFile;		
		m_XmlChartFile->ReadXmlChartFile(CPaintManagerUI::GetInstancePath() + "res\\体温表设计.xml");

		m_tInDate    = 0;
		m_tFistDay   = 0;                            // 第一天日期
		m_nWeekIndex = 0;                            // 第几周
		memset(m_TempData, 0, sizeof(m_TempData));   // 温度数据

		for (int i = 0; i < 7; i++) {
			for (int j = 0; j < 6; j++) {
				m_TempData[i][j] = GetRand( 3700, 4100);
			}
		}
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
		char buf[8192];
		CDuiString tmp;
		RECT r = this->GetPos();

		if ( m_XmlChartFile->m_ChartUI ) {
			int a = ( (r.right - r.left - 1) - 120 ) / 42;
			int b = ((1000 - 1) - 85 - 125 - 200) / 40;

			int right  = a * 42 + 120 + r.left + 1;
			int h = b * 40 + 85 + 125 + 200 + 1;

			m_XmlChartFile->m_ChartUI->SetRect( r.left, r.top, right, r.top + h );
			m_XmlChartFile->m_ChartUI->RecacluteLayout();

			char szHospitalName[256];
			g_cfg->GetConfig( "hospital name", buf, sizeof(buf), "某某医院" );
			if (g_bUseUtf8) {
				Utf8ToAnsi( szHospitalName, sizeof(szHospitalName), buf );
			}
			else {
				strncpy_s(szHospitalName, buf, sizeof(szHospitalName));
			}

			CXml2ChartUI * pUI = m_XmlChartFile->FindChartUIByName("hospital_name");
			if (pUI) {
				pUI->SetText(szHospitalName);
			}

			pUI = m_XmlChartFile->FindChartUIByName("patient_name");
			if (pUI) {
				tmp = "姓名：";
				tmp += m_sPatientName;
				pUI->SetText((const char *)tmp);
			}

			CXml2ChartUI * pIndate = m_XmlChartFile->FindChartUIByName("indate");
			if (pIndate) {
				tmp = "入院日期：";
				tmp += ConvertDate(buf, sizeof(buf), &m_tInDate);
				pIndate->SetText((const char *)tmp);
			}

			pUI = m_XmlChartFile->FindChartUIByName("office");
			if (pUI) {
				tmp = "科室：";
				tmp += m_sOffice;
				pUI->SetText((const char *)tmp);
			}

			pUI = m_XmlChartFile->FindChartUIByName("bed_no");
			if (pUI) {
				tmp = "床号：";
				tmp += m_sBedNo;
				pUI->SetText((const char *)tmp);
			}

			pUI = m_XmlChartFile->FindChartUIByName("inno");
			if (pUI) {
				tmp = "住院号：";
				tmp += m_sInNo;
				pUI->SetText((const char *)tmp);
			}

			for (int i = 0; i < 7; i++) {
				CDuiString name;
				name.Format("no%d", i+1);
				pUI = m_XmlChartFile->FindChartUIByName( (const char *)name );
				if (pUI) {
					CDuiString tmpStr;
					tmpStr.Format("%d", m_nWeekIndex * 7 + i + 1);
					pUI->SetText( (const char *)tmpStr );
				}
			}

			for (int i = 0; i < 7; i++) {
				CDuiString name;
				name.Format("day%d", i + 1);
				pUI = m_XmlChartFile->FindChartUIByName((const char *)name);
				if (pUI) {
					time_t t = m_tFistDay + 3600 * 24 * i;
					ConvertDate(buf, sizeof(buf), &t);
					pUI->SetText(buf);
				}
			}
			

			DrawXml2ChartUI( hDC, m_XmlChartFile->m_ChartUI);

			// 画温度点 
			CXml2ChartUI * pMainBlock = m_XmlChartFile->FindChartUIByName("MainBlock");
			if (pMainBlock != 0) {
				RECT rectBlock = pMainBlock->GetAbsoluteRect();
				int w = rectBlock.right - rectBlock.left;
				int h = rectBlock.bottom - rectBlock.top;

				int grid_h = h / 40;
				int grid_w = w / 42;

				//给出合理的画点的园半径
				int m = grid_h < grid_w ? grid_h : grid_w;
				int RADIUS = 6;
				if (m >= 14) {
					RADIUS = 6;
				}
				else {
					RADIUS = 4;
				}

				BOOL bFistPoint = TRUE;
				for (int i = 0; i < 7; i++) {
					for (int j = 0; j < 6; j++) {
						int nTemp = m_TempData[i][j];

						// 不在35-42度范围内
						if ( !(nTemp >= 3400 && nTemp <= 4200) ) {
							continue;
						}

						// 高度比例
						double h_ratio = (nTemp - 3400.0) / (4200.0 - 3400.0);
						int    nTempHeight = (int)(h_ratio * h);

						CPoint pt;
						pt.x = grid_w * (i * 6 + j) + grid_w / 2 + rectBlock.left;
						pt.y = rectBlock.bottom - nTempHeight;

						DrawTempPoint(pt, hDC, RADIUS);

						// 是否第一个点
						if (bFistPoint) {
							::MoveToEx(hDC, pt.x, pt.y, 0);
							bFistPoint = FALSE;
						}
						else {
							::LineTo(hDC, pt.x, pt.y);
						}
					}
				}
			}
		}
		//return CControlUI::DoPaint(hDC, rcPaint, pStopControl);
		return true;
	}

	void DrawTempPoint(CPoint point, HDC hDc, int RADIUS = 6)
	{
		::Ellipse(hDc, point.x - RADIUS, point.y - RADIUS, point.x + RADIUS, point.y + RADIUS);

		int nTmp = (int)(0.707 * RADIUS);

		POINT points[2] = { { point.x - nTmp, point.y - nTmp },{ point.x + nTmp, point.y + nTmp } };
		::Polyline(hDc, points, 2);

		POINT points1[2] = { { point.x + nTmp, point.y - nTmp },{ point.x - nTmp, point.y + nTmp } };
		::Polyline(hDc, points1, 2);
	}

	void SetTempData( std::vector<TagData*> & vTempData ) {
		std::vector<TagData*>::iterator it = vTempData.begin();

		memset(m_TempData, 0, sizeof(m_TempData));
		for (int i = 0; i < 7; i++) {
			time_t  t = m_tFistDay + 3600 * 24 * i;
			for (int j = 0; j < 6; j++) {
				BOOL bFind = FALSE;
				for (; it != vTempData.end(); it++) {
					TagData* pData = *it;
					// 如果数据时间小于格子的时间
					if ( pData->tTime < t ) {
						continue;
					}
					// 时间差小于一天
					if ( pData->tTime - t < 3600 * 24) {
						m_TempData[i][j] = pData->dwTemperature;
						it++;
						bFind = TRUE;
						break;
					}
					else {
						break;
					}
				}
				// 数据时间的最小的都比当前格子时间大
				if (!bFind) {
					break;
				}
			}
		}
	}

	CXml2ChartFile *  m_XmlChartFile;

	CDuiString        m_sPatientName;                // 病人名字
	time_t            m_tInDate;                     // 住院日期
	CDuiString        m_sOffice;                     // 科室
	CDuiString        m_sBedNo;                      // 床号
	CDuiString        m_sInNo;                       // 住院号

	time_t            m_tFistDay;                     // 第一天日期
	int               m_nWeekIndex;                   // 第几周

	int               m_TempData[7][6];               // 温度数据
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
protected:
	TagId                  m_tTagId;
	int                    m_nInventoryError;
	SerialPortStatus       m_eSerialPortStatus;
	std::vector<TagData*>  m_vTempetatureData;                   // 温度数据，用于温度曲线绘图
	PatientInfo            m_tPatient;                           // 病人信息，用于温度曲线绘图

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

	void BindingPatient() {
		CListUI * pList = (CListUI *)m_PaintManager.FindControl("patients_list");
		if (0 == pList) {
			return;
		}

		int nIndex = pList->GetCurSel();
		if (nIndex < 0) {
			::MessageBox(m_hWnd, "没有选中病人信息", "绑定病人温度贴", 0);
			return;
		}

		CListTextElementUI* pItem = (CListTextElementUI*)pList->GetItemAt(nIndex);
		char * szId = (char *)pItem->GetTag();

		TagInfo tTagInfo;
		memcpy( &tTagInfo.tagId, &m_tTagId, sizeof(TagId));
		strncpy_s(tTagInfo.szPatientId, szId, sizeof(tTagInfo.szPatientId));
		tTagInfo.tBindingDate = time(0);
		int ret = CBusiness::GetInstance()->BindingTag(&tTagInfo);

		if (0 == ret) {
			CBusiness::GetInstance()->sigBinding.emit(&tTagInfo);
			::MessageBox(m_hWnd, "绑定Tag成功！", "绑定Tag", 0);
		}
		else
			::MessageBox(m_hWnd, GetErrDescription((GlobalErrorNo)ret), "绑定Tag", 0);
	}

	void  OnItemSelected(CDuiString name) {
		char buf[8192];

		if (name == "patients_list_short") {
			CListUI * pListShort = (CListUI *)m_PaintManager.FindControl(name);
			if (0 == pListShort) {
				return;
			}

			CControlUI * pBlock0 = m_PaintManager.FindControl("VerticalLayout_Temp");
			CControlUI * pBlock1 = m_PaintManager.FindControl("VerticalLayout_Temp_none");

			int nIndex = pListShort->GetCurSel();
			if (nIndex < 0) {
				if (pBlock0) {
					pBlock0->SetVisible(false);
				}
				if (pBlock1) {
					pBlock1->SetVisible(false);
				}
				return;
			}

			CListTextElementUI* pItem = (CListTextElementUI*)pListShort->GetItemAt(nIndex);
			char * szId = (char *)pItem->GetTag();

			PatientInfo tPatientInfo;
			int ret = CBusiness::GetInstance()->GetPatient(szId, &tPatientInfo);
			if (0 != ret) {
				return;
			}

			std::vector<TagInfo *> vTags;
			ret = CBusiness::GetInstance()->GetPatientTags(szId, vTags);
			if (0 != ret) {
				return;
			}

			// 没有绑定tag
			if (0 == vTags.size()) {
				if (pBlock0) {
					pBlock0->SetVisible(false);
				}
				if (pBlock1) {
					pBlock1->SetVisible(true);
				}
			}
			// 绑定了Tag;
			else {
				ClearVector(m_vTempetatureData);
				memset(&m_tPatient, 0, sizeof(m_tPatient));

				CBusiness::GetInstance()->GetLatestTempData(szId, m_vTempetatureData);
				CBusiness::GetInstance()->GetPatient(szId, &m_tPatient);

				time_t  tFirstDay = 0;
				if (m_vTempetatureData.size() > 0) {
					TagData* pData = m_vTempetatureData.at( m_vTempetatureData.size() - 1 );
					time_t  tMaxTime = TrimDatetime( pData->tTime );

					pData = m_vTempetatureData.at(0);
					time_t tMinTime = TrimDatetime( pData->tTime );

					// 时间跨度小于一周
					if (tMaxTime - tMinTime < 3600 * 24 * 6 ) {
						tFirstDay = tMinTime;
					}
				}
				else {
					tFirstDay = TrimDatetime( time(0) - 3600 * 24 * 6 );
				}

				CDateTimeUI * pDateTime = (CDateTimeUI *)m_PaintManager.FindControl("DateTime1");
				if (0 != pDateTime) {
					SYSTEMTIME s;
					struct tm t;
					localtime_s(&t, &tFirstDay);
					memset(&s, 0, sizeof(s));
					s.wYear  = t.tm_year + 1900;
					s.wMonth = t.tm_mon + 1;
					s.wDay   = t.tm_mday;

					pDateTime->SetText(ConvertDate(buf, sizeof(buf), &tFirstDay) );
					pDateTime->SetTime(&s);
				}
				

				CTempChartUI * pChart0 = (CTempChartUI *)m_PaintManager.FindControl("chart0");
				if (pChart0) {
					pChart0->m_tFistDay = tFirstDay;
					pChart0->m_sPatientName = m_tPatient.szName;
					pChart0->m_sBedNo = m_tPatient.szBedNo;
					pChart0->m_sInNo = m_tPatient.szInNo;
					pChart0->m_sOffice = m_tPatient.szOffice;
					pChart0->m_tInDate = m_tPatient.tInDate;
					pChart0->SetTempData(m_vTempetatureData);
				}

				// 界面显示
				if (pBlock0) {
					pBlock0->SetVisible(true);

					CComboUI * pCombo = (CComboUI *)m_PaintManager.FindControl("cmbTimeSpan");
					if (pCombo) {
						pCombo->SelectItem(0);
					}

					CVerticalLayoutUI * pLayoutChart = (CVerticalLayoutUI*)m_PaintManager.FindControl("layoutChart");
					if (pLayoutChart) {
						CScrollBarUI* pScrollBar = pLayoutChart->GetVerticalScrollBar();
						if (pScrollBar) {
							pScrollBar->SetScrollPos(0);
						}
					}
				}

				if (pBlock1) {
					pBlock1->SetVisible(false);
				}
			}

			ClearVector(vTags);
		}
		else if (name == "cmbTimeSpan") {
			CComboUI * pCombo = (CComboUI *)m_PaintManager.FindControl(name);
			if (0 == pCombo) {
				return;
			}

			CControlUI * pChart0 = m_PaintManager.FindControl("chart0");
			CControlUI * pChart1 = m_PaintManager.FindControl("chart1");
			CControlUI * pChart2 = m_PaintManager.FindControl("chart2");
			CControlUI * pChart3 = m_PaintManager.FindControl("chart3");

			int nSel = pCombo->GetCurSel();
			switch (nSel) {
			case 0:
			{
				if (pChart0)
					pChart0->SetVisible(true);
				if (pChart1)
					pChart1->SetVisible(false);
				if (pChart2)
					pChart2->SetVisible(false);
				if (pChart3)
					pChart3->SetVisible(false);
			}
			break;
			case 1:
			{
				if (pChart0)
					pChart0->SetVisible(true);
				if (pChart1)
					pChart1->SetVisible(true);
				if (pChart2)
					pChart2->SetVisible(false);
				if (pChart3)
					pChart3->SetVisible(false);
			}
			break;
			case 2:
			{
				if (pChart0)
					pChart0->SetVisible(true);
				if (pChart1)
					pChart1->SetVisible(true);
				if (pChart2)
					pChart2->SetVisible(true);
				if (pChart3)
					pChart3->SetVisible(false);
			}
			break;
			case 3:
			{
				if (pChart0)
					pChart0->SetVisible(true);
				if (pChart1)
					pChart1->SetVisible(true);
				if (pChart2)
					pChart2->SetVisible(true);
				if (pChart3)
					pChart3->SetVisible(true);
			}
			break;
			default:
				break;
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
				{
					pControl->SelectItem(0);
					CListUI* pListShort = static_cast<CListUI*>(m_PaintManager.FindControl(_T("patients_list_short")));
					if (pListShort) {
						if (pListShort->GetCurSel() < 0 ) {
							if ( pListShort->GetCount() > 0 )
								pListShort->SelectItem(0);
						}
						else {
							OnItemSelected("patients_list_short");
						}
					}
				}
				else if (name == _T("patients_info"))
				{
					pControl->SelectItem(1);
					CListUI* pList = static_cast<CListUI*>(m_PaintManager.FindControl(_T("patients_list")));
					if (pList) {
						if (pList->GetCurSel() < 0 && pList->GetCount() > 0) {
							pList->SelectItem(0);
						}
					}
				}					
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
			else if (name == "btnBinding") {
				BindingPatient();
				return;
			}
			else if (name == "btnSync") {
				SyncReader();
				CButtonUI * pBtn = (CButtonUI *)m_PaintManager.FindControl(name);
				assert(pBtn);
				pBtn->SetEnabled(false);

				CMyProgress * pProgress = (CMyProgress*)m_PaintManager.FindControl("sync_progresss");
				if (0 != pProgress ) {
					pProgress->SetVisible( true );
					pProgress->Start();
				}

				CLabelUI * pLbl = (CLabelUI*)m_PaintManager.FindControl("sync_text");
				if (0 != pLbl) {
					pLbl->SetVisible(true);
				}

				return;
			}
		}
		else if (msg.sType == "itemselect") {
			OnItemSelected(name);
			return;
		}
		else if (msg.sType == "itemactivate") {
			CTabLayoutUI* pTab = static_cast<CTabLayoutUI*>(m_PaintManager.FindControl(_T("switch")));
			if (pTab) {
				if (pTab->GetCurSel() == 1) {
					ModifyPatient();
				}
			}			
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

		//if (msg.sType == "textchanged") {
		//	int a = 100;
		//}

		//if (msg.pSender->GetName() == "DateTime1") {
		//	g_log->Output(ILog::LOG_SEVERITY_ERROR, "%s\n", (const char *)msg.sType);
		//}

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
		else if (0 == _stricmp("MyProgress", pstrClass)) {
			return new CMyProgress(&m_PaintManager, "progress_fore.png");
		}
		return 0;
	}

	virtual void   InitWindow()
	{
		memset(&m_tTagId, 0, sizeof(TagId));
		m_nInventoryError = -1;
		m_eSerialPortStatus = (SerialPortStatus)-1;

		PostMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0);

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
				pListElement->SetText(SHORT_INDEX_PATIENT_ID, pInfo->szId);
				pListElement->SetText(SHORT_INDEX_PATIENT_NAME, pInfo->szName);
				pListElement->SetText(SHORT_INDEX_PATIENT_BEDNO, pInfo->szBedNo);

				char * szId = new char[sizeof(pInfo->szId)];
				strncpy_s(szId, sizeof(pInfo->szId),  pInfo->szId, sizeof(pInfo->szId) );
				pListElement->SetTag((UINT_PTR)szId);
			}

			if (pListShort->GetCount() > 0) {
				pListShort->SelectItem(0);
			}
		}

		if (pList) {
			std::vector<PatientInfo *>::iterator it;
			for (it = v.begin(); it != v.end(); ++it) {
				PatientInfo * pInfo = *it;

				CListTextElementUI* pListElement = new CListTextElementUI;
				pList->Add(pListElement);
				pListElement->SetText(INDEX_PATIENT_ID, pInfo->szId);
				pListElement->SetText(INDEX_PATIENT_INNO, pInfo->szInNo);
				pListElement->SetText(INDEX_PATIENT_NAME, pInfo->szName);
				pListElement->SetText(INDEX_PATIENT_SEX, GetSex(pInfo->bMale) );
				pListElement->SetText(INDEX_PATIENT_AGE, GetInt( buf, sizeof(buf), pInfo->nAge ) );
				pListElement->SetText(INDEX_PATIENT_OFFICE, pInfo->szOffice);
				pListElement->SetText(INDEX_PATIENT_BEDNO, pInfo->szBedNo);
				pListElement->SetText(INDEX_PATIENT_INDATE, ConvertDate( buf, sizeof(buf), &pInfo->tInDate ) );
				pListElement->SetText(INDEX_PATIENT_CARDNO, pInfo->szCardNo);

				std::vector<TagInfo *> vTags;
				CBusiness::GetInstance()->GetPatientTags(pInfo->szId, vTags);
				if (vTags.size() > 0) {
					CDuiString strText = GetPatientTagsText(vTags);
					pListElement->SetText(INDEX_PATIENT_TAGS, strText);
				}				
				ClearVector(vTags);

				char * szId = new char[sizeof(pInfo->szId)];
				strcpy_s(szId, sizeof(pInfo->szId), pInfo->szId);
				pListElement->SetTag((UINT_PTR)szId);
			}

			if (pList->GetCount() > 0) {
				pList->SelectItem(0);
			}
		}

		ClearVector(v);

		CTempChartUI * pChart = (CTempChartUI *)m_PaintManager.FindControl("chart0");
		if (pChart) {
			pChart->m_nWeekIndex = 0;
		}

		pChart = (CTempChartUI *)m_PaintManager.FindControl("chart1");
		if (pChart) {
			pChart->m_nWeekIndex = 1;
		}

		pChart = (CTempChartUI *)m_PaintManager.FindControl("chart2");
		if (pChart) {
			pChart->m_nWeekIndex = 2;
		}

		pChart = (CTempChartUI *)m_PaintManager.FindControl("chart3");
		if (pChart) {
			pChart->m_nWeekIndex = 3;
		}

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
				pListElement->SetText(SHORT_INDEX_PATIENT_ID, pInfo->szId);
				pListElement->SetText(SHORT_INDEX_PATIENT_NAME, pInfo->szName);
				pListElement->SetText(SHORT_INDEX_PATIENT_BEDNO, pInfo->szBedNo);

				char * szId = new char[sizeof(pInfo->szId)];
				strcpy_s(szId, sizeof(pInfo->szId), pInfo->szId);
				pListElement->SetTag((UINT_PTR)szId);
			}

			if (pList) {
				CListTextElementUI* pListElement = new CListTextElementUI;
				pList->Add(pListElement);
				pListElement->SetText(INDEX_PATIENT_ID, pInfo->szId);
				pListElement->SetText(INDEX_PATIENT_INNO, pInfo->szInNo);
				pListElement->SetText(INDEX_PATIENT_NAME, pInfo->szName);
				pListElement->SetText(INDEX_PATIENT_SEX, GetSex(pInfo->bMale));
				pListElement->SetText(INDEX_PATIENT_AGE, GetInt(buf, sizeof(buf), pInfo->nAge));
				pListElement->SetText(INDEX_PATIENT_OFFICE, pInfo->szOffice);
				pListElement->SetText(INDEX_PATIENT_BEDNO, pInfo->szBedNo);
				pListElement->SetText(INDEX_PATIENT_INDATE, ConvertDate(buf, sizeof(buf), &pInfo->tInDate));
				pListElement->SetText(INDEX_PATIENT_CARDNO, pInfo->szCardNo);

				char * szId = new char[sizeof(pInfo->szId)];
				strcpy_s(szId, sizeof(pInfo->szId), pInfo->szId);
				pListElement->SetTag((UINT_PTR)szId);

				if (pList->GetCurSel() < 0) {
					pList->SelectItem(0);
				}
			}
		}
		else if (eType == OPERATION_MODIFY) {
			if (pListShort) {
				int n = pListShort->GetCount();
				for (int i = 0; i < n; i++) {
					CListTextElementUI* pItem = (CListTextElementUI*)pListShort->GetItemAt(i);
					char * szId = (char *)pItem->GetTag();
					if (0 == strcmp(szId, pInfo->szId)) {
						pItem->SetText(SHORT_INDEX_PATIENT_NAME, pInfo->szName);
						pItem->SetText(SHORT_INDEX_PATIENT_BEDNO, pInfo->szBedNo);
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
						pItem->SetText(INDEX_PATIENT_INNO, pInfo->szInNo);
						pItem->SetText(INDEX_PATIENT_NAME, pInfo->szName);
						pItem->SetText(INDEX_PATIENT_SEX, GetSex(pInfo->bMale));
						pItem->SetText(INDEX_PATIENT_AGE, GetInt(buf, sizeof(buf), pInfo->nAge));
						pItem->SetText(INDEX_PATIENT_OFFICE, pInfo->szOffice);
						pItem->SetText(INDEX_PATIENT_BEDNO, pInfo->szBedNo);
						pItem->SetText(INDEX_PATIENT_INDATE, ConvertDate(buf, sizeof(buf), &pInfo->tInDate));
						pItem->SetText(INDEX_PATIENT_CARDNO, pInfo->szCardNo);

						std::vector<TagInfo *> vTags;
						CBusiness::GetInstance()->GetPatientTags(pInfo->szId, vTags);
						if (vTags.size() > 0) {
							CDuiString strText = GetPatientTagsText(vTags);
							pItem->SetText(INDEX_PATIENT_TAGS, strText);
						}
						else {
							pItem->SetText(INDEX_PATIENT_TAGS, "");
						}
						ClearVector(vTags);
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
						delete[] szId;
						pListShort->RemoveAt(i);
						break;
					}
				}

				CControlUI * pBlock0 = m_PaintManager.FindControl("VerticalLayout_Temp");
				CControlUI * pBlock1 = m_PaintManager.FindControl("VerticalLayout_Temp_none");

				int nIndex = pListShort->GetCurSel();
				if (nIndex < 0) {
					if (pBlock0) {
						pBlock0->SetVisible(false);
					}
					if (pBlock1) {
						pBlock1->SetVisible(false);
					}
				}
			}

			if (pList) {
				int n = pList->GetCount();
				for (int i = 0; i < n; i++) {
					CListTextElementUI* pItem = (CListTextElementUI*)pList->GetItemAt(i);
					char * szId = (char *)pItem->GetTag();

					if (0 == strcmp(szId, pInfo->szId)) {
						delete[] szId;
						pList->RemoveAt(i);
						break;
					}
				}
			}
		}
	}

	CDuiString  GetPatientTagsText(std::vector<TagInfo *> & vTags) {
		char buf[8192];
		CDuiString strText;
		strText.Format("个数 %lu：", vTags.size());

		std::vector<TagInfo *>::iterator it;
		int i = 0;
		for (it = vTags.begin(), i = 0; it != vTags.end(); it++, i++) {
			TagInfo * p = *it;
			if (i > 0) {
				strText += ";";
			}
			ConvertTagId(buf, sizeof(buf), &p->tagId);
			strText += buf;
			strText += " ";
			ConvertDate(buf, sizeof(buf), &p->tBindingDate);
			strText += buf;
		}
		return strText;
	}

	// 绑定成功一个Tag后
	void OnBindingEvent(TagInfo * pTagInfo) {
		CListUI* pList = static_cast<CListUI*>(m_PaintManager.FindControl(_T("patients_list")));
		if (0 == pList) {
			return;
		}

		int n = pList->GetCount();
		for (int i = 0; i < n; i++) {
			CListTextElementUI* pItem = (CListTextElementUI*)pList->GetItemAt(i);
			char * szId = (char *)pItem->GetTag();
			// 找到病人，更新其绑定的温度贴数据
			// 格式为：温度贴个数：XX-XX-XX-XX-XX-XX-XX-XX 日期;XX-XX-XX-XX-XX-XX-XX-XX 日期......
			if (0 == strcmp(szId, pTagInfo->szPatientId)) {
				std::vector<TagInfo *> vTags;
				CBusiness::GetInstance()->GetPatientTags(pTagInfo->szPatientId, vTags);
				CDuiString strText = GetPatientTagsText(vTags);				
				pItem->SetText(INDEX_PATIENT_TAGS,strText);
				ClearVector(vTags);
				break;
			}
		}
	}

	// 病人信息长列表和短列表，都当前选中一个病人
	void SelectPatient(const char * szPatientId) {
		CListUI* pListShort = static_cast<CListUI*>(m_PaintManager.FindControl(_T("patients_list_short")));
		CListUI* pList = static_cast<CListUI*>(m_PaintManager.FindControl(_T("patients_list")));

		if (pListShort) {
			int n = pListShort->GetCount();
			for (int i = 0; i < n; i++) {
				CListTextElementUI* pItem = (CListTextElementUI*)pListShort->GetItemAt(i);
				char * szId = (char *)pItem->GetTag();
				if (0 == strcmp(szId, szPatientId)) {
					int nSel = pListShort->GetCurSel();
					if (nSel != i) {
						pListShort->SelectItem(i);
					}
					break;
				}
			}
		}

		if (pList) {
			int n = pList->GetCount();
			for (int i = 0; i < n; i++) {
				CListTextElementUI* pItem = (CListTextElementUI*)pList->GetItemAt(i);
				char * szId = (char *)pItem->GetTag();
				if (0 == strcmp(szId, szPatientId)) {
					int nSel = pList->GetCurSel();
					if (nSel != i) {
						pList->SelectItem(i);
					}
					break;
				}
			}
		}
	}

	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		if (UM_INVENTORY_RET == uMsg) {
			int nResult = (int)wParam;
			TagId * pTagId = (TagId *)lParam;			

			CTabLayoutUI* pTab = static_cast<CTabLayoutUI*>(m_PaintManager.FindControl(_T("switch")));
			if (0 == pTab) {
				delete pTagId;
				return 0;
			}			

			CLabelUI * pLblBinding = static_cast<CLabelUI*>(m_PaintManager.FindControl(_T("lblBindingReader")));
			CLabelUI * pLblOther   = static_cast<CLabelUI*>(m_PaintManager.FindControl(_T("lblOther")));
			CButtonUI * pBtn = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("btnBinding")));

			if (nResult == ERROR_NOT_FOUND_TAG) {
				if (m_nInventoryError != nResult) {
					if (pLblOther) {
						pLblOther->SetText("");
					}

					if (pLblBinding) {
						pLblBinding->SetText("绑定温度贴的读卡器OK");
						pLblBinding->SetTextColor(0xFFFFFFFF);
					}

					if (pBtn) {
						pBtn->SetEnabled(false);
					}
				}				
			}
			else if (nResult == ERROR_BINDING_READER_FAILED_TO_INVENTORY) {
				if (m_nInventoryError != nResult) {
					if (pLblOther) {
						pLblOther->SetText("");
					}

					if (pLblBinding) {
						pLblBinding->SetText("绑定温度贴的读卡器连接失败");
						pLblBinding->SetTextColor(0xFFCAF100);
					}

					if (pBtn) {
						pBtn->SetEnabled(false);
					}
				}
			}
			else if (nResult == ERROR_BINDING_READER_FAILED_TOO_MANY_CARDS) {
				if (m_nInventoryError != nResult) {
					if (pLblOther) {
						pLblOther->SetText("读到两个或两个以上的温度贴");
						pLblOther->SetTextColor(0xFFCAF100);
					}

					if (pLblBinding) {
						pLblBinding->SetText("绑定温度贴的读卡器OK");
						pLblBinding->SetTextColor(0xFFFFFFFF);
					}

					if (pBtn) {
						pBtn->SetEnabled(false);
					}
				}
			}
			else if (0 == nResult) {
				if (m_nInventoryError != nResult) {
					if (pLblOther) {
						pLblOther->SetText("");
					}

					if (pLblBinding) {
						pLblBinding->SetText("绑定温度贴的读卡器OK");
						pLblBinding->SetTextColor(0xFFFFFFFF);
					}
				}

				// 如果是病人信息管理tab页
				if (1 == pTab->GetCurSel()) {
					PatientInfo tPatient;
					memset(&tPatient, 0, sizeof(PatientInfo));
					int ret = CBusiness::GetInstance()->GetPatientByTag(pTagId, &tPatient);

					// 该Tag已经绑定
					if (0 == ret) {
						// 选中已经绑定的病人
						SelectPatient(tPatient.szId);
						if (pBtn) {
							pBtn->SetEnabled(false);
						}
					}
					else {
						memcpy(&m_tTagId, pTagId, sizeof(TagId));
						if (pBtn) {
							pBtn->SetEnabled(true);
						}
					}
				}
			}
			m_nInventoryError = nResult;

			delete pTagId;
			return 0;
		} else if (UM_SERIAL_PORT_STATUS == uMsg){
			SerialPortStatus eStatus = (SerialPortStatus)wParam;
			if (eStatus != m_eSerialPortStatus) {
				CLabelUI * pLblSyncReader = (CLabelUI *)m_PaintManager.FindControl("lblSyncReader");
				if (0 != pLblSyncReader) {
					if (eStatus == SERIAL_PORT_STATUS_CLOSED) {
						pLblSyncReader->SetText("同步温度贴数据的读卡器连接失败");
						pLblSyncReader->SetTextColor(0xFFCAF100);
					}
					else if (eStatus == SERIAL_PORT_STATUS_OPENING) {
						pLblSyncReader->SetText("同步温度贴数据的读卡器正在连接");
						pLblSyncReader->SetTextColor(0xFFCAF100);
					}
					else if (eStatus == SERIAL_PORT_STATUS_OPEND) {
						pLblSyncReader->SetText("同步温度贴数据的读卡器连接OK");
						pLblSyncReader->SetTextColor(0xFFFFFFFF);
					}
				}
			}
			m_eSerialPortStatus = eStatus;
			return 0;
		}
		else if (UM_SERIAL_PORT_SYNC_RET == uMsg) {
			SerialPortSyncError eError = (SerialPortSyncError)wParam;
			std::vector<TagData*> * pVRet = (std::vector<TagData*> *)lParam;

			CButtonUI * pBtn = (CButtonUI *)m_PaintManager.FindControl("btnSync");
			assert(pBtn);
			pBtn->SetEnabled(true);			

			if (eError == SERIAL_PORT_SYNC_ERROR_NOT_OPENED) {
				::MessageBox(m_hWnd, "同步读卡器没有连接上", "同步温度数据", 0);
			}
			else if (eError == SERIAL_PORT_SYNC_ERROR_FAILED_TO_SYNC) {
				::MessageBox(m_hWnd, "同步温度数据失败！请检查同步读卡器", "同步温度数据", 0);
			}
			else if (eError == SERIAL_PORT_SYNC_ERROR_FAILED_TO_RECEIVE_RET) {
				::MessageBox(m_hWnd, "接收或解析同步温度数据失败！请检查同步读卡器", "同步温度数据", 0);
			}

			CMyProgress * pProgress = (CMyProgress*)m_PaintManager.FindControl("sync_progresss");
			if (0 != pProgress) {
				pProgress->Stop();
				pProgress->SetVisible(false);
			}

			CLabelUI * pLbl = (CLabelUI*)m_PaintManager.FindControl("sync_text");
			if (0 != pLbl) {
				pLbl->SetVisible(false);
			}

			// 保存温度数据
			if (eError == SERIAL_PORT_SYNC_ERROR_OK && 0 != pVRet) {
				int ret = CBusiness::GetInstance()->SaveTempData(*pVRet);
				BOOL bRemoveData = FALSE;
				if (0 != ret) {
					if (IDYES == ::MessageBox(m_hWnd, "同步过程中，部分Tag没有绑定！要清空该读卡器存储的温度数据吗？", "同步数据", MB_YESNO | MB_DEFBUTTON2)) {
						bRemoveData = TRUE;
					}
				}
				else {
					bRemoveData = TRUE;
				}

				if (bRemoveData) {
					ClearReader();
				}
			}

			if (pVRet) {
				ClearVector(*pVRet);
				delete pVRet;
			}
			return 0;
		}
		return WindowImplBase::HandleMessage(uMsg, wParam, lParam);
	}

};

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	char buf[8192];

	g_cfg = new FileConfig;
	g_cfg->Init("TelemedNurse.cfg");

	g_log = new FileLog;
	g_log->Init("TelemedNurse.log");

	g_cfg->GetConfig("use utf8", buf, sizeof(buf), "true");
	g_bUseUtf8 = GetBoolean(buf);

	int ret = 0;
	CBusiness::GetInstance()->sigInit.emit(&ret);
	if (0 != ret) {
		::MessageBox(0, GetErrDescription((GlobalErrorNo)ret), "应用程序初始化错误", 0);

		g_cfg->Deinit();
		g_log->Deinit();

		delete g_cfg;
		delete g_log;
		return -1;
	}

	

	HICON hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));

	CPaintManagerUI::SetInstance(hInstance);

	CDuiFrameWnd duiFrame;
	CBusiness::GetInstance()->sigAMPatientRet.connect(&duiFrame, &CDuiFrameWnd::OnPatientEvent);
	CBusiness::GetInstance()->sigBinding.connect(&duiFrame, &CDuiFrameWnd::OnBindingEvent);

	duiFrame.Create(NULL, _T("DUIWnd"), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE, 0, 0, 800, 600);
	duiFrame.CenterWindow();

	g_hWnd = duiFrame.GetHWND();
	::SendMessage(g_hWnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
	::SendMessage(g_hWnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);

	duiFrame.ShowModal();	

	CBusiness::GetInstance()->sigDeinit.emit(&ret);

	g_cfg->Deinit();
	g_log->Deinit();

	delete g_cfg;
	delete g_log;
	return 0;

}
