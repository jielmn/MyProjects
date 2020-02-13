#include "business.h"
#include "main.h"
#include "resource.h"
#include "LmnFile.h"

#define   CHECK_SQLITE_INTERVAL_TIME       60000


CBusiness * CBusiness::pInstance = 0;

CBusiness *  CBusiness::GetInstance() {
	if (0 == pInstance) {
		pInstance = new CBusiness;
	}
	return pInstance;
}

CBusiness::CBusiness() {
	LmnInitLock(&m_lock);
	//m_launch.m_sigStatus.connect(this, &CBusiness::OnStatus);
	//m_launch.m_sigReaderTemp.connect(this, &CBusiness::OnReaderTemp);
	//m_launch.m_sigHandReaderTemp.connect(this, &CBusiness::OnHandReaderTemp);

	m_bExcelSupport = CExcelEx::IfExcelInstalled();;
	m_excel = 0;
	memset( m_excel_row,         0,  sizeof(m_excel_row) );
	memset( m_excel_tag_id,       0, sizeof(m_excel_tag_id) );
	memset( m_excel_patient_name, 0, sizeof(m_excel_patient_name) );
}

CBusiness::~CBusiness() {
	Clear();

	std::vector<CMyComPort *>::iterator it_com;
	for (it_com = m_vSerialPorts.begin(); it_com != m_vSerialPorts.end(); ++it_com) {
		CMyComPort * pItem = *it_com;
		pItem->m_com.CloseUartPort();
		delete pItem;
	}

	LmnDeinitLock(&m_lock);
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

	g_data.m_cfg->GetConfig("temp char footer", g_data.m_TempChartFooter, sizeof(g_data.m_szLaunchPort), "A��22051612");

	g_data.m_cfg->GetBooleanConfig("t1-t2", g_data.m_bDelta[0], FALSE);
	g_data.m_cfg->GetBooleanConfig("t1-t3", g_data.m_bDelta[1], FALSE);
	g_data.m_cfg->GetBooleanConfig("t2-t3", g_data.m_bDelta[2], TRUE);

	// �������˿�
	char  szComPorts[256];
	g_data.m_cfg->GetConfig(CFG_LAUNCH_COM_PORT, szComPorts, sizeof(szComPorts), "");

	// ���û��ָ����ȡ�Ĵ���
	if ( szComPorts[0] == '\0' ) {
		g_data.m_bSpecifiedComports = FALSE;
		g_data.m_cfg->GetBooleanConfig("multiple com port", g_data.m_bMultipleComport, FALSE);
	}
	else {
		Str2Lower(szComPorts);
		SplitString s;
		s.Split(szComPorts, ',');
		if ( s.Size() > 0 ) {
			int m = s.Size() > MAX_COM_PORTS_CNT ? MAX_COM_PORTS_CNT : s.Size();
			for (int i = 0; i < m; i++) {
				int nCom = 0;
				int ret = sscanf(s[i], " com%d", &nCom);
				if ( 1 == ret ) {
					g_data.m_nComports[g_data.m_nComportsCnt] = nCom;
					g_data.m_nComportsCnt++;
				}
			}
		}
		
		// ����û��ָ����ȡ�Ĵ���
		if ( g_data.m_nComportsCnt == 0 ) {
			g_data.m_bSpecifiedComports = FALSE;
			g_data.m_cfg->GetBooleanConfig("multiple com port", g_data.m_bMultipleComport, FALSE);
		}
		else {
			g_data.m_bSpecifiedComports = TRUE;
		}
	}

	DWORD  dwValue = 0;
	g_data.m_cfg->GetConfig("high temperature", dwValue, 3850);
	g_data.m_nCubeHighTemp = dwValue;
	/******************** end ������ **********************/


	/***************     ���ݿ�    ******************/
	m_sqlite.InitDb();
	/***************   end ���ݿ�    ******************/


	/******** �߳� ********/
	//g_data.m_thrd_launch = new LmnToolkits::Thread();
	//if (0 == g_data.m_thrd_launch) {
	//	return -1;
	//}
	//g_data.m_thrd_launch->Start();

	//g_data.m_thrd_sqlite = new LmnToolkits::Thread();
	//if (0 == g_data.m_thrd_sqlite) {
	//	return -1;
	//}
	//g_data.m_thrd_sqlite->Start();

	//g_data.m_thrd_work = new LmnToolkits::Thread();
	//if (0 == g_data.m_thrd_work) {
	//	return -1;
	//}
	//g_data.m_thrd_work->Start();

	//g_data.m_thrd_excel = new LmnToolkits::Thread();
	//if (0 == g_data.m_thrd_excel) {
	//	return -1;
	//}
	//g_data.m_thrd_excel->Start();
	//InitThreadExcelAsyn();

	// ������ʱ���tag patient name�Ƿ����
	//CheckSqliteAsyn();

	g_data.m_thrd_launch_cube = new LmnToolkits::Thread();
	if (0 == g_data.m_thrd_launch_cube) {
		return -1;
	}
	g_data.m_thrd_launch_cube->Start();

	g_data.m_thrd_sqlite_cube = new LmnToolkits::Thread();
	if (0 == g_data.m_thrd_sqlite_cube) {
		return -1;
	}
	g_data.m_thrd_sqlite_cube->Start();

	// ��ͣ�Ķ�ȡ��������
	g_data.m_thrd_launch_cube->PostMessage( this, MSG_READ_COM_PORTS );

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
#if !TRI_TAGS_FLAG
		dwValue = CModeButton::Mode_Hand;
#else
		dwValue = CModeButton::Mode_Multiple;
#endif
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

		strText.Format("%s %lu", CFG_PATIENT_NAME, i + 1);
		g_data.m_cfg->GetConfig(strText, g_data.m_CfgData.m_GridCfg[i].m_szPatientName, MAX_TAG_PNAME_LENGTH, 0);

#if TRI_TAGS_FLAG
		strText.Format("bed %lu", i + 1);
		char szTemp[1024];
		g_data.m_cfg->GetConfig(strText, szTemp, sizeof(szTemp), 0);
		StrTrim(szTemp);
		if (szTemp[0] != '\0') {
			SplitString split;
			split.Split(szTemp, ',');
			if ( split.Size() == 2 ) {
				BatteryBinding bind;
				memset(&bind, 0, sizeof(bind));

				CLmnString sDeviceId = split[0];
				CLmnString sIndex    = split[1];

				sDeviceId.Trim();
				sDeviceId.MakeLower();
				sIndex.Trim();

				STRNCPY( bind.m_szDeviceId, sDeviceId, sizeof(bind.m_szDeviceId) );
				sscanf(sIndex, "%d", &bind.m_nIndex);
				bind.m_nGridIndex = i + 1;

				DWORD j = 0;
				for ( j = 0; j < g_data.m_dwBatteryBindCnt; j++ ) {
					if ( 0 == strcmp(g_data.m_battery_binding[i].m_szDeviceId, bind.m_szDeviceId)
						&& g_data.m_battery_binding[i].m_nIndex == bind.m_nIndex ) {
						g_data.m_battery_binding[i].m_nIndex = i + 1;
						break;
					}
				}

				if (j >= g_data.m_dwBatteryBindCnt) {
					if ( g_data.m_dwBatteryBindCnt < MAX_GRID_COUNT ) {
						memcpy(&g_data.m_battery_binding[g_data.m_dwBatteryBindCnt], 
							&bind, sizeof(BatteryBinding));
						g_data.m_dwBatteryBindCnt++;
					}
				}
			}
		}
#endif
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

	if (g_data.m_thrd_excel) {
		// post save ��Ϣ
		SaveExcelAsyn();
		// post quit ��Ϣ
		// g_data.m_thrd_excel->PostMessage(this, LmnToolkits::Thread::MESSAGE_ID_CLOSE_THREAD);
		// NOT SET stop flag
		g_data.m_thrd_excel->Stop( FALSE );

		delete g_data.m_thrd_excel;
		g_data.m_thrd_excel = 0;
	}

	// ����
	if (g_data.m_thrd_launch_cube) {
		g_data.m_thrd_launch_cube->Stop();
		delete g_data.m_thrd_launch_cube;
		g_data.m_thrd_launch_cube = 0;
	}

	if (g_data.m_thrd_sqlite_cube) {
		g_data.m_thrd_sqlite_cube->Stop();
		delete g_data.m_thrd_sqlite_cube;
		g_data.m_thrd_sqlite_cube = 0;
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
	m_sigBindingRet.connect(pMainWnd, &CDuiFrameWnd::OnBindingRetNotify);
	m_sigQueyHandTemp.connect(pMainWnd, &CDuiFrameWnd::OnQueryHandTagRetNotify);
	m_sigQueryBindingByTag.connect(pMainWnd, &CDuiFrameWnd::OnQueryBindingByTagRetNotify);
	m_sigDelTag.connect(pMainWnd, &CDuiFrameWnd::OnDelTagRetNotify);
	m_sigInHospitalRet.connect(pMainWnd, &CDuiFrameWnd::OnQueryInHospitalNotify);
	m_sigOutHospitalRet.connect(pMainWnd, &CDuiFrameWnd::OnQueryOutHospitalNotify);
	return;
}

// Ӳ���Ķ���������������״̬
void  CBusiness::CheckLaunchAsyn() {
	//g_data.m_thrd_launch->PostMessage(this, MSG_CHECK_LAUNCH_STATUS, 0, TRUE);
	g_data.m_thrd_launch_cube->PostMessage(this, MSG_CHECK_LAUNCH_STATUS, 0, TRUE);
}

void  CBusiness::CheckLaunch() {
	//m_launch.CheckStatus();

	// ��ȡ��ǰ���������д���(���ںţ������ַ���)
	std::map<int, std::string>  all_ports;
	GetAllSerialPorts(all_ports);

	// ���㽻�����������ȱ�ٵĲ���
	std::map<int, std::string>   m1;
	std::map<int, std::string>   m2;
	std::vector<int>             v3;

	std::vector<CMyComPort *>::iterator it_com;
	std::map<int, std::string>::iterator it;

	for (it_com = m_vSerialPorts.begin(); it_com != m_vSerialPorts.end(); ++it_com) {
		CMyComPort * pItem = *it_com;
		CLmnSerialPort * pComPort = &pItem->m_com;
		assert(pComPort->GetStatus() == CLmnSerialPort::OPEN);

		it = all_ports.find( pComPort->GetPort() );
		if ( it != all_ports.end() ) {
			m1.insert(std::make_pair(it->first, it->second));
			all_ports.erase(it);
		}
		else {
			v3.push_back(pComPort->GetPort());
		}
	}

	// ��v3��Ĵ��ڹرյ�
	std::vector<int>::iterator ix;
	for (ix = v3.begin(); ix != v3.end(); ++ix) {
		int nCom = *ix;
		for (it_com = m_vSerialPorts.begin(); it_com != m_vSerialPorts.end(); ++it_com) {
			CMyComPort * pItem = *it_com;
			CLmnSerialPort * pComPort = &pItem->m_com;
			if ( pComPort->GetPort() == nCom ) {
				pComPort->CloseUartPort();
				m_vSerialPorts.erase(it_com);
				break;
			}
		}
	}	

	// �����ָ��������ڻ�ȡ����
	if ( g_data.m_bSpecifiedComports ) {		
		for (it = all_ports.begin(); it != all_ports.end(); ++it) {
			int nCom = it->first;
			// �����µĴ����Ƿ�Ҫ��
			for (int i = 0; i < g_data.m_nComportsCnt; i++) {
				if (nCom == g_data.m_nComports[i]) {
					CMyComPort * pItem = new CMyComPort;
					CLmnSerialPort * pPort = &pItem->m_com;
					BOOL bRet = pPort->OpenUartPort(nCom);
					if (bRet) {
						m_vSerialPorts.push_back(pItem);
					}
					else {
						pPort->CloseUartPort();
						delete pItem;
					}
					break;
				}
			}
		}
	}
	else {
		for (it = all_ports.begin(); it != all_ports.end(); ++it) {
			int nCom = it->first;
			std::string & s = it->second;

			char buf[256];
			Str2Lower(s.c_str(), buf, sizeof(buf));

			if ( 0 != strstr(buf, "usb-serial ch340")) {
				CMyComPort * pItem = new CMyComPort;
				CLmnSerialPort * pPort = &pItem->m_com;
				BOOL bRet = pPort->OpenUartPort(nCom);
				if (bRet) {
					m_vSerialPorts.push_back(pItem);
				}
				else {
					pPort->CloseUartPort();
					delete pItem; 
				}
			}
		}
	}

	std::vector<int> * pvRet = new std::vector<int>;
	for (it_com = m_vSerialPorts.begin(); it_com != m_vSerialPorts.end(); ++it_com) {
		CMyComPort * pItem = *it_com;
		CLmnSerialPort * pCom = &pItem->m_com;
		pvRet->push_back(pCom->GetPort());
	}

	::PostMessage(g_data.m_hWnd, UM_CHECK_COM_PORTS_RET, (WPARAM)pvRet, 0);
}

// �������ӽ�����(�����øĶ���grid count�仯)
void   CBusiness::RestartLaunchAsyn() {
	//g_data.m_thrd_launch->DeleteMessages();
	//g_data.m_thrd_launch->PostMessage(this, MSG_RESTART_LAUNCH);
}

void   CBusiness::RestartLaunch() {
	//m_launch.Reconnect();
}

// ��ȡ�¶�
void  CBusiness::GetGridTemperatureAsyn(DWORD  dwIndex, DWORD dwDelay /*= 0*/) {
	//if ( m_launch.GetStatus() == CLmnSerialPort::CLOSE ) {
	//	return;
	//}

	if (0 == dwDelay) {
		//g_data.m_thrd_launch->PostMessage(this, MSG_GET_GRID_TEMP + dwIndex, new CGetGridTempParam(dwIndex), TRUE);
	}
	else {
		//g_data.m_thrd_launch->PostDelayMessage(dwDelay, this, MSG_GET_GRID_TEMP + dwIndex, new CGetGridTempParam(dwIndex), TRUE);
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
#if !TRI_TAGS_FLAG
		GetGridTemperature( i, 0, byArea, cfg.m_dwGridMode);
#endif
		GetGridTemperatureAsyn(i, dwDelay);
		return;
	}

	// ����Ƕ���������ж�����
	for ( DWORD j = 0; j < MAX_READERS_PER_GRID; j++) {
#if !TRI_TAGS_FLAG
		// ���Reader�����Ѿ���
		if ( cfg.m_ReaderCfg[j].m_bSwitch )
			GetGridTemperature( i,j, byArea, cfg.m_dwGridMode );
#else
		//if (j < 3) {
		//	GetGridTemperature(i, j, byArea, cfg.m_dwGridMode);
		//}
		//else {
		//	//if (cfg.m_ReaderCfg[j].m_bSwitch)
		//	//	GetGridTemperature(i, j, byArea, cfg.m_dwGridMode);
		//}		
#endif
	}
	GetGridTemperatureAsyn(i, dwDelay);
}

void  CBusiness::GetGridTemperature(DWORD i, DWORD j, BYTE byArea, DWORD  dwOldMode) {
//	DWORD  dwQueryTick   = 0;
//	DWORD  dwTryCnt      = 0;
//	DWORD  dwCurTick     = 0;
//
//	WORD   wBed = (WORD)(i * MAX_READERS_PER_GRID + j + 1);
//	m_sigTrySurReader.emit( wBed, TRUE );
//	do 
//	{		
//		dwQueryTick = LmnGetTickCount();
//		//m_launch.QueryTemperature(byArea, wBed);		
//		//m_launch.ReadComData();		
//
//		// ����õ�����
//		if ( m_bSurReaderTemp[i][j] ) {
//			m_sigTrySurReader.emit(wBed,FALSE);
//			return;
//		}
//
//		while (TRUE) {
//			LmnSleep(200);			
//			//m_launch.ReadComData();
//
//			// ����Ͽ�
//			if ( m_launch.GetStatus() == CLmnSerialPort::CLOSE ) {
//				m_sigTrySurReader.emit(wBed, FALSE);				
//				return;
//			}
//
//			// ����õ�����
//			if ( m_bSurReaderTemp[i][j] ) {
//				m_sigTrySurReader.emit(wBed, FALSE);
//				return;
//			}
//			
//			// ���Ӧ�ó������ڹر�
//			if (g_data.m_bClosing) {
//				return;
//			}
//
//			DWORD  dwNewMode = g_data.m_CfgData.m_GridCfg[i].m_dwGridMode;			
//
//			// ���ģʽ�ı�( hand <---> single <--->  multiple )
//			if (dwOldMode != dwNewMode) {
//				// �ֳ�ģʽ
//				if (dwNewMode == CModeButton::Mode_Hand) {
//					m_sigTrySurReader.emit(wBed, FALSE);
//					return;
//				}
//				// ����ģʽ
//				else if (dwNewMode == CModeButton::Mode_Single) {
//					// ������ǵ�һ��������
//					if (0 != j) {
//						m_sigTrySurReader.emit(wBed, FALSE);
//						return;
//					}
//				}
//				// ���ģʽ
//				else {
//#if !TRI_TAGS_FLAG
//					// ������عر�
//					if (!g_data.m_CfgData.m_GridCfg[i].m_ReaderCfg[j].m_bSwitch) {
//						m_sigTrySurReader.emit(wBed, FALSE);
//						return;
//					}
//#else
//					if ( j >= 3 && !g_data.m_CfgData.m_GridCfg[i].m_ReaderCfg[j].m_bSwitch) {
//						m_sigTrySurReader.emit(wBed, FALSE);
//						return;
//					}
//#endif
//				}
//			}
//
//			dwCurTick = LmnGetTickCount();
//			// �����ʱ
//			if ( dwCurTick - dwQueryTick >= MAX_TIME_NEEDED_BY_SUR_TEMP) {
//				dwTryCnt++;
//				break;
//			}
//		}
//	} while (dwTryCnt < 3);	
//
//	// 3�γ�ʱ
//	if ( g_data.m_bSurReaderConnected[i][j]) {
//		g_data.m_bSurReaderConnected[i][j] = FALSE;
//		m_sigSurReaderStatus.emit(wBed, FALSE);
//	}	
//	m_sigTrySurReader.emit(wBed, FALSE);
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
		//g_data.m_thrd_launch->DeleteMessages();
		memset(g_data.m_bSurReaderConnected, 0, sizeof(g_data.m_bSurReaderConnected));
		for (DWORD i = 0; i < MAX_GRID_COUNT; i++) {
			for (DWORD j = 0; j < MAX_READERS_PER_GRID; j++) {
				WORD  wBed = (WORD)(i*MAX_READERS_PER_GRID + j+1);
				m_sigSurReaderStatus.emit(wBed, FALSE);
			}
		}

		// ����
		//g_data.m_thrd_launch->PostDelayMessage( 10000, this, MSG_RESTART_LAUNCH );
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
		//g_data.m_thrd_launch->PostMessage(this, MSG_READ_LAUNCH);
	}
	else {
		//g_data.m_thrd_launch->PostDelayMessage(dwDelayTime, this, MSG_READ_LAUNCH);
	}
}

