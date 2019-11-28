#include "GridUI.h"
#include "business.h"
#include "LmnExcel.h"

CGridUI::CGridUI() :m_callback(m_pManager) {
	m_bInited = FALSE;
	m_tabs = 0;

	m_btnBedNo = 0;
	m_btnBedNoM = 0;
	m_dwBedNo = 0;

	m_lblReaderNo = 0;

	m_layReaders = 0;
	memset(m_readers, 0, sizeof(m_readers));

	m_cstModeBtn = 0;
	m_hand_reader = 0;
	m_CurReaderState = 0;

	memset(m_aLastTemp, 0, sizeof(m_aLastTemp));
	m_dwSelSurReaderIndex = 0;

	m_cstImgLabel = 0;
	memset( &m_HandLastTemp, 0, sizeof(TempItem) );

	m_lblElapsed = 0;
	m_cstImg = 0;
	m_cstPatientName = 0;
	m_cstPatientNameM = 0;

	m_layHead = 0;
	m_bedImg = 0;
	m_patientImg = 0;
	m_sperate = 0;
}

CGridUI::~CGridUI() {
	for ( DWORD i = 0; i < MAX_READERS_PER_GRID; i++ ) {
		ClearVector(m_vTemp[i]);
	}
	ClearVector(m_vHandTemp);
}

LPCTSTR CGridUI::GetClass() const {
	return "GridUI";
}

void CGridUI::DoInit() {
	if (m_bInited) {
		return;
	}

	CDialogBuilder builder;
	if ( m_callback.m_pManager == 0 )
		m_callback.m_pManager = m_pManager;
	CContainerUI* pChildWindow = static_cast<CHorizontalLayoutUI*>(builder.Create(GRID_XML_FILE, (UINT)0, &m_callback, m_pManager));
	if (pChildWindow) {
		this->Add(pChildWindow);
		m_pManager->AddNotifier(this);
	}
	else {
		this->RemoveAll();
		m_bInited = TRUE;
		return;
	}

	CDuiString  strText;

	m_tabs = static_cast<DuiLib::CTabLayoutUI  *>(m_pManager->FindControl(GRID_TABS));

	m_layHead = static_cast<DuiLib::CHorizontalLayoutUI  *>(m_pManager->FindControl(GRID_HEAD_LAYOUT));
	m_bedImg = m_pManager->FindControl("bedImg");
	m_patientImg = m_pManager->FindControl("patientImg");
	m_sperate = m_pManager->FindControl("sperate");

	m_btnBedNo = static_cast<DuiLib::CButtonUI *>(m_pManager->FindControl(BTN_BED_NO));
	m_btnBedNoM = static_cast<DuiLib::CButtonUI *>(m_pManager->FindControl(BTN_BED_NO_M));

	strText.Format("%02u", m_dwBedNo);
	m_btnBedNo->SetText(strText);
	m_btnBedNoM->SetText(strText);
	
	m_lblReaderNo = static_cast<DuiLib::CLabelUI *>(m_pManager->FindControl(LBL_READER_NO));	
	m_lblReaderNo->SetText("");

	m_CurReaderState = m_pManager->FindControl(CTR_CUR_READER_STATE);
	m_CurReaderState->SetVisible(false);

	m_cstModeBtn = static_cast<CModeButton *>(m_pManager->FindControl(MODE_BUTTON));

	// 格子模式
	strText = this->GetCustomAttribute("GridMode");
	DWORD dwMode = 0;
	sscanf(strText, "%lu", &dwMode);
	m_cstModeBtn->SetMode((CModeButton::Mode)dwMode);

	// A~F 6个连续测温读卡器
	m_layReaders = static_cast<DuiLib::CVerticalLayoutUI *>(m_pManager->FindControl(LAY_READERS));
	for (DWORD i = 0; i < MAX_READERS_PER_GRID; i++) {
		m_readers[i] = new CReaderUI(CReaderUI::Surgency);
		m_readers[i]->SetTag(i+1);
		m_readers[i]->SetIndicator(i);    
		m_readers[i]->SetFixedHeight(100);
		m_readers[i]->SetName(LAY_READER);
		m_layReaders->Add(m_readers[i]);
	}

	m_hand_reader = new CReaderUI;
	m_hand_reader->SetTag(0);
	m_hand_reader->SetFixedHeight(100);       
	m_layReaders->Add(m_hand_reader);	

	m_cstImgLabel = static_cast<CImageLabelUI *>(m_pManager->FindControl(CST_IMG_LBL_TEMP));
	m_cstImgLabel->SetTag(GetTag());
	m_cstImgLabel->SetEnabledEffect(true);

	m_lblElapsed = static_cast<CLabelUI *>(m_pManager->FindControl(LBL_ELAPSED));
	m_lblElapsed->SetText("");
	m_cstImg = static_cast<CMyImageUI *>(m_pManager->FindControl(CST_IMAGE));
	m_cstImg->SetTag(GetTag());
	m_cstImgLabel->SetMyImage(m_cstImg);

	m_cstPatientName = static_cast<CEditableButtonUI *>(m_pManager->FindControl(CST_PATIENT_NAME));
	m_cstPatientNameM = static_cast<CEditableButtonUI *>(m_pManager->FindControl(CST_PATIENT_NAME_M));

#if TRI_TAGS_FLAG
	m_human = new CHumanUI;
	m_layReaders->Add(m_human);         
#endif

	OnModeChanged(0);

	m_bInited = TRUE;
}

void CGridUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue) {
	CContainerUI::SetAttribute(pstrName, pstrValue);
}

void  CGridUI::SetBedNo(DWORD dwIndex) {
	m_dwBedNo = dwIndex;

	if (m_btnBedNo) {
		CDuiString  strText;
		strText.Format("%02u", m_dwBedNo);
		m_btnBedNo->SetText(strText);
	}
}

// 双击后的视图切换
void  CGridUI::SwitchView() {
	int nSel = m_tabs->GetCurSel();

	if (0 == nSel) {
		m_tabs->SelectItem(1);
	}
	else {
		m_tabs->SelectItem(0);
	}
}

