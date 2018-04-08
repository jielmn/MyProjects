#include <afx.h>
#include "main.h"
#include "resource.h"
#include "Business.h"
#include "PatientDlg.h"
#include "NurseDlg.h"
#include "LmnString.h"


#define  TEMPERATURE_DATA_INDEX        0
#define  PATIENTS_INDEX                1
#define  NURSES_INDEX                  2
#define  SYNCHRONIZE_INDEX             3
#define  PURE_DATA_INDEX               0
#define  CURVE_INDEX                   1

#define  PATIENTS_INFO_ID_INDEX          0
#define  PATIENTS_INFO_NAME_INDEX        1
#define  PATIENTS_INFO_GENDER_INDEX      2
#define  PATIENTS_INFO_BED_NO_INDEX      3
#define  PATIENTS_INFO_TAGS_INDEX        4
#define  PATIENTS_INFO_STATUS_INDEX      5

#define  NURSES_INFO_ID_INDEX            0
#define  NURSES_INFO_NAME_INDEX          1


#define  RET_OK                          0
#define  RET_FAIL                        -1
#define  RET_PROCESSING                  1

DuiLib::CControlUI* CTempDataBuilderCallback::CreateControl(LPCTSTR pstrClass) {
	if (0 == strcmp(PURE_DATA_CONTROL_ID, pstrClass)) {
		DuiLib::CDialogBuilder builder;
		DuiLib::CControlUI * pUI = builder.Create(_T(PURE_DATA_FILE), (UINT)0, 0, m_PaintManager);
		return pUI;
	}
	else if (0 == strcmp(CURVE_CONTROL_ID, pstrClass)) {
		DuiLib::CDialogBuilder builder;
		DuiLib::CControlUI * pUI = builder.Create(_T(CURVE_FILE), (UINT)0, 0, m_PaintManager);
		return pUI;
	}
	return 0;
}


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







// ���ݿ�������
void   CDuiFrameWnd::OnFistDbConnected() {
	// ������ǵ�һ��
	if (!m_bFisrDbConnect) {
		return;
	}

	CBusiness::GetInstance()->GetAllPatientsAsyn();
	CBusiness::GetInstance()->GetAllNursesAsyn();

	m_bFisrDbConnect = FALSE;
}

void  CDuiFrameWnd::AddPatientItem2List(DuiLib::CListTextElementUI* pListElement, const PatientInfo * pPatient, BOOL bSetTag /*= TRUE */) {
	pListElement->SetText(PATIENTS_INFO_ID_INDEX,     pPatient->szId);
	pListElement->SetText(PATIENTS_INFO_NAME_INDEX,   pPatient->szName);
	pListElement->SetText(PATIENTS_INFO_GENDER_INDEX, GetGender(pPatient->bFemale));
	pListElement->SetText(PATIENTS_INFO_BED_NO_INDEX, pPatient->szBedNo);
	pListElement->SetText(PATIENTS_INFO_STATUS_INDEX, GetInHosStatus(pPatient->bOutHos));

	DuiLib::CDuiString  strText;
	for (DWORD i = 0; i < pPatient->dwTagsCnt; i++) {
		if (strText.GetLength() > 0) {
			strText += ",";
		}

		char szTag[256] = {0};
		GetUid(szTag, sizeof(szTag), pPatient->tags[i].abyUid, pPatient->tags[i].dwUidLen, '-');
		strText += szTag;
	}
	pListElement->SetText(PATIENTS_INFO_TAGS_INDEX, strText);

	if (bSetTag)
		pListElement->SetTag(pPatient->dwId);
}

void   CDuiFrameWnd::OnGetAllPatientsRet( const std::vector<PatientInfo*> & vRet ) {
	std::vector<PatientInfo*>::const_iterator it;
	for (it = vRet.begin(); it != vRet.end(); it++) {
		PatientInfo*  pPatient = *it;

		DuiLib::CListTextElementUI* pListElement = new DuiLib::CListTextElementUI;
		m_lstPatients->Add(pListElement);

		AddPatientItem2List(pListElement, pPatient);
	}

	m_bGetAllPatients = TRUE;
}

// ɾ������ret
void   CDuiFrameWnd::OnDeletePatientRet(int ret, DWORD dwId) {
	if (0 == ret)
	{
		assert(m_lstPatients);
		int nSelIndex = m_lstPatients->GetCurSel();
		DuiLib::CListTextElementUI* pListElement = (DuiLib::CListTextElementUI*)m_lstPatients->GetItemAt(nSelIndex);
		assert(pListElement);

		// ���list����û�б仯
		if ( pListElement->GetTag() == dwId ) {
			m_lstPatients->RemoveAt(nSelIndex);
		}
		// ��������list
		else {
			int cnt = m_lstPatients->GetCount();
			for (int i = 0; i < cnt; i++) {
				pListElement = (DuiLib::CListTextElementUI*)m_lstPatients->GetItemAt(i);
				assert(pListElement);
				if (pListElement->GetTag() == dwId) {
					m_lstPatients->RemoveAt(nSelIndex);
					break;
				}
			}
		}
		
		::MessageBox(this->GetHWND(), "ɾ�����˳ɹ�", "ɾ������", 0);
	}
	else {
		::MessageBox(this->GetHWND(), GetErrDescription(ret), "ɾ������", 0);
	}
}

