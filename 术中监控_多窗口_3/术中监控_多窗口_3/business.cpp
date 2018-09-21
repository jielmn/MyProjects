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
	if (g_data.m_log) {
		g_data.m_log->Deinit();
		delete g_data.m_log;
		g_data.m_log = 0;
	}

	if (g_data.m_cfg) {
		g_data.m_cfg->Deinit();
		delete g_data.m_cfg;
		g_data.m_cfg = 0;
	}

	ClearVector(g_vArea);
}

int CBusiness::Init() {
	DuiLib::CDuiString  strText;

	g_data.m_log = new FileLog();
	if (0 == g_data.m_log) {
		return -1;
	}
#ifdef _DEBUG
	g_data.m_log->Init(LOG_FILE_NAME);
#else
	g_data.m_log->Init(LOG_FILE_NAME, 0, ILog::LOG_SEVERITY_INFO, TRUE);
#endif

	g_data.m_cfg = new FileConfigEx();
	if (0 == g_data.m_cfg) {
		return -1;
	}
	g_data.m_cfg->Init(CONFIG_FILE_NAME);

	DWORD  dwCfgValue = 0;
	g_data.m_cfg->GetConfig(CFG_MAIN_LAYOUT_COLUMNS, g_data.m_dwLayoutColumns, DEFAULT_MAIN_LAYOUT_COLUMNS);
	g_data.m_cfg->GetConfig(CFG_MAIN_LAYOUT_ROWS, g_data.m_dwLayoutRows, DEFAULT_MAIN_LAYOUT_ROWS);
	g_data.m_cfg->GetConfig(CFG_AREA_ID_NAME, g_data.m_dwAreaNo, 0);
	g_data.m_cfg->GetConfig(CFG_ALARM_VOICE_SWITCH, dwCfgValue, DEFAULT_ALARM_VOICE_SWITCH);
	g_data.m_bAlarmVoiceOff = dwCfgValue;
	g_data.m_cfg->GetConfig(CFG_SKIN, g_data.m_dwSkinIndex, DEFAULT_SKIN);
	g_data.m_skin.SetSkin( (CMySkin::ENUM_SKIN)g_data.m_dwSkinIndex );
	
	for (int i = 0; i < MAX_GRID_COUNT; i++) {
		
	}

	// 校验
	if (g_data.m_dwLayoutColumns == 0 || g_data.m_dwLayoutRows == 0 || g_data.m_dwLayoutColumns * g_data.m_dwLayoutRows > MAX_GRID_COUNT) {
		g_data.m_dwLayoutColumns = DEFAULT_MAIN_LAYOUT_COLUMNS;
		g_data.m_dwLayoutRows = DEFAULT_MAIN_LAYOUT_ROWS;
	}

	if ( g_data.m_dwSkinIndex > 1 ) {
		g_data.m_dwSkinIndex = 0;
	}

	// 区号列表
	IConfig * cfg_area = new FileConfigEx();
	if (0 == cfg_area) {
		return -1;
	}
	cfg_area->Init(AREA_CFG_FILE_NAME);

	for (int i = 0; i < MAX_AREA_COUNT; i++) {
		TArea area;
		strText.Format(CFG_AREA_NAME " %d", i + 1);
		cfg_area->GetConfig(strText, area.szAreaName, sizeof(area.szAreaName), "");
		if (area.szAreaName[0] == '\0') {
			break;
		}

		strText.Format(CFG_AREA_NO " %d", i + 1);
		cfg_area->GetConfig(strText, area.dwAreaNo, 0);
		if (0 == area.dwAreaNo || area.dwAreaNo > MAX_AREA_ID) {
			break;
		}

		TArea * pArea = new TArea;
		memcpy(pArea, &area, sizeof(TArea));
		g_vArea.push_back(pArea);
	}
	cfg_area->Deinit();
	delete cfg_area;

	if ( g_data.m_dwAreaNo > 0) {
		std::vector<TArea *>::iterator it;
		for (it = g_vArea.begin(); it != g_vArea.end(); ++it) {
			TArea * pArea = *it;
			if (pArea->dwAreaNo == g_data.m_dwAreaNo) {
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
	// END OF 区号

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



// 消息处理
void CBusiness::OnMessage(DWORD dwMessageId, const  LmnToolkits::MessageData * pMessageData) {

}