void  CGridUI::SetView(int nTabIndex) {
	if (0 == nTabIndex) {
		m_tabs->SelectItem(0);
	}
	else {
		m_tabs->SelectItem(1);
	}
}

void CGridUI::Notify(TNotifyUI& msg) {
	if (msg.sType == "ModeChanged") {
		if (msg.pSender == m_cstModeBtn) {
			OnModeChanged(1);
		}
	}
	else if (msg.sType == "textchanged") {
		if (msg.pSender == m_cstPatientName) {
			OnPatientNameChanged(m_cstPatientName);
		}
		else if (msg.pSender == m_cstPatientNameM) {
			OnPatientNameChanged(m_cstPatientNameM);
		}
	}
}

void CGridUI::OnModeChanged(int nSource) {
	DWORD  dwIndex = GetTag();
	char szPName[MAX_TAG_PNAME_LENGTH] = {0};

	switch (m_cstModeBtn->GetMode())
	{
	case CModeButton::Mode_Hand:
	{
		m_hand_reader->SetVisible(true);
		m_hand_reader->m_cstBodyPart->SetText(g_data.m_CfgData.m_GridCfg[dwIndex].m_HandReaderCfg.m_szReaderName);
		for (DWORD i = 0; i < MAX_READERS_PER_GRID; i++) {
			m_readers[i]->SetVisible(false);
		}
		m_CurReaderState->SetVisible(false);
#if TRI_TAGS_FLAG
		m_human->SetVisible(false);
#endif

		m_lblReaderNo->SetText("");
		m_dwSelSurReaderIndex = 0;

		m_cstImgLabel->SetText(m_hand_reader->m_lblTemp->GetText());
		m_cstImgLabel->SetOpacityTextColor(m_hand_reader->m_lblTemp->GetTextColor());

		if ( nSource != 0 ) {
			m_cstPatientName->SetText(CBusiness::GetInstance()->GetTagPName(m_HandLastTemp.m_szTagId, szPName, sizeof(szPName)) );
			m_cstPatientNameM->SetText(m_cstPatientName->GetText());
		}

		m_cstPatientName->SetEnabled(false);
		m_cstPatientNameM->SetEnabled(false);
	}
	break;

	case CModeButton::Mode_Single:
	{
		m_hand_reader->SetVisible(false);
		for (DWORD i = 0; i < MAX_READERS_PER_GRID; i++) {
			if ( 0 == i ) {
				m_readers[i]->SetVisible(true);
				m_readers[i]->m_optSelected->SetVisible(false);
				m_readers[i]->m_cstBodyPart->SetText(g_data.m_CfgData.m_GridCfg[dwIndex].m_ReaderCfg[i].m_szReaderName);
				g_data.m_bSurReaderConnected[dwIndex][i] = FALSE;
				m_readers[i]->SetReaderStatus(FALSE);
				m_readers[i]->SetTemp(m_aLastTemp[i].m_dwTemp);
			}				
			else
				m_readers[i]->SetVisible(false);
		}
		m_CurReaderState->SetVisible(true);
#if TRI_TAGS_FLAG
		m_human->SetVisible(false);
#endif
		
		OnSurReaderSelected(0);

		if (nSource != 0) {
			m_cstPatientName->SetText(g_data.m_CfgData.m_GridCfg[dwIndex].m_szPatientName);
			m_cstPatientNameM->SetText(m_cstPatientName->GetText());
		}

		m_cstPatientName->SetEnabled(true);
		m_cstPatientNameM->SetEnabled(true);
	}
	break;

	case CModeButton::Mode_Multiple:
	{
		m_hand_reader->SetVisible(false);
		int  nSel = -1;
		for (DWORD i = 0; i < MAX_READERS_PER_GRID; i++) {
#if !TRI_TAGS_FLAG
			m_readers[i]->SetVisible(true);
#else
			m_readers[i]->SetVisible(false); 
#endif
			if (0==i)
				m_readers[i]->m_optSelected->SetVisible(true);

			if (g_data.m_CfgData.m_GridCfg[dwIndex].m_ReaderCfg[i].m_bSwitch) {
				m_readers[i]->m_optSelected->Selected(true,false);
				if (nSel < 0)
					nSel = i;
			}
			else {
				m_readers[i]->m_optSelected->Selected(false,false);								
			}
			g_data.m_bSurReaderConnected[dwIndex][i] = FALSE;
			m_readers[i]->SetReaderStatus(FALSE);
			m_readers[i]->SetTemp(m_aLastTemp[i].m_dwTemp);
			m_readers[i]->m_cstBodyPart->SetText(g_data.m_CfgData.m_GridCfg[dwIndex].m_ReaderCfg[i].m_szReaderName);
		}
#if !TRI_TAGS_FLAG 
		m_CurReaderState->SetVisible(true);
#endif
#if TRI_TAGS_FLAG
		m_human->SetVisible(true);
#endif
		
		if (nSel < 0)
			nSel = 0;
		OnSurReaderSelected(nSel);

		if (nSource != 0) {
			m_cstPatientName->SetText(g_data.m_CfgData.m_GridCfg[dwIndex].m_szPatientName);
			m_cstPatientNameM->SetText(m_cstPatientName->GetText());
		}

		m_cstPatientName->SetEnabled(true);
		m_cstPatientNameM->SetEnabled(true);
	}
	break;

	default:
		break;
	}

	CDuiString  strText;	
	strText.Format("%s %lu", CFG_GRID_MODE, dwIndex+1);

	g_data.m_CfgData.m_GridCfg[dwIndex].m_dwGridMode = (DWORD)m_cstModeBtn->GetMode();
#if !TRI_TAGS_FLAG
	DWORD dwDefValue = CModeButton::Mode_Hand;
#else
	DWORD dwDefValue = CModeButton::Mode_Multiple;
#endif
	g_data.m_cfg->SetConfig(strText, g_data.m_CfgData.m_GridCfg[dwIndex].m_dwGridMode, &dwDefValue);
}

void  CGridUI::SetMode(CModeButton::Mode e) {
	CDuiString strText;
	strText.Format("%lu", (DWORD)e);
	this->SetAttribute("GridMode", strText);

	if ( m_bInited ) {
		m_cstModeBtn->SetMode(e);
	}
}