// ���벡��ret
void  CDuiFrameWnd::OnImportPatientsRet(int ret, const std::vector<PatientInfo*> & vRet) {
	if (0 == m_lstPatients) {
		return;
	}

	std::vector<PatientInfo*>::const_iterator it;
	for (it = vRet.begin(); it != vRet.end(); it++) {
		PatientInfo*  pPatient = *it;

		if (pPatient->bToUpdated) {
			int nCnt = m_lstPatients->GetCount();
			for (int i = 0; i < nCnt; i++) {
				DuiLib::CListTextElementUI* pListElement = (DuiLib::CListTextElementUI*)m_lstPatients->GetItemAt(i);
				if (pListElement->GetTag() == pPatient->dwId) {
					AddPatientItem2List(pListElement, pPatient, FALSE);
				}
			}
		}
		else {
			DuiLib::CListTextElementUI* pListElement = new DuiLib::CListTextElementUI;
			m_lstPatients->Add(pListElement);

			AddPatientItem2List(pListElement, pPatient);
		}		
	}
}

void  CDuiFrameWnd::AddNurseItem2List(DuiLib::CListTextElementUI* pListElement, const NurseInfo * pNurse, BOOL bSetTag /*= TRUE*/ ) {
	pListElement->SetText(NURSES_INFO_ID_INDEX,    pNurse->szId);
	pListElement->SetText(NURSES_INFO_NAME_INDEX, pNurse->szName);

	if (bSetTag)
		pListElement->SetTag(pNurse->dwId);
}

void   CDuiFrameWnd::OnGetAllNursesRet(const std::vector<NurseInfo*> & vRet) {
	std::vector<NurseInfo*>::const_iterator it;
	for (it = vRet.begin(); it != vRet.end(); it++) {
		NurseInfo*  pNurse = *it;

		DuiLib::CListTextElementUI* pListElement = new DuiLib::CListTextElementUI;
		m_lstNurses->Add(pListElement);

		AddNurseItem2List(pListElement, pNurse);
	}
}

// ɾ����ʿret
void   CDuiFrameWnd::OnDeleteNurseRet(int ret, DWORD dwId) {
	if (0 == ret)
	{
		assert(m_lstNurses);
		int nSelIndex = m_lstNurses->GetCurSel();
		DuiLib::CListTextElementUI* pListElement = (DuiLib::CListTextElementUI*)m_lstNurses->GetItemAt(nSelIndex);
		assert(pListElement);

		// ���list����û�б仯
		if (pListElement->GetTag() == dwId) {
			m_lstNurses->RemoveAt(nSelIndex);
		}
		// ��������list
		else {
			int cnt = m_lstNurses->GetCount();
			for (int i = 0; i < cnt; i++) {
				pListElement = (DuiLib::CListTextElementUI*)m_lstNurses->GetItemAt(i);
				assert(pListElement);
				if (pListElement->GetTag() == dwId) {
					m_lstNurses->RemoveAt(nSelIndex);
					break;
				}
			}
		}

		::MessageBox(this->GetHWND(), "ɾ����ʿ�ɹ�", "ɾ����ʿ", 0);
	}
	else {
		::MessageBox(this->GetHWND(), GetErrDescription(ret), "ɾ����ʿ", 0);
	}
}

void   CDuiFrameWnd::OnImportNursesRet(int ret, const std::vector<NurseInfo*> & vRet) {
	if (0 == m_lstNurses) {
		return;
	}

	std::vector<NurseInfo*>::const_iterator it;
	for (it = vRet.begin(); it != vRet.end(); it++) {
		NurseInfo*  pNurse = *it;

		if (pNurse->bToUpdated) {
			int nCnt = m_lstNurses->GetCount();
			for (int i = 0; i < nCnt; i++) {
				DuiLib::CListTextElementUI* pListElement = (DuiLib::CListTextElementUI*)m_lstNurses->GetItemAt(i);
				if (pListElement->GetTag() == pNurse->dwId) {
					AddNurseItem2List(pListElement, pNurse, FALSE);
				}
			}
		}
		else {
			DuiLib::CListTextElementUI* pListElement = new DuiLib::CListTextElementUI;
			m_lstNurses->Add(pListElement);

			AddNurseItem2List(pListElement, pNurse);
		}
	}
}

