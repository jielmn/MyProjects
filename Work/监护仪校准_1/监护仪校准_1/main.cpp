// win32_1.cpp : 定义应用程序的入口点。
//

#ifdef _DEBUG
#include "vld.h"
#endif

#include "Windows.h"
#include "main.h"
#include "business.h"
#include "resource.h"
#include "SaveAsDlg.h"
#include "LmnSerialPort.h"

CDuiFrameWnd::CDuiFrameWnd() : m_callback(&m_PaintManager) {
	m_nCurColumns = 0;
	m_nCurRows = 0;
	m_layMain = 0;
	memset(m_layColumns, 0, sizeof(m_layColumns));
	memset(m_temp_items, 0, sizeof(m_temp_items));

	m_btnSave = 0;
	m_btnSaveAs = 0;
	m_bBusy = FALSE;
	m_btnAdjustAll = 0;
	m_btnDiff = 0;
	m_waiting_bar = 0;
	m_nHighlightIndex = -1;
	m_bItemsAdded = FALSE;
	m_tabs = 0;
	m_bAdjustTagSelected = FALSE;

	m_opSingle = 0;
	m_opBroadcast = 0;
	m_opAdjustMode = 0;
	m_opTempMode = 0;
	m_edReaderAddr = 0;
	m_btnOk1 = 0;

	m_edReaderAddr_1 = 0;
	m_edReaderChannel_1 = 0;
	m_edStationAddr_1 = 0;
	m_edStationChannel_1 = 0;
	m_edQueryStationChannel_1 = 0;
	m_btnOk2 = 0;
	m_btnOk3 = 0;
	m_btnOk4 = 0;
	m_edReaderSn_1 = 0;
	m_btnOk5 = 0;
}

CDuiFrameWnd::~CDuiFrameWnd() {

}

void CDuiFrameWnd::OnFinalMessage(HWND hWnd) {
	if (!m_bItemsAdded) {
		for (int i = 0; i < MAX_TEMP_ITEMS_CNT; i++) {
			delete m_temp_items[i]; 
		}
	}
}


void  CDuiFrameWnd::InitWindow() {
	g_data.m_hWnd = m_hWnd;
	PostMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0);

	m_tabs = static_cast<DuiLib::CTabLayoutUI*>(m_PaintManager.FindControl("switch"));
	m_cmbComPorts = static_cast<DuiLib::CComboUI*>(m_PaintManager.FindControl("cmComPort"));
	m_cmbMachineType = static_cast<DuiLib::CComboUI*>(m_PaintManager.FindControl("cmMachine"));
	m_cmbFiles = static_cast<DuiLib::CComboUI*>(m_PaintManager.FindControl("cmFiles"));
	m_layMain = static_cast<DuiLib::CHorizontalLayoutUI*>(m_PaintManager.FindControl("layMain"));
	m_btnSave = static_cast<DuiLib::CButtonUI*>(m_PaintManager.FindControl("btnSave"));
	m_btnSaveAs = static_cast<DuiLib::CButtonUI*>(m_PaintManager.FindControl("btnSaveAs"));
	m_btnAdjustAll = static_cast<DuiLib::CButtonUI*>(m_PaintManager.FindControl("btnAdjustAll"));
	m_btnDiff = static_cast<DuiLib::CButtonUI*>(m_PaintManager.FindControl("btnDiff"));
	m_waiting_bar = static_cast<CProgressUI*>(m_PaintManager.FindControl("waiting")); 
	m_waiting_bar->SetVisible(false);  

	m_opSingle = static_cast<COptionUI*>(m_PaintManager.FindControl("rdSingle"));
	m_opBroadcast = static_cast<COptionUI*>(m_PaintManager.FindControl("rdBroadcast"));
	m_opAdjustMode = static_cast<COptionUI*>(m_PaintManager.FindControl("rdAdjust"));
	m_opTempMode = static_cast<COptionUI*>(m_PaintManager.FindControl("rdTemp"));
	m_edReaderAddr = static_cast<CEditUI*>(m_PaintManager.FindControl("edReaderAddr"));
	m_btnOk1 = static_cast<CButtonUI*>(m_PaintManager.FindControl("btnOk1"));

	m_opSingle->Selected(true, false);
	m_opAdjustMode->Selected(true, false);

	m_edReaderAddr_1 = static_cast<CEditUI*>(m_PaintManager.FindControl("edAddr"));
	m_edReaderChannel_1 = static_cast<CEditUI*>(m_PaintManager.FindControl("edChannel"));
	m_edStationAddr_1 = static_cast<CEditUI*>(m_PaintManager.FindControl("edAddr_1"));
	m_edStationChannel_1 = static_cast<CEditUI*>(m_PaintManager.FindControl("edChannel_1"));
	m_edQueryStationChannel_1 = static_cast<CEditUI*>(m_PaintManager.FindControl("edChannel_2"));
	m_btnOk2 = static_cast<CButtonUI*>(m_PaintManager.FindControl("btnOk2"));
	m_btnOk3 = static_cast<CButtonUI*>(m_PaintManager.FindControl("btnOk3"));
	m_btnOk4 = static_cast<CButtonUI*>(m_PaintManager.FindControl("btnOk4"));
	m_edReaderSn_1 = static_cast<CEditUI*>(m_PaintManager.FindControl("edSN"));
	m_btnOk5 = static_cast<CButtonUI*>(m_PaintManager.FindControl("btnOk5"));

	for ( int i = 0; i < MAX_COLUMNS_CNT; i++ ) {
		m_layColumns[i] = new CVerticalLayoutUI;
		m_layColumns[i]->SetVisible(false);
		m_layMain->Add(m_layColumns[i]);
	}
	m_layMain->OnSize += MakeDelegate(this, &CDuiFrameWnd::OnMainSize);	

	for (int i = 0; i < MAX_TEMP_ITEMS_CNT; i++) {
		m_temp_items[i] = new CTempItemUI;
		m_temp_items[i]->SetTemp(FIRST_TEMP + i * 10);
		m_temp_items[i]->SetTag(i);
	}