// 设置术中读卡器的连接状态
void  CGridUI::SetSurReaderStatus(DWORD j, BOOL bConnected) {
	// 如果是手持状态
	if (m_cstModeBtn->GetMode() == CModeButton::Mode_Hand) {
		return;
	}

	assert(m_dwSelSurReaderIndex >= 1);
	m_readers[j]->SetReaderStatus(bConnected);
	if ( !bConnected )
		m_readers[j]->SetTemp(m_aLastTemp[j].m_dwTemp);

	// 如果当前选中的Reader Index和数据的index一致
	if (m_dwSelSurReaderIndex == j + 1) {
		m_CurReaderState->SetBkImage(m_readers[j]->m_state->GetBkImage());
		m_cstImgLabel->SetText(m_readers[j]->m_lblTemp->GetText());
	}
}

// 术中读卡器温度
void  CGridUI::OnSurReaderTemp(DWORD j, const TempItem & item) {

	assert(j < MAX_READERS_PER_GRID);

	// 如果是新的TagID，则请求历史记录
	if ( 0 != strcmp(m_aLastTemp[j].m_szTagId, item.m_szTagId) ) {
		DWORD i = GetTag();
		WORD  wBed = (WORD)(i * MAX_READERS_PER_GRID + j + 1);
		CBusiness::GetInstance()->SaveLastSurTagIdAsyn(wBed, item.m_szTagId);
		CBusiness::GetInstance()->QueryTempByTagAsyn(item.m_szTagId, wBed);
		return;
	}

	memcpy( &m_aLastTemp[j], &item, sizeof(TempItem) );	
	TempItem * pNewItem = new TempItem;
	memcpy(pNewItem, &item, sizeof(TempItem));
	m_vTemp[j].push_back(pNewItem);

	ShowSurReaderTemp(j, item);
}

void  CGridUI::ShowSurReaderTemp(DWORD j, const TempItem & item) {
	DWORD  i = GetTag();
	DWORD  dwHighAlarm = g_data.m_CfgData.m_GridCfg[i].m_ReaderCfg[j].m_dwHighTempAlarm;
	DWORD  dwLowAlarm = g_data.m_CfgData.m_GridCfg[i].m_ReaderCfg[j].m_dwLowTempAlarm;
	SetReaderTemp(j, item.m_dwTemp, dwHighAlarm, dwLowAlarm, item.m_time);
	m_readers[j]->m_lblReaderId->SetText(item.m_szReaderId);
	m_readers[j]->m_lblTagId->SetText(item.m_szTagId);

	// 如果当前选中的Reader Index和数据的index一致
	if (m_dwSelSurReaderIndex == j + 1) {
		SetCurReaderTemp(m_readers[j]->m_lblTemp);
		UpdateElapsed();
	}

	m_cstImg->MyInvalidate();
	m_cstImgLabel->Invalidate();
}

// 手持读卡器温度
void  CGridUI::OnHandReaderTemp(const TempItem & item) {

	memcpy(&m_HandLastTemp, &item, sizeof(TempItem));
	TempItem * pNewItem = new TempItem;
	memcpy(pNewItem, &item, sizeof(TempItem));
	m_vHandTemp.push_back(pNewItem);

	ShowHandReaderTemp(item);
}

// 显示手持Tag温度
void CGridUI::ShowHandReaderTemp(const TempItem & item) {
	DWORD  i = GetTag();
	DWORD  dwHighAlarm = g_data.m_CfgData.m_GridCfg[i].m_HandReaderCfg.m_dwHighTempAlarm;
	DWORD  dwLowAlarm = g_data.m_CfgData.m_GridCfg[i].m_HandReaderCfg.m_dwLowTempAlarm;
	SetHandReaderTemp(item.m_dwTemp, dwHighAlarm, dwLowAlarm, item.m_time);
	m_hand_reader->m_lblReaderId->SetText(item.m_szReaderId);
	m_hand_reader->m_lblTagId->SetText(item.m_szTagId);

	// 如果当前选中的Reader Index和数据的index一致
	if (m_dwSelSurReaderIndex == 0) {
		SetCurReaderTemp(m_hand_reader->m_lblTemp);
		UpdateElapsed();
	}

	m_cstImg->MyInvalidate();
}

void CGridUI::SetCurReaderTemp(CLabelUI * pReaderUI) {
	assert(pReaderUI);	
	m_cstImgLabel->SetText(pReaderUI->GetText());
	m_cstImgLabel->SetOpacityTextColor(pReaderUI->GetTextColor());
}

void CGridUI::SetReaderTemp(DWORD j, DWORD  dwTemp, DWORD dwHighAlarm, DWORD dwLowAlarm, time_t t) {
	//CDuiString  strText;
	//strText.Format("%.2f", dwTemp / 100.0);
	//m_readers[j]->m_lblTemp->SetText(strText);
	m_readers[j]->SetTemp(dwTemp);
#if TRI_TAGS_FLAG
	if (j < 3) {
		m_human->SetTemp(j, m_readers[j]->GetTemp());
	}
#endif

	if (dwTemp >= dwHighAlarm) {
		m_readers[j]->m_lblTemp->SetTextColor(HIGH_TEMP_TEXT_COLOR);
#if TRI_TAGS_FLAG
		if (j < 3) {
			m_human->SetTempColor(j, HIGH_TEMP_TEXT_COLOR);
		}
#endif

		time_t now = time(0);
		time_t diff_t = GetTimeDiff(t, now);
		if ( diff_t <= 2)
			CBusiness::GetInstance()->AlarmAsyn();
	}
	else if (dwTemp <= dwLowAlarm) {
		m_readers[j]->m_lblTemp->SetTextColor(LOW_TEMP_TEXT_COLOR);
#if TRI_TAGS_FLAG
		if (j < 3) {
			m_human->SetTempColor(j, LOW_TEMP_TEXT_COLOR);
		}
#endif

		time_t now = time(0);
		time_t diff_t = GetTimeDiff(t, now);
		if (diff_t <= 2)
			CBusiness::GetInstance()->AlarmAsyn();
	}
	else {
		m_readers[j]->m_lblTemp->SetTextColor(NORMAL_TEMP_TEXT_COLOR);
#if TRI_TAGS_FLAG
		if (j < 3) {
			m_human->SetTempColor(j, NORMAL_TEMP_TEXT_COLOR); 
		}
#endif
	}

#if TRI_TAGS_FLAG
	if (j < 3) {
		for (int k = 0; k < 3; k++) {
			if ( k == j )
				continue;
			m_human->SetDelta(j, k, m_readers[j]->IsConnected(), m_readers[k]->IsConnected(),
				m_readers[j]->GetIntTemp(), m_readers[k]->GetIntTemp());
		}
	}
#endif
}

