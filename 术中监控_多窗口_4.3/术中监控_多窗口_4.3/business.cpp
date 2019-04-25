#include "business.h"
#include "main.h"

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

	/*********************    配置项    ***********************/
	g_data.m_cfg->GetConfig(CFG_MAIN_LAYOUT_COLUMNS, g_data.m_CfgData.m_dwLayoutColumns, DEFAULT_MAIN_LAYOUT_COLUMNS);
	g_data.m_cfg->GetConfig(CFG_MAIN_LAYOUT_ROWS,    g_data.m_CfgData.m_dwLayoutRows,    DEFAULT_MAIN_LAYOUT_ROWS);
	g_data.m_cfg->GetConfig(CFG_MAIN_LAYOUT_GRIDS_COUNT, g_data.m_CfgData.m_dwLayoutGridsCnt, DEFAULT_MAIN_LAYOUT_GRIDS_COUNT);

	// g_data.m_CfgData.m_dwLayoutGridsCnt范围：4~60
	if ( g_data.m_CfgData.m_dwLayoutGridsCnt > MAX_GRID_COUNT ) {
		g_data.m_CfgData.m_dwLayoutGridsCnt = MAX_GRID_COUNT;
	}
	else if (g_data.m_CfgData.m_dwLayoutGridsCnt < DEFAULT_MAIN_LAYOUT_GRIDS_COUNT) {
		g_data.m_CfgData.m_dwLayoutGridsCnt = DEFAULT_MAIN_LAYOUT_GRIDS_COUNT;
	}

	// g_data.m_CfgData.m_dwLayoutColumns必须大于等于2，小于等于5
	// g_data.m_CfgData.m_dwLayoutRows必须大于等于2，小于等于5
	if ( g_data.m_CfgData.m_dwLayoutColumns <= 1 || g_data.m_CfgData.m_dwLayoutRows <= 1 ) {
		g_data.m_CfgData.m_dwLayoutColumns = DEFAULT_MAIN_LAYOUT_COLUMNS;
		g_data.m_CfgData.m_dwLayoutRows = DEFAULT_MAIN_LAYOUT_ROWS;
	}
	if ( g_data.m_CfgData.m_dwLayoutColumns > MAX_COLUMNS_COUNT) {
		g_data.m_CfgData.m_dwLayoutColumns = MAX_COLUMNS_COUNT;
	}
	if (g_data.m_CfgData.m_dwLayoutRows > MAX_ROWS_COUNT) {
		g_data.m_CfgData.m_dwLayoutRows = MAX_ROWS_COUNT;
	}

	// 格子顺序
	GetGridsOrderCfg();

	// 报警声音开关
	g_data.m_cfg->GetBooleanConfig(CFG_ALARM_VOICE_SWITCH, g_data.m_CfgData.m_bAlarmVoiceOff, DEFAULT_ALARM_VOICE_SWITCH);
	// 自动保存excel
	g_data.m_cfg->GetBooleanConfig(CFG_AUTO_SAVE_EXCEL, g_data.m_CfgData.m_bAutoSaveExcel, FALSE);
	// 十字锚
	g_data.m_cfg->GetBooleanConfig(CFG_CROSS_ANCHOR, g_data.m_CfgData.m_bCrossAnchor, FALSE);

	// 手持最低显示温度
	g_data.m_cfg->GetConfig(CFG_HAND_MIN_TEMP_IN_SHOW, g_data.m_CfgData.m_dwHandReaderMinTemp, DEFAULT_MIN_TEMP_IN_SHOW);
	if (g_data.m_CfgData.m_dwHandReaderMinTemp < MIN_TEMP_IN_SHOW) {
		g_data.m_CfgData.m_dwHandReaderMinTemp = MIN_TEMP_IN_SHOW;
	}

	// 手持最高显示温度
	g_data.m_cfg->GetConfig(CFG_HAND_MAX_TEMP_IN_SHOW, g_data.m_CfgData.m_dwHandReaderMaxTemp, DEFAULT_MAX_TEMP_IN_SHOW);
	if (g_data.m_CfgData.m_dwHandReaderMaxTemp > MAX_TEMP_IN_SHOW) {
		g_data.m_CfgData.m_dwHandReaderMaxTemp = MAX_TEMP_IN_SHOW;
	}

	if (g_data.m_CfgData.m_dwHandReaderMinTemp >= g_data.m_CfgData.m_dwHandReaderMaxTemp) {
		g_data.m_CfgData.m_dwHandReaderMinTemp = DEFAULT_MIN_TEMP_IN_SHOW;
		g_data.m_CfgData.m_dwHandReaderMaxTemp = DEFAULT_MAX_TEMP_IN_SHOW;
	}

	// 手持低温报警
	g_data.m_cfg->GetConfig(CFG_HAND_LOW_TEMP_ALARM, g_data.m_CfgData.m_dwHandReaderLowTempAlarm, DEFAULT_LOW_TEMP_ALARM);
	if (g_data.m_CfgData.m_dwHandReaderLowTempAlarm < MIN_TEMP_IN_SHOW) {
		g_data.m_CfgData.m_dwHandReaderLowTempAlarm = MIN_TEMP_IN_SHOW;
	}

	// 手持高温报警
	g_data.m_cfg->GetConfig(CFG_HAND_HIGH_TEMP_ALARM, g_data.m_CfgData.m_dwHandReaderHighTempAlarm, DEFAULT_HIGH_TEMP_ALARM);
	if (g_data.m_CfgData.m_dwHandReaderHighTempAlarm > MAX_TEMP_IN_SHOW) {
		g_data.m_CfgData.m_dwHandReaderHighTempAlarm = MAX_TEMP_IN_SHOW;
	}

	if ( g_data.m_CfgData.m_dwHandReaderLowTempAlarm >= g_data.m_CfgData.m_dwHandReaderHighTempAlarm) {
		g_data.m_CfgData.m_dwHandReaderLowTempAlarm = DEFAULT_LOW_TEMP_ALARM;
		g_data.m_CfgData.m_dwHandReaderHighTempAlarm = DEFAULT_HIGH_TEMP_ALARM;
	}

	// 病区
	GetAreaCfg();

	GetGridsCfg();

	/******************** end 配置项 **********************/


	/***************     数据库    ******************/
	m_sqlite.InitDb();
	/***************   end 数据库    ******************/


	/******** 线程 ********/
	g_data.m_thrd_launch = new LmnToolkits::Thread();
	if (0 == g_data.m_thrd_launch) {
		return -1;
	}
	g_data.m_thrd_launch->Start();

	g_data.m_thrd_sqlite = new LmnToolkits::Thread();
	if (0 == g_data.m_thrd_sqlite) {
		return -1;
	}
	g_data.m_thrd_sqlite->Start();

	return 0;
}