// Inventory ret
void   CDuiFrameWnd::OnInventoryRet( const TagItem * pItem ) {
	// ��û�л�ȡ��������Ϣ
	if (!m_bGetAllPatients) {
		return;
	}

	int nInventoryRet = RET_FAIL;              // �����̵�����-1��ʾʧ�ܣ�0��ʾ�ɹ�, 1��ʾ������
	// �����̵�OK
	if (pItem) {
		nInventoryRet = RET_OK;
	}

	int nSelectedTab = m_tabs->GetCurSel();
	if (PATIENTS_INDEX == nSelectedTab) {

		// ������ڴ���
		if (m_nLastInventoryRet_Patients == RET_PROCESSING) {
			return;
		}

		// �̵����б仯
		if ( (m_nLastInventoryRet_Patients != nInventoryRet) || ( nInventoryRet==0 && !IsSameTag( &m_LastInventoryRet_Patients,pItem ) ) ) {

			// �̵�ʧ��
			if ( nInventoryRet == RET_FAIL) {
				m_btnBindingPatient->SetEnabled(false);

				m_nLastInventoryRet_Patients = nInventoryRet;
				memset(&m_LastInventoryRet_Patients, 0, sizeof(TagItem));
			}
			// �̵�ɹ�
			else {
				// ���Tag���ް�
				CBusiness::GetInstance()->CheckTagBindingAsyn(pItem);

				m_btnBindingPatient->SetEnabled(false);
				m_nLastInventoryRet_Patients = RET_PROCESSING;

				// m_nLastInventoryRet_Patients = nInventoryRet;
				// memcpy(&m_LastInventoryRet_Patients, pItem, sizeof(TagItem));
			}			
		}
		// �ޱ仯
		else {
			if ( 0 == pItem ) {
				return;
			}

			// �̵�ɹ�����λlist
			char szTag[256] = { 0 };
			GetUid(szTag, sizeof(szTag), pItem->abyUid, pItem->dwUidLen, '-');
			int nCnt = m_lstPatients->GetCount();
			for (int i = 0; i < nCnt; i++) {
				DuiLib::CListTextElementUI* pListElement = (DuiLib::CListTextElementUI*)m_lstPatients->GetItemAt(i);
				DuiLib::CDuiString strText = pListElement->GetText(PATIENTS_INFO_TAGS_INDEX);
				if (0 != strstr(strText, szTag)) {
					m_lstPatients->SelectItem(i);
					break;
				}
			}
		}
	}
	else if (NURSES_INDEX == nSelectedTab) {
		// �̵����б仯
		if (m_nLastInventoryRet_Nurses != nInventoryRet) {
			//

			m_nLastInventoryRet_Nurses = nInventoryRet;
		}
	}
}

// Check Tag binding ret
void   CDuiFrameWnd::OnCheckTagBindingRet(DWORD dwPatientId, const TagItem * pItem) {
	if (pItem) {
		m_nLastInventoryRet_Patients = RET_OK;
		memcpy(&m_LastInventoryRet_Patients, pItem, sizeof(TagItem));
	}
	else {
		m_nLastInventoryRet_Patients = RET_FAIL;
		memset(&m_LastInventoryRet_Patients, 0, sizeof(TagItem));
	}
	
	if ( dwPatientId == -1 ) {
		return;
	}

	// û�а󶨵�Tag
	if (0 == dwPatientId) {
		m_btnBindingPatient->SetEnabled(true);
	}
	// �Ѿ��󶨵�Tag
	else {
		if (m_lstPatients == 0) {
			return;
		}

		int nCnt = m_lstPatients->GetCount();
		for (int i = 0; i < nCnt; i++) {
			DuiLib::CListTextElementUI* pListElement = (DuiLib::CListTextElementUI* )m_lstPatients->GetItemAt(i);
			assert(pListElement);

			// �ҵ��ò��ˣ����Ҹ���
			if (pListElement->GetTag() == dwPatientId) {
				m_lstPatients->SelectItem(i);
				break;
			}
		}
	}
}

void   CDuiFrameWnd::OnBindingPatientRet(int nError, CBindingPatientParam * pParam) {
	m_nLastInventoryRet_Patients = RET_FAIL;
	memset(&m_LastInventoryRet_Patients, 0, sizeof(TagItem));
	SET_CONTROL_ENABLED(m_btnBindingPatient, false);

	if (0 != nError) {
		MessageBox(this->GetHWND(), GetErrDescription(nError), "�󶨲���Tag", 0);
		return;
	}	

	if (0 == pParam) {
		return;
	}

	if (0 == m_lstPatients) {
		return;
	}

	char szTag[256] = {0};
	GetUid(szTag, sizeof(szTag), pParam->m_tag.abyUid, pParam->m_tag.dwUidLen, '-');

	int nCnt = m_lstPatients->GetCount();
	for (int i = 0; i < nCnt; i++) {
		DuiLib::CListTextElementUI* pListElement = (DuiLib::CListTextElementUI*)m_lstPatients->GetItemAt(i);
		assert(pListElement);

		// �ҵ��ò��ˣ����Ҹ���
		if (pListElement->GetTag() == pParam->m_dwPatientId) {
			DuiLib::CDuiString strText = pListElement->GetText(PATIENTS_INFO_TAGS_INDEX);
			if ( strText.GetLength() == 0 ) {
				strText = szTag;
			}
			else {
				strText += ",";
				strText += szTag;
			}
			pListElement->SetText(PATIENTS_INFO_TAGS_INDEX, strText);
			break;
		}
	}

}