void CGridUI::SetHandReaderTemp(DWORD  dwTemp, DWORD dwHighAlarm, DWORD dwLowAlarm, time_t t) {
	m_hand_reader->SetTemp(dwTemp);

	if (dwTemp >= dwHighAlarm) {
		m_hand_reader->m_lblTemp->SetTextColor(HIGH_TEMP_TEXT_COLOR);

		time_t now = time(0);
		time_t diff_t = GetTimeDiff(t, now);
		if (diff_t <= 2)
			CBusiness::GetInstance()->AlarmAsyn();
	}
	else if (dwTemp <= dwLowAlarm) {
		m_hand_reader->m_lblTemp->SetTextColor(LOW_TEMP_TEXT_COLOR);

		time_t now = time(0);
		time_t diff_t = GetTimeDiff(t, now);
		if (diff_t <= 2)
			CBusiness::GetInstance()->AlarmAsyn();
	}
	else {
		m_hand_reader->m_lblTemp->SetTextColor(NORMAL_TEMP_TEXT_COLOR);
	}
}

// 选中读卡器
void CGridUI::OnSurReaderSelected(DWORD  dwSelected) {
	assert(dwSelected < MAX_READERS_PER_GRID);
	for (DWORD j = 0; j < MAX_READERS_PER_GRID; j++) {
		if (j == dwSelected) {
			m_readers[j]->SetBkColor(SELECTED_READER_BG_COLOR);
		}
		else {
			m_readers[j]->SetBkColor(0);
		}
	}

	m_dwSelSurReaderIndex = dwSelected + 1;

	CDuiString strText;
	strText.Format("%c", ('A' + (char)dwSelected));
	m_lblReaderNo->SetText(strText);

	m_CurReaderState->SetBkImage(m_readers[dwSelected]->m_state->GetBkImage());

	m_cstImgLabel->SetText(m_readers[dwSelected]->m_lblTemp->GetText());
	m_cstImgLabel->SetOpacityTextColor(m_readers[dwSelected]->m_lblTemp->GetTextColor());

	m_cstImg->MyInvalidate();
}

// 设置温度字体
void CGridUI::SetFont(int index) {
	if (m_cstImgLabel)
		m_cstImgLabel->SetFont(index);
}

void CGridUI::OnSizeChanged(const SIZE & s) {
	
	// 修改字体大小
	if (s.cx >= 950 && s.cy >= 425) {
		SetFont(27);
	}
	else if (s.cx >= 816 && s.cy >= 365) {
		SetFont(26);
	}
	else if (s.cx > 680 && s.cy > 304) {
		SetFont(25);
	}
	else if (s.cx > 572 && s.cy > 256) {
		SetFont(24);
	}
	else if (s.cx > 466 && s.cy > 208) {
		SetFont(23);
	}
	else if (s.cx > 350 && s.cy > 157) {
		SetFont(22);
	}
	else if (s.cx > 280 && s.cy > 125) {
		SetFont(21);
	}
	else {
		SetFont(20);
	}

	if (s.cy >= 198) {
		m_layHead->SetFixedHeight(74);	

		m_cstModeBtn->GetParent()->SetFixedHeight(30);
		m_cstModeBtn->GetParent()->GetParent()->SetFixedWidth(30);

		m_bedImg->SetFixedHeight(50);
		m_bedImg->GetParent()->SetFixedWidth(50);

		m_btnBedNo->SetFixedWidth(110);
		m_btnBedNo->SetFont(6);
		m_lblReaderNo->SetFont(4);
		m_lblReaderNo->SetFixedWidth(30);
		m_sperate->SetFixedWidth(30);

		m_patientImg->SetFixedWidth(40);
		m_patientImg->GetParent()->SetFixedHeight(40);

		m_cstPatientName->SetFixedWidth(100);
		m_cstPatientName->SetAttribute("btnfont", "7");
		m_cstPatientName->SetAttribute("editfont", "7");
		m_cstPatientName->GetParent()->GetParent()->SetFixedWidth(140);

		m_CurReaderState->SetFixedHeight(30);
		m_CurReaderState->SetFixedWidth(30);
		m_CurReaderState->GetParent()->SetFixedHeight(30);

		m_lblElapsed->SetFont(5);
		m_lblElapsed->GetParent()->SetFixedWidth(80);

	}
	else if (s.cy >= 168) {
		m_layHead->SetFixedHeight(64);

		m_cstModeBtn->GetParent()->SetFixedHeight(26);
		m_cstModeBtn->GetParent()->GetParent()->SetFixedWidth(26);

		m_bedImg->SetFixedHeight(42);
		m_bedImg->GetParent()->SetFixedWidth(42);

		m_btnBedNo->SetFixedWidth(82);
		m_btnBedNo->SetFont(50);
		m_lblReaderNo->SetFont(60);
		m_lblReaderNo->SetFixedWidth(25);
		m_sperate->SetFixedWidth(22);

		m_patientImg->SetFixedWidth(34);
		m_patientImg->GetParent()->SetFixedHeight(34);

		m_cstPatientName->SetFixedWidth(82);
		m_cstPatientName->SetAttribute("btnfont", "70");
		m_cstPatientName->SetAttribute("editfont", "70");
		m_cstPatientName->GetParent()->GetParent()->SetFixedWidth(116);

		m_CurReaderState->SetFixedHeight(26);
		m_CurReaderState->SetFixedWidth(26);
		m_CurReaderState->GetParent()->SetFixedHeight(26);

		m_lblElapsed->SetFont(80);
		m_lblElapsed->GetParent()->SetFixedWidth(70);
	}
	else if (s.cy >= 138) {
		m_layHead->SetFixedHeight(54);

		m_cstModeBtn->GetParent()->SetFixedHeight(23);
		m_cstModeBtn->GetParent()->GetParent()->SetFixedWidth(23);

		m_bedImg->SetFixedHeight(36);
		m_bedImg->GetParent()->SetFixedWidth(36);

		m_btnBedNo->SetFixedWidth(58);
		m_btnBedNo->SetFont(51);
		m_lblReaderNo->SetFont(61);
		m_lblReaderNo->SetFixedWidth(20);
		m_sperate->SetFixedWidth(16);

		m_patientImg->SetFixedWidth(29);
		m_patientImg->GetParent()->SetFixedHeight(29);

		m_cstPatientName->SetFixedWidth(66);
		m_cstPatientName->SetAttribute("btnfont", "71");
		m_cstPatientName->SetAttribute("editfont", "71");
		m_cstPatientName->GetParent()->GetParent()->SetFixedWidth(95);

		m_CurReaderState->SetFixedHeight(23);
		m_CurReaderState->SetFixedWidth(23);
		m_CurReaderState->GetParent()->SetFixedHeight(23);

		m_lblElapsed->SetFont(81);
		m_lblElapsed->GetParent()->SetFixedWidth(62);
	}
	else {
		m_layHead->SetFixedHeight(44);

		m_cstModeBtn->GetParent()->SetFixedHeight(20);
		m_cstModeBtn->GetParent()->GetParent()->SetFixedWidth(20);

		m_bedImg->SetFixedHeight(30);
		m_bedImg->GetParent()->SetFixedWidth(30);

		m_btnBedNo->SetFixedWidth(34);
		m_btnBedNo->SetFont(52);
		m_lblReaderNo->SetFont(62);
		m_lblReaderNo->SetFixedWidth(15);
		m_sperate->SetFixedWidth(10);

		m_patientImg->SetFixedWidth(24);
		m_patientImg->GetParent()->SetFixedHeight(24);

		m_cstPatientName->SetFixedWidth(50);
		m_cstPatientName->SetAttribute("btnfont", "72");
		m_cstPatientName->SetAttribute("editfont", "72");
		m_cstPatientName->GetParent()->GetParent()->SetFixedWidth(74);

		m_CurReaderState->SetFixedHeight(20);
		m_CurReaderState->SetFixedWidth(20);
		m_CurReaderState->GetParent()->SetFixedHeight(20);

		m_lblElapsed->SetFont(82);
		m_lblElapsed->GetParent()->SetFixedWidth(54);
	}
}