void CBusiness::GetGridsOrderCfg() {
	char buf[8192];

	g_data.m_cfg->GetConfig(CFG_GRIDS_ORDER, buf, sizeof(buf), "");
	SplitString  split;
	split.Split(buf, ",");

	// 是否有效的顺序配置
	BOOL  bValidOrder = TRUE;

	DWORD  dwGridsCount = g_data.m_CfgData.m_dwLayoutGridsCnt;
	// 个数必须为MAX_GRID_COUNT个
	if ( split.Size() != dwGridsCount) {
		bValidOrder = FALSE;
	}
	else {
		for (DWORD i = 0; i < dwGridsCount; i++) {
			DWORD  dwTemp = 0;
			// 是数字
			if (1 == sscanf(split[i], "%lu", &dwTemp)) {
				if (dwTemp == 0 || dwTemp > dwGridsCount) {
					bValidOrder = FALSE;
					break;
				}
				g_data.m_CfgData.m_GridOrder[i] = dwTemp - 1;
			}
			// 不是数字
			else {
				bValidOrder = FALSE;
				break;
			}
		}

		// 再检查
		if (bValidOrder) {
			for (DWORD i = 0; i < dwGridsCount - 1; i++) {
				for (DWORD j = i + 1; j < dwGridsCount; j++) {
					// 重复
					if (g_data.m_CfgData.m_GridOrder[i] == g_data.m_CfgData.m_GridOrder[j]) {
						bValidOrder = FALSE;
						break;
					}
				}
				if (!bValidOrder) {
					break;
				}
			}
		}
	}

	// 如果没有正确的顺序配置，取默认的顺序
	if (!bValidOrder) {
		ResetGridOrder();
	}
}