// ��Ӳ���
void   CDuiFrameWnd::OnAddPatient() {
	CPatientWnd * pPatientDlg = new CPatientWnd;
	if (0 == pPatientDlg) {
		return;
	}

	pPatientDlg->Create(this->m_hWnd, _T("����������Ϣ"), UI_WNDSTYLE_FRAME | WS_POPUP, NULL, 0, 0, 0, 0);
	pPatientDlg->CenterWindow();
	int ret = pPatientDlg->ShowModal();

	// �����ӳɹ�
	if (0 == ret && m_lstPatients) {
		DuiLib::CListTextElementUI* pListElement = new DuiLib::CListTextElementUI;
		m_lstPatients->Add(pListElement);

		AddPatientItem2List(pListElement, &pPatientDlg->m_tPatientInfo);
	}

	delete pPatientDlg;
}

// �޸Ĳ���
void   CDuiFrameWnd::OnModifyPatient() {
	if (0 == m_lstPatients) {
		return;
	}

	int nSelIndex = m_lstPatients->GetCurSel();
	DuiLib::CListTextElementUI* pListElement = (DuiLib::CListTextElementUI*)m_lstPatients->GetItemAt(nSelIndex);
	if (0 == pListElement) {
		return;
	}

	CPatientWnd * pPatientDlg = new CPatientWnd(FALSE);
	if (0 == pPatientDlg) {
		return;
	}
	memset(&pPatientDlg->m_tPatientInfo, 0, sizeof(PatientInfo));

	strncpy_s(pPatientDlg->m_tPatientInfo.szId,    pListElement->GetText(PATIENTS_INFO_ID_INDEX),     sizeof(pPatientDlg->m_tPatientInfo.szId));
	strncpy_s(pPatientDlg->m_tPatientInfo.szName,  pListElement->GetText(PATIENTS_INFO_NAME_INDEX),   sizeof(pPatientDlg->m_tPatientInfo.szName));
	strncpy_s(pPatientDlg->m_tPatientInfo.szBedNo, pListElement->GetText(PATIENTS_INFO_BED_NO_INDEX), sizeof(pPatientDlg->m_tPatientInfo.szBedNo));
	pPatientDlg->m_tPatientInfo.bFemale = GetGender(pListElement->GetText(PATIENTS_INFO_GENDER_INDEX));
	pPatientDlg->m_tPatientInfo.bOutHos = GetInHosStatus(pListElement->GetText(PATIENTS_INFO_STATUS_INDEX));

	DuiLib::CDuiString  strText = pListElement->GetText(PATIENTS_INFO_TAGS_INDEX);
	SplitString SplitObj;
	SplitObj.Split(strText, ',');
	DWORD i = 0;
	for ( i = 0; i < SplitObj.Size() && i < MAX_PATIENT_TAGS; i++ ) {
		GetUid(&pPatientDlg->m_tPatientInfo.tags[i], SplitObj[i], '-');
	}
	pPatientDlg->m_tPatientInfo.dwTagsCnt = i;

	pPatientDlg->m_tPatientInfo.dwId = (DWORD)pListElement->GetTag();
	
	pPatientDlg->Create(this->m_hWnd, _T("����������Ϣ"), UI_WNDSTYLE_FRAME | WS_POPUP, NULL, 0, 0, 0, 0);
	pPatientDlg->CenterWindow();
	int ret = pPatientDlg->ShowModal();

	// ����޸ĳɹ�
	if (0 == ret && m_lstPatients) {
		AddPatientItem2List(pListElement, &pPatientDlg->m_tPatientInfo, FALSE);
	}

	delete pPatientDlg;
}

// ɾ������
void   CDuiFrameWnd::OnDeletePatient() {
	if (0 == m_lstPatients) {
		return;
	}

	int nSelIndex = m_lstPatients->GetCurSel();
	DuiLib::CListTextElementUI* pListElement = (DuiLib::CListTextElementUI*)m_lstPatients->GetItemAt(nSelIndex);
	if (0 == pListElement) {
		return;
	}

	if ( IDYES == ::MessageBox(GetHWND(), "��ȷ��Ҫɾ���ò�����Ϣ��", "ɾ������", MB_YESNO | MB_DEFBUTTON2) ) {
		CBusiness::GetInstance()->DeletePatientAsyn( (DWORD)pListElement->GetTag() );
	}
}

// ���벡��
void   CDuiFrameWnd::OnImportPatients() {
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
		CBusiness::GetInstance()->ImportPatientsAsyn( strFilename );
	}
}