// 更新显示逝去的时间
void CGridUI::UpdateElapsed() {
	char buf[256];
	DWORD dwBufSize = sizeof(buf);

	time_t last_time = 0;
	if (m_dwSelSurReaderIndex < 1) {
		last_time = m_HandLastTemp.m_time;
	}
	else {
		last_time = m_aLastTemp[m_dwSelSurReaderIndex - 1].m_time;
	}

	time_t  now = time(0);
	if ( 0 == last_time || now < last_time ) {
		buf[0] = '\0';
	}
	else {
		time_t diff = now - last_time;

		if (diff < 60) {
			SNPRINTF(buf, dwBufSize, "刚刚");
		}
		else if (diff < 3600) {
			SNPRINTF(buf, dwBufSize, "%lu分钟前", (DWORD)((diff - 60) / 60 + 1));
		}
		else if (diff < 86400) {
			SNPRINTF(buf, dwBufSize, "%lu小时前", (DWORD)((diff - 3600) / 3600 + 1));
		}
		else {
			SNPRINTF(buf, dwBufSize, "%lu天前", (DWORD)((diff - 86400) / 86400 + 1));
		}
	}

	if ( m_lblElapsed )
		m_lblElapsed->SetText(buf);
}

// 得到温度历史数据
void CGridUI::OnQueryTempRet(DWORD j, const char * szTagId, const std::vector<TempItem*> & vRet) {
	assert(j < MAX_READERS_PER_GRID);

	ClearVector(m_vTemp[j]);
	m_vTemp[j].insert(m_vTemp[j].begin(), vRet.begin(), vRet.end());

	DWORD  dwCnt = vRet.size();
	if ( dwCnt > 0 ) {		
		TempItem* pLastItem = vRet[dwCnt - 1];
		memcpy(&m_aLastTemp[j], pLastItem, sizeof(TempItem));
		STRNCPY(m_aLastTemp[j].m_szTagId, szTagId, MAX_TAG_ID_LENGTH);
		ShowSurReaderTemp(j, m_aLastTemp[j]);
	}
	else {
		//assert(0);
	}
	
}

// 得到温度历史数据(手持Tag)
void  CGridUI::OnQueryHandTempRet(const char * szTagId, const std::vector<TempItem*> & vRet) {
	ClearVector(m_vHandTemp);
	m_vHandTemp.insert(m_vHandTemp.begin(), vRet.begin(), vRet.end());

	DWORD  dwCnt = vRet.size();
	if (dwCnt > 0) {
		TempItem* pLastItem = vRet[dwCnt - 1];
		memcpy(&m_HandLastTemp, pLastItem, sizeof(TempItem));
		STRNCPY(m_HandLastTemp.m_szTagId, szTagId, MAX_TAG_ID_LENGTH);
		ShowHandReaderTemp(m_HandLastTemp);
	}
	else {
		//assert(0);
	}
}

// 一周前的数据删除
void  CGridUI::PruneData() {
	std::vector<TempItem * >::iterator it;
	time_t today_zero_time = GetTodayZeroTime();
	// 一周前0点时分
#if TEST_MEMORY_LEAK_FLAG
	time_t tWeekBegin = time(0) - 300;
#else
	time_t tWeekBegin = today_zero_time - 3600 * 24 * 6;
#endif

	for ( DWORD i = 0; i < MAX_READERS_PER_GRID; i++ ) {
		PruneData(m_vTemp[i], tWeekBegin);
	}
	PruneData(m_vHandTemp, tWeekBegin);
}

