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
	}

	g_cfg->GetConfig(CFG_SHOWING_LOWEST_TEMP, g_data.m_dwMyImageMinTemp,       DEFAULT_LOWEST_TEMP);
	g_cfg->GetConfig(CFG_MYIMAGE_LEFT_BLANK,  g_data.m_dwMyImageLeftBlank,     DEFAULT_MYIMAGE_LEFT_BLANK);
	g_cfg->GetConfig(CFG_MYIMAGE_RIGHT_BLANK, g_data.m_dwMyImageRightBlank,    DEFAULT_MYIMAGE_RIGHT_BLANK);
	g_cfg->GetConfig(CFG_COLLECT_INTERVAL,    g_data.m_dwCollectInterval,      DEFAULT_COLLECT_INTERVAL);
	g_cfg->GetConfig(CFG_ONCE_COLLECT_WIDTH,  g_data.m_dwCollectIntervalWidth, DEFAULT_COLLECT_INTERVAL_WIDTH);

	g_thrd_db = new LmnToolkits::Thread();
	if (0 == g_thrd_db) {
		return -1;
	}
	g_thrd_db->Start();

	return 0;
}

int CBusiness::DeInit() {

	if (g_thrd_db) {
		g_thrd_db->Stop();
		delete g_thrd_db;
		g_thrd_db = 0;
	}

	Clear();
	return 0;
}



// 消息处理
void CBusiness::OnMessage(DWORD dwMessageId, const  LmnToolkits::MessageData * pMessageData) {

}