// ��ӻ�ʿ
void   CDuiFrameWnd::OnAddNurse() {
	CNurseWnd * pNurseDlg = new CNurseWnd;
	if ( 0 == pNurseDlg ) {
		return;
	}

	pNurseDlg->Create(this->m_hWnd, _T("������ʿ��Ϣ"), UI_WNDSTYLE_FRAME | WS_POPUP, NULL, 0, 0, 0, 0);
	pNurseDlg->CenterWindow();
	int ret = pNurseDlg->ShowModal();

	// �����ӳɹ�
	if ( 0 == ret && m_lstNurses ) {
		DuiLib::CListTextElementUI* pListElement = new DuiLib::CListTextElementUI;
		m_lstNurses->Add(pListElement);

		AddNurseItem2List(pListElement, &pNurseDlg->m_tNurseInfo);
	}

	delete pNurseDlg;
}

// �޸Ļ�ʿ
void   CDuiFrameWnd::OnModifyNurse() {
	if (0 == m_lstNurses) {
		return;
	}

	int nSelIndex = m_lstNurses->GetCurSel();
	DuiLib::CListTextElementUI* pListElement = (DuiLib::CListTextElementUI*)m_lstNurses->GetItemAt(nSelIndex);
	if (0 == pListElement) {
		return;
	}

	CNurseWnd * pNurseDlg = new CNurseWnd(FALSE);
	if (0 == pNurseDlg) {
		return;
	}
	memset(&pNurseDlg->m_tNurseInfo, 0, sizeof(NurseInfo));

	strncpy_s(pNurseDlg->m_tNurseInfo.szId,   pListElement->GetText(NURSES_INFO_ID_INDEX),   sizeof(pNurseDlg->m_tNurseInfo.szId));
	strncpy_s(pNurseDlg->m_tNurseInfo.szName, pListElement->GetText(NURSES_INFO_NAME_INDEX), sizeof(pNurseDlg->m_tNurseInfo.szName));	
	pNurseDlg->m_tNurseInfo.dwId = (DWORD)pListElement->GetTag();

	pNurseDlg->Create(this->m_hWnd, _T("�޸Ĳ�����Ϣ"), UI_WNDSTYLE_FRAME | WS_POPUP, NULL, 0, 0, 0, 0);
	pNurseDlg->CenterWindow();
	int ret = pNurseDlg->ShowModal();

	// ����޸ĳɹ�
	if (0 == ret) {
		AddNurseItem2List(pListElement, &pNurseDlg->m_tNurseInfo, FALSE);
	}

	delete pNurseDlg;
}

// ɾ����ʿ
void   CDuiFrameWnd::OnDeleteNurse() {
	if ( 0 == m_lstNurses ) {
		return;
	}

	int nSelIndex = m_lstNurses->GetCurSel();
	DuiLib::CListTextElementUI* pListElement = (DuiLib::CListTextElementUI*)m_lstNurses->GetItemAt(nSelIndex);
	if (0 == pListElement) {
		return;
	}

	if (IDYES == ::MessageBox(GetHWND(), "��ȷ��Ҫɾ���û�ʿ��Ϣ��", "ɾ����ʿ", MB_YESNO | MB_DEFBUTTON2)) {
		CBusiness::GetInstance()->DeleteNurseAsyn((DWORD)pListElement->GetTag());
	}
}

// ���뻤ʿ
void  CDuiFrameWnd::OnImportNurses() {
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
		CBusiness::GetInstance()->ImportNursesAsyn(strFilename);
	}
}

void   CDuiFrameWnd::OnBindingPatient() {
	if (m_lstPatients == 0) {
		return;
	}

	int nSelIndex = m_lstPatients->GetCurSel();
	DuiLib::CListTextElementUI* pListElement = (DuiLib::CListTextElementUI*)m_lstPatients->GetItemAt(nSelIndex);
	if (0 == pListElement) {
		return;
	}

	DWORD dwPatientId = pListElement->GetTag();

	CBusiness::GetInstance()->BindingPatientAsyn(dwPatientId, &m_LastInventoryRet_Patients);
}