void CGridUI::PruneData(std::vector<TempItem*> & v, time_t t) {
	std::vector<TempItem * >::iterator it;
	for (it = v.begin(); it != v.end(); ++it) {
		TempItem* pItem = *it;
		if (pItem->m_time < t) {
			delete pItem;
		}
		else {
			break;
		}
	}
	v.erase(v.begin(), it);
}

DWORD  CGridUI::GetReaderIndex() const {
	return m_dwSelSurReaderIndex;
}

const std::vector<TempItem * > & CGridUI::GetTempData(DWORD dwReaderIndex) const {
	if (0 == dwReaderIndex) {
		return  m_vHandTemp;
	}
	else {
		assert(dwReaderIndex <= MAX_READERS_PER_GRID);
		return m_vTemp[dwReaderIndex - 1];
	}
}

CModeButton::Mode  CGridUI::GetMode() const {
	return m_cstModeBtn->GetMode();
}

void CGridUI::ResetData() {
	for (DWORD i = 0; i < MAX_READERS_PER_GRID; i++) {
		ClearVector(m_vTemp[i]);
	}
	ClearVector(m_vHandTemp);

	memset(m_aLastTemp, 0, sizeof(m_aLastTemp));
	memset(&m_HandLastTemp, 0, sizeof(m_HandLastTemp));
	m_dwSelSurReaderIndex = 0;

	SetMode(CModeButton::Mode_Hand);

	m_tabs->SelectItem(0);
	m_lblReaderNo->SetText("");
	m_CurReaderState->SetBkImage("");
	m_lblElapsed->SetText("");
	m_cstImgLabel->SetText("");

	m_hand_reader->m_lblTagId->SetText("");
	m_hand_reader->m_lblReaderId->SetText("");
	m_hand_reader->m_lblTemp->SetText("");

	for ( int i = 0; i < MAX_READERS_PER_GRID; i++ ) {
		m_readers[i]->m_lblTagId->SetText("");
		m_readers[i]->m_lblReaderId->SetText("");
		m_readers[i]->m_lblTemp->SetText("");
	}

	m_cstPatientName->SetText("");
	m_cstPatientNameM->SetText("");
}

void CGridUI::OnPatientNameChanged(CEditableButtonUI * pSource) {
	DWORD  i = GetTag();
	CDuiString  strPatientName = pSource->GetText();
	STRNCPY(g_data.m_CfgData.m_GridCfg[i].m_szPatientName, strPatientName, MAX_TAG_PNAME_LENGTH);
	SavePatientName(i);

	if ( pSource == m_cstPatientName ) {
		m_cstPatientNameM->SetText(strPatientName);
	}
	else {
		m_cstPatientName->SetText(strPatientName);
	}	
}

void CGridUI::SetPatientName(const char * szName) {
	assert(m_bInited);
	m_cstPatientName->SetText(szName);
	m_cstPatientNameM->SetText(szName);
}

void CGridUI::SetPatientNameInHandMode(const char * szName) {
	assert(m_bInited);
	if (m_cstModeBtn->GetMode() == CModeButton::Mode_Hand) {
		m_cstPatientName->SetText(szName);
		m_cstPatientNameM->SetText(szName);
	}	
}

