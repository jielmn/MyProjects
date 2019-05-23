#include "business.h"
#include "main.h"

#define   CHECK_SQLITE_INTERVAL_TIME       10000


CBusiness * CBusiness::pInstance = 0;

CBusiness *  CBusiness::GetInstance() {
	if (0 == pInstance) {
		pInstance = new CBusiness;
	}
	return pInstance;
}

CBusiness::CBusiness() {
	m_launch.m_sigStatus.connect(this, &CBusiness::OnStatus);
	m_launch.m_sigReaderTemp.connect(this, &CBusiness::OnReaderTemp);
	m_launch.m_sigHandReaderTemp.connect(this, &CBusiness::OnHandReaderTemp);
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

	std::map<std::string, TagPName*>::iterator it;
	for (it = m_tag_patient_name.begin(); it != m_tag_patient_name.end(); ++it) {
		if ( it->second == 0 ) {
			continue;
		}

		delete it->second;
	}
	m_tag_patient_name.clear();
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

	/*********************    ������    ***********************/
	g_data.m_cfg->GetConfig(CFG_MAIN_LAYOUT_COLUMNS, g_data.m_CfgData.m_dwLayoutColumns, DEFAULT_MAIN_LAYOUT_COLUMNS);
	g_data.m_cfg->GetConfig(CFG_MAIN_LAYOUT_ROWS,    g_data.m_CfgData.m_dwLayoutRows,    DEFAULT_MAIN_LAYOUT_ROWS);
	g_data.m_cfg->GetConfig(CFG_MAIN_LAYOUT_GRIDS_COUNT, g_data.m_CfgData.m_dwLayoutGridsCnt, DEFAULT_MAIN_LAYOUT_GRIDS_COUNT);

	// g_data.m_CfgData.m_dwLayoutGridsCnt��Χ��4~60
	if ( g_data.m_CfgData.m_dwLayoutGridsCnt > MAX_GRID_COUNT ) {
		g_data.m_CfgData.m_dwLayoutGridsCnt = MAX_GRID_COUNT;
	}
	else if (g_data.m_CfgData.m_dwLayoutGridsCnt < DEFAULT_MAIN_LAYOUT_GRIDS_COUNT) {
		g_data.m_CfgData.m_dwLayoutGridsCnt = DEFAULT_MAIN_LAYOUT_GRIDS_COUNT;
	}

	// g_data.m_CfgData.m_dwLayoutColumns������ڵ���2��С�ڵ���5
	// g_data.m_CfgData.m_dwLayoutRows������ڵ���2��С�ڵ���5
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

	// ����˳��
	GetGridsOrderCfg();

	// ������������
	g_data.m_cfg->GetBooleanConfig(CFG_ALARM_VOICE_SWITCH, g_data.m_CfgData.m_bAlarmVoiceOff, DEFAULT_ALARM_VOICE_SWITCH);
	// �Զ�����excel
	g_data.m_cfg->GetBooleanConfig(CFG_AUTO_SAVE_EXCEL, g_data.m_CfgData.m_bAutoSaveExcel, FALSE);
	// ʮ��ê
	g_data.m_cfg->GetBooleanConfig(CFG_CROSS_ANCHOR, g_data.m_CfgData.m_bCrossAnchor, FALSE);

	// �ֳ������ʾ�¶�
	g_data.m_cfg->GetConfig(CFG_HAND_MIN_TEMP_IN_SHOW, g_data.m_CfgData.m_dwHandReaderMinTemp, DEFAULT_MIN_TEMP_IN_SHOW);
	if (g_data.m_CfgData.m_dwHandReaderMinTemp < MIN_TEMP_IN_SHOW) {
		g_data.m_CfgData.m_dwHandReaderMinTemp = MIN_TEMP_IN_SHOW;
	}

	// �ֳ������ʾ�¶�
	g_data.m_cfg->GetConfig(CFG_HAND_MAX_TEMP_IN_SHOW, g_data.m_CfgData.m_dwHandReaderMaxTemp, DEFAULT_MAX_TEMP_IN_SHOW);
	if (g_data.m_CfgData.m_dwHandReaderMaxTemp > MAX_TEMP_IN_SHOW) {
		g_data.m_CfgData.m_dwHandReaderMaxTemp = MAX_TEMP_IN_SHOW;
	}

	if (g_data.m_CfgData.m_dwHandReaderMinTemp >= g_data.m_CfgData.m_dwHandReaderMaxTemp) {
		g_data.m_CfgData.m_dwHandReaderMinTemp = DEFAULT_MIN_TEMP_IN_SHOW;
		g_data.m_CfgData.m_dwHandReaderMaxTemp = DEFAULT_MAX_TEMP_IN_SHOW;
	}

	// �ֳֵ��±���
	g_data.m_cfg->GetConfig(CFG_HAND_LOW_TEMP_ALARM, g_data.m_CfgData.m_dwHandReaderLowTempAlarm, DEFAULT_LOW_TEMP_ALARM);

	// �ֳָ��±���
	g_data.m_cfg->GetConfig(CFG_HAND_HIGH_TEMP_ALARM, g_data.m_CfgData.m_dwHandReaderHighTempAlarm, DEFAULT_HIGH_TEMP_ALARM);

	if ( g_data.m_CfgData.m_dwHandReaderLowTempAlarm >= g_data.m_CfgData.m_dwHandReaderHighTempAlarm) {
		g_data.m_CfgData.m_dwHandReaderLowTempAlarm = DEFAULT_LOW_TEMP_ALARM;
		g_data.m_CfgData.m_dwHandReaderHighTempAlarm = DEFAULT_HIGH_TEMP_ALARM;
	}

	// ����
	GetAreaCfg();

	GetGridsCfg();

	// �������˿�
	g_data.m_cfg->GetConfig(CFG_LAUNCH_COM_PORT, g_data.m_szLaunchPort, sizeof(g_data.m_szLaunchPort), "");
	/******************** end ������ **********************/


	/***************     ���ݿ�    ******************/
	m_sqlite.InitDb();
	/***************   end ���ݿ�    ******************/


	/******** �߳� ********/
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

	g_data.m_thrd_work = new LmnToolkits::Thread();
	if (0 == g_data.m_thrd_work) {
		return -1;
	}
	g_data.m_thrd_work->Start();

	// ������ʱ���tag patient name�Ƿ����
	CheckSqliteAsyn();
	return 0;
}