void CDuiFrameWnd::InitWindow() {
	m_bFisrDbConnect = TRUE;
	g_hWnd = GetHWND();

	PostMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0);

	m_Callback.m_PaintManager = &m_PaintManager;

	m_lblDbStatus = (DuiLib::CLabelUI *)m_PaintManager.FindControl(DATABASE_STATUS_LABEL_ID);
	m_lblBindingReaderStatus = (DuiLib::CLabelUI *)m_PaintManager.FindControl(BINDING_READER_STATUS_LABEL_ID);
	m_lblSyncReaderStatus = (DuiLib::CLabelUI *)m_PaintManager.FindControl(SYNC_READER_STATUS_LABEL_ID);
	m_tabs = static_cast<DuiLib::CTabLayoutUI*>(m_PaintManager.FindControl(_T(TABS_ID)));
	m_sub_tabs = static_cast<DuiLib::CTabLayoutUI*>(m_PaintManager.FindControl(_T(SUB_TABS_ID)));

	m_lstPatients = static_cast<DuiLib::CListUI*>(m_PaintManager.FindControl(_T(PATIENTS_LIST_ID)));
	m_lstNurses   = static_cast<DuiLib::CListUI*>(m_PaintManager.FindControl(_T(NURSES_LIST_ID)));
	
	m_btnBindingPatient = static_cast<DuiLib::CButtonUI*>(m_PaintManager.FindControl(_T(BINDING_PATIENT_BUTTON_ID)));
	m_btnBindingNurse   = static_cast<DuiLib::CButtonUI*>(m_PaintManager.FindControl(_T(BINDING_NURSE_BUTTON_ID)));


	CZsDatabase::DATABASE_STATUS eDbStatus = CBusiness::GetInstance()->GetDbStatus();
	if (eDbStatus == CZsDatabase::STATUS_OPEN) {
		SET_CONTROL_TEXT(m_lblDbStatus, DB_STATUS_OK_TEXT);
		OnFistDbConnected();
	}
	else {
		SET_CONTROL_TEXT(m_lblDbStatus, DB_STATUS_CLOSE_TEXT);
	}

	CZsBindingReader::BINDING_STATUS eBindingReaderStatus = CBusiness::GetInstance()->GetBindingReaderStatus();
	if (eBindingReaderStatus == CZsBindingReader::STATUS_OPEN) {
		SET_CONTROL_TEXT(m_lblBindingReaderStatus, BINDING_READER_STATUS_OK_TEXT);
	}
	else {
		SET_CONTROL_TEXT(m_lblBindingReaderStatus, BINDING_READER_STATUS_CLOSE_TEXT);
	}

	m_nLastInventoryRet_Patients = RET_FAIL;
	m_nLastInventoryRet_Nurses   = RET_FAIL;
	memset(&m_LastInventoryRet_Patients, 0, sizeof(m_LastInventoryRet_Patients));
	memset(&m_LastInventoryRet_Nurses, 0, sizeof(m_LastInventoryRet_Nurses));

	m_bGetAllPatients = FALSE;

	WindowImplBase::InitWindow();
}

DuiLib::CControlUI * CDuiFrameWnd::CreateControl(LPCTSTR pstrClass) {
	if (0 == strcmp(TEMPERATURE_DATA_CONTROL_ID, pstrClass)) {
		DuiLib::CDialogBuilder builder;
		DuiLib::CControlUI * pUI = builder.Create(_T(TEMPERATURE_DATA_FILE), (UINT)0, &m_Callback, &m_PaintManager);
		return pUI;
	}
	else if (0 == strcmp(PATIENTS_CONTROL_ID, pstrClass)) {
		DuiLib::CDialogBuilder builder;
		DuiLib::CControlUI * pUI = builder.Create(_T(PATIENTS_FILE), (UINT)0, 0, &m_PaintManager);
		return pUI;
	}
	else if (0 == strcmp(NURSES_CONTROL_ID, pstrClass)) {
		DuiLib::CDialogBuilder builder;
		DuiLib::CControlUI * pUI = builder.Create(_T(NURSES_FILE), (UINT)0, 0, &m_PaintManager);
		return pUI;
	}
	else if (0 == strcmp(SYNCHRONIZE_CONTROL_ID, pstrClass)) {
		DuiLib::CDialogBuilder builder;
		DuiLib::CControlUI * pUI = builder.Create(_T(SYNCHRONIZE_FILE), (UINT)0, 0, &m_PaintManager);
		return pUI;
	}
	return 0;
}