#if !NEW_VERSION_FLAG
	for (int i = MachineType_MR; i <= MachineType_GE2; i++) {
#else
	for (int i = MachineType_MR; i <= MachineType(MachineType_GE2 + g_data.m_vOtherMachineType.size()); i++) {
#endif
		CListLabelElementUI * pElement = new CListLabelElementUI;
		pElement->SetText(GetMachineTypeStr((MachineType)i));
		pElement->SetTag(i);
		m_cmbMachineType->Add(pElement);
	}
	m_cmbMachineType->SelectItem(0);

	CheckDevice();     
	WindowImplBase::InitWindow();

	m_PaintManager.AddPreMessageFilter(&m_filter);
}
 
CControlUI * CDuiFrameWnd::CreateControl(LPCTSTR pstrClass) {
	DuiLib::CDialogBuilder builder;
	DuiLib::CDuiString  strText;
	DuiLib::CControlUI * pUI = 0;

	if (0 == strcmp(pstrClass, "Adjust") || 0 == strcmp(pstrClass, "Setting") || 0 == strcmp(pstrClass, "Mode") ) {
		strText.Format("%s.xml", pstrClass); 
		pUI = builder.Create((const char *)strText, (UINT)0, &m_callback, &m_PaintManager); 
		return pUI;
	}
	return WindowImplBase::CreateControl(pstrClass);  
}

void CDuiFrameWnd::Notify(TNotifyUI& msg) {
	CDuiString  name = msg.pSender->GetName();
	if (msg.sType == "windowinit") {
		OnWindowInit();
	}
	else if (msg.sType == "itemselect") {
		if (name == "cmFiles") {
			OnFileChanged();
		}
		else if (name == "cmMachine") {
			OnMachineChanged();
		}
	}
	else if (msg.sType == "adjust") {
		int nIndex = msg.pSender->GetTag();
		OnAdjust(nIndex);
	}
	else if (msg.sType == "click") {
		if (name == "btnDiff") {
			OnDiff();
		}
		else if (name == "btnSaveAs") {
			OnSaveAs();
		}
		else if (name == "btnSave") {
			OnSave();
		}
		else if (name == "btnAdjustAll") {
			OnAdjustAll();
		}
		else if (name == "btnOk1") {
			OnSetWorkMode();
		}
		else if (name == "btnOk2") {
			OnSetReader();
		}
		else if (name == "btnOk3") {
			OnSetStation();
		}
		else if (name == "btnOk4") {
			OnQueryStation();
		}
		else if (name == "btnOk5") {
			OnSetReaderSn();
		}
	}
	else if (msg.sType == "myselected") {
		if (m_nHighlightIndex >= 0) {
			m_temp_items[m_nHighlightIndex]->SetHighlight(FALSE);
		}
		m_nHighlightIndex = msg.pSender->GetTag();
		m_temp_items[m_nHighlightIndex]->SetHighlight(TRUE);
	}
	else if (msg.sType == "myunselected") {
		int nIndex = msg.pSender->GetTag();
		m_temp_items[nIndex]->SetHighlight(FALSE);
		if (nIndex == m_nHighlightIndex) {
			m_nHighlightIndex = -1;
		}
	}
	else if (msg.sType == "selectchanged") {
		if (name == _T("opn_mode")) {
			m_bAdjustTagSelected = FALSE;
			m_tabs->SelectItem(0);
		}
		else if (name == _T("opn_adjust")) {
			m_bAdjustTagSelected = TRUE;
			m_tabs->SelectItem(1); 			
		}
		else if (name == _T("opn_setting")) {
			m_bAdjustTagSelected = FALSE;
			m_tabs->SelectItem(2);
		}
		else if (name == "rdSingle") {
			m_edReaderAddr->SetText("");
			m_edReaderAddr->SetReadOnly(false);
			m_edReaderAddr->SetBkColor(0xFFFFFFFF);
			m_edReaderAddr->SetNativeEditBkColor(0xFFFFFFFF);
		}
		else if (name == "rdBroadcast") {
			m_edReaderAddr->SetText("");
			m_edReaderAddr->SetReadOnly(true);
			m_edReaderAddr->SetBkColor(0xFFCCCCCC);
			m_edReaderAddr->SetNativeEditBkColor(0xFFCCCCCC);
		}
	}

	WindowImplBase::Notify(msg);
}