// 导出excel
void  CGridUI::ExportExcel() {
	CDuiString strText;

	char szPatientName[256] = { 0 };
	CDuiString strPatientName = m_cstPatientName->GetText();
	STRNCPY(szPatientName, strPatientName, sizeof(szPatientName));

	if (!CExcel::IfExcelInstalled()) {
		::MessageBox(g_data.m_hWnd, "没有检测到系统安装了excel", "导出excel", 0);
		return;
	}

	CModeButton::Mode mode = m_cstModeBtn->GetMode();

	DWORD  dwDataCnt = 0;
	if (mode == CModeButton::Mode_Multiple) {
		for (DWORD i = 0; i < MAX_READERS_PER_GRID; i++) {
			dwDataCnt += m_vTemp[i].size();
		}
		dwDataCnt += m_vHandTemp.size();
	}
	else if (mode == CModeButton::Mode_Single) {
		dwDataCnt += m_vTemp[0].size();
	}
	else {
		dwDataCnt += m_vHandTemp.size();
	}
	
	if (dwDataCnt == 0) {
		strText.Format("没有温度数据，放弃保存excel");
		::MessageBox(g_data.m_hWnd, strText, "导出excel", 0);
		return;
	}

	OPENFILENAME ofn = { 0 };
	TCHAR strFilename[MAX_PATH] = { 0 };//用于接收文件名  

	char szTime[256];
	time_t now = time(0);
	DateTime2StringCn(szTime, sizeof(szTime), &now);
	SNPRINTF(strFilename, sizeof(strFilename), "%s_%s", szPatientName, szTime);

	ofn.lStructSize = sizeof(OPENFILENAME);//结构体大小  
	ofn.hwndOwner = g_data.m_hWnd;//拥有着窗口句柄，为NULL表示对话框是非模态的，实际应用中一般都要有这个句柄  
	ofn.lpstrFilter = TEXT("Excel Flie(*.xls)\0*.xls\0Excel Flie(*.xlsx)\0*.xlsx\0\0");//设置过滤  
	ofn.nFilterIndex = 1;//过滤器索引  
	ofn.lpstrFile = strFilename;//接收返回的文件名，注意第一个字符需要为NULL  
	ofn.nMaxFile = sizeof(strFilename);//缓冲区长度  
	ofn.lpstrInitialDir = CPaintManagerUI::GetInstancePath();
	ofn.lpstrTitle = TEXT("请选择一个文件");//使用系统默认标题留空即可  
	ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;//文件、目录必须存在，隐藏只读选项  
	if (!GetSaveFileName(&ofn)) {
		strText.Format("GetSaveFileName失败，放弃保存excel");
		::MessageBox(g_data.m_hWnd, strText, "导出excel", 0);
		return;
	}

	CExcel  excel;
	excel.WriteGrid(0, 0, "姓名");
	excel.WriteGrid(0, 1, szPatientName);

	char tmp[64];
	SNPRINTF(tmp, sizeof(tmp), "手持测量时间");
	excel.WriteGrid(2, 0, tmp);

	SNPRINTF(tmp, sizeof(tmp), "手持测量体温");
	excel.WriteGrid(2, 1, tmp);

	SNPRINTF(tmp, sizeof(tmp), "手持测量注释");
	excel.WriteGrid(2, 2, tmp);

	for (DWORD i = 0; i < MAX_READERS_PER_GRID; i++) {
		SNPRINTF(tmp, sizeof(tmp), "连续测量时间%lu", i + 1);
		excel.WriteGrid(2, 0 + (i + 1) * 4, tmp);

		SNPRINTF(tmp, sizeof(tmp), "连续测量体温%lu", i + 1);
		excel.WriteGrid(2, 1 + (i + 1) * 4, tmp);

		SNPRINTF(tmp, sizeof(tmp), "连续测量注释%lu", i + 1);
		excel.WriteGrid(2, 2 + (i + 1) * 4, tmp);
	}

	std::vector<const char *> vValues;
	vector<TempItem *>::iterator it;
	std::vector<const char *>::iterator  ix;

	// 术中数据
	if ( mode != CModeButton::Mode_Hand ) {
		for (DWORD i = 0; i < MAX_READERS_PER_GRID; i++) {
			if ( mode == CModeButton::Mode_Single && i > 0 ) {
				break;
			}

			vector<TempItem *> & vTempData = m_vTemp[i];
			for (it = vTempData.begin(); it != vTempData.end(); it++) {
				TempItem * pItem = *it;

				char * p = new char[32];
				DateTime2String(p, 32, &pItem->m_time);
				vValues.push_back(p);

				p = new char[8];
				SNPRINTF(p, 8, "%.2f", pItem->m_dwTemp / 100.0);
				vValues.push_back(p);

				if (pItem->m_szRemark[0] != 0) {
					p = new char[MAX_REMARK_LENGTH];
					STRNCPY(p, pItem->m_szRemark, MAX_REMARK_LENGTH);
					vValues.push_back(p);
				}
				else {
					vValues.push_back(0);
				}
			}

			excel.WriteRange(3, 0 + (i + 1) * 4, vTempData.size() + 3 - 1, 2 + (i + 1) * 4, vValues);

			for (ix = vValues.begin(); ix != vValues.end(); ++ix) {
				if (*ix != 0) {
					delete[] * ix;
				}
			}
			vValues.clear();
		}
	}
	else {

		// 手持数据
		for (it = m_vHandTemp.begin(); it != m_vHandTemp.end(); it++) {
			TempItem * pItem = *it;

			char * p = new char[32];
			DateTime2String(p, 32, &pItem->m_time);
			vValues.push_back(p);

			p = new char[8];
			SNPRINTF(p, 8, "%.2f", pItem->m_dwTemp / 100.0);
			vValues.push_back(p);

			if (pItem->m_szRemark[0] != 0) {
				p = new char[MAX_REMARK_LENGTH];
				STRNCPY(p, pItem->m_szRemark, MAX_REMARK_LENGTH);
				vValues.push_back(p);
			}
			else {
				vValues.push_back(0);
			}
		}
		excel.WriteRange(3, 0, m_vHandTemp.size() + 3 - 1, 2, vValues);
		for (ix = vValues.begin(); ix != vValues.end(); ++ix) {
			if (*ix != 0) {
				delete[] * ix;
			}
		}
		vValues.clear();
		// end of 手持数据

	}
	

	char buf[8192];
	if (1 == ofn.nFilterIndex) {
		SNPRINTF(buf, size(buf), "%s.xls", strFilename);
	}
	else if (2 == ofn.nFilterIndex) {
		SNPRINTF(buf, size(buf), "%s.xlsx", strFilename);
	}
	else {
		SNPRINTF(buf, size(buf), "%s", strFilename);
	}

	int ret = excel.SaveAs(buf);
	excel.Quit();

	if (ret != 0) {
		strText.Format("导出excel数据失败。请确保excel文件没有打开");
		::MessageBox(g_data.m_hWnd, strText, "导出excel", 0);
	}
}