void CDuiFrameWnd::Notify(DuiLib::TNotifyUI& msg) {
	DuiLib::CDuiString name = msg.pSender->GetName();
	DuiLib::CDuiString strText;

	if (msg.sType == _T("selectchanged"))
	{
		if (name == _T(TEMPERATURE_DATA_OPTION_ID)) {
			if (m_tabs)
				m_tabs->SelectItem(TEMPERATURE_DATA_INDEX);
		}
		else if (name == _T(PATIENTS_OPTION_ID)) {
			if (m_tabs)
				if (m_tabs)m_tabs->SelectItem(PATIENTS_INDEX);

			if (m_lstPatients) {
				if (m_lstPatients->GetCount() > 0) {
					if (m_lstPatients->GetCurSel() < 0)
					{
						m_lstPatients->SelectItem(0);
					}
				}
			}			
		}
		else if (name == _T(NURSES_OPTION_ID)) {
			if (m_tabs)
				m_tabs->SelectItem(NURSES_INDEX);
			if (m_lstNurses) {
				if (m_lstNurses->GetCount() > 0) {
					if (m_lstNurses->GetCurSel() < 0)
					{
						m_lstNurses->SelectItem(0);
					}
				}
			}
		}
		else if (name == SYNCHRONIZE_OPTION_ID) {
			if (m_tabs)
				m_tabs->SelectItem(SYNCHRONIZE_INDEX);
		}
		else if (name == PURE_DATA_OPTION_ID) {
			if (m_sub_tabs)
				m_sub_tabs->SelectItem(PURE_DATA_INDEX);
		}
		else if (name == CURVE_OPTION_ID) {
			if (m_sub_tabs)
				m_sub_tabs->SelectItem(CURVE_INDEX);
		}

		int nSel = m_tabs->GetCurSel();
		if ( nSel != PATIENTS_INDEX ) {
			m_nLastInventoryRet_Patients = -1;
			memset(&m_LastInventoryRet_Patients, 0, sizeof(m_LastInventoryRet_Patients));
			SET_CONTROL_ENABLED(m_btnBindingPatient,false);
		}

		if (nSel != NURSES_INDEX) {
			m_nLastInventoryRet_Nurses = -1;
			memset(&m_LastInventoryRet_Nurses, 0, sizeof(m_LastInventoryRet_Nurses));
			SET_CONTROL_ENABLED(m_btnBindingNurse, false);
		}
	}
	else if (msg.sType == "click") {
		if (name == ADD_PATIENT_BUTTON_ID) {
			OnAddPatient();
		}
		else if (name == MOD_PATIENT_BUTTON_ID) {
			OnModifyPatient();
		}
		else if (name == DEL_PATIENT_BUTTON_ID) {
			OnDeletePatient();
		}
		else if (name == IMPORT_PATIENT_BUTTON_ID) {
			OnImportPatients();
		}
		else if (name == BINDING_PATIENT_BUTTON_ID) {
			OnBindingPatient();
		}
		else if (name == ADD_NURSE_BUTTON_ID) {
			OnAddNurse();
		}
		else if (name == MOD_NURSE_BUTTON_ID) {
			OnModifyNurse();
		}
		else if (name == DEL_NURSE_BUTTON_ID) {
			OnDeleteNurse();
		}
		else if (name == IMPORT_NURSE_BUTTON_ID) {
			OnImportNurses();
		}
		else if (name == BINDING_NURSE_BUTTON_ID) {

		}
	}
	else if (msg.sType == "itemactivate") {
		if ( m_tabs && m_tabs->GetCurSel() == PATIENTS_INDEX) {
			OnModifyPatient();
		}
	}
	else if (msg.sType == _T("menu"))
	{
		POINT pt = { msg.ptMouse.x, msg.ptMouse.y };
		CControlUI * pControl = m_PaintManager.FindControl(pt);
		if (0 == pControl) {
			return;
		}

		CDuiString sFindCtlClass = pControl->GetClass();
		if (sFindCtlClass == "ListTextElement") {
			CDuiString sListName = pControl->GetParent()->GetParent()->GetName();
			if (sListName == "patients_list") {
				CDuiMenu *pMenu = new CDuiMenu(_T("menu.xml"), msg.pSender);
				pMenu->Init(*this, pt);
				pMenu->ShowWindow(TRUE);
			}
		}
		return;
	}
	else if (msg.sType == "menu_modify_patient") {
		OnModifyPatient();
		return;
	}
	else if (msg.sType == "menu_delete_patient") {
		OnDeletePatient();
		return;
	}
	WindowImplBase::Notify(msg);
}