LRESULT CDuiFrameWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	CDuiString strText;

	if (uMsg == UM_WRONG_DATA) {
		MessageBox(m_hWnd, "数据文件格式不对", "错误", 0);
	}
	else if (uMsg == UM_ADJUST_RET) {
		SetBusy(FALSE);
		if (wParam != 0) {
			MessageBox(m_hWnd, "单个校准失败!", "失败", 0);
		}		
	}
	else if (uMsg == UM_ADJUST_ALL_RET) {
		SetBusy(FALSE, TRUE);
		if (wParam != 0) {
			MessageBox(m_hWnd, "数据上传失败!", "失败", 0); 
		}		
	}
	else if (uMsg == UM_ADJUST_ALL_PROGRESS) {
		m_waiting_bar->SetValue(wParam + 1);
		
		strText.Format("%d%%", (int)round((double)(wParam + 1) / MAX_TEMP_ITEMS_CNT * 100.0) );
		m_waiting_bar->SetText(strText);
	}
	else if (uMsg == UM_SET_WORK_MODE_RET) {
		SetBusy(FALSE);
	}
	else if (uMsg == UM_SET_READER_RET) {
		SetBusy(FALSE);
		if (wParam != 0) {
			MessageBox(m_hWnd, "设置Reader失败!", "失败", 0);
		}
	}
	else if (uMsg == UM_SET_READER_SN_RET) {
		SetBusy(FALSE);
	}
	else if (uMsg == UM_SET_STATION_RET) {
		SetBusy(FALSE);
		if (wParam != 0) {
			MessageBox(m_hWnd, "设置基站失败!", "失败", 0);
		}
	}
	else if (uMsg == UM_QUERY_STATION_RET) {
		SetBusy(FALSE);
		if (wParam != 0) {
			m_edQueryStationChannel_1->SetText("");
			MessageBox(m_hWnd, "查询基站失败!", "失败", 0);
		}
		else {
			strText.Format("%d", (int)lParam);
			m_edQueryStationChannel_1->SetText(strText);
		}
	}
	else if (uMsg == WM_DEVICECHANGE) {
		CheckDevice();
	}
	else if (uMsg == WM_KEYDOWN) {
		if ( m_bAdjustTagSelected ) {
			if (wParam == VK_DOWN) {
				if (m_nHighlightIndex < 0) {
					m_nHighlightIndex = 0;
					m_temp_items[m_nHighlightIndex]->SetHighlight(TRUE);
				}
				else {
					if (m_nHighlightIndex < MAX_TEMP_ITEMS_CNT - 1) {
						m_nHighlightIndex++;
						m_temp_items[m_nHighlightIndex]->SetHighlight(TRUE);
						m_temp_items[m_nHighlightIndex - 1]->SetHighlight(FALSE);
					}
				}
			}
			else if (wParam == VK_UP) {
				if (m_nHighlightIndex < 0) {
					m_nHighlightIndex = MAX_TEMP_ITEMS_CNT - 1;
					m_temp_items[m_nHighlightIndex]->SetHighlight(TRUE);
				}
				else {
					if (m_nHighlightIndex > 0) {
						m_nHighlightIndex--;
						m_temp_items[m_nHighlightIndex]->SetHighlight(TRUE);
						m_temp_items[m_nHighlightIndex + 1]->SetHighlight(FALSE);
					}
				}
			}
			else if (wParam == VK_LEFT) {
				if (m_nHighlightIndex >= 0) {
					int nRows = m_nCurRows;
					int nColumnIndex = m_nHighlightIndex / nRows;
					if (nColumnIndex > 0) {
						m_nHighlightIndex -= nRows;
						m_temp_items[m_nHighlightIndex]->SetHighlight(TRUE);
						m_temp_items[m_nHighlightIndex + nRows]->SetHighlight(FALSE);
					}
				}
			}
			else if (wParam == VK_RIGHT) {
				if (m_nHighlightIndex >= 0) {
					int nRows = m_nCurRows;
					int nColumnIndex = m_nHighlightIndex / nRows;
					if (m_nHighlightIndex + nRows < MAX_TEMP_ITEMS_CNT) {
						m_nHighlightIndex += nRows;
						m_temp_items[m_nHighlightIndex]->SetHighlight(TRUE);
						m_temp_items[m_nHighlightIndex - nRows]->SetHighlight(FALSE);
					}
				}
			}
			else if (wParam == 'S') {
				if (m_nHighlightIndex >= 0 && !m_bBusy) {
					m_temp_items[m_nHighlightIndex]->SetSelected();
				}
			}
			else if (wParam == VK_RETURN) {
				if (m_nHighlightIndex >= 0 && !m_bBusy) {
					OnAdjust(m_nHighlightIndex);
				}
			}
			else if (wParam == VK_ADD || wParam == VK_OEM_PLUS) {
				if (m_nHighlightIndex >= 0 && !m_bBusy) {
					m_temp_items[m_nHighlightIndex]->Upper();
				}
			}
			else if (wParam == VK_SUBTRACT || wParam == VK_OEM_MINUS) {
				if (m_nHighlightIndex >= 0 && !m_bBusy) {
					m_temp_items[m_nHighlightIndex]->Downer();
				}
			}
		}		
	}
	return WindowImplBase::HandleMessage(uMsg,wParam,lParam);
}

