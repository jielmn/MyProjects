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
#include "LmnTemplates.h"
#include "ManulSelectDlg.h"

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

#define  CHART_UI_HEIGHT                   1000

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
		m_XmlChartFile->ReadXmlChartFile(CPaintManagerUI::GetInstancePath() + "res\\���±����.xml");

		m_tInDate    = 0;
		m_tFistDay   = 0;                            // ��һ������
		m_nWeekIndex = 0;                            // �ڼ���
		memset(m_TempData, 0, sizeof(m_TempData));   // �¶�����
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

	void Print(HDC hDC, RECT & r, int nChartUIHeight) {
		char buf[8192];
		CDuiString tmp;
		//RECT r = this->GetPos();

		if (m_XmlChartFile->m_ChartUI) {
			int a = ((r.right - r.left - 1) - 120) / 42;
			int b = ((nChartUIHeight - 1) - 85 - 125 - 200) / 40;

			int right = a * 42 + 120 + r.left + 1;
			int h = b * 40 + 85 + 125 + 200 + 1;

			m_XmlChartFile->m_ChartUI->SetRect(r.left, r.top, right, r.top + h);
			m_XmlChartFile->m_ChartUI->RecacluteLayout();

			char szHospitalName[256];
			g_cfg->GetConfig("hospital name", buf, sizeof(buf), "ĳĳҽԺ");
			if (g_bUseUtf8) {
				Utf8ToAnsi(szHospitalName, sizeof(szHospitalName), buf);
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
				tmp = "������";
				tmp += m_sPatientName;
				pUI->SetText((const char *)tmp);
			}

			CXml2ChartUI * pIndate = m_XmlChartFile->FindChartUIByName("indate");
			if (pIndate) {
				tmp = "��Ժ���ڣ�";
				tmp += ConvertDate(buf, sizeof(buf), &m_tInDate);
				pIndate->SetText((const char *)tmp);
			}

			pUI = m_XmlChartFile->FindChartUIByName("office");
			if (pUI) {
				tmp = "���ң�";
				tmp += m_sOffice;
				pUI->SetText((const char *)tmp);
			}

			pUI = m_XmlChartFile->FindChartUIByName("bed_no");
			if (pUI) {
				tmp = "���ţ�";
				tmp += m_sBedNo;
				pUI->SetText((const char *)tmp);
			}

			pUI = m_XmlChartFile->FindChartUIByName("inno");
			if (pUI) {
				tmp = "סԺ�ţ�";
				tmp += m_sInNo;
				pUI->SetText((const char *)tmp);
			}

			for (int i = 0; i < 7; i++) {
				CDuiString name;
				name.Format("no%d", i + 1);
				pUI = m_XmlChartFile->FindChartUIByName((const char *)name);
				if (pUI) {
					CDuiString tmpStr;
					tmpStr.Format("%d", m_nWeekIndex * 7 + i + 1);
					pUI->SetText((const char *)tmpStr);
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


			DrawXml2ChartUI(hDC, m_XmlChartFile->m_ChartUI);

			// ���¶ȵ� 
			CXml2ChartUI * pMainBlock = m_XmlChartFile->FindChartUIByName("MainBlock");
			if (pMainBlock != 0) {
				RECT rectBlock = pMainBlock->GetAbsoluteRect();
				int w = rectBlock.right - rectBlock.left;
				int h = rectBlock.bottom - rectBlock.top;

				int grid_h = h / 40;
				int grid_w = w / 42;

				//���������Ļ����԰�뾶
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
						TagData tTemp = m_TempData[i][j];

						// ����35-42�ȷ�Χ��
						if (!(tTemp.dwTemperature >= 3400 && tTemp.dwTemperature <= 4200)) {
							continue;
						}

						// �߶ȱ���
						double h_ratio = (tTemp.dwTemperature - 3400.0) / (4200.0 - 3400.0);
						int    nTempHeight = (int)(h_ratio * h);

						CPoint pt;
						pt.x = grid_w * (i * 6 + j) + grid_w / 2 + rectBlock.left;
						pt.y = rectBlock.bottom - nTempHeight;

						DrawTempPoint(pt, hDC, RADIUS);

						// �Ƿ��һ����
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
	}

	virtual bool DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl) {
		RECT r = this->GetPos();
		Print(hDC,r, CHART_UI_HEIGHT);
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

	void SetTempData(TagData arrTempData[][6]) {
		memcpy(m_TempData, arrTempData, sizeof(m_TempData));
	}

	CXml2ChartFile *  m_XmlChartFile;

	CDuiString        m_sPatientName;                // ��������
	time_t            m_tInDate;                     // סԺ����
	CDuiString        m_sOffice;                     // ����
	CDuiString        m_sBedNo;                      // ����
	CDuiString        m_sInNo;                       // סԺ��

	time_t            m_tFistDay;                     // ��һ������
	int               m_nWeekIndex;                   // �ڼ���

	TagData           m_TempData[7][6];               // �¶�����
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
	std::vector<TagData*>  m_vTempetatureData;                   // �¶����ݣ������¶����߻�ͼ
	PatientInfo            m_tPatient;                           // ������Ϣ�������¶����߻�ͼ
	TagData                m_TempData[MAX_SPAN_DAYS][6];         // �¶����ݡ����������ڻ�ͼ
	time_t                 m_tFirstTime;                         // �����¶����ߵĵ�һ��(��ӦUI�ؼ���FirstDay)

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
			::MessageBox(m_hWnd, "û��ѡ�в�����Ϣ", "�޸Ĳ�����Ϣ", 0);
			return;
		}

		CListTextElementUI* pItem = (CListTextElementUI*)pList->GetItemAt(nIndex);
		char * szId = (char *)pItem->GetTag();
		PatientInfo tPatient;
		CBusiness::GetInstance()->GetPatient(szId, &tPatient);

		CPatientWnd * pPatientDlg = new CPatientWnd(FALSE);
		memcpy(&pPatientDlg->m_tPatientInfo, &tPatient, sizeof(PatientInfo));
		pPatientDlg->Create(this->m_hWnd, _T("�޸Ĳ�����Ϣ"), UI_WNDSTYLE_FRAME | WS_POPUP, NULL, 0, 0, 0, 0);
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
			::MessageBox(m_hWnd, "û��ѡ�в�����Ϣ", "ɾ��������Ϣ", 0);
			return;
		}

		CListTextElementUI* pItem = (CListTextElementUI*)pList->GetItemAt(nIndex);
		char * szId = (char *)pItem->GetTag();

		if (::MessageBox(m_hWnd, "ȷ��Ҫɾ����", "ɾ��������Ϣ", MB_YESNO) == IDYES) {
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
			::MessageBox(m_hWnd, "û��ѡ�в�����Ϣ", "�󶨲����¶���", 0);
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
			::MessageBox(m_hWnd, "��Tag�ɹ���", "��Tag", 0);
		}
		else
			::MessageBox(m_hWnd, GetErrDescription((GlobalErrorNo)ret), "��Tag", 0);
	}

	void OnTempCondChange( ) {
		CComboUI * pCombo = (CComboUI *)m_PaintManager.FindControl("cmbTimeSpan");
		if (0 == pCombo) {
			return;
		}

		CControlUI *   pControl[3] = { 0 };
		CTempChartUI * pChart[4] = { 0 };
		for (int i = 0; i < 4; i++) {
			CDuiString str;
			str.Format("chart%d", i);
			pChart[i] = (CTempChartUI *)m_PaintManager.FindControl(str);

			if (i > 0) {
				str.Format("control_%d", i);
				pControl[i-1] = m_PaintManager.FindControl(str);
			}
		}

		int nSel = pCombo->GetCurSel();
		int mask = (( 0xF << (nSel + 1) ) & 0xF0) >> 4;

		for (int i = 0; i < 4; i++) {
			if (0 == pChart[i]) {
				continue;
			}

			pChart[i]->m_tFistDay = m_tFirstTime + 3600 * 24 * 7 * i;
			pChart[i]->SetTempData(m_TempData+7*i);

			if (IfHasBit(mask, i)) {
				pChart[i]->SetVisible(true);
				if (i > 0) {
					pControl[i - 1]->SetVisible(true);
				}
			}
			else {
				pChart[i]->SetVisible(false);
				if (i > 0) {
					pControl[i - 1]->SetVisible(false);
				}
			}

			pChart[i]->Invalidate();
		}
	}

	// �����¶����ݸ���
	void  SaveCopyTempData() {
		std::vector<TagData*>::iterator it = m_vTempetatureData.begin();
		memset(m_TempData, 0, sizeof(m_TempData));

		for (int i = 0; i < MAX_SPAN_DAYS; i++) {
			time_t  t = m_tFirstTime + 3600 * 24 * i;

			for (int j = 0; j < 6; j++) {
				BOOL bFind = FALSE;
				for (; it != m_vTempetatureData.end(); it++) {
					TagData* pData = *it;
					// �������ʱ��С�ڸ��ӵ�ʱ��
					if (pData->tTime < t) {
						continue;
					}
					// ʱ���С��һ��
					if (pData->tTime - t < 3600 * 24) {
						m_TempData[i][j] = *pData;
						it++;
						bFind = TRUE;
						break;
					}
					else {
						break;
					}
				}
				// ����ʱ�����С�Ķ��ȵ�ǰ����ʱ���
				if (!bFind) {
					break;
				}
			}
		}
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

			// û�а�tag
			//if (0 == vTags.size()) {
			//	if (pBlock0) {
			//		pBlock0->SetVisible(false);
			//	}
			//	if (pBlock1) {
			//		pBlock1->SetVisible(true);
			//	}
			//}
			//// ����Tag;
			//else 
			

			// ��ձ���Ļ�ͼ������
			ClearVector(m_vTempetatureData);
			memset(m_TempData, 0, sizeof(m_TempData));
			memset(&m_tPatient, 0, sizeof(m_tPatient));
			m_tFirstTime = 0;

			CBusiness::GetInstance()->GetLatestTempData(szId, m_vTempetatureData);
			CBusiness::GetInstance()->GetPatient(szId, &m_tPatient);

			// ����Ϊ�գ���û�а�tag
			if (0 == vTags.size() && 0 == m_vTempetatureData.size()) {
				if (pBlock0) {
					pBlock0->SetVisible(false);
				}
				if (pBlock1) {
					pBlock1->SetVisible(true);
				}
				ClearVector(vTags);
				return;
			}

			// �õ���ʼʱ��
			if (m_vTempetatureData.size() > 0) {
				TagData* pData = m_vTempetatureData.at(0);
				time_t tMinTime = TrimDatetime(pData->tTime);
				m_tFirstTime = tMinTime;
			}
			else {
				m_tFirstTime = TrimDatetime(time(0) - 3600 * 24 * 6);
			}
			SaveCopyTempData();

			// ������ʼʱ��UI
			CDateTimeUI * pDateTime = (CDateTimeUI *)m_PaintManager.FindControl("DateTime1");
			if (0 != pDateTime) {
				SYSTEMTIME s = ConvertDateTime(m_tFirstTime);
				pDateTime->SetText(ConvertDate(buf, sizeof(buf), &m_tFirstTime));
				pDateTime->SetTime(&s);
			}

			// ���û�����Ϣ
			for (int i = 0; i < 4; i++) {
				CDuiString  str;
				str.Format("chart%d", i);
				CTempChartUI * pChart = (CTempChartUI *)m_PaintManager.FindControl((const char *)str);
				if (pChart) {
					pChart->m_sPatientName = m_tPatient.szName;
					pChart->m_sBedNo = m_tPatient.szBedNo;
					pChart->m_sInNo = m_tPatient.szInNo;
					pChart->m_sOffice = m_tPatient.szOffice;
					pChart->m_tInDate = m_tPatient.tInDate;
				}
			}

			// ������ʾ
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

			OnTempCondChange();

			ClearVector(vTags);
		}
		else if (name == "cmbTimeSpan") {
			OnTempCondChange();
		}
	}

	void OnPrint() {
		PRINTDLG printInfo;
		ZeroMemory(&printInfo, sizeof(printInfo));  //��ոýṹ     
		printInfo.lStructSize = sizeof(printInfo);
		printInfo.hwndOwner = 0;
		printInfo.hDevMode = 0;
		printInfo.hDevNames = 0;
		//����ǹؼ���PD_RETURNDC ������������־�����ò���hDC��      
		//            PD_RETURNDEFAULT ������ǵõ�Ĭ�ϴ�ӡ��������Ҫ�����öԻ���     
		//printInfo.Flags = PD_RETURNDC | PD_RETURNDEFAULT;   
		printInfo.Flags = PD_USEDEVMODECOPIESANDCOLLATE | PD_RETURNDC;
		printInfo.nCopies = 1;
		printInfo.nFromPage = 0xFFFF;
		printInfo.nToPage = 0xFFFF;
		printInfo.nMinPage = 1;
		printInfo.nMaxPage = 0xFFFF;

		//����API�ó�Ĭ�ϴ�ӡ��     
		//PrintDlg(&printInfo);
		if (PrintDlg(&printInfo) == TRUE)
		{
			DOCINFO di;
			ZeroMemory(&di, sizeof(DOCINFO));
			di.cbSize = sizeof(DOCINFO);
			di.lpszDocName = _T("MyXPS");

			CDC *pDC = CDC::FromHandle(printInfo.hDC);
			pDC->SetMapMode(MM_ANISOTROPIC); //ת������ӳ�䷽ʽ

			CRect rect(10, 10, 960+1+10, 930+1+10);
			CSize size = CSize(rect.Width()+20, rect.Height()+20);

			pDC->SetWindowExt(size);
			pDC->SetViewportExt(pDC->GetDeviceCaps(HORZRES), pDC->GetDeviceCaps(VERTRES));

			CTempChartUI * pChart[4] = { 0 };
			for (int i = 0; i < 4; i++) {
				CDuiString str;
				str.Format("chart%d", i);
				pChart[i] = (CTempChartUI *)m_PaintManager.FindControl(str);
			}

			StartDoc(printInfo.hDC, &di);

			for (int i = 0; i < 4; i++) {
				if (pChart[i] && pChart[i]->IsVisible() ) {
					StartPage(printInfo.hDC);
					pChart[i]->Print(pDC->m_hDC, rect, rect.Height());
					EndPage(printInfo.hDC);
				}
			}

			EndDoc(printInfo.hDC);

			// Delete DC when done.
			DeleteDC(printInfo.hDC);

			//OnOK();
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
				pPatientDlg->Create( this->m_hWnd, _T("����������Ϣ"), UI_WNDSTYLE_FRAME | WS_POPUP, NULL, 0, 0, 0, 0 );
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
				TCHAR strFilename[MAX_PATH] = { 0 };//���ڽ����ļ���  
				ofn.lStructSize = sizeof(OPENFILENAME);//�ṹ���С  
				ofn.hwndOwner = NULL;//ӵ���Ŵ��ھ����ΪNULL��ʾ�Ի����Ƿ�ģ̬�ģ�ʵ��Ӧ����һ�㶼Ҫ��������  
				ofn.lpstrFilter = TEXT("Excel Flie(*.xls)\0*.xls\0�����ļ�\0*.*\0\0");//���ù���  
				ofn.nFilterIndex = 1;//����������  
				ofn.lpstrFile = strFilename;//���շ��ص��ļ�����ע���һ���ַ���ҪΪNULL  
				ofn.nMaxFile = sizeof(strFilename);//����������  
				//ofn.lpstrInitialDir = NULL;//��ʼĿ¼ΪĬ��  
				ofn.lpstrInitialDir = CPaintManagerUI::GetInstancePath();
				ofn.lpstrTitle = TEXT("��ѡ��һ���ļ�");//ʹ��ϵͳĬ�ϱ������ռ���  
				ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;//�ļ���Ŀ¼������ڣ�����ֻ��ѡ��  
				if (GetOpenFileName(&ofn))
				{
					int ret = CBusiness::GetInstance()->ImportExcel(strFilename);
					if (0 == ret) 
						::MessageBox(m_hWnd, "����Excel�ɹ���", "����Excel", 0);
					else
						::MessageBox(m_hWnd, GetErrDescription((GlobalErrorNo)ret), "����Excel", 0);
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
			else if (name == "btnPrint") {				
				OnPrint();
				return;
			}
			else if (name == "manul_select") {
				CManulSelectWnd * pManulSelectDlg = new CManulSelectWnd;
				pManulSelectDlg->m_pvTempetatureData = &m_vTempetatureData;
				pManulSelectDlg->m_tFirstTime = m_tFirstTime;
				memcpy(pManulSelectDlg->m_TempData, m_TempData, sizeof(pManulSelectDlg->m_TempData));
				CComboUI * pTimeSpan = (CComboUI *)m_PaintManager.FindControl("cmbTimeSpan");
				if (pTimeSpan) {
					pManulSelectDlg->m_nWeekIndex = pTimeSpan->GetCurSel();
				}
				pManulSelectDlg->m_sigOK.connect(this, &CDuiFrameWnd::OnManulSelectOK);

				pManulSelectDlg->Create(this->m_hWnd, _T("�ֶ�ѡ������"), UI_WNDSTYLE_FRAME | WS_POPUP, NULL, 0, 0, 0, 0);
				pManulSelectDlg->CenterWindow();
				pManulSelectDlg->ShowModal();
				delete pManulSelectDlg;
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

		memset(&m_tPatient, 0, sizeof(m_tPatient));
		memset(m_TempData, 0, sizeof(m_TempData));
		m_tFirstTime = 0;		

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
		strText.Format("���� %lu��", vTags.size());

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

	// �󶨳ɹ�һ��Tag��
	void OnBindingEvent(TagInfo * pTagInfo) {
		CListUI* pList = static_cast<CListUI*>(m_PaintManager.FindControl(_T("patients_list")));
		if (0 == pList) {
			return;
		}

		int n = pList->GetCount();
		for (int i = 0; i < n; i++) {
			CListTextElementUI* pItem = (CListTextElementUI*)pList->GetItemAt(i);
			char * szId = (char *)pItem->GetTag();
			// �ҵ����ˣ�������󶨵��¶�������
			// ��ʽΪ���¶���������XX-XX-XX-XX-XX-XX-XX-XX ����;XX-XX-XX-XX-XX-XX-XX-XX ����......
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

	// ������Ϣ���б��Ͷ��б�������ǰѡ��һ������
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
						pLblBinding->SetText("���¶����Ķ�����OK");
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
						pLblBinding->SetText("���¶����Ķ���������ʧ��");
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
						pLblOther->SetText("�����������������ϵ��¶���");
						pLblOther->SetTextColor(0xFFCAF100);
					}

					if (pLblBinding) {
						pLblBinding->SetText("���¶����Ķ�����OK");
						pLblBinding->SetTextColor(0xFFFFFFFF);
					}

					if (pBtn) {
						pBtn->SetEnabled(false);
					}
				}
			}
			else if (0 == nResult) {
				if (m_nInventoryError != nResult) {				
					if (pLblBinding) {
						pLblBinding->SetText("���¶����Ķ�����OK");
						pLblBinding->SetTextColor(0xFFFFFFFF);
					}
				}

				if (pLblOther) {
					pLblOther->SetText("");
				}

				// ����ǲ�����Ϣ����tabҳ
				if (1 == pTab->GetCurSel()) {
					PatientInfo tPatient;
					memset(&tPatient, 0, sizeof(PatientInfo));
					int ret = CBusiness::GetInstance()->GetPatientByTag(pTagId, &tPatient);

					// ��Tag�Ѿ���
					if (0 == ret) {
						// ѡ���Ѿ��󶨵Ĳ���
						SelectPatient(tPatient.szId);
						if (pBtn) {
							pBtn->SetEnabled(false);
						}
						if (pLblOther) {
							pLblOther->SetText("���¶����Ѿ��󶨣���ѡ�иð󶨵Ĳ���");
							pLblOther->SetTextColor(0xFFCAF100);
						}
					}
					else {
						memcpy(&m_tTagId, pTagId, sizeof(TagId));
						if (pBtn) {
							pBtn->SetEnabled(true);
						}
						if (pLblOther) {
							pLblOther->SetText("��ѡ���˰��¶���");
							pLblOther->SetTextColor(0xFFFFFFFF);
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
						pLblSyncReader->SetText("ͬ���¶������ݵĶ���������ʧ��");
						pLblSyncReader->SetTextColor(0xFFCAF100);
					}
					else if (eStatus == SERIAL_PORT_STATUS_OPENING) {
						pLblSyncReader->SetText("ͬ���¶������ݵĶ�������������");
						pLblSyncReader->SetTextColor(0xFFCAF100);
					}
					else if (eStatus == SERIAL_PORT_STATUS_OPEND) {
						pLblSyncReader->SetText("ͬ���¶������ݵĶ���������OK");
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
				::MessageBox(m_hWnd, "ͬ��������û��������", "ͬ���¶�����", 0);
			}
			else if (eError == SERIAL_PORT_SYNC_ERROR_FAILED_TO_SYNC) {
				::MessageBox(m_hWnd, "ͬ���¶�����ʧ�ܣ�����ͬ��������", "ͬ���¶�����", 0);
			}
			else if (eError == SERIAL_PORT_SYNC_ERROR_FAILED_TO_RECEIVE_RET) {
				::MessageBox(m_hWnd, "���ջ����ͬ���¶�����ʧ�ܣ�����ͬ��������", "ͬ���¶�����", 0);
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

			// �����¶�����
			if (eError == SERIAL_PORT_SYNC_ERROR_OK && 0 != pVRet) {
				int ret = CBusiness::GetInstance()->SaveTempData(*pVRet);
				BOOL bRemoveData = FALSE;
				if (0 != ret) {
					if (IDYES == ::MessageBox(m_hWnd, "ͬ�������У�����Tagû�а󶨣�Ҫ��ոö������洢���¶�������", "ͬ������", MB_YESNO | MB_DEFBUTTON2)) {
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

			if (m_tPatient.szId[0] != '\0' && m_tFirstTime != 0) {
				ClearVector(m_vTempetatureData);
				memset(m_TempData, 0, sizeof(m_TempData));

				CBusiness::GetInstance()->GetTempData(m_tPatient.szId, m_tFirstTime, m_vTempetatureData);
				SaveCopyTempData();
				OnTempCondChange();
			}
			

			return 0;
		}
		else if (WM_NOTIFY == uMsg) {
			NMHDR* pHeader = (NMHDR*)lParam;
			if (pHeader && pHeader->code == DTN_DATETIMECHANGE) {
				CDateTimeUI * pTempDataStartTime = (CDateTimeUI *)m_PaintManager.FindControl("DateTime1");
				if ( pTempDataStartTime && pTempDataStartTime->GetNativeWindow() == pHeader->hwndFrom ) {
					LPNMDATETIMECHANGE lpChage = (LPNMDATETIMECHANGE)lParam;				
					time_t tStartTime = ConvertDateTime(lpChage->st);
					ClearVector(m_vTempetatureData);
					memset(m_TempData, 0, sizeof(m_TempData));
					m_tFirstTime = TrimDatetime( tStartTime );
					CBusiness::GetInstance()->GetTempData( m_tPatient.szId, m_tFirstTime, m_vTempetatureData);
					SaveCopyTempData();
					OnTempCondChange();
				}				
				return 0;
			}
		}
		return WindowImplBase::HandleMessage(uMsg, wParam, lParam);
	}

	void  OnManulSelectOK(TagData pData[MAX_SPAN_DAYS][6]) {
		memcpy(m_TempData, pData, sizeof(m_TempData));
		OnTempCondChange();
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
		::MessageBox(0, GetErrDescription((GlobalErrorNo)ret), "Ӧ�ó����ʼ������", 0);

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