void  CBusiness::ReadLaunch() {
	// m_launch.ReadComData();
	ReadLaunchAsyn(READ_LAUNCH_INTERVAL_TIME);
}

// ��ӡ״̬(������)
void   CBusiness::PrintStatusAsyn() {
	//g_data.m_thrd_work->PostMessage(this, MSG_PRINT_STATUS, 0, TRUE);
	g_data.m_thrd_launch_cube->PostMessage(this, MSG_PRINT_STATUS, 0, TRUE);
}

void  CBusiness::PrintStatus() {
	//JTelSvrPrint("launch status: %s", m_launch.GetStatus() == CLmnSerialPort::OPEN ? "open" : "close");
	//if (m_launch.GetStatus() == CLmnSerialPort::OPEN) {
	//	JTelSvrPrint("launch com port = com%d", m_launch.GetPort());
	//}
	//JTelSvrPrint("launch messages count: %lu", g_data.m_thrd_launch->GetMessagesCount());
	//JTelSvrPrint("work messages count: %lu", g_data.m_thrd_work->GetMessagesCount());

	std::vector<CMyComPort *>::iterator it_com;
	for ( it_com = m_vSerialPorts.begin(); it_com != m_vSerialPorts.end(); ++it_com ) {
		CMyComPort * pItem = *it_com;
		CLmnSerialPort * pCom = &pItem->m_com;
		JTelSvrPrint("com[%d] status: %s", pCom->GetPort(), 
			pCom->GetStatus() == CLmnSerialPort::OPEN ? "open" : "close");
	}
}