void  CDuiFrameWnd::OnWindowInit() {
	
}

LRESULT CDuiFrameWnd::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
	return WindowImplBase::OnSize(uMsg, wParam, lParam, bHandled);
}

bool CDuiFrameWnd::OnMainSize(void * pParam) {
	RECT rcMain = m_layMain->GetPos();
	int nWidth = rcMain.right - rcMain.left - 2;
	int nColumn = nWidth / CTempItemUI::WIDTH;
	if (nColumn > MAX_COLUMNS_CNT)
		nColumn = MAX_COLUMNS_CNT;
	else if (nColumn <= 0)
		nColumn = 1;

	// 如果列数不变
	if (m_nCurColumns == nColumn)
		return true;

	for (int i = 0; i < MAX_COLUMNS_CNT; i++) {
		int nCnt = m_layColumns[i]->GetCount();
		for (int j = 0; j < nCnt; j++) {
			m_layColumns[i]->RemoveAt( 0, true );
		}
		m_layColumns[i]->SetVisible(false);
	}
	
	int nRows = (MAX_TEMP_ITEMS_CNT - 1) / nColumn + 1;
	int nResume = nRows % 10;
	// 如果不是整数10行
	if (0 != nResume ) {
		nRows += 10 - nResume;
	}
	// 重新计算列数
	nColumn = ( MAX_TEMP_ITEMS_CNT - 1 ) / nRows + 1;
	m_nCurColumns = nColumn;
	m_nCurRows = nRows;

	int nItemIndex = 0;
	for ( int i = 0; i < m_nCurColumns; i++ ) {
		m_layColumns[i]->SetVisible(true);   
		for ( int j = nRows * i; j < (i + 1) * nRows && nItemIndex < MAX_TEMP_ITEMS_CNT; j++, nItemIndex++ ) {
			m_layColumns[i]->Add(m_temp_items[j]);
		}
	}

	m_layMain->SetFixedHeight(nRows * CTempItemUI::HEIGHT + 2); 

	m_bItemsAdded = TRUE; 
	return true;
}   

