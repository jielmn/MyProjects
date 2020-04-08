#include "QueryDlg.h"
#include "business.h"
#include "LmnTemplates.h"
#include <time.h>

CQueryDlg::CQueryDlg() {
	m_StartTime = 0;
	m_EndTime = 0;
	m_edMac = 0;
	m_lstResult = 0;
	m_btnQuery = 0;
	m_btnExport = 0;
}                
 
void  CQueryDlg::Notify(DuiLib::TNotifyUI& msg) {
	CDuiString  strName = msg.pSender->GetName();

	if (msg.sType == "click") {
		if (strName == "btnQuery") {
			OnQuery();
		}
		else if (strName == "btnExport") {
			OnExport();
		}
	}
	WindowImplBase::Notify(msg);
}

void  CQueryDlg::InitWindow() {
	m_StartTime = static_cast<DuiLib::CDateTimeUI*>(m_PaintManager.FindControl("DateTimeStart"));
	m_EndTime = static_cast<DuiLib::CDateTimeUI*>(m_PaintManager.FindControl("DateTimeEnd"));
	m_edMac = static_cast<DuiLib::CEditUI*>(m_PaintManager.FindControl("edQMac"));
	m_lstResult = static_cast<DuiLib::CListUI*>(m_PaintManager.FindControl("lstResult"));
	m_btnQuery = static_cast<DuiLib::CButtonUI*>(m_PaintManager.FindControl("btnQuery"));
	m_btnExport = static_cast<DuiLib::CButtonUI*>(m_PaintManager.FindControl("btnExport"));

	WindowImplBase::InitWindow();
}

LRESULT  CQueryDlg::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if ( uMsg == UM_QUERY_DATA_RET ) {
		std::vector<ReaderItem*> * pvRet = (std::vector<ReaderItem*> *)wParam;

		m_lstResult->RemoveAll();
		std::vector<ReaderItem*>::iterator it;
		for (it = pvRet->begin(); it != pvRet->end(); ++it) {
			ReaderItem* pItem = *it;

			CListTextElementUI * pUI = new CListTextElementUI;
			m_lstResult->Add(pUI);

			pUI->SetText(0, pItem->m_szMac);  
			pUI->SetText(1, pItem->m_bPass?"��":"��");
			pUI->SetText(2, IfHasBit(pItem->m_dwFact, 0) ? "��" : "" );
			pUI->SetText(3, IfHasBit(pItem->m_dwFact, 1) ? "��" : "" );
			pUI->SetText(4, IfHasBit(pItem->m_dwFact, 2) ? "��" : "" );

			char szTime[256];
			pUI->SetText(5, LmnFormatTime(szTime, sizeof(szTime), pItem->m_time, "%Y-%m-%d %H:%M:%S" ) );
		}

		ClearVector(*pvRet);
		delete pvRet;

		m_btnQuery->SetEnabled(true);
		m_btnExport->SetEnabled(true);
	}
	return WindowImplBase::HandleMessage(uMsg, wParam, lParam);
}

CControlUI * CQueryDlg::CreateControl(LPCTSTR pstrClass) {
	return WindowImplBase::CreateControl(pstrClass);
}

void  CQueryDlg::OnQuery() {
	SYSTEMTIME s1 =  m_StartTime->GetTime();
	SYSTEMTIME s2 =  m_EndTime->GetTime();

	time_t t1 = GetAdZeroTime(SystemTime2Time(s1));
	time_t t2 = GetAdZeroTime(SystemTime2Time(s2));

	CDuiString  strMac = m_edMac->GetText();
	strMac.MakeLower();

	CBusiness::GetInstance()->QueryDataAsyn(t1, t2 + 86400, strMac, GetHWND());
	m_btnQuery->SetEnabled(false);
	m_btnExport->SetEnabled(false);
}

void   CQueryDlg::OnExport() {
	OPENFILENAME ofn = { 0 };
	TCHAR strFilename[MAX_PATH] = { 0 };//���ڽ����ļ���  

	ofn.lStructSize     = sizeof(OPENFILENAME);//�ṹ���С  
	ofn.hwndOwner       = GetHWND();//ӵ���Ŵ��ھ����ΪNULL��ʾ�Ի����Ƿ�ģ̬�ģ�ʵ��Ӧ����һ�㶼Ҫ��������  
	ofn.lpstrFilter     = TEXT("Text Flie(*.txt)\0*.txt\0");//���ù���  
	ofn.nFilterIndex    = 0;//����������  
	ofn.lpstrFile       = strFilename;//���շ��ص��ļ�����ע���һ���ַ���ҪΪNULL  
	ofn.nMaxFile        = sizeof(strFilename);//����������  
	ofn.lpstrInitialDir = CPaintManagerUI::GetInstancePath();
	ofn.lpstrTitle = TEXT("��ѡ��һ���ļ�");//ʹ��ϵͳĬ�ϱ������ռ���  
	ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;//�ļ���Ŀ¼������ڣ�����ֻ��ѡ��  
	if (!GetSaveFileName(&ofn)) {
		::MessageBox(GetHWND(), "��������ʧ��!", "����", 0);
		return;
	}

	char  szName[256];
	char  szExtension[256];
	char  szPath[256];
	ParseFileName(strFilename, 0, 0, szName, sizeof(szName), szExtension, sizeof(szExtension));
	if ( szExtension[0] == '\0' ) {
		SNPRINTF(szPath, sizeof(szPath), "%s.txt", strFilename);
	}
	else {
		SNPRINTF(szPath, sizeof(szPath), "%s", strFilename);
	}

	FILE * fp = fopen(szPath, "wb");
	if (0 == fp) {
		::MessageBox(GetHWND(), "��������ʧ��!", "����", 0);
		return;
	}

	int nCnt = m_lstResult->GetCount();
	char buf[8192];
	for (int i = 0; i < nCnt; i++) {
		CListTextElementUI * pUi = (CListTextElementUI *)m_lstResult->GetItemAt(i);
		SNPRINTF(buf, sizeof(buf), "%s\t%s\t%s\t%s\t%s\t%s\r\n", 
			pUi->GetText(0), pUi->GetText(1), pUi->GetText(2), 
			pUi->GetText(3), pUi->GetText(4), pUi->GetText(5));
		fwrite(buf, 1, strlen(buf), fp);
	}

	::MessageBox(GetHWND(), "�������ݳɹ�!", "�ɹ�", 0);
	fclose(fp);
}