// �����¶�����
void  CBusiness::SaveSurTempAsyn(WORD wBedNo, const TempItem & item) {
	//g_data.m_thrd_sqlite->PostMessage(this, MSG_SAVE_SUR_TEMP, new CSaveSurTempParam(wBedNo, item));
}

void  CBusiness::SaveSurTemp(CSaveSurTempParam * pParam) {
	m_sqlite.SaveSurTemp(pParam);
	m_sigSurReaderTemp.emit(pParam->m_wBedNo, pParam->m_item);

	DWORD  i = (pParam->m_wBedNo - 1) / MAX_READERS_PER_GRID;
	DWORD  j = (pParam->m_wBedNo - 1) % MAX_READERS_PER_GRID;
	WriteTemp2ExcelAsyn(i, j+1, &pParam->m_item, g_data.m_CfgData.m_GridCfg[i].m_szPatientName);
}

// �����ֳ��¶�����
void  CBusiness::SaveHandeTempAsyn(const TempItem & item) {
	//g_data.m_thrd_sqlite->PostMessage(this, MSG_SAVE_HAND_TEMP, new CSaveHandTempParam(item));
}

void  CBusiness::SaveHandTemp(CSaveHandTempParam * pParam) {
	m_sqlite.SaveHandTemp(pParam);

	CFuncLock cLock(&m_lock);
	TagPName * pPName = m_tag_patient_name[pParam->m_item.m_szTagId];

	// ���û�У����ѯһ�Σ���ȡ���ֵ
	if ( 0 == pPName) {
		pPName = new TagPName;
		memset(pPName, 0, sizeof(TagPName));
		m_sqlite.QueryTagPNameByTagId(pParam->m_item.m_szTagId, pPName->m_szPName, MAX_TAG_PNAME_LENGTH);
		pPName->m_time = time(0);
		pPName->m_nParam0 = m_sqlite.QueryBindingIndexByTag(pParam->m_item.m_szTagId);
		m_tag_patient_name[pParam->m_item.m_szTagId] = pPName;
	}
	else {
		pPName->m_time = time(0);
	}

	m_sigHandReaderTemp.emit(pParam->m_item, pPName->m_szPName);

	if ( pPName->m_nParam0 > 0 ) {
		WriteTemp2ExcelAsyn( pPName->m_nParam0 - 1, 0, &pParam->m_item, pPName->m_szPName);
	}	
}

