#include "business.h"

CBusiness * CBusiness::pInstance = 0;

CBusiness *  CBusiness::GetInstance() {
	if (0 == pInstance) {
		pInstance = new CBusiness;
	}
	return pInstance;
}

CBusiness::CBusiness() {
	memset(m_szAlarmFile, 0, sizeof(m_szAlarmFile));
}

CBusiness::~CBusiness() {
	Clear();
}

void CBusiness::Clear() {
	if (g_log) {
		g_log->Deinit();
		delete g_log;
		g_log = 0;
	}

	if (g_cfg) {
		g_cfg->Deinit();
		delete g_cfg;
		g_cfg = 0;
	}
}

int CBusiness::Init() {
	DWORD  dwCfgValue = 0;

	g_log = new FileLog();
	if (0 == g_log) {
		return -1;
	}
	g_log->Init(LOG_FILE_NAME);

	g_cfg = new FileConfigEx();
	if (0 == g_cfg) {
		return -1;
	}
	g_cfg->Init(CONFIG_FILE_NAME);

	g_cfg_area = new FileConfigEx();
	if (0 == g_cfg_area) {
		return -1;
	}
	g_cfg_area->Init(AREA_CFG_FILE_NAME);

	g_cfg->GetConfig(CFG_PATIENT_NAME, g_data.m_szPatietName, sizeof(g_data.m_szPatietName), "--");
	g_cfg->GetConfig(CFG_PATIENT_SEX,  g_data.m_szPatietSex,  sizeof(g_data.m_szPatietSex),  "--");
	g_cfg->GetConfig(CFG_PATIENT_AGE,  g_data.m_szPatietAge,  sizeof(g_data.m_szPatietAge),  "--");

	DuiLib::CDuiString  strText;
	for (DWORD i = 0; i < MAX_READERS_COUNT; i++) {
		strText.Format("%s %lu", CFG_READER_NAME, i + 1);
		g_cfg->GetConfig(strText, g_data.m_szReaderName[i], sizeof(g_data.m_szReaderName[i]), "--");

		strText.Format("%s %lu", CFG_LOW_TEMP_ALARM, i + 1);
		g_cfg->GetConfig(strText, g_data.m_dwLowTempAlarm[i], DEFAULT_LOW_TEMP_ALARM);

		strText.Format("%s %lu", CFG_HIGH_TEMP_ALARM, i + 1);
		g_cfg->GetConfig(strText, g_data.m_dwHighTempAlarm[i], DEFAULT_HIGH_TEMP_ALARM);

		strText.Format("%s %lu", CFG_ARGB, i + 1);
		g_cfg->GetConfig(strText, g_data.m_argb[i], g_default_argb[i]);

		strText.Format(CFG_BED_NO " %d", i + 1);
		g_cfg->GetConfig(strText, g_data.m_dwBedNo[i], -1);

		strText.Format(CFG_READER_SWITCH " %d", i + 1);		
		g_cfg->GetConfig(strText, dwCfgValue, TRUE);
		g_data.m_bReaderSwitch[i] = dwCfgValue;
	}

	
	g_cfg->GetConfig(CFG_SHOWING_LOWEST_TEMP, g_data.m_dwMyImageMinTemp,       DEFAULT_LOWEST_TEMP);
	g_cfg->GetConfig(CFG_MYIMAGE_LEFT_BLANK,  g_data.m_dwMyImageLeftBlank,     DEFAULT_MYIMAGE_LEFT_BLANK);
	g_cfg->GetConfig(CFG_MYIMAGE_RIGHT_BLANK, g_data.m_dwMyImageRightBlank,    DEFAULT_MYIMAGE_RIGHT_BLANK);
	g_cfg->GetConfig(CFG_COLLECT_INTERVAL,    g_data.m_dwCollectInterval,      DEFAULT_COLLECT_INTERVAL);
	g_cfg->GetConfig(CFG_ONCE_COLLECT_WIDTH,  g_data.m_dwCollectIntervalWidth, DEFAULT_COLLECT_INTERVAL_WIDTH);
	g_cfg->GetConfig(CFG_AREA_ID_NAME, g_data.m_dwAreaNo, 0);
	g_cfg->GetConfig(CFG_ALARM_VOICE_SWITCH, dwCfgValue, DEFAULT_ALARM_VOICE_SWITCH);
	g_data.m_bAlarmVoiceOff = dwCfgValue;

	// 给默认的床号1, 2, 3
	for (int i = 0; i < MAX_READERS_COUNT; i++) {
		if ( g_data.m_dwBedNo[i] == -1 ) {

			int j = 0;
			for (j = 0; j < MAX_READERS_COUNT; j++) {
				if (g_data.m_dwBedNo[j] == i + 1) {
					break;
				}
			}

			if (j >= MAX_READERS_COUNT) {
				g_data.m_dwBedNo[i] = i + 1;
			}
			else {
				g_data.m_dwBedNo[i] = 0;
			}
		}
	}

	for (int i = 0; i < MAX_AREA_COUNT; i++) {
		TArea area;
		strText.Format(CFG_AREA_NAME " %d", i + 1);
		g_cfg_area->GetConfig(strText, area.szAreaName, sizeof(area.szAreaName), "");
		if (area.szAreaName[0] == '\0') {
			break;
		}

		strText.Format(CFG_AREA_NO " %d", i + 1);
		g_cfg_area->GetConfig(strText, area.dwAreaNo, 0);
		if (0 == area.dwAreaNo || area.dwAreaNo > MAX_AREA_ID) {
			break;
		}

		TArea * pArea = new TArea;
		memcpy(pArea, &area, sizeof(TArea));
		g_vArea.push_back(pArea);
	}

	if ( g_data.m_dwAreaNo > 0 ) {
		std::vector<TArea *>::iterator it;
		for (it = g_vArea.begin(); it != g_vArea.end(); ++it) {
			TArea * pArea = *it;
			if (pArea->dwAreaNo == g_data.m_dwAreaNo ) {
				break;
			}
		}

		// 如果g_dwAreaNo不在g_vArea之内
		if (it == g_vArea.end()) {
			TArea * pArea = new TArea;
			pArea->dwAreaNo = g_data.m_dwAreaNo;
			SNPRINTF(pArea->szAreaName, sizeof(pArea->szAreaName), "(编号：%lu)", g_data.m_dwAreaNo);
			g_vArea.push_back(pArea);
		}
	}

	g_cfg->GetConfig(CFG_ALARM_FILE, g_data.m_szAlarmFilePath, sizeof(g_data.m_szAlarmFilePath), "");
	if (g_data.m_szAlarmFilePath[0] == '\0') {
		GetDefaultAlarmFile(g_data.m_szAlarmFilePath, sizeof(g_data.m_szAlarmFilePath));
	}
	else {
		if (-1 == GetFileAttributes(g_data.m_szAlarmFilePath))
		{
			GetDefaultAlarmFile(g_data.m_szAlarmFilePath, sizeof(g_data.m_szAlarmFilePath));
		}
	}

	g_data.m_bAutoScroll = TRUE;

	g_thrd_work = new LmnToolkits::Thread();
	if (0 == g_thrd_work) {
		return -1;
	}
	g_thrd_work->Start();

	return 0;
}

