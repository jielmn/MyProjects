#include "GridUI.h"
#include "business.h"

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
	m_lblElapsed = static_cast<CImageLabelUI *>(m_pManager->FindControl(LBL_ELAPSED));
	m_lblElapsed->SetText("");
	m_cstImg = static_cast<CMyImageUI *>(m_pManager->FindControl(CST_IMAGE));
	m_cstImg->SetTag(GetTag());
	m_cstImgLabel->SetMyImage(m_cstImg);

	OnModeChanged();

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

void CGridUI::Notify(TNotifyUI& msg) {
	if (msg.sType == "ModeChanged") {
		if (msg.pSender == m_cstModeBtn) {
			OnModeChanged();
		}
	}
}

void CGridUI::OnModeChanged() {
	DWORD  dwIndex = GetTag();

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

		m_lblReaderNo->SetText("");
		m_dwSelSurReaderIndex = 0;

		m_cstImgLabel->SetText(m_hand_reader->m_lblTemp->GetText());
		m_cstImgLabel->SetTextColor(m_hand_reader->m_lblTemp->GetTextColor());
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
		
		OnSurReaderSelected(0);
	}
	break;

	case CModeButton::Mode_Multiple:
	{
		m_hand_reader->SetVisible(false);
		int  nSel = -1;
		for (DWORD i = 0; i < MAX_READERS_PER_GRID; i++) {
			m_readers[i]->SetVisible(true);
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
		m_CurReaderState->SetVisible(true);
		
		if (nSel < 0)
			nSel = 0;
		OnSurReaderSelected(nSel);
	}
	break;

	default:
		break;
	}

	CDuiString  strText;	
	strText.Format("%s %lu", CFG_GRID_MODE, dwIndex+1);

	g_data.m_CfgData.m_GridCfg[dwIndex].m_dwGridMode = (DWORD)m_cstModeBtn->GetMode();
	DWORD dwDefValue = CModeButton::Mode_Hand;
	g_data.m_cfg->SetConfig(strText, g_data.m_CfgData.m_GridCfg[dwIndex].m_dwGridMode, &dwDefValue);
}

void  CGridUI::SetMode(CModeButton::Mode e) {
	CDuiString strText;
	strText.Format("%lu", (DWORD)e);
	this->SetAttribute("GridMode", strText);
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
	// 如果是手持状态
	if (m_cstModeBtn->GetMode() == CModeButton::Mode_Hand) {
		return;
	}

	assert(m_dwSelSurReaderIndex >= 1);
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
	SetReaderTemp(j, item.m_dwTemp, dwHighAlarm, dwLowAlarm);
	m_readers[j]->m_lblReaderId->SetText(item.m_szReaderId);
	m_readers[j]->m_lblTagId->SetText(item.m_szTagId);

	// 如果当前选中的Reader Index和数据的index一致
	if (m_dwSelSurReaderIndex == j + 1) {
		SetCurReaderTemp(m_readers[j]->m_lblTemp);
		UpdateElapsed();
	}

	m_cstImg->MyInvalidate();
}

void CGridUI::SetCurReaderTemp(CLabelUI * pReaderUI) {
	assert(pReaderUI);	
	m_cstImgLabel->SetText(pReaderUI->GetText());
	m_cstImgLabel->SetTextColor(pReaderUI->GetTextColor());
}

void CGridUI::SetReaderTemp(DWORD j, DWORD  dwTemp, DWORD dwHighAlarm, DWORD dwLowAlarm) {
	//CDuiString  strText;
	//strText.Format("%.2f", dwTemp / 100.0);
	//m_readers[j]->m_lblTemp->SetText(strText);
	m_readers[j]->SetTemp(dwTemp);

	if (dwTemp >= dwHighAlarm) {
		m_readers[j]->m_lblTemp->SetTextColor(HIGH_TEMP_TEXT_COLOR);
	}
	else if (dwTemp <= dwLowAlarm) {
		m_readers[j]->m_lblTemp->SetTextColor(LOW_TEMP_TEXT_COLOR);
	}
	else {
		m_readers[j]->m_lblTemp->SetTextColor(NORMAL_TEMP_TEXT_COLOR);
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
	m_cstImgLabel->SetTextColor(m_readers[dwSelected]->m_lblTemp->GetTextColor());

	m_cstImg->MyInvalidate();
}

// 设置温度字体
void CGridUI::SetFont(int index) {
	if (m_cstImgLabel)
		m_cstImgLabel->SetFont(index);
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

// 一周前的数据删除
void  CGridUI::PruneData() {
	std::vector<TempItem * >::iterator it;
	time_t today_zero_time = GetTodayZeroTime();
	// 一周前0点时分
	time_t tWeekBegin = today_zero_time - 3600 * 24 * 6;

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