// 病区配置
void CBusiness::GetAreaCfg() {
	CDuiString  strText;

	g_data.m_cfg->GetConfig(CFG_AREA_ID_NAME, g_data.m_CfgData.m_dwAreaNo, 0);

	// 区号列表
	IConfig * cfg_area = new FileConfigEx();
	if (0 == cfg_area) {
		return;
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

	if (g_data.m_CfgData.m_dwAreaNo > 0) {
		std::vector<TArea *>::iterator it;
		for (it = g_vArea.begin(); it != g_vArea.end(); ++it) {
			TArea * pArea = *it;
			if (pArea->dwAreaNo == g_data.m_CfgData.m_dwAreaNo) {
				break;
			}
		}

		// 如果g_dwAreaNo不在g_vArea之内
		if (it == g_vArea.end()) {
			TArea * pArea = new TArea;
			pArea->dwAreaNo = g_data.m_CfgData.m_dwAreaNo;
			SNPRINTF(pArea->szAreaName, sizeof(pArea->szAreaName), "(编号：%lu)", g_data.m_CfgData.m_dwAreaNo);
			g_vArea.push_back(pArea);
		}
	}
	// END OF 区号
}

// 床位的配置
void CBusiness::GetGridsCfg() {
	CDuiString  strText;

	for (int i = 0; i < MAX_GRID_COUNT; i++) {
		strText.Format("%s %lu", CFG_GRID_SWITCH, i + 1);
		g_data.m_cfg->GetBooleanConfig(strText, g_data.m_CfgData.m_GridCfg[i].m_bSwitch, DEFAULT_GRID_SWITCH);

		strText.Format("%s %lu", CFG_COLLECT_INTERVAL, i + 1);
		g_data.m_cfg->GetConfig(strText, g_data.m_CfgData.m_GridCfg[i].m_dwCollectInterval, 0);
		if (g_data.m_CfgData.m_GridCfg[i].m_dwCollectInterval > MAX_COLLECT_INTERVAL_INDEX) {
			g_data.m_CfgData.m_GridCfg[i].m_dwCollectInterval = 0;
		}

		strText.Format("%s %lu", CFG_GRID_MIN_TEMP, i + 1);
		g_data.m_cfg->GetConfig(strText, g_data.m_CfgData.m_GridCfg[i].m_dwMinTemp, DEFAULT_MIN_TEMP_IN_SHOW);
		if (g_data.m_CfgData.m_GridCfg[i].m_dwMinTemp < MIN_TEMP_IN_SHOW) {
			g_data.m_CfgData.m_GridCfg[i].m_dwMinTemp = MIN_TEMP_IN_SHOW;
		}

		strText.Format("%s %lu", CFG_GRID_MAX_TEMP, i + 1);
		g_data.m_cfg->GetConfig(strText, g_data.m_CfgData.m_GridCfg[i].m_dwMaxTemp, DEFAULT_MAX_TEMP_IN_SHOW);
		if (g_data.m_CfgData.m_GridCfg[i].m_dwMaxTemp > MAX_TEMP_IN_SHOW) {
			g_data.m_CfgData.m_GridCfg[i].m_dwMaxTemp = MAX_TEMP_IN_SHOW;
		}

		if (g_data.m_CfgData.m_GridCfg[i].m_dwMinTemp >= g_data.m_CfgData.m_GridCfg[i].m_dwMaxTemp) {
			g_data.m_CfgData.m_GridCfg[i].m_dwMinTemp = DEFAULT_MIN_TEMP_IN_SHOW;
			g_data.m_CfgData.m_GridCfg[i].m_dwMaxTemp = DEFAULT_MAX_TEMP_IN_SHOW;
		}

		for (int j = 0; j < MAX_READERS_PER_GRID; j++) {
			strText.Format("%s %lu %lu", CFG_READER_SWITCH, i + 1, j + 1);
			g_data.m_cfg->GetBooleanConfig(strText, g_data.m_CfgData.m_GridCfg[i].m_ReaderCfg[j].m_bSwitch, DEFAULT_READER_SWITCH);

			strText.Format("%s %lu %lu", CFG_LOW_TEMP_ALARM, i + 1, j + 1);
			g_data.m_cfg->GetConfig(strText, g_data.m_CfgData.m_GridCfg[i].m_ReaderCfg[j].m_dwLowTempAlarm, DEFAULT_LOW_TEMP_ALARM);
			if (g_data.m_CfgData.m_GridCfg[i].m_ReaderCfg[j].m_dwLowTempAlarm < LOW_TEMP_ALARM) {
				g_data.m_CfgData.m_GridCfg[i].m_ReaderCfg[j].m_dwLowTempAlarm = LOW_TEMP_ALARM;
			}

			strText.Format("%s %lu %lu", CFG_HIGH_TEMP_ALARM, i + 1, j + 1);
			g_data.m_cfg->GetConfig(strText, g_data.m_CfgData.m_GridCfg[i].m_ReaderCfg[j].m_dwHighTempAlarm, DEFAULT_HIGH_TEMP_ALARM);
			if (g_data.m_CfgData.m_GridCfg[i].m_ReaderCfg[j].m_dwHighTempAlarm > HIGH_TEMP_ALARM) {
				g_data.m_CfgData.m_GridCfg[i].m_ReaderCfg[j].m_dwHighTempAlarm = HIGH_TEMP_ALARM;
			}

			if (g_data.m_CfgData.m_GridCfg[i].m_ReaderCfg[j].m_dwLowTempAlarm >= g_data.m_CfgData.m_GridCfg[i].m_ReaderCfg[j].m_dwHighTempAlarm) {
				g_data.m_CfgData.m_GridCfg[i].m_ReaderCfg[j].m_dwLowTempAlarm = DEFAULT_LOW_TEMP_ALARM;
				g_data.m_CfgData.m_GridCfg[i].m_ReaderCfg[j].m_dwHighTempAlarm = DEFAULT_HIGH_TEMP_ALARM;
			}
		}
	}
}

int CBusiness::DeInit() {

	if (g_data.m_thrd_launch) {
		g_data.m_thrd_launch->Stop();
		delete g_data.m_thrd_launch;
		g_data.m_thrd_launch = 0;
	}

	if (g_data.m_thrd_sqlite) {
		g_data.m_thrd_sqlite->Stop();
		delete g_data.m_thrd_sqlite;
		g_data.m_thrd_sqlite = 0;
	}

	Clear();

	m_sqlite.DeinitDb();

	return 0;
}

void CBusiness::InitSigslot(CDuiFrameWnd * pMainWnd) {
	return;
}


// 消息处理
void CBusiness::OnMessage(DWORD dwMessageId, const  LmnToolkits::MessageData * pMessageData) {
	switch (dwMessageId)
	{
	case 0:
	{

	}
	break;

	default:
		break;
	}
}