// 打印excel表格
void  CGridUI::PrintExcel() {
	CDuiString strText;

	if (!CExcelEx::IfExcelInstalled()) {
		::MessageBox(g_data.m_hWnd, "没有检测到系统安装了excel", "打印excel图表", 0);
		return;
	}

	char szPatientName[256] = { 0 };
	CDuiString strPatientName = m_cstPatientName->GetText();
	STRNCPY(szPatientName, strPatientName, sizeof(szPatientName));

	CModeButton::Mode mode = m_cstModeBtn->GetMode();

	DWORD  dwDataCnt = 0;
	if (mode == CModeButton::Mode_Multiple) {
		for (DWORD i = 0; i < MAX_READERS_PER_GRID; i++) {
			dwDataCnt += m_vTemp[i].size();
		}
		dwDataCnt += m_vHandTemp.size();
	}
	else if (mode == CModeButton::Mode_Single) {
		dwDataCnt += m_vTemp[0].size();
	}
	else {
		dwDataCnt += m_vHandTemp.size();
	}
	
	if (dwDataCnt == 0) {
		strText.Format("没有温度数据，放弃打印excel图表");
		::MessageBox(g_data.m_hWnd, strText, "打印excel图表", 0);
		return;
	}

	TCHAR strFilename[MAX_PATH] = { 0 };//用于接收文件名  
	char szTime[256];
	time_t now = time(0);
	Date2String(szTime, sizeof(szTime), &now);
	SNPRINTF(strFilename, sizeof(strFilename), "%s %s", szPatientName, szTime);

	CExcelEx  excel;
	vector<TempItem *>::iterator it;
	CExcelEx::Series s[MAX_READERS_PER_GRID] = { 0 };
	DWORD  dwMin = 3500;
	for (DWORD i = 0; i < MAX_READERS_PER_GRID; i++) {
		s[i].bEmpty = TRUE;
	}

	// 如果是单点或多点
	if (mode != CModeButton::Mode_Hand) {
		for (DWORD i = 0; i < MAX_READERS_PER_GRID; i++) {
			if ( mode == CModeButton::Mode_Single && i > 0 ) {
				break;
			}

			vector<TempItem *> & vTempData = m_vTemp[i];
			if (vTempData.size() == 0) {
				s[i].bEmpty = TRUE;
				continue;
			}

			char szReaderName[64] = { 0 };
			CDuiString strBodyPart = m_readers[i]->m_cstBodyPart->GetText();
			if (strBodyPart.GetLength() > 0) {
				STRNCPY(szReaderName, strBodyPart, sizeof(szReaderName));
			}
			else {
				szReaderName[0] = (char)('A' + i);
			}

			s[i].bEmpty = FALSE;
			s[i].dwStartRowIndex = 0;
			s[i].dwStartColIndex = i * 2;
			s[i].dwEndRowIndex = vTempData.size() - 1;
			STRNCPY(s[i].szName, szReaderName, sizeof(s[i].szName));

			if (vTempData.size() > 100) {
				s[i].dwStartRowIndex = vTempData.size() - 100;
			}

			DWORD j = 0;
			for (it = vTempData.begin(), j = 0; it != vTempData.end(); it++, j++) {
				TempItem * pItem = *it;

				DateTime2String(szTime, sizeof(szTime), &pItem->m_time);
				excel.WriteGrid(j, i * 2, szTime);

				char szTemperature[8];
				SNPRINTF(szTemperature, 8, "%.2f", pItem->m_dwTemp / 100.0);
				excel.WriteGrid(j, i * 2 + 1, szTemperature);

				if (pItem->m_dwTemp < dwMin) {
					dwMin = pItem->m_dwTemp;
				}
			}
		}
	}
	// 如果是手持
	else {
		DWORD i = 0;
		vector<TempItem *> & vTempData = m_vHandTemp;

		char szReaderName[64] = { 0 };
		CDuiString strBodyPart = m_readers[i]->m_cstBodyPart->GetText();
		if (strBodyPart.GetLength() > 0) {
			STRNCPY(szReaderName, strBodyPart, sizeof(szReaderName));
		}
		else {
			szReaderName[0] = (char)('A' + i);
		}

		s[i].bEmpty = FALSE;
		s[i].dwStartRowIndex = 0;
		s[i].dwStartColIndex = i * 2;
		s[i].dwEndRowIndex = vTempData.size() - 1;
		STRNCPY(s[i].szName, szReaderName, sizeof(s[i].szName));

		if (vTempData.size() > 100) {
			s[i].dwStartRowIndex = vTempData.size() - 100;
		}

		DWORD j = 0;
		for (it = vTempData.begin(), j = 0; it != vTempData.end(); it++, j++) {
			TempItem * pItem = *it;

			DateTime2String(szTime, sizeof(szTime), &pItem->m_time);
			excel.WriteGrid(j, i * 2, szTime);

			char szTemperature[8];
			SNPRINTF(szTemperature, 8, "%.2f", pItem->m_dwTemp / 100.0);
			excel.WriteGrid(j, i * 2 + 1, szTemperature);

			if (pItem->m_dwTemp < dwMin) {
				dwMin = pItem->m_dwTemp;
			}
		}
	}
	

	double dMin = 0.0;
	if (dwMin >= 3500) {
		dMin = 34.0;
	}
	else if (dwMin > 3000) {
		dMin = 30.0;
	}
	else if (dwMin > 2600) {
		dMin = 26.0;
	}
	else if (dwMin > 2200) {
		dMin = 22.0;
	}
	
	int ret = excel.PrintChartWithMultiSeries(s, MAX_READERS_PER_GRID, strFilename, 0, 0, TRUE, &dMin);
	excel.Quit();

	if (0 != ret) {
		strText.Format("打印excel图表失败");
		::MessageBox(g_data.m_hWnd, strText, "打印excel图表", 0);
	}
}

// 获得当前的TagID
CDuiString  CGridUI::GetCurTagId() {
	if ( 0 == m_dwSelSurReaderIndex ) {
		return m_HandLastTemp.m_szTagId;
	}
	else {
		return m_aLastTemp[m_dwSelSurReaderIndex - 1].m_szTagId;
	}
}

// 获得当前的病人姓名
CDuiString  CGridUI::GetCurPatientName() {
	if (m_cstPatientName)
		return m_cstPatientName->GetText();
	else
		return "";
}

// 设置当前的病人姓名
void  CGridUI::SetCurPatientName(const char * szName) {
	if (m_cstPatientName) {
		m_cstPatientName->SetText(szName);
		m_cstPatientNameM->SetText(szName);

		// 如果是单点和多点模式，还需要保存配置文件
		if (m_cstModeBtn->GetMode() == CModeButton::Mode_Single
			|| m_cstModeBtn->GetMode() == CModeButton::Mode_Multiple) {
			DWORD  i = GetTag();
			STRNCPY(g_data.m_CfgData.m_GridCfg[i].m_szPatientName, szName, MAX_TAG_PNAME_LENGTH);
			SavePatientName(i);
		}		
	}
}

// 当格子的绑定Tag被其他格子夺去
void  CGridUI::OnReleaseTagBinding() {
	// 清空手持Reader相关数据
	ClearVector(m_vHandTemp);
	memset(&m_HandLastTemp, 0, sizeof(m_HandLastTemp));	

	m_hand_reader->m_lblTagId->SetText("");
	m_hand_reader->m_lblReaderId->SetText("");
	m_hand_reader->m_lblTemp->SetText("");

	// 如果是手持模式
	if (m_cstModeBtn->GetMode() == CModeButton::Mode_Hand) {
		m_lblElapsed->SetText("");
		m_cstImgLabel->SetText("");
		m_cstPatientName->SetText("");
		m_cstPatientNameM->SetText("");
	}

	this->Invalidate();
}

// 当病人出院(单点模式下)
void CGridUI::OnOutHospital() {
	assert(m_cstModeBtn->GetMode() == CModeButton::Mode_Single);

	ClearVector(m_vTemp[0]);
	memset(&m_aLastTemp[0], 0, sizeof(m_aLastTemp[0]));

	m_readers[0]->m_lblTagId->SetText("");
	m_readers[0]->m_lblReaderId->SetText("");
	m_readers[0]->m_lblTemp->SetText("");

	m_lblElapsed->SetText("");
	m_cstImgLabel->SetText("");
	//m_cstPatientName->SetText("");
	//m_cstPatientNameM->SetText("");

	this->Invalidate();
}