// 检查硬件(串口)
void  CDuiFrameWnd::CheckDevice() {
	m_cmbComPorts->RemoveAll();

	//std::vector<std::string> vComPorts;
	//EnumPortsWdm(vComPorts);
	std::map<int, std::string> mComPorts;
	GetAllSerialPorts(mComPorts);

	BOOL  bFindReader = FALSE;
	int   nFindeIndex = -1;

	std::map<int, std::string>::iterator it;
	int i = 0;
	for (it = mComPorts.begin(); it != mComPorts.end(); it++, i++) {
		std::string & s = it->second;

		CListLabelElementUI * pElement = new CListLabelElementUI;
		pElement->SetText(s.c_str());
		m_cmbComPorts->Add(pElement);

		int nComPort = it->first;
		pElement->SetTag(nComPort);

		if (!bFindReader) {
			char tmp[256];
			Str2Lower(s.c_str(), tmp, sizeof(tmp));
			if (0 != strstr(tmp, "usb-serial ch340")) {
				bFindReader = TRUE;
				nFindeIndex = i;
			}
		}
	}

	if (m_cmbComPorts->GetCount() > 0) {
		if (nFindeIndex >= 0) {
			m_cmbComPorts->SelectItem(nFindeIndex,false,false);
		}
		else {
			m_cmbComPorts->SelectItem(0,false,false);
		}
	}
}

void  CDuiFrameWnd::OnFileChanged() {
	int nFileSel     = m_cmbFiles->GetCurSel();
	int nMachineSel  = m_cmbMachineType->GetCurSel();
	assert(nFileSel >= 0 && nMachineSel >= 0);
	int nMachine = m_cmbMachineType->GetItemAt(nMachineSel)->GetTag();

	// 如果是标准类型
	if (nFileSel == 0) {
		for (int i = 0; i < MAX_TEMP_ITEMS_CNT; i++) {
			m_temp_items[i]->SetDutyCycle( g_data.m_standard_items[nMachine][i].m_nDutyCycle );
		}

		for (int i = 0; i < MAX_TEMP_ITEMS_CNT; i++) {
			m_temp_items[i]->SetChecked(FALSE);
		}

		m_btnSave->SetEnabled(false);
	}
	// 不是标准类型
	else {
		BOOL bRet = LoadFileData( m_temp_items, MAX_TEMP_ITEMS_CNT, GetMachineType(), GetTempDataFileName() );
		m_btnSave->SetEnabled(true);
		if (!bRet) {
			this->PostMessage(UM_WRONG_DATA);			
		}
	}	
}

void  CDuiFrameWnd::OnMachineChanged() {
	// 清空
	m_cmbFiles->RemoveAll();

	// 添加标准
	CListLabelElementUI * pElement = new CListLabelElementUI;	
	m_cmbFiles->Add(pElement);
	pElement->SetText("标准");

	// 添加其他
	int nMachineSel = m_cmbMachineType->GetCurSel();
	assert(nMachineSel >= 0);
	int nMachine = m_cmbMachineType->GetItemAt(nMachineSel)->GetTag();
	CDuiString strFoldName = GetMachineTypeStr((MachineType)nMachine);

	WIN32_FIND_DATA FindData;
	HANDLE hFind;
	CDuiString strCurPath = DuiLib::CPaintManagerUI::GetInstancePath();

	char szFilePathName[256];
	SNPRINTF(szFilePathName, sizeof(szFilePathName), "%s%s\\*.txt",
		(const char *)strCurPath, (const char *)strFoldName);

	// 列出已有的文件
	hFind = FindFirstFile(szFilePathName, &FindData);
	if (hFind != INVALID_HANDLE_VALUE) {
		do {			
			char szFile[256];
			ParseFileName(FindData.cFileName, 0, 0,szFile, sizeof(szFile));

			pElement = new CListLabelElementUI;
			m_cmbFiles->Add(pElement);
			pElement->SetText(szFile);

			if (!::FindNextFile(hFind, &FindData)) {
				break;
			}
		} while (TRUE);
		FindClose(hFind);
	}
	
	m_cmbFiles->SelectItem(0);
}

void  CDuiFrameWnd::OnAdjust(int nIndex) {	
	assert(nIndex >= 0 && nIndex < MAX_TEMP_ITEMS_CNT);
	CBusiness::GetInstance()->AdjustAsyn(GetComPort(), m_temp_items[nIndex]->GetTemp(), 
		m_temp_items[nIndex]->GetDutyCycle());
	SetBusy();
}