void CBusiness::GetGridsOrderCfg() {
	char buf[8192];

	g_data.m_cfg->GetConfig(CFG_GRIDS_ORDER, buf, sizeof(buf), "");
	SplitString  split;
	split.Split(buf, ",");

	// �Ƿ���Ч��˳������
	BOOL  bValidOrder = TRUE;

	DWORD  dwGridsCount = g_data.m_CfgData.m_dwLayoutGridsCnt;
	// ��������ΪMAX_GRID_COUNT��
	if ( split.Size() != dwGridsCount) {
		bValidOrder = FALSE;
	}
	else {
		for (DWORD i = 0; i < dwGridsCount; i++) {
			DWORD  dwTemp = 0;
			// ������
			if (1 == sscanf(split[i], "%lu", &dwTemp)) {
				if (dwTemp == 0 || dwTemp > dwGridsCount) {
					bValidOrder = FALSE;
					break;
				}
				g_data.m_CfgData.m_GridOrder[i] = dwTemp - 1;
			}
			// ��������
			else {
				bValidOrder = FALSE;
				break;
			}
		}

		// �ټ��
		if (bValidOrder) {
			for (DWORD i = 0; i < dwGridsCount - 1; i++) {
				for (DWORD j = i + 1; j < dwGridsCount; j++) {
					// �ظ�
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

	// ���û����ȷ��˳�����ã�ȡĬ�ϵ�˳��
	if (!bValidOrder) {
		ResetGridOrder();
	}
}

// ��������
void CBusiness::GetAreaCfg() {
	CDuiString  strText;

	g_data.m_cfg->GetConfig(CFG_AREA_ID_NAME, g_data.m_CfgData.m_dwAreaNo, 0);

	// �����б�
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

		// ���g_dwAreaNo����g_vArea֮��
		if (it == g_vArea.end()) {
			TArea * pArea = new TArea;
			pArea->dwAreaNo = g_data.m_CfgData.m_dwAreaNo;
			SNPRINTF(pArea->szAreaName, sizeof(pArea->szAreaName), "(��ţ�%lu)", g_data.m_CfgData.m_dwAreaNo);
			g_vArea.push_back(pArea);
		}
	}
	// END OF ����
}

// ��λ������
void CBusiness::GetGridsCfg() {
	CDuiString  strText;
	DWORD  dwValue = 0;

	for (int i = 0; i < MAX_GRID_COUNT; i++) {

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

		strText.Format("%s %lu", CFG_GRID_MODE, i + 1);
		dwValue = CModeButton::Mode_Hand;
		g_data.m_cfg->GetConfig(strText, g_data.m_CfgData.m_GridCfg[i].m_dwGridMode, dwValue);
		if (g_data.m_CfgData.m_GridCfg[i].m_dwGridMode > CModeButton::Mode_Multiple) {
			g_data.m_CfgData.m_GridCfg[i].m_dwGridMode = CModeButton::Mode_Hand;
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

			strText.Format("%s %lu %lu", CFG_READER_NAME, i + 1, j + 1);
			g_data.m_cfg->GetConfig(strText, g_data.m_CfgData.m_GridCfg[i].m_ReaderCfg[j].m_szReaderName, MAX_READER_NAME_LENGTH, g_BodyParts[j] );
		}


		// hand reader
		strText.Format("%s %lu", CFG_HAND_READER_LOW_TEMP_ALARM, i + 1);
		g_data.m_cfg->GetConfig(strText, g_data.m_CfgData.m_GridCfg[i].m_HandReaderCfg.m_dwLowTempAlarm, DEFAULT_LOW_TEMP_ALARM);
		if (g_data.m_CfgData.m_GridCfg[i].m_HandReaderCfg.m_dwLowTempAlarm < LOW_TEMP_ALARM) {
			g_data.m_CfgData.m_GridCfg[i].m_HandReaderCfg.m_dwLowTempAlarm = LOW_TEMP_ALARM;
		}

		strText.Format("%s %lu", CFG_HAND_READER_HIGH_TEMP_ALARM, i + 1);
		g_data.m_cfg->GetConfig(strText, g_data.m_CfgData.m_GridCfg[i].m_HandReaderCfg.m_dwHighTempAlarm, DEFAULT_HIGH_TEMP_ALARM);
		if (g_data.m_CfgData.m_GridCfg[i].m_HandReaderCfg.m_dwHighTempAlarm > HIGH_TEMP_ALARM) {
			g_data.m_CfgData.m_GridCfg[i].m_HandReaderCfg.m_dwHighTempAlarm = HIGH_TEMP_ALARM;
		}

		if (g_data.m_CfgData.m_GridCfg[i].m_HandReaderCfg.m_dwLowTempAlarm >= g_data.m_CfgData.m_GridCfg[i].m_HandReaderCfg.m_dwHighTempAlarm) {
			g_data.m_CfgData.m_GridCfg[i].m_HandReaderCfg.m_dwLowTempAlarm = DEFAULT_LOW_TEMP_ALARM;
			g_data.m_CfgData.m_GridCfg[i].m_HandReaderCfg.m_dwHighTempAlarm = DEFAULT_HIGH_TEMP_ALARM;
		}

		strText.Format("%s %lu", CFG_HAND_READER_NAME, i + 1);
		g_data.m_cfg->GetConfig(strText, g_data.m_CfgData.m_GridCfg[i].m_HandReaderCfg.m_szReaderName, MAX_READER_NAME_LENGTH, g_BodyPart);
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

	if (g_data.m_thrd_work) {
		g_data.m_thrd_work->Stop();
		delete g_data.m_thrd_work;
		g_data.m_thrd_work = 0;
	}

	Clear();

	m_sqlite.DeinitDb();

	return 0;
}

void CBusiness::InitSigslot(CDuiFrameWnd * pMainWnd) {
	m_sigLanchStatus.connect(pMainWnd, &CDuiFrameWnd::OnLauchStatusNotify);
	m_sigTrySurReader.connect(pMainWnd, &CDuiFrameWnd::OnTrySurReaderNotify);
	m_sigSurReaderStatus.connect(pMainWnd, &CDuiFrameWnd::OnSurReaderStatusNotify);
	m_sigSurReaderTemp.connect(pMainWnd, &CDuiFrameWnd::OnSurReaderTempNotify);
	m_sigQueyTemp.connect(pMainWnd, &CDuiFrameWnd::OnQueryTempRetNotify);	
	m_sigHandReaderTemp.connect(pMainWnd, &CDuiFrameWnd::OnHandReaderTempNotify);
	m_sigAllHandTagTempData.connect(pMainWnd, &CDuiFrameWnd::OnAllHandTagTempDataNotify);
	m_prepared.connect(pMainWnd, &CDuiFrameWnd::OnPreparedNotify);
	return;
}

// Ӳ���Ķ���������������״̬
void  CBusiness::CheckLaunchAsyn() {
	g_data.m_thrd_launch->PostMessage(this, MSG_CHECK_LAUNCH_STATUS, 0, TRUE);
}

void  CBusiness::CheckLaunch() {
	m_launch.CheckStatus();
}

// �������ӽ�����(�����øĶ���grid count�仯)
void   CBusiness::RestartLaunchAsyn() {
	g_data.m_thrd_launch->DeleteMessages();
	g_data.m_thrd_launch->PostMessage(this, MSG_RESTART_LAUNCH);
}

void   CBusiness::RestartLaunch() {
	m_launch.Reconnect();
}

// ��ȡ�¶�
void  CBusiness::GetGridTemperatureAsyn(DWORD  dwIndex, DWORD dwDelay /*= 0*/) {
	if ( m_launch.GetStatus() == CLmnSerialPort::CLOSE ) {
		return;
	}

	if (0 == dwDelay) {
		g_data.m_thrd_launch->PostMessage(this, MSG_GET_GRID_TEMP + dwIndex, new CGetGridTempParam(dwIndex), TRUE);
	}
	else {
		g_data.m_thrd_launch->PostDelayMessage(dwDelay, this, MSG_GET_GRID_TEMP + dwIndex, new CGetGridTempParam(dwIndex), TRUE);
	}
}

void  CBusiness::GetGridTemperature(const CGetGridTempParam * pParam) {
	DWORD   i      = pParam->m_dwIndex;
	GridCfg cfg    = g_data.m_CfgData.m_GridCfg[i];
	BYTE    byArea = (BYTE)g_data.m_CfgData.m_dwAreaNo;
	DWORD  dwDelay = GetCollectInterval(cfg.m_dwCollectInterval) * 1000;

	// ������ֳֶ�����ģʽ
	if ( cfg.m_dwGridMode == CModeButton::Mode_Hand ) {
		GetGridTemperatureAsyn(i, DEF_GET_TEMPERATURE_DELAY);
		return;
	}

	// ���Ƿ�ȡ�����ݽ����ΪFalse
	memset( m_bSurReaderTemp[i], 0, sizeof(BOOL) * MAX_READERS_PER_GRID );

	// ����ǵ����������ж�����
	if ( cfg.m_dwGridMode == CModeButton::Mode_Single ) {
		GetGridTemperature( i, 0, byArea, cfg.m_dwGridMode);
		GetGridTemperatureAsyn(i, dwDelay);
		return;
	}

	// ����Ƕ���������ж�����
	for ( DWORD j = 0; j < MAX_READERS_PER_GRID; j++) {
		// ���Reader�����Ѿ���
		if ( cfg.m_ReaderCfg[j].m_bSwitch )
			GetGridTemperature( i,j, byArea, cfg.m_dwGridMode );
	}
	GetGridTemperatureAsyn(i, dwDelay);
}

void  CBusiness::GetGridTemperature(DWORD i, DWORD j, BYTE byArea, DWORD  dwOldMode) {
	DWORD  dwQueryTick   = 0;
	DWORD  dwTryCnt      = 0;
	DWORD  dwCurTick     = 0;

	WORD   wBed = (WORD)(i * MAX_READERS_PER_GRID + j + 1);
	m_sigTrySurReader.emit( wBed, TRUE );
	do 
	{		
		dwQueryTick = LmnGetTickCount();
		m_launch.QueryTemperature(byArea, wBed);		
		m_launch.ReadComData();		

		// ����õ�����
		if ( m_bSurReaderTemp[i][j] ) {
			m_sigTrySurReader.emit(wBed,FALSE);
			return;
		}

		while (TRUE) {
			LmnSleep(200);			
			m_launch.ReadComData();

			// ����Ͽ�
			if ( m_launch.GetStatus() == CLmnSerialPort::CLOSE ) {
				m_sigTrySurReader.emit(wBed, FALSE);				
				return;
			}

			// ����õ�����
			if ( m_bSurReaderTemp[i][j] ) {
				m_sigTrySurReader.emit(wBed, FALSE);
				return;
			}
			
			// ���Ӧ�ó������ڹر�
			if (g_data.m_bClosing) {
				return;
			}

			DWORD  dwNewMode = g_data.m_CfgData.m_GridCfg[i].m_dwGridMode;			

			// ���ģʽ�ı�( hand <---> single <--->  multiple )
			if (dwOldMode != dwNewMode) {
				// �ֳ�ģʽ
				if (dwNewMode == CModeButton::Mode_Hand) {
					m_sigTrySurReader.emit(wBed, FALSE);
					return;
				}
				// ����ģʽ
				else if (dwNewMode == CModeButton::Mode_Single) {
					// ������ǵ�һ��������
					if (0 != j) {
						m_sigTrySurReader.emit(wBed, FALSE);
						return;
					}
				}
				// ���ģʽ
				else {
					// ������عر�
					if (!g_data.m_CfgData.m_GridCfg[i].m_ReaderCfg[j].m_bSwitch) {
						m_sigTrySurReader.emit(wBed, FALSE);
						return;
					}
				}
			}

			dwCurTick = LmnGetTickCount();
			// �����ʱ
			if ( dwCurTick - dwQueryTick >= MAX_TIME_NEEDED_BY_SUR_TEMP) {
				dwTryCnt++;
				break;
			}
		}
	} while (dwTryCnt < 3);	

	// 3�γ�ʱ
	if ( g_data.m_bSurReaderConnected[i][j]) {
		g_data.m_bSurReaderConnected[i][j] = FALSE;
		m_sigSurReaderStatus.emit(wBed, FALSE);
	}	
	m_sigTrySurReader.emit(wBed, FALSE);
}

void  CBusiness::OnStatus(CLmnSerialPort::PortStatus e) {
	m_sigLanchStatus.emit(e);

	if (e == CLmnSerialPort::OPEN) {
		DWORD  dwDelay = 200;
		for (DWORD i = 0; i < MAX_GRID_COUNT; i++) {
			GetGridTemperatureAsyn(i, dwDelay);
			dwDelay += 200;
		}
		ReadLaunchAsyn(1000);
	}
	else {
		g_data.m_thrd_launch->DeleteMessages();
		memset(g_data.m_bSurReaderConnected, 0, sizeof(g_data.m_bSurReaderConnected));
		for (DWORD i = 0; i < MAX_GRID_COUNT; i++) {
			for (DWORD j = 0; j < MAX_READERS_PER_GRID; j++) {
				WORD  wBed = (WORD)(i*MAX_READERS_PER_GRID + j+1);
				m_sigSurReaderStatus.emit(wBed, FALSE);
			}
		}
	}
}

void  CBusiness::OnHandReaderTemp(const TempItem & item) {
	SaveHandeTempAsyn(item);
}

void CBusiness::OnReaderTemp(WORD wBed, const TempItem & item) {
	DWORD  i = (wBed - 1) / MAX_READERS_PER_GRID;
	DWORD  j = (wBed - 1) % MAX_READERS_PER_GRID;
	assert(i < MAX_GRID_COUNT);
	assert(j < MAX_READERS_PER_GRID);
	if (i >= MAX_GRID_COUNT || j >= MAX_READERS_PER_GRID) {
		g_data.m_log->Output(ILog::LOG_SEVERITY_ERROR, "���ж������ķ������ݵĴ�λ��������(bed=%lu)\n", (DWORD)wBed);
		return;
	}

	// �����Ѿ��õ�����
	m_bSurReaderTemp[i][j] = TRUE;
	// �ȱ������ݿ⣬�õ�item id�������͸�����
	// m_sigSurReaderTemp.emit(wBed, item);
	SaveSurTempAsyn( wBed, item);

	// ���ö���������״̬
	if (!g_data.m_bSurReaderConnected[i][j]) {
		g_data.m_bSurReaderConnected[i][j] = TRUE;
		m_sigSurReaderStatus.emit( wBed,TRUE);
	}
}

void  CBusiness::ReadLaunchAsyn(DWORD dwDelayTime /*= 0*/) {
	if (0 == dwDelayTime) {
		g_data.m_thrd_launch->PostMessage(this, MSG_READ_LAUNCH);
	}
	else {
		g_data.m_thrd_launch->PostDelayMessage(dwDelayTime, this, MSG_READ_LAUNCH);
	}
}

void  CBusiness::ReadLaunch() {
	m_launch.ReadComData();
	ReadLaunchAsyn(READ_LAUNCH_INTERVAL_TIME);
}

// ��ӡ״̬(������)
void   CBusiness::PrintStatusAsyn() {
	g_data.m_thrd_work->PostMessage(this, MSG_PRINT_STATUS, 0, TRUE);
}

void  CBusiness::PrintStatus() {
	JTelSvrPrint("launch status: %s", m_launch.GetStatus() == CLmnSerialPort::OPEN ? "open" : "close");
	if (m_launch.GetStatus() == CLmnSerialPort::OPEN) {
		JTelSvrPrint("launch com port = com%d", m_launch.GetPort());
	}
	JTelSvrPrint("launch messages count: %lu", g_data.m_thrd_launch->GetMessagesCount());
}

// �����¶�����
void  CBusiness::SaveSurTempAsyn(WORD wBedNo, const TempItem & item) {
	g_data.m_thrd_sqlite->PostMessage(this, MSG_SAVE_SUR_TEMP, new CSaveSurTempParam(wBedNo, item));
}

void  CBusiness::SaveSurTemp(CSaveSurTempParam * pParam) {
	m_sqlite.SaveSurTemp(pParam);
	m_sigSurReaderTemp.emit(pParam->m_wBedNo, pParam->m_item);
}

// �����ֳ��¶�����
void  CBusiness::SaveHandeTempAsyn(const TempItem & item) {
	g_data.m_thrd_sqlite->PostMessage(this, MSG_SAVE_HAND_TEMP, new CSaveHandTempParam(item));
}

void  CBusiness::SaveHandTemp(CSaveHandTempParam * pParam) {
	m_sqlite.SaveHandTemp(pParam);

	TagPName * pPName = m_tag_patient_name[pParam->m_item.m_szTagId];

	// ���û�д����ݿ����ѯ��tag�Ĳ������ƣ����ѯһ��
	if ( 0 == pPName) {
		pPName = new TagPName;
		memset(pPName, 0, sizeof(TagPName));
		m_sqlite.QueryTagPNameByTagId(pParam->m_item.m_szTagId, pPName->m_szPName, MAX_TAG_PNAME_LENGTH);
		pPName->m_time = time(0);
		m_tag_patient_name[pParam->m_item.m_szTagId] = pPName;
	}

	m_sigHandReaderTemp.emit(pParam->m_item, pPName->m_szPName);
}

void  CBusiness::QueryTempByTagAsyn(const char * szTagId, WORD wBedNo) {
	g_data.m_thrd_sqlite->PostMessage(this, MSG_QUERY_TEMP_BY_TAG, new CQueryTempByTagParam(szTagId, wBedNo));
}

void  CBusiness::QueryTempByTag(const CQueryTempByTagParam * pParam) {
	std::vector<TempItem*> * pvRet = new std::vector<TempItem*>;
	m_sqlite.QueryTempByTag(pParam->m_szTagId, *pvRet);
	m_sigQueyTemp.emit(pParam->m_szTagId, pParam->m_wBedNo, pvRet);
}

// ����ע��
void  CBusiness::SaveRemarkAsyn(DWORD  dwDbId, const char * szRemark) {
	g_data.m_thrd_sqlite->PostMessage( this, MSG_SAVE_REMARK, new CSaveRemarkParam(dwDbId, szRemark) );
}

void  CBusiness::SaveRemark(const CSaveRemarkParam * pParam) {
	m_sqlite.SaveRemark(pParam);
}

// ��ʱ���tag patient name��û�й���(��sqlite�߳��� )
void  CBusiness::CheckSqliteAsyn() {
	g_data.m_thrd_sqlite->PostDelayMessage( CHECK_SQLITE_INTERVAL_TIME, this, MSG_CHECK_SQLITE );
}

void  CBusiness::CheckSqlite() {
	time_t now = time(0);
	std::map<std::string, TagPName*>::iterator it;
	for ( it = m_tag_patient_name.begin(); it != m_tag_patient_name.end();) {
		TagPName* p = it->second;
		if (p == 0) {
			it = m_tag_patient_name.erase(it);
			return;
		}

		// �����ʱ
		if ( now - p->m_time >= 10 ) {
			delete p;
			it = m_tag_patient_name.erase(it);
			return;
		}

		++it;
	}

	CheckSqliteAsyn();
}

// ������������TagID����λ��
void  CBusiness::SaveLastSurTagIdAsyn(WORD wBedId, const char * szTagId) {
	g_data.m_thrd_sqlite->PostMessage( this, MSG_SAVE_LAST_SUR_TAG_ID, new CSaveLastSurTagId(wBedId, szTagId) );
}

void  CBusiness::SaveLastSurTagId(const CSaveLastSurTagId * pParam) {
	m_sqlite.SaveLastSurTagId(pParam);
}

// ������е�ʱ���ȴ����ݿ��ȡ��һ�ε�������Ϣ
void  CBusiness::PrepareAsyn() {
	g_data.m_thrd_sqlite->PostMessage( this, MSG_PREPARE );
}

// Tag�󶨴���
void  CBusiness::TagBindingGridAsyn(const char * szTagId, int nGridIndex) {

}

void  CBusiness::TagBindingGrid() {

}

void  CBusiness::Prepare() {
	/********************  ��ѯ������һ�ε��¶�����  **********************/
	std::vector<LastSurTagItem *> vLastSurTags;
	m_sqlite.GetAllLastSurTags(vLastSurTags);

	std::vector<LastSurTagItem *>::iterator it;
	for ( it = vLastSurTags.begin(); it != vLastSurTags.end(); ++it ) {
		LastSurTagItem * pItem = *it;

		std::vector<TempItem*> * pvRet = new std::vector<TempItem*>;
		m_sqlite.QueryTempByTag(pItem->m_szTagId, *pvRet);
		m_sigQueyTemp.emit(pItem->m_szTagId, pItem->m_wBedId, pvRet);
	}

	ClearVector(vLastSurTags);	
	/******************** end  ��ѯ������һ�ε��¶�����  **********************/


	/********************  ��ѯ�ֳ��¶�����  **********************/
	std::vector<HandTagResult *> * pvHandTagRet = new std::vector<HandTagResult *>;
	m_sqlite.GetAllHandTagTempData(*pvHandTagRet);
	m_sigAllHandTagTempData.emit(pvHandTagRet);
	/******************** end  ��ѯ�ֳ��¶�����  **********************/

	m_prepared.emit();
}


// ��Ϣ����
void CBusiness::OnMessage(DWORD dwMessageId, const  LmnToolkits::MessageData * pMessageData) {
	switch (dwMessageId)
	{
	case MSG_CHECK_LAUNCH_STATUS:
	{
		CheckLaunch();
	}
	break;

	case MSG_PRINT_STATUS:
	{
		PrintStatus();
	}
	break;

	case MSG_RESTART_LAUNCH:
	{
		RestartLaunch();
	}
	break;

	case MSG_READ_LAUNCH:
	{
		ReadLaunch();
	}
	break;

	case MSG_SAVE_SUR_TEMP:
	{
		CSaveSurTempParam * pParam = (CSaveSurTempParam *)pMessageData;
		SaveSurTemp(pParam);
	}
	break;

	case MSG_QUERY_TEMP_BY_TAG:
	{
		CQueryTempByTagParam * pParam = (CQueryTempByTagParam *)pMessageData;
		QueryTempByTag(pParam);
	}
	break;

	case MSG_SAVE_REMARK:
	{
		CSaveRemarkParam * pParam = (CSaveRemarkParam *)pMessageData;
		SaveRemark(pParam);
	}
	break;

	case MSG_SAVE_HAND_TEMP:
	{
		CSaveHandTempParam * pParam = (CSaveHandTempParam *)pMessageData;
		SaveHandTemp(pParam);
	}
	break;

	case MSG_CHECK_SQLITE:
	{
		CheckSqlite();
	}
	break;

	case MSG_SAVE_LAST_SUR_TAG_ID:
	{
		CSaveLastSurTagId * pParam = (CSaveLastSurTagId *)pMessageData;
		SaveLastSurTagId(pParam);
	}
	break;

	case MSG_PREPARE:
	{
		Prepare();
	}
	break;

	default:
	{
		if ( dwMessageId >= MSG_GET_GRID_TEMP && dwMessageId <= MSG_GET_GRID_TEMP_MAX ) {
			CGetGridTempParam * pParam = (CGetGridTempParam *)pMessageData;
			GetGridTemperature(pParam);
		}
	}
	break;

	}
}