LRESULT CDuiFrameWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	int nError = 0;
	//char buf[8192];

	if (uMsg == UM_SHOW_DB_STATUS) {
		CZsDatabase::DATABASE_STATUS eStatus = (CZsDatabase::DATABASE_STATUS)wParam;
		if (eStatus == CZsDatabase::STATUS_OPEN) {
			SET_CONTROL_COLOR(m_lblDbStatus, COLOR_OK);
			SET_CONTROL_TEXT(m_lblDbStatus, DB_STATUS_OK_TEXT);
			OnFistDbConnected();
		}
		else {
			SET_CONTROL_COLOR(m_lblDbStatus, COLOR_ERROR);
			SET_CONTROL_TEXT(m_lblDbStatus, DB_STATUS_CLOSE_TEXT);
		}
		return 0;
	}
	else if (uMsg == UM_SHOW_BINDING_READER_STATUS) {
		CZsBindingReader::BINDING_STATUS eStatus = (CZsBindingReader::BINDING_STATUS)wParam;
		if (eStatus == CZsBindingReader::STATUS_OPEN) {
			SET_CONTROL_COLOR(m_lblBindingReaderStatus, COLOR_OK);
			SET_CONTROL_TEXT(m_lblBindingReaderStatus, BINDING_READER_STATUS_OK_TEXT);
		}
		else {
			SET_CONTROL_COLOR(m_lblBindingReaderStatus, COLOR_ERROR);
			SET_CONTROL_TEXT(m_lblBindingReaderStatus, BINDING_READER_STATUS_CLOSE_TEXT);
		}
		return 0;
	}
	else if (uMsg == UM_GET_ALL_PATIENTS_RET) {
		nError = wParam;
		std::vector<PatientInfo * > * pvRet = (std::vector<PatientInfo *> *)lParam;

		if ( (0 == nError) && pvRet )
			OnGetAllPatientsRet(*pvRet);

		if (pvRet) {
			ClearVector(*pvRet);
		}
		return 0;
	}
	else if (uMsg == UM_DELETE_PATIENT_RET) {
		nError = wParam;
		DWORD dwId = (DWORD)lParam;

		OnDeletePatientRet( nError, dwId );
		return 0;
	}
	else if (uMsg == UM_NOTIFY_IMPORT_PATIENTS_RET) {
		nError = wParam;
		std::vector<PatientInfo*> * pvRet = (std::vector<PatientInfo*> *)lParam;

		if ( 0 == nError ) {
			if (pvRet) {
				OnImportPatientsRet(nError, *pvRet);
			}			
		}
		// ����ʧ��
		else if ( ZS_ERR_PARTIALLY_FAILED_TO_IMPORT_EXCEL == nError) {
			if (pvRet) {
				OnImportPatientsRet(nError, *pvRet);
			}
			::MessageBox(this->GetHWND(), GetErrDescription(nError), "���벡����Ϣ", 0);
		}
		// ��ȫʧ��
		else {
			::MessageBox(this->GetHWND(), GetErrDescription(nError), "���벡����Ϣ", 0);
		}

		if (pvRet) {
			ClearVector(*pvRet);
			delete pvRet;
		}
		return 0;
	}
	else if (uMsg == UM_GET_ALL_NURSES_RET) {
		nError = wParam;
		std::vector<NurseInfo * > * pvRet = (std::vector<NurseInfo *> *)lParam;

		if ((0 == nError) && pvRet)
			OnGetAllNursesRet(*pvRet);

		if (pvRet) {
			ClearVector(*pvRet);
		}
		return 0;
	}
	else if (uMsg == UM_DELETE_NURSE_RET) {
		nError = wParam;
		DWORD dwId = (DWORD)lParam;

		OnDeleteNurseRet(nError, dwId);
		return 0;
	}
	else if (uMsg == UM_NOTIFY_IMPORT_NURSES_RET) {
		nError = wParam;
		std::vector<NurseInfo*> * pvRet = (std::vector<NurseInfo*> *)lParam;

		if (0 == nError) {
			if (pvRet) {
				OnImportNursesRet(nError, *pvRet);
			}
		}
		// ����ʧ��
		else if (ZS_ERR_PARTIALLY_FAILED_TO_IMPORT_EXCEL == nError) {
			if (pvRet) {
				OnImportNursesRet(nError, *pvRet);
			}
			::MessageBox(this->GetHWND(), GetErrDescription(nError), "���뻤ʿ��Ϣ", 0);
		}
		// ��ȫʧ��
		else {
			::MessageBox(this->GetHWND(), GetErrDescription(nError), "���뻤ʿ��Ϣ", 0);
		}

		if (pvRet) {
			ClearVector(*pvRet);
			delete pvRet;
		}
		return 0;
	}
	else if ( uMsg == UM_INVENTORY_RET )
	{
		nError = wParam;
		TagItem * pItem = (TagItem *)lParam;

		OnInventoryRet(pItem);		

		if (pItem) {
			delete pItem;
		}
		return 0;
	}
	else if (uMsg == UM_CHECK_TAG_BINDING_RET) {
		DWORD  dwPatientId = wParam;
		TagItem * pItem = (TagItem *)lParam;

		OnCheckTagBindingRet(dwPatientId, pItem);

		if (pItem) {
			delete pItem;
		}
	}
	else if (uMsg == UM_NOTIFY_BINDING_PATIENT_RET) {
		nError = wParam;
		CBindingPatientParam * pParam = (CBindingPatientParam *)lParam;

		OnBindingPatientRet(nError, pParam);
		

		if (pParam) {
			delete pParam;
		}
	}

	return WindowImplBase::HandleMessage(uMsg, wParam, lParam);
}


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	int ret = 0;
	LmnToolkits::ThreadManager::GetInstance();
	CBusiness::GetInstance()->sigInit.emit(&ret);

	DuiLib::CPaintManagerUI::SetInstance(hInstance);

	HICON hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));

	// ������	
	CDuiFrameWnd duiFrame;
	duiFrame.Create(NULL, _T("DUIWnd"), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
	duiFrame.CenterWindow();

	// ����icon
	::SendMessage(duiFrame.GetHWND(), WM_SETICON, ICON_BIG, (LPARAM)hIcon);
	::SendMessage(duiFrame.GetHWND(), WM_SETICON, ICON_SMALL, (LPARAM)hIcon);	

	// show model
	duiFrame.ShowModal();

	CBusiness::GetInstance()->sigDeinit.emit(&ret);
	delete CBusiness::GetInstance();
	LmnToolkits::ThreadManager::ReleaseInstance();

	return 0;
}