void  CDuiFrameWnd::OnDiff() {
	int nCheckedCnt = 0;
	for (int i = 0; i < MAX_TEMP_ITEMS_CNT; i++) {
		if (m_temp_items[i]->IsChecked()) {
			nCheckedCnt++;
		}
	}

	if (nCheckedCnt < 2) {
		MessageBox(GetHWND(), "红色的基准点必须两个以上", "错误", 0);
		return;
	}

	// 开始计算线性差值
	// 分成 nCheckedCnt + 1 段，但是我们只要计算出中间的 nCheckedCnt - 1 段即可
	float * arrDiffs = new float[nCheckedCnt - 1];
	memset(arrDiffs, 0, sizeof(arrDiffs));

	int nLastCheckIndex  = -1;
	int nSegmentIndex    = 0;
	int nFirstCheckIndex = -1;
	for (int i = 0; i < MAX_TEMP_ITEMS_CNT; i++) {
		if ( m_temp_items[i]->IsChecked() ) {
			// 计算diff
			if ( nLastCheckIndex != -1 ) {
				int m = m_temp_items[i]->GetDutyCycle();
				int n = m_temp_items[nLastCheckIndex]->GetDutyCycle();
				assert(nSegmentIndex < nCheckedCnt - 1);
				arrDiffs[nSegmentIndex] = (float)(m - n) / (float)(i - nLastCheckIndex);
				for ( int j = nLastCheckIndex + 1; j < i; j++ ) {
					int w = (int)round(n + arrDiffs[nSegmentIndex] * (j - nLastCheckIndex));
					m_temp_items[j]->SetDutyCycle(w);
				}
				nSegmentIndex++;
			}
			else {
				nFirstCheckIndex = i;
			}
			nLastCheckIndex = i;
		}
	}

	// 计算头和尾
	int m = m_temp_items[nFirstCheckIndex]->GetDutyCycle();
	int n = m_temp_items[nLastCheckIndex]->GetDutyCycle();
	for ( int i = 0; i < nFirstCheckIndex; i++ ) {
		int w = (int)round(m - arrDiffs[0] * (nFirstCheckIndex - i));
		m_temp_items[i]->SetDutyCycle(w);
	}

	for (int i = nLastCheckIndex + 1; i < MAX_TEMP_ITEMS_CNT; i++) {
		int w = (int)round(n + arrDiffs[nCheckedCnt - 2] * (i - nLastCheckIndex));
		m_temp_items[i]->SetDutyCycle(w);
	}

	delete[] arrDiffs;
}

void  CDuiFrameWnd::OnSaveAs() {
	int nFileSel = m_cmbFiles->GetCurSel();
	int nMachineSel = m_cmbMachineType->GetCurSel();
	assert(nFileSel >= 0 && nMachineSel >= 0);
	int nMachine = m_cmbMachineType->GetItemAt(nMachineSel)->GetTag();

	CSaveAsDlg * pSaveAsDlg = new CSaveAsDlg;
	pSaveAsDlg->m_strFoldName = GetMachineTypeStr( (MachineType)nMachine );
	if (nFileSel == 0) {
		pSaveAsDlg->m_strFileName = "";
	}
	else {
		CListLabelElementUI * pElement = (CListLabelElementUI *)m_cmbFiles->GetItemAt(nFileSel);
		pSaveAsDlg->m_strFileName = pElement->GetText();
	}
	CDuiString  strOldFileName = pSaveAsDlg->m_strFileName; 
	memcpy(pSaveAsDlg->m_temp_items, m_temp_items, sizeof(m_temp_items));

	pSaveAsDlg->Create(this->m_hWnd, _T("另存为"), UI_WNDSTYLE_FRAME | WS_POPUP, NULL, 0, 0, 0, 0);
	pSaveAsDlg->CenterWindow();
	int ret = pSaveAsDlg->ShowModal();

	// 如果不是click ok
	if (0 != ret) {
		delete pSaveAsDlg;
		return;
	}

	// 如果文件名另存为成功
	if (pSaveAsDlg->m_strFileName != strOldFileName) {
		CListLabelElementUI * pElement = new CListLabelElementUI;
		pElement->SetText(pSaveAsDlg->m_strFileName);
		m_cmbFiles->Add(pElement);
		int nCnt = m_cmbFiles->GetCount();
		m_cmbFiles->SelectItem(nCnt - 1, false, false);
		m_btnSave->SetEnabled(true);
	}

	delete pSaveAsDlg;
	return;
}