void  CBusiness::QueryTempByTagAsyn(const char * szTagId, WORD wBedNo) {
	//g_data.m_thrd_sqlite->PostMessage(this, MSG_QUERY_TEMP_BY_TAG, new CQueryTempByTagParam(szTagId, wBedNo));
}

void  CBusiness::QueryTempByTag(const CQueryTempByTagParam * pParam) {
	std::vector<TempItem*> * pvRet = new std::vector<TempItem*>;
	m_sqlite.QueryTempByTag(pParam->m_szTagId, *pvRet);
	m_sigQueyTemp.emit(pParam->m_szTagId, pParam->m_wBedNo, pvRet);
}

// ����ע��
void  CBusiness::SaveRemarkAsyn(DWORD  dwDbId, const char * szRemark) {
	//g_data.m_thrd_sqlite->PostMessage( this, MSG_SAVE_REMARK, new CSaveRemarkParam(dwDbId, szRemark) );
}

void  CBusiness::SaveRemark(const CSaveRemarkParam * pParam) {
	m_sqlite.SaveRemark(pParam);
}

// ��ʱ���tag patient name��û�й���(��sqlite�߳��� )
void  CBusiness::CheckSqliteAsyn() {
	//g_data.m_thrd_sqlite->PostDelayMessage( CHECK_SQLITE_INTERVAL_TIME, this, MSG_CHECK_SQLITE );
}

void  CBusiness::CheckSqlite() {
	time_t now = time(0);
	std::map<std::string, TagPName*>::iterator it;
	CFuncLock cLock(&m_lock);
	for ( it = m_tag_patient_name.begin(); it != m_tag_patient_name.end();) {
		TagPName* p = it->second;
		if (p == 0) {
			it = m_tag_patient_name.erase(it);
			return;
		}

		// �����ʱ
		if ( now - p->m_time >= TAG_PNAME_OVERTIME ) {
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
	//g_data.m_thrd_sqlite->PostMessage( this, MSG_SAVE_LAST_SUR_TAG_ID, new CSaveLastSurTagId(wBedId, szTagId) );
}

void  CBusiness::SaveLastSurTagId(const CSaveLastSurTagId * pParam) {
	m_sqlite.SaveLastSurTagId(pParam);
}

// ������е�ʱ���ȴ����ݿ��ȡ��һ�ε�������Ϣ
void  CBusiness::PrepareAsyn() {
	//g_data.m_thrd_sqlite->PostMessage( this, MSG_PREPARE );

	g_data.m_thrd_sqlite_cube->PostMessage(this, MSG_PREPARE);
}

void  CBusiness::Prepare() {
	///********************  ��ѯ������һ�ε��¶�����  **********************/
	//std::vector<LastSurTagItem *> vLastSurTags;
	//m_sqlite.GetAllLastSurTags(vLastSurTags);

	//std::vector<LastSurTagItem *>::iterator it;
	//for ( it = vLastSurTags.begin(); it != vLastSurTags.end(); ++it ) {
	//	LastSurTagItem * pItem = *it;

	//	std::vector<TempItem*> * pvRet = new std::vector<TempItem*>;
	//	m_sqlite.QueryTempByTag(pItem->m_szTagId, *pvRet);
	//	m_sigQueyTemp.emit(pItem->m_szTagId, pItem->m_wBedId, pvRet);
	//}

	//ClearVector(vLastSurTags);	
	///******************** end  ��ѯ������һ�ε��¶�����  **********************/


	///********************  ��ѯ�ֳ��¶�����  **********************/
	//std::vector<HandTagResult *> * pvHandTagRet = new std::vector<HandTagResult *>;
	//m_sqlite.GetAllHandTagTempData(*pvHandTagRet);
	//m_sigAllHandTagTempData.emit(pvHandTagRet);
	//time_t now = time(0);

	//CFuncLock cLock(&m_lock);	
	//std::vector<HandTagResult *>::iterator ix;
	//for ( ix = pvHandTagRet->begin(); ix != pvHandTagRet->end(); ++ix ) {
	//	HandTagResult * p = *ix;

	//	if ( p->m_szTagId[0] != '\0') {
	//		TagPName* pTagName = new TagPName;
	//		STRNCPY(pTagName->m_szPName, p->m_szTagPName, MAX_TAG_PNAME_LENGTH);
	//		pTagName->m_time = now;
	//		pTagName->m_nParam0 = p->m_nBindingGridIndex;
	//		m_tag_patient_name[p->m_szTagId] = pTagName;
	//	}
	//}
	///******************** end  ��ѯ�ֳ��¶�����  **********************/

	std::vector<CubeItem*> * pvRet = new std::vector<CubeItem*>;
	m_sqlite.GetAllCubeBeds( *pvRet );

	::PostMessage(g_data.m_hWnd, UM_GET_ALL_CUBE_ITEMS, (WPARAM)pvRet, 0);

	m_prepared.emit();
}

// Tag�󶨴���
void  CBusiness::TagBindingGridAsyn(const char * szTagId, int nGridIndex) {
	//g_data.m_thrd_sqlite->PostMessage(this, MSG_BINDING_TAG_GRID, new CBindingTagGrid(szTagId, nGridIndex));
}

void  CBusiness::TagBindingGrid(const CBindingTagGrid * pParam) {
	std::string old_tagid;
	int old_grid_index = 0;
	m_sqlite.TagBindingGrid(pParam, old_tagid, old_grid_index);

	TagBindingGridRet ret;
	memset(&ret, 0, sizeof(TagBindingGridRet));
	STRNCPY(ret.m_szTagId, pParam->m_szTagId, MAX_TAG_ID_LENGTH);
	STRNCPY(ret.m_szOldTagId, old_tagid.c_str(), MAX_TAG_ID_LENGTH);
	ret.m_nGridIndex = pParam->m_nGridIndex;
	ret.m_nOldGridIndex = old_grid_index;

	m_sigBindingRet.emit(ret);
}

// ��ȡ�󶨴���
void  CBusiness::QueryBindingByTagIdAsyn(const char * szTagId) {
	//g_data.m_thrd_sqlite->PostMessage(this, MSG_QUERY_BINDING_BY_TAG, 
	//	new CQueryBindingByTag(szTagId));
}

void  CBusiness::QueryBindingByTagId(const CQueryBindingByTag * pParam) {
	int nGridIndex = m_sqlite.QueryBindingGridIndexByTagId(pParam->m_szTagId);
	// ����а�
	if (nGridIndex > 0) {
		TagBindingGridRet ret;
		memset(&ret, 0, sizeof(ret));
		STRNCPY(ret.m_szTagId, pParam->m_szTagId, MAX_TAG_ID_LENGTH);
		ret.m_nGridIndex = nGridIndex;
		m_sigQueryBindingByTag.emit(ret);
	}
}

// ����Hand Tag��ʷ�¶�����
void  CBusiness::QueryTempByHandTagAsyn(const char * szTagId, int nGridIndex) {
	//g_data.m_thrd_sqlite->PostMessage(this, MSG_QUERY_TEMP_BY_HAND_TAG, 
	//	new CQueryTempByHandTagParam(szTagId, nGridIndex));
}

void  CBusiness::QueryTempByHandTag(const CQueryTempByHandTagParam * pParam) {
	std::vector<TempItem*> * pvRet = new std::vector<TempItem*>;
	m_sqlite.QueryTempByTag(pParam->m_szTagId, *pvRet);
	m_sigQueyHandTemp.emit(pParam->m_szTagId, pParam->m_nGridIndex, pvRet);
}

// ɾ�����ӵ��ֳ�Tag��
void  CBusiness::RemoveGridBindingAsyn(int nGridIndex) {
	//g_data.m_thrd_sqlite->PostMessage(this, MSG_REMOVE_GRID_BINDING,
	//	new CRemoveGridBindingParam(nGridIndex));
}

void  CBusiness::RemoveGridBinding(const CRemoveGridBindingParam * pParam) {
	m_sqlite.RemoveGridBinding(pParam);
}

// ����tag��patient name
void  CBusiness::SaveTagPNameAsyn(const char * szTagId, const char * szPName) {
	//g_data.m_thrd_sqlite->PostMessage(this, MSG_SAVE_TAG_PNAME,
	//	                 new CSaveTagPNameParam(szTagId, szPName));
}

void  CBusiness::SaveTagPName(const CSaveTagPNameParam * pParam) {
	m_sqlite.SaveTagPName(pParam);

	CFuncLock cLock(&m_lock);
	TagPName* p = m_tag_patient_name[pParam->m_szTagId];
	if ( 0 == p ) {
		p = new TagPName;
		STRNCPY(p->m_szPName, pParam->m_szPName, MAX_TAG_PNAME_LENGTH);
		p->m_time = time(0);
		p->m_nParam0 = m_sqlite.QueryBindingIndexByTag(pParam->m_szTagId);
		m_tag_patient_name[pParam->m_szTagId] = p;
	}
	else {
		STRNCPY(p->m_szPName, pParam->m_szPName, MAX_TAG_PNAME_LENGTH);
		p->m_time = time(0);
	}
}

// ��ȡtag��patient id
char *  CBusiness::GetTagPName(const char * szTagId, char * szPName, DWORD dwPNameLen) {
	CFuncLock cLock(&m_lock);
	if ( szTagId == 0 || szTagId[0] == '\0' ) {
		szPName[0] = '\0';
		return szPName;
	}

	std::map<std::string, TagPName*>::iterator it =  m_tag_patient_name.find(szTagId);
	if ( it == m_tag_patient_name.end() ) {
		szPName[0] = '\0';
		return szPName;
	}

	TagPName* p = it->second;
	STRNCPY(szPName, p->m_szPName, dwPNameLen);
	return szPName;
}

// ������
void  CBusiness::AlarmAsyn() {
	//g_data.m_thrd_work->PostMessage(this, MSG_ALARM, 0, TRUE);
}

void  CBusiness::Alarm() {
	// ����رձ�������
	if (g_data.m_CfgData.m_bAlarmVoiceOff) {
		return;
	}

	PlaySound((LPCTSTR)IDR_WAVE1, GetModuleHandle(0), SND_RESOURCE | SND_ASYNC);
	LmnSleep(1500);
}

// ����excel
void  CBusiness::SaveExcelAsyn() {
	// �����֧��excel
	if (!m_bExcelSupport) {
		return;
	}

	// ���û�д��Զ�����excel���� 
	if (!g_data.m_CfgData.m_bAutoSaveExcel) {
		return;
	}

	//g_data.m_thrd_excel->PostMessage(this, MSG_SAVE_EXCEL);
}

void  CBusiness::SaveExcel() {

	if (0 == m_excel) {
		return;
	}

	char szPath[256];
	GetModuleFileName(0, szPath, sizeof(szPath));
	const char * pStr = strrchr(szPath, '\\');
	assert(pStr);
	DWORD  dwTemp = pStr - szPath;
	szPath[dwTemp] = '\0';

	const char * szFolder = "auto_save_excel";
	char buf[256];
	SNPRINTF(buf, sizeof(buf), "%s\\%s", szPath, szFolder);
	LmnCreateFolder(buf);

	time_t t = time(0);
	struct tm  tmp;
	localtime_s(&tmp, &t);

	char szFileName[256];
	SNPRINTF(szFileName, sizeof(szFileName), "%s\\%04d%02d%02d%02d%02d%02d.xlsx",
		buf, tmp.tm_year + 1900, tmp.tm_mon + 1, tmp.tm_mday, tmp.tm_hour, tmp.tm_min, tmp.tm_sec);
	m_excel->SaveAs(szFileName);
	m_excel->Quit();

	delete m_excel;
	m_excel = 0;
	memset(m_excel_row, 0, sizeof(m_excel_row));
	memset(m_excel_tag_id, 0, sizeof(m_excel_tag_id));
	memset(m_excel_patient_name, 0, sizeof(m_excel_patient_name));
}

// д�¶����ݵ�excel
void  CBusiness::WriteTemp2ExcelAsyn(DWORD i, DWORD  j, const TempItem * pTemp, const char * szPName) {
	assert(pTemp);

	// �����֧��excel
	if (!m_bExcelSupport) {
		return;
	}

	// ���û�д��Զ�����excel���� 
	if (!g_data.m_CfgData.m_bAutoSaveExcel) {
		return;
	}

	//g_data.m_thrd_excel->PostMessage( this, MSG_WRITE_TEMP_2_EXCEL, 
	//	new CWriteTemp2ExcelParam(i, j, pTemp, szPName) );
}

void  CBusiness::WriteTemp2Excel(const CWriteTemp2ExcelParam * pParam) {
	DWORD  dwIndex    = pParam->m_i;
	DWORD  dwSubIndex = pParam->m_j;

	if (0 == m_excel) {
		m_excel = new CExcelEx;
	}

	int n = m_excel->GetSheetCount();
	if (n <= (int)dwIndex) {
		for (int i = n; i <= (int)dwIndex; i++) {
			m_excel->AddSheet();
		}
	}

	BOOL  bTagOrNameChanged = FALSE;

	// ���tag�����
	if ( 0 != strcmp(m_excel_tag_id[dwIndex][dwSubIndex], pParam->m_szTagId) ) {
		bTagOrNameChanged = TRUE;
		STRNCPY( m_excel_tag_id[dwIndex][dwSubIndex], pParam->m_szTagId, 
			     sizeof(m_excel_tag_id[dwIndex][dwSubIndex]) );
	}

	// ���patient name�����
	if ( 0 != strcmp(m_excel_patient_name[dwIndex][dwSubIndex], pParam->m_szPName) ) {
		bTagOrNameChanged = TRUE;
		STRNCPY( m_excel_patient_name[dwIndex][dwSubIndex], pParam->m_szPName, 
			     sizeof(m_excel_patient_name[dwIndex][dwSubIndex]) );
	}

	int nCol = dwSubIndex * 3;
	// ���tag���������ı�
	if (bTagOrNameChanged) {

		// ��һ��
		if (m_excel_row[dwIndex][dwSubIndex] > 0) {
			m_excel_row[dwIndex][dwSubIndex]++;
		}

		m_excel->WriteGridEx(dwIndex + 1, m_excel_row[dwIndex][dwSubIndex], nCol, "tag id");
		m_excel->WriteGridEx(dwIndex + 1, m_excel_row[dwIndex][dwSubIndex], nCol + 1, pParam->m_szTagId);
		m_excel_row[dwIndex][dwSubIndex]++;

		m_excel->WriteGridEx(dwIndex + 1, m_excel_row[dwIndex][dwSubIndex], nCol, "��������");
		m_excel->WriteGridEx(dwIndex + 1, m_excel_row[dwIndex][dwSubIndex], nCol + 1, pParam->m_szPName);
		m_excel_row[dwIndex][dwSubIndex]++;

		m_excel->WriteGridEx(dwIndex + 1, m_excel_row[dwIndex][dwSubIndex], nCol, "ʱ��");
		m_excel->WriteGridEx(dwIndex + 1, m_excel_row[dwIndex][dwSubIndex], nCol + 1, "�¶�");
		m_excel_row[dwIndex][dwSubIndex]++;
	}

	char szTime[256];
	DateTime2String(szTime, sizeof(szTime), &pParam->m_time);

	char szTemp[256];
	SNPRINTF(szTemp, sizeof(szTemp), "%.2f", pParam->m_dwTemp / 100.0);

	// д�¶�
	m_excel->WriteGridEx(dwIndex + 1, m_excel_row[dwIndex][dwSubIndex], nCol, szTime);
	m_excel->WriteGridEx(dwIndex + 1, m_excel_row[dwIndex][dwSubIndex], nCol + 1, szTemp);
	m_excel_row[dwIndex][dwSubIndex]++;

}

// ��ʼ��excel�߳�(CoInitialize)
void  CBusiness::InitThreadExcelAsyn() {
	//g_data.m_thrd_excel->PostMessage(this, MSG_INIT_EXCEL);
}

void  CBusiness::InitThreadExcel() {
	CoInitialize(NULL);
}

// ��ȡ���˵Ļ�����Ϣ
void  CBusiness::QueryPatientInfoAsyn(const char * szTagId) {
	//g_data.m_thrd_sqlite->PostMessage(this, MSG_QUERY_PATIENT_INFO,
	//	new CQueryPatientInfoParam(szTagId) );
}

void  CBusiness::QueryPatientInfo(const CQueryPatientInfoParam * pParam) {
	PatientInfo tRet;
	std::vector<PatientEvent * > vEvents;
	m_sqlite.QueryPatientInfo(pParam, &tRet, vEvents);
	m_sigPatientInfo.emit(&tRet, vEvents);
	ClearVector(vEvents);
}

// ���������Ϣ
void  CBusiness::SavePatientInfoAsyn(const PatientInfo * pInfo) {
	//g_data.m_thrd_sqlite->PostMessage( this, MSG_SAVE_PATIENT_INFO,
	//	new CSavePatientInfoParam(pInfo) );
}

void  CBusiness::SavePatientInfo(const CSavePatientInfoParam * pParam) {
	m_sqlite.SavePatientInfo(pParam);
}

// ���没���¼���Ϣ
void  CBusiness::SavePatientEventsAsyn(const char * szTagId, const std::vector<PatientEvent*> & vEvents) {
	//g_data.m_thrd_sqlite->PostMessage(this, MSG_SAVE_PATIENT_EVENTS,
	//	new CSavePatientEventsParam(szTagId, vEvents));
}

void  CBusiness::SavePatientEvents(const CSavePatientEventsParam * pParam) {
	m_sqlite.SavePatientEvents(pParam);
}

// ��ȡĳ��Ĳ��˵ķ�����������Ϣ
void  CBusiness::QueryPatientDataAsyn(const char * szTagId, time_t tFirstDay) {
	//g_data.m_thrd_sqlite->PostMessage(this, MSG_QUERY_PATIENT_DATA,
	//	new CQueryPatientDataParam(szTagId, tFirstDay));
}

void  CBusiness::QueryPatientData(const CQueryPatientDataParam * pParam) {
	PatientData patient_data[7];
	std::vector<TempItem *> vTemp;
	m_sqlite.QueryPatientData(pParam, patient_data, 7, vTemp);
	m_sigPatientData.emit(patient_data, 7, vTemp);
	ClearVector(vTemp);
}

// ���没�˷��¶�����
void  CBusiness::SavePatientDataAsyn(const PatientData * pData) {
	//g_data.m_thrd_sqlite->PostMessage( this, MSG_SAVE_PATIENT_DATA,
	//	                               new CSavePatientDataParam(pData) );
}

void  CBusiness::SavePatientData(const CSavePatientDataParam * pParam) {
	m_sqlite.SavePatientData(pParam);
}

// ɾ��Tag����
void  CBusiness::DelTagAsyn(const char * szTagId) {
	//g_data.m_thrd_sqlite->PostMessage( this, MSG_DEL_TAG, new CDelTag(szTagId) );
}

void  CBusiness::DelTag(const CDelTag * pParam) {
	m_sqlite.DelTag(pParam);
	m_sigDelTag.emit(pParam->m_szTagId);
}

// ��ѯסԺ��Ϣ
void  CBusiness::QueryInHospitalAsyn(const TQueryInHospital * pQuery) {
	//g_data.m_thrd_sqlite->PostMessage( this, MSG_QUERY_INHOSPITAL, new CQueryInHospital(pQuery) );
}

void  CBusiness::QueryInHospital(const CQueryInHospital * pParam) {
	std::vector<InHospitalItem * > vRet;
	m_sqlite.QueryInHospital(pParam, vRet);
	m_sigInHospitalRet.emit(vRet);
	ClearVector(vRet);
}

// ��ѯ��Ժ��Ϣ
void  CBusiness::QueryOutHospitalAsyn(const TQueryOutHospital * pQuery) {
	//g_data.m_thrd_sqlite->PostMessage(this, MSG_QUERY_OUTHOSPITAL, new CQueryOutHospital(pQuery));
}

void  CBusiness::QueryOutHospital(const CQueryOutHospital * pParam) {
	std::vector<OutHospitalItem * > vRet;
	m_sqlite.QueryOutHospital(pParam, vRet);
	m_sigOutHospitalRet.emit(vRet);
	ClearVector(vRet);
}

void   CBusiness::SaveCubeBedAsyn(int nBedNo, const char * szName, const char * szPhone) {
	g_data.m_thrd_sqlite_cube->PostMessage(this, MSG_SAVE_CUBE_BED, 
		new CSaveCubeBedParam(nBedNo, szName, szPhone) );
}

void   CBusiness::SaveCubeBed(const CSaveCubeBedParam * pParam) {
	m_sqlite.SaveCubeBed(pParam);

	CubeItem * pItem = new CubeItem;
	memset(pItem, 0, sizeof(CubeItem));

	pItem->nBedNo = pParam->m_nBedNo;
	STRNCPY(pItem->szName,  pParam->m_szName, sizeof(pItem->szName));
	STRNCPY(pItem->szPhone, pParam->m_szPhone, sizeof(pItem->szPhone));

	::PostMessage(g_data.m_hWnd, UM_ADD_CUBE_ITEMS_RET, (WPARAM)pItem, 0);
}

// ��ȡ���еĴ�������
void   CBusiness::ReadAllComPorts() {
	BYTE   buf[8192];
	DWORD  dwBufLen = 0;
	std::vector<CMyComPort *>::iterator  it_com;
	char debug_buf[8192];

	const int MIN_DATA_LENGTH = 16;
	const int BATERRY_TEMP_DATAl_LENGTH = 16;       // ��Դtag
	const int SURGENCY_TEMP_DATA_LENGTH = 29;
	const int HAND_TEMP_DATA_LENGTH = 32;

	for (it_com = m_vSerialPorts.begin(); it_com != m_vSerialPorts.end(); ) {
		CMyComPort * pItem = *it_com;
		CLmnSerialPort * pCom = &pItem->m_com;
		CDataBuf * pBuf = &pItem->m_buf;
		
		BOOL  bClosed = FALSE;
		while (TRUE) {
			dwBufLen = sizeof(buf);
			if ( pCom->Read(buf, dwBufLen) ) {
				if (dwBufLen > 0) {
					pBuf->Append(buf, dwBufLen);
				}
				else {
					break;
				}
			}
			else {
				pCom->CloseUartPort();
				bClosed = TRUE;
				break;
			}
		}

		// ����ر�
		if ( bClosed ) {
			delete pItem;
			it_com = m_vSerialPorts.erase(it_com);
			continue;
		}
		

		// ���û�йرգ���������
		if ( pBuf->GetDataLength() < MIN_DATA_LENGTH ) {
			++it_com;
			continue;
		}

		while ( pBuf->Read(buf, MIN_DATA_LENGTH) ) {

			if (buf[0] != 0x55) {
				DebugStream(debug_buf, sizeof(debug_buf), buf, MIN_DATA_LENGTH);
				g_data.m_log->Output(ILog::LOG_SEVERITY_ERROR, "���������ͷ��\n%s\n", debug_buf);
				pCom->CloseUartPort();
				bClosed = TRUE;
				break;
			}
			
			// ������ֳ��¶�����
			if (buf[1] == 0x1E) {
				DWORD  dwExtraLength = HAND_TEMP_DATA_LENGTH - MIN_DATA_LENGTH;
				DWORD  dwExpectedLength = HAND_TEMP_DATA_LENGTH;

				// ���û���㹻����
				if ( pBuf->GetDataLength() < dwExtraLength ) {
					pBuf->ResetReadPos();
					break;
				}

				// �������������� 
				pBuf->Read(buf + MIN_DATA_LENGTH, dwExtraLength);
				pBuf->Reform();

				TempItem item;
				BOOL bHandled = ProcHandeReader( pCom, buf, dwExpectedLength, item);
				// �����β���ܴ��ڵ�"dd aa"
				ProcTail(pBuf);

				// �õ�һ���¶�����
				if ( bHandled ) {
					TempItem * pNewItem = new TempItem;
					memcpy( pNewItem, &item, sizeof(TempItem) );
					::PostMessage(g_data.m_hWnd, UM_CUBE_TEMP_ITEM, (WPARAM)pNewItem, 0);
				}
			}
		}		

		// ����ر�
		if (bClosed) {
			delete pItem;
			it_com = m_vSerialPorts.erase(it_com);
			continue;
		}

		++it_com;
	}

	g_data.m_thrd_launch_cube->PostDelayMessage( 1000, this, MSG_READ_COM_PORTS );
}

// �����β���ܴ��ڵ�"dd aa"
void  CBusiness::ProcTail(CDataBuf * pBuf) {
	if (pBuf->GetDataLength() >= 2) {
		BYTE  buf[32];
		pBuf->Read(buf, 2);
		// ����� dd aa��β
		if (buf[0] == 0xDD && buf[1] == 0xAA) {
			pBuf->Reform();
		}
		else {
			pBuf->ResetReadPos();
		}
	}
}

// �����ֳֶ���������
BOOL   CBusiness::ProcHandeReader( CLmnSerialPort * pCom, const BYTE * pData, DWORD dwDataLen, TempItem & item) {
	char debug_buf[8192];

	// ������һ���ֽڲ���0xFF
	if (pData[dwDataLen - 1] != 0xFF) {
		DebugStream(debug_buf, sizeof(debug_buf), pData, dwDataLen);
		g_data.m_log->Output(ILog::LOG_SEVERITY_ERROR, "���������β(������������)��\n%s\n", debug_buf);
		return FALSE;
	}

	memset(&item, 0, sizeof(item));
	item.m_dwTemp = pData[16] * 100 + pData[17];
	item.m_time = time(0);
	GetTagId(item.m_szTagId, sizeof(item.m_szTagId), pData + 8, 8);
	GetHandReaderId(item.m_szReaderId, sizeof(item.m_szReaderId), pData + 4);

	return TRUE;
}

// ���մ�λ��tag
void   CBusiness::CubeBindingTagAsyn(int nBedNo, const char * szTagId) {
	assert(szTagId && szTagId[0] != '\0');
	g_data.m_thrd_sqlite_cube->PostMessage(this, MSG_CUBE_BINDING_TAG,
		new CCubeBindTagParam(nBedNo, szTagId));

}

void   CBusiness::CubeBindingTag(const CCubeBindTagParam * pParam) {
	int ret = m_sqlite.CubeBindingTag(pParam);
	if (0 == ret) {
		char * szTagId = new char[MAX_TAG_ID_LENGTH];
		STRNCPY(szTagId, pParam->m_szTagId, MAX_TAG_ID_LENGTH);

		::PostMessage(g_data.m_hWnd, UM_CUBE_BINDING_RET, pParam->m_nBedNo, (LPARAM)szTagId);
	}
}

// �����¶�����
void   CBusiness::SaveCubeTempAsyn(int nBedNo, int nTemp, time_t time) {
	g_data.m_thrd_sqlite_cube->PostMessage(this, MSG_CUBE_SAVE_TEMP,
		new CCubeSaveTempParam(nBedNo, nTemp, time));
}

void   CBusiness::SaveCubeTemp(const CCubeSaveTempParam * pParam) {
	m_sqlite.SaveCubeTemp(pParam);
}

// ���������¶�����
void   CBusiness::QueryCubeTempAsyn(int nBedNo) {
	g_data.m_thrd_sqlite_cube->PostMessage(this, MSG_CUBE_QUERY_TEMP,
		new CCubeQueryTempParam(nBedNo) );
}

void   CBusiness::QueryCubeTemp(const CCubeQueryTempParam * pParam) {
	std::vector<CubeTempItem *> * pvRet = new std::vector<CubeTempItem *>;
	m_sqlite.QueryCubeTemp(pParam, *pvRet);
	::PostMessage(g_data.m_hWnd, UM_CUBE_QUERY_TEMP_RET, (WPARAM)pvRet, 0);
}

// ��������
void   CBusiness::UpdateCubeItemAsyn(int nBedNo, const char * szName, const char * szPhone) {
	g_data.m_thrd_sqlite_cube->PostMessage(this, MSG_UPDATE_CUBE_ITEM,
		new CUpdateCubeItemParam(nBedNo, szName, szPhone) );
}

void   CBusiness::UpdateCubeItem(const CUpdateCubeItemParam * pParam) {
	m_sqlite.UpdateCubeItem(pParam);
	CubeItem * pItem = new CubeItem;
	memset(pItem, 0, sizeof(CubeItem));
	pItem->nBedNo = pParam->m_nBedNo;
	STRNCPY(pItem->szName,  pParam->m_szName,  sizeof(pItem->szName));
	STRNCPY(pItem->szPhone, pParam->m_szPhone, sizeof(pItem->szPhone));
	::PostMessage(g_data.m_hWnd, UM_UPDATE_CUBE_ITEM_RET, (WPARAM)pItem, 0);
}

// �����
void   CBusiness::CubeDismissBindingAsyn(int nBedNo) {
	g_data.m_thrd_sqlite_cube->PostMessage(this, MSG_CUBE_DISMISS_BINDING,
		new CDismissBindingParam(nBedNo));
}

void   CBusiness::CubeDismissBinding(const CDismissBindingParam * pParam) {
	m_sqlite.CubeDismissBinding(pParam);
	::PostMessage(g_data.m_hWnd, UM_CUBE_DISMISS_BINDING_RET, pParam->m_nBedNo, 0);
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

	case MSG_BINDING_TAG_GRID:
	{
		CBindingTagGrid * pParam = (CBindingTagGrid *)pMessageData;
		TagBindingGrid(pParam);
	}
	break;

	case MSG_QUERY_TEMP_BY_HAND_TAG:
	{
		CQueryTempByHandTagParam * pParam = (CQueryTempByHandTagParam *)pMessageData;
		QueryTempByHandTag(pParam);
	}
	break;

	case MSG_REMOVE_GRID_BINDING:
	{
		CRemoveGridBindingParam * pParam = (CRemoveGridBindingParam *)pMessageData;
		RemoveGridBinding(pParam);
	}
	break;

	case MSG_SAVE_TAG_PNAME:
	{
		CSaveTagPNameParam * pParam = (CSaveTagPNameParam *)pMessageData;
		SaveTagPName(pParam);
	}
	break;

	case MSG_ALARM:
	{
		Alarm();
	}
	break;

	case MSG_WRITE_TEMP_2_EXCEL:
	{
		CWriteTemp2ExcelParam * pParam = (CWriteTemp2ExcelParam *)pMessageData;
		WriteTemp2Excel(pParam);
	}
	break;

	case MSG_SAVE_EXCEL:
	{
		SaveExcel();
	}
	break;

	case MSG_INIT_EXCEL:
	{
		InitThreadExcel();
	}
	break;

	case MSG_QUERY_PATIENT_INFO:
	{
		CQueryPatientInfoParam * pParam = (CQueryPatientInfoParam *)pMessageData;
		QueryPatientInfo(pParam);
	}
	break;

	case MSG_QUERY_PATIENT_DATA:
	{
		CQueryPatientDataParam * pParam = (CQueryPatientDataParam *)pMessageData;
		QueryPatientData(pParam);
	}
	break;

	case MSG_SAVE_PATIENT_INFO:
	{
		CSavePatientInfoParam * pParam = (CSavePatientInfoParam *)pMessageData;
		SavePatientInfo(pParam);
	}
	break;

	case MSG_SAVE_PATIENT_DATA:
	{
		CSavePatientDataParam * pParam = (CSavePatientDataParam *)pMessageData;
		SavePatientData(pParam);
	}
	break;

	case MSG_SAVE_PATIENT_EVENTS:
	{
		CSavePatientEventsParam * pParam = (CSavePatientEventsParam *)pMessageData;
		SavePatientEvents(pParam);
	}
	break;

	case MSG_QUERY_BINDING_BY_TAG:
	{
		CQueryBindingByTag * pParam = (CQueryBindingByTag *)pMessageData;
		QueryBindingByTagId(pParam);
	}
	break;

	case MSG_DEL_TAG:
	{
		CDelTag * pParam = (CDelTag *)pMessageData;
		DelTag(pParam);
	}
	break;

	case MSG_QUERY_INHOSPITAL:
	{
		CQueryInHospital * pParam = (CQueryInHospital *)pMessageData;
		QueryInHospital(pParam);
	}
	break;

	case MSG_QUERY_OUTHOSPITAL:
	{
		CQueryOutHospital * pParam = (CQueryOutHospital *)pMessageData;
		QueryOutHospital(pParam);
	}
	break;

	case MSG_SAVE_CUBE_BED:
	{
		CSaveCubeBedParam * pParam = (CSaveCubeBedParam *)pMessageData;
		SaveCubeBed(pParam);
	}
	break;

	case MSG_READ_COM_PORTS:
	{
		ReadAllComPorts();
	}
	break;

	case MSG_CUBE_BINDING_TAG:
	{
		CCubeBindTagParam * pParam = (CCubeBindTagParam *)pMessageData;
		CubeBindingTag(pParam);
	}
	break;

	case MSG_CUBE_SAVE_TEMP:
	{
		CCubeSaveTempParam * pParam = (CCubeSaveTempParam *)pMessageData;
		SaveCubeTemp(pParam);
	}
	break;

	case MSG_CUBE_QUERY_TEMP:
	{
		CCubeQueryTempParam * pParam = (CCubeQueryTempParam *)pMessageData;
		QueryCubeTemp(pParam);
	}
	break;

	case MSG_UPDATE_CUBE_ITEM:
	{
		CUpdateCubeItemParam * pParam = (CUpdateCubeItemParam *)pMessageData;
		UpdateCubeItem(pParam);
	}
	break;

	case MSG_CUBE_DISMISS_BINDING:
	{
		CDismissBindingParam * pParam = (CDismissBindingParam *)pMessageData;
		CubeDismissBinding(pParam);
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