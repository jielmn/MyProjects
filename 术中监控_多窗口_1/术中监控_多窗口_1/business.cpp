#include "business.h"

CBusiness * CBusiness::pInstance = 0;

CBusiness *  CBusiness::GetInstance() {
	if (0 == pInstance) {
		pInstance = new CBusiness;
	}
	return pInstance;
}

CBusiness::CBusiness() {

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

	g_cfg->GetConfig(CFG_MAIN_LAYOUT_COLUMNS, g_dwLayoutColumns,  DEFAULT_MAIN_LAYOUT_COLUMNS);
	g_cfg->GetConfig(CFG_MAIN_LAYOUT_ROWS,    g_dwLayoutRows,     DEFAULT_MAIN_LAYOUT_ROWS);
	g_cfg->GetConfig(CFG_TIME_UNIT_WIDTH,     g_dwTimeUnitWidth,  DEFAULT_TIME_UNIT_WIDTH);
	g_cfg->GetConfig(CFG_MAX_POINTS_COUNT,    g_dwMaxPointsCount, DEFAULT_MAX_POINTS_COUNT);
	if (g_dwMaxPointsCount < DEFAULT_MAX_POINTS_COUNT) {
		g_dwMaxPointsCount = DEFAULT_MAX_POINTS_COUNT;
	}
	g_cfg->GetConfig(CFG_MYIMAGE_LEFT_BLANK,  g_dwMyImageLeftBlank,  DEFALUT_MYIMAGE_LEFT_BLANK);
	g_cfg->GetConfig(CFG_MYIMAGE_RIGHT_BLANK, g_dwMyImageRightBlank, DEFALUT_MYIMAGE_RIGHT_BLANK);
	g_cfg->GetConfig(CFG_MYIMAGE_TIME_TEXT_OFFSET_X, g_dwMyImageTimeTextOffsetX, DEFAULT_MYIMAGE_TIME_TEXT_OFFSET_X);
	g_cfg->GetConfig(CFG_MYIMAGE_TIME_TEXT_OFFSET_Y, g_dwMyImageTimeTextOffsetY, DEFAULT_MYIMAGE_TIME_TEXT_OFFSET_Y);
	g_cfg->GetConfig(CFG_MYIMAGE_TEMP_TEXT_OFFSET_X, g_dwMyImageTempTextOffsetX, DEFAULT_MYIMAGE_TEMP_TEXT_OFFSET_X);
	g_cfg->GetConfig(CFG_MYIMAGE_TEMP_TEXT_OFFSET_Y, g_dwMyImageTempTextOffsetY, DEFAULT_MYIMAGE_TEMP_TEXT_OFFSET_Y);

	DuiLib::CDuiString  strText;
	for (int i = 0; i < MAX_GRID_COUNT; i++) {
		strText.Format(CFG_LOW_ALARM " %d", i + 1);
		g_cfg->GetConfig(strText, g_dwLowTempAlarm[i], DEFAULT_LOW_ALARM);

		strText.Format(CFG_HIGH_ALARM " %d", i + 1);
		g_cfg->GetConfig(strText, g_dwHighTempAlarm[i], DEFAULT_HI_ALARM);

		strText.Format(CFG_COLLECT_INTERVAL " %d", i + 1);
		g_cfg->GetConfig(strText, g_dwCollectInterval[i], DEFAULT_COLLECT_INTERVAL);

		strText.Format(CFG_MIN_TEMP " %d", i + 1);
		g_cfg->GetConfig(strText, g_dwMyImageMinTemp[i], DEFAULT_MIN_TEMP);

		strText.Format(CFG_COM_PORT " %d", i + 1);
		g_cfg->GetConfig(strText, g_szComPort[i], MAX_COM_PORT_LENGTH, "");
	}

	g_cfg->GetConfig(CFG_ALARM_FILE, g_szAlarmFilePath, sizeof(g_szAlarmFilePath), "");
	if (g_szAlarmFilePath[0] == '\0') {
		GetDefaultAlarmFile(g_szAlarmFilePath, sizeof(g_szAlarmFilePath));
	}
	else {
		if (-1 == GetFileAttributes(g_szAlarmFilePath))
		{
			GetDefaultAlarmFile(g_szAlarmFilePath, sizeof(g_szAlarmFilePath));
		}
	}

	



	if ( g_dwLayoutRows * g_dwLayoutColumns > MAX_GRID_COUNT ) {
		g_log->Output(ILog::LOG_SEVERITY_ERROR, "too much columns * rows(%lu * %lu) \n", g_dwLayoutColumns, g_dwLayoutRows );
		return -1;
	}

	//g_thrd_db = new LmnToolkits::Thread();
	//if (0 == g_thrd_db) {
	//	return -1;
	//}
	//g_thrd_db->Start();

	return 0;
}

int CBusiness::DeInit() {

	//if (g_thrd_db) {
	//	g_thrd_db->Stop();
	//	delete g_thrd_db;
	//	g_thrd_db = 0;
	//}

	Clear();
	return 0;
}

// 调整滑动条
int  CBusiness::UpdateScrollAsyn(int nIndex) {
	return 0;
}

int   CBusiness::AlarmAsyn(const char * szAlarmFile) {
	// g_thrd_background->PostMessage(this, MSG_ALARM, new CAlarmParam(szAlarmFile));
	return 0;
}

int   CBusiness::Alarm(const CAlarmParam * pParam) {
	DuiLib::CDuiString strText;

	//if (m_szAlarmFile[0] == '\0') {
	//	// open
	//	strText.Format("open %s", pParam->m_szAlarmFile);
	//	mciSendString(strText, NULL, 0, 0);

	//	// play
	//	strText.Format("play %s", pParam->m_szAlarmFile);
	//	mciSendString(strText, NULL, 0, 0);
	//}
	//else {
	//	// stop
	//	strText.Format("close %s", m_szAlarmFile);
	//	mciSendString(strText, NULL, 0, 0);

	//	// open
	//	strText.Format("open %s", pParam->m_szAlarmFile);
	//	mciSendString(strText, NULL, 0, 0);

	//	// play
	//	strText.Format("play %s", pParam->m_szAlarmFile);
	//	mciSendString(strText, NULL, 0, 0);

	//}
	//strncpy_s(m_szAlarmFile, pParam->m_szAlarmFile, sizeof(m_szAlarmFile));
	return 0;
}


// 消息处理
void CBusiness::OnMessage(DWORD dwMessageId, const  LmnToolkits::MessageData * pMessageData) {

}