void  CDuiFrameWnd::OnSave() {
	CDuiString  strFileName = GetTempDataFileName();
	CDuiString strPath = DuiLib::CPaintManagerUI::GetInstancePath();
	MachineType eType = GetMachineType();
	strPath += GetMachineTypeStr(eType);

	CDuiString strFilePath = strPath + "\\" + strFileName + ".txt";
	FILE * fp = fopen(strFilePath, "wb");
	if (0 == fp) {
		MessageBox(GetHWND(), "保存失败!", "错误", 0);
		return;
	}

	CDuiString  strChecked;
	char buf[8192];
	for (int i = 0; i < MAX_TEMP_ITEMS_CNT; i++) {
		int nTemp = m_temp_items[i]->GetTemp();
		int nDutyCycle = m_temp_items[i]->GetDutyCycle();
		if (m_temp_items[i]->IsChecked()) {
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

		SNPRINTF(buf, sizeof(buf), "%.1f\t%d\r\n", nTemp / 100.0f, nDutyCycle);
		int nSize = strlen(buf);
		fwrite(buf, 1, nSize, fp);
	}

	fwrite("\r\n", 1, 2, fp);
	fwrite(strChecked, 1, strChecked.GetLength(), fp);
	fwrite("\r\n", 1, 2, fp);

	fclose(fp);
}

MachineType CDuiFrameWnd::GetMachineType() {
	int nMachineSel = m_cmbMachineType->GetCurSel();
	assert(nMachineSel >= 0);
	int nMachine = m_cmbMachineType->GetItemAt(nMachineSel)->GetTag();
	return (MachineType)nMachine;
}

CDuiString CDuiFrameWnd::GetTempDataFileName() {
	int nFileSel = m_cmbFiles->GetCurSel();
	assert(nFileSel >= 0);
	if (0 == nFileSel)
		return "";

	CListLabelElementUI * pElement = (CListLabelElementUI *)m_cmbFiles->GetItemAt(nFileSel);
	return pElement->GetText();
}

int  CDuiFrameWnd::GetComPort() {
	int nSel = m_cmbComPorts->GetCurSel();
	if (nSel < 0)
		return -1;

	return m_cmbComPorts->GetItemAt(nSel)->GetTag();
}

void  CDuiFrameWnd::SetBusy(BOOL bBusy /*= TRUE*/, BOOL bWaitingBar /*= FALSE*/) {
	m_bBusy = bBusy;
	if ( m_bBusy ) {
		m_cmbComPorts->SetEnabled(false);
		m_cmbMachineType->SetEnabled(false);
		m_cmbFiles->SetEnabled(false);
		m_btnSaveAs->SetEnabled(false);
		m_btnSave->SetEnabled(false);
		m_btnAdjustAll->SetEnabled(false);
		m_btnDiff->SetEnabled(false);
		for (int i = 0; i < MAX_TEMP_ITEMS_CNT; i++) {
			m_temp_items[i]->SetMouseChildEnabled(false);
		}
		if (bWaitingBar) {
			m_waiting_bar->SetVisible(true);
			m_waiting_bar->SetValue(0);
			m_waiting_bar->SetText("0%"); 
		}		

		m_opSingle->SetEnabled(false);
		m_opBroadcast->SetEnabled(false);
		m_opAdjustMode->SetEnabled(false);
		m_opTempMode->SetEnabled(false);
		m_edReaderAddr->SetEnabled(false);
		m_btnOk1->SetEnabled(false);

		m_edReaderAddr_1->SetEnabled(false);
		m_edReaderChannel_1->SetEnabled(false);
		m_edStationAddr_1->SetEnabled(false);
		m_edStationChannel_1->SetEnabled(false);
		m_edQueryStationChannel_1->SetEnabled(false);
		m_btnOk2->SetEnabled(false);
		m_btnOk3->SetEnabled(false);
		m_btnOk4->SetEnabled(false);
		m_edReaderSn_1->SetEnabled(false);
		m_btnOk5->SetEnabled(false);

	}
	else {
		m_cmbComPorts->SetEnabled(true);
		m_cmbMachineType->SetEnabled(true);
		m_cmbFiles->SetEnabled(true);
		m_btnSaveAs->SetEnabled(true);
		if ( m_cmbFiles->GetCurSel() > 0 )
			m_btnSave->SetEnabled(true);
		m_btnAdjustAll->SetEnabled(true);
		m_btnDiff->SetEnabled(true);
		for (int i = 0; i < MAX_TEMP_ITEMS_CNT; i++) {
			m_temp_items[i]->SetMouseChildEnabled(true);  
		}		
		if (bWaitingBar) {
			m_waiting_bar->SetVisible(false);
		}	

		m_opSingle->SetEnabled(true);
		m_opBroadcast->SetEnabled(true);
		m_opAdjustMode->SetEnabled(true);
		m_opTempMode->SetEnabled(true);
		m_edReaderAddr->SetEnabled(true);
		m_btnOk1->SetEnabled(true);

		m_edReaderAddr_1->SetEnabled(true);
		m_edReaderChannel_1->SetEnabled(true);
		m_edStationAddr_1->SetEnabled(true);
		m_edStationChannel_1->SetEnabled(true);
		m_edQueryStationChannel_1->SetEnabled(true);
		m_btnOk2->SetEnabled(true);
		m_btnOk3->SetEnabled(true);
		m_btnOk4->SetEnabled(true);
		m_edReaderSn_1->SetEnabled(true);
		m_btnOk5->SetEnabled(true);
	}
}

void  CDuiFrameWnd::OnAdjustAll() {
	TempAdjust items[MAX_TEMP_ITEMS_CNT];
	memset(items, 0, sizeof(items));

	MachineType eType = GetMachineType();
	int nType = (int)eType;

	for (int i = 0; i < MAX_TEMP_ITEMS_CNT; i++) {
		items[i].m_nTemp   = m_temp_items[i]->GetTemp();
#if !NEW_VERSION_FLAG
		items[i].m_nOffset = m_temp_items[i]->GetDutyCycle() - g_data.m_standard_items[nType][i].m_nDutyCycle;
#else
		items[i].m_nOffset = m_temp_items[i]->GetDutyCycle();
#endif
	}

	CBusiness::GetInstance()->AdjustAllAsyn(GetComPort(), GetMachineType(), 
		items, MAX_TEMP_ITEMS_CNT);
	SetBusy(TRUE, TRUE);
}

void CDuiFrameWnd::OnSetWorkMode() {
	CDuiString  strText;
	int nAddr = 0;

	if (m_opSingle->IsSelected()) {
		strText = m_edReaderAddr->GetText();
		sscanf(strText, " %d", &nAddr);
	}
	else {
		nAddr = 0xFFFF;
	}

	WorkMode eMode = WorkMode_Adjust;
	if (m_opTempMode->IsSelected()) {
		eMode = WorkMode_Temp;
	}

	CBusiness::GetInstance()->SetReaderModeAsyn(GetComPort(), (WORD)nAddr, eMode);
	SetBusy(TRUE, FALSE);
}

void  CDuiFrameWnd::OnSetReader() {
	CDuiString  strText;
	int   nAddr     = 0;
	int   nChannel = 0;

	strText = m_edReaderAddr_1->GetText();
	sscanf(strText, " %d", &nAddr);

	strText = m_edReaderChannel_1->GetText();
	sscanf(strText, " %d", &nChannel);

	CBusiness::GetInstance()->SetReaderAsyn( GetComPort(), (WORD)nAddr, (BYTE)nChannel );
	SetBusy(TRUE, FALSE);
}

void  CDuiFrameWnd::OnSetStation() {
	CDuiString  strText;
	int   nAddr = 0;
	int   nChannel = 0;

	strText = m_edStationAddr_1->GetText();
	sscanf(strText, " %d", &nAddr);

	strText = m_edStationChannel_1->GetText();
	sscanf(strText, " %d", &nChannel);

	CBusiness::GetInstance()->SetStationAsyn(GetComPort(), (WORD)nAddr, (BYTE)nChannel);
	SetBusy(TRUE, FALSE);
}

void  CDuiFrameWnd::OnQueryStation() {
	CBusiness::GetInstance()->QueryStationAsyn(GetComPort());
	SetBusy(TRUE, FALSE);
}

void  CDuiFrameWnd::OnSetReaderSn() {
	CDuiString  strText;
	int nSn = 0;

	strText = m_edReaderSn_1->GetText();
	sscanf(strText, " %d", &nSn);

	CBusiness::GetInstance()->SetReaderSnAsyn( GetComPort(), (DWORD)nSn );
	SetBusy(TRUE, FALSE);
}
    
                            
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	DWORD  dwPort = 1601;
	JTelSvrStart((unsigned short)dwPort, 3);

	LmnToolkits::ThreadManager::GetInstance();
	int ret = CBusiness::GetInstance()->Init();
	if (0 != ret) {
		MessageBox(0, "初始化失败!", "错误", 0);
		return -1;
	}
	g_data.m_log->Output(ILog::LOG_SEVERITY_INFO, "main begin.\n");

	CPaintManagerUI::SetInstance(hInstance);
	HICON hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));

	CDuiFrameWnd * duiFrame = new CDuiFrameWnd;
	duiFrame->Create(NULL, _T("DUIWnd"), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
	duiFrame->CenterWindow();

	// 设置icon
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


