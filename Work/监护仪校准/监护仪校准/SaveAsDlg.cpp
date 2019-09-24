#include "SaveAsDlg.h"
#include "LmnFile.h"

CSaveAsDlg::CSaveAsDlg() {
	m_lst = 0;
	m_edFileName = 0;
}

void  CSaveAsDlg::Notify(DuiLib::TNotifyUI& msg) {
	CDuiString name = msg.pSender->GetName();
	if ( msg.sType == "click" ) {
		if (name == "btnOK") {
			OnMyOk();
		}
	}
	WindowImplBase::Notify(msg);
}

void  CSaveAsDlg::InitWindow() {
	m_lst = static_cast<DuiLib::CListUI*>(m_PaintManager.FindControl("lstFiles"));
	m_edFileName = static_cast<DuiLib::CEditUI*>(m_PaintManager.FindControl("edtFileName"));

	WIN32_FIND_DATA FindData;
	HANDLE hFind;

	CDuiString strCurPath = DuiLib::CPaintManagerUI::GetInstancePath();

	char szFilePathName[256];
	SNPRINTF( szFilePathName, sizeof(szFilePathName), "%s%s\\*.txt", 
		      (const char *)strCurPath, (const char *)m_strFoldName );

	// �г����е��ļ�
	hFind = FindFirstFile(szFilePathName, &FindData);
	if (hFind != INVALID_HANDLE_VALUE) {
		do {
			CListTextElementUI* pItem = new CListTextElementUI;
			pItem->SetText(0, FindData.cFileName);
			m_lst->Add(pItem);

			if (!::FindNextFile(hFind, &FindData)) {
				break;
			}
		} while (TRUE);
		FindClose(hFind);
	}

	WindowImplBase::InitWindow();
}

LRESULT  CSaveAsDlg::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	return WindowImplBase::HandleMessage(uMsg, wParam, lParam);
}

DuiLib::CControlUI * CSaveAsDlg::CreateControl(LPCTSTR pstrClass) {
	return WindowImplBase::CreateControl(pstrClass);           
}

void  CSaveAsDlg::OnMyOk() {
	CDuiString  strFileName = m_edFileName->GetText();
	if (strFileName.GetLength() == 0) {
		MessageBox(GetHWND(), "���������Ϊ���ļ���", "����", 0);
		return;
	}

	BOOL  bFind = FALSE;
	int nCnt = m_lst->GetCount();
	for (int i = 0; i < nCnt; i++) {
		CListTextElementUI* pItem = (CListTextElementUI*)m_lst->GetItemAt(i);
		if (strFileName == pItem->GetText(0)) {
			bFind = TRUE;
			break;
		}
	}

	// ���Ҫ���������ļ�
	if (bFind) {
		CDuiString  strTemp;
		strTemp.Format("ȷ��Ҫ����%s�ļ���?", (const char *)strFileName);
		if (IDNO == MessageBox(GetHWND(), strTemp, "���Ϊ", MB_YESNO | MB_DEFBUTTON2)) {
			return;
		}
	}

	CDuiString strPath = DuiLib::CPaintManagerUI::GetInstancePath();
	strPath += m_strFoldName;
	
	int ret = 0;
	// ����Ŀ¼(bIgnoreExisted, �������Ŀ¼�Ƿ���Ըô���)
	ret = LmnCreateFolder(strPath);
	if (0 != ret) {
		MessageBox(GetHWND(), "�����ļ�ʧ��!", "����", 0);
		return;
	}

	CDuiString strFilePath = strPath + "\\" + strFileName;
	FILE * fp = fopen(strFilePath, "wb");
	if (0 == fp) {
		MessageBox(GetHWND(), "�����ļ�ʧ��!", "����", 0);
		return;
	}

	CDuiString  strChecked;
	char buf[8192];
	for ( int i = 0; i < MAX_TEMP_ITEMS_CNT; i++ ) {
		int nTemp      = m_temp_items[i]->GetTemp();
		int nDutyCycle = m_temp_items[i]->GetDutyCycle();
		if ( m_temp_items[i]->IsChecked() ) {
			if (strChecked.GetLength() == 0) {
				strChecked.Format("%d", i);
			}
			else {
				CDuiString strTemp;
				strTemp.Format("%d", i);
				strChecked += ",";
				strChecked += strTemp;
			}				
		}

		SNPRINTF( buf, sizeof(buf), "%.1f\t%d\r\n", nTemp / 100.0f, nDutyCycle);
		int nSize = strlen(buf);
		fwrite(buf, 1, nSize, fp);
	}

	fwrite("\r\n", 1, 2, fp);
	fwrite(strChecked, 1, strChecked.GetLength(), fp);
	fwrite("\r\n", 1, 2, fp);

	fclose(fp);

	m_strFileName = strFileName;
	this->PostMessage(WM_CLOSE);
}