int CBusiness::DeInit() {

	if (g_thrd_work) {
		g_thrd_work->Stop();
		delete g_thrd_work;
		g_thrd_work = 0;
	}

	Clear();
	return 0;
}

// 调整滑动条
int    CBusiness::UpdateScrollAsyn(DWORD dwDelay /*= 0*/) {
	if (0 == dwDelay) {
		g_thrd_work->PostMessage(this, MSG_UPDATE_SCROLL );
	}
	else {
		g_thrd_work->PostDelayMessage(dwDelay, this, MSG_UPDATE_SCROLL );
	}
	return 0;
}

int    CBusiness::UpdateScroll() {
	::PostMessage(g_hWnd, UM_UPDATE_SCROLL, 0, 0);
	return 0;
}

// 报警
int   CBusiness::AlarmAsyn(const char * szAlarmFile) {
	g_thrd_work->PostMessage(this, MSG_ALARM, new CAlarmParam(szAlarmFile));
	return 0;
}

int   CBusiness::Alarm(const CAlarmParam * pParam) {
	DuiLib::CDuiString strText;

	// 如果关闭报警开关
	if ( g_data.m_bAlarmVoiceOff ) {
		return 0;
	}

	if (m_szAlarmFile[0] == '\0') {
		// open
		strText.Format("open %s", pParam->m_szAlarmFile);
		mciSendString(strText, NULL, 0, 0);

		// play
		strText.Format("play %s", pParam->m_szAlarmFile);
		mciSendString(strText, NULL, 0, 0);
	}
	else {
		// stop
		strText.Format("close %s", m_szAlarmFile);
		mciSendString(strText, NULL, 0, 0);

		// open
		strText.Format("open %s", pParam->m_szAlarmFile);
		mciSendString(strText, NULL, 0, 0);

		// play
		strText.Format("play %s", pParam->m_szAlarmFile);
		mciSendString(strText, NULL, 0, 0);

	}
	strncpy_s(m_szAlarmFile, pParam->m_szAlarmFile, sizeof(m_szAlarmFile));
	return 0;
}


// 消息处理
void CBusiness::OnMessage(DWORD dwMessageId, const  LmnToolkits::MessageData * pMessageData) {
	switch (dwMessageId)
	{
	case MSG_UPDATE_SCROLL:
	{
		UpdateScroll();
	}
	break;

	case MSG_ALARM: 
	{
		CAlarmParam * pParam = (CAlarmParam *)pMessageData;
		Alarm(pParam);
	}
	break;

	default:
		break;
	}
}