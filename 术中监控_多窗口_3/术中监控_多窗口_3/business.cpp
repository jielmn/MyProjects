#include "business.h"
#include "LmnTelSvr.h"
#include "resource.h"

CBusiness * CBusiness::pInstance = 0;

CBusiness *  CBusiness::GetInstance() {
	if (0 == pInstance) {
		pInstance = new CBusiness;
	}
	return pInstance;
}

CBusiness::CBusiness() {
	m_launch.m_sigReconnect.connect(this, &CBusiness::OnReconnect);
	m_launch.m_sigStatus.connect(this, &CBusiness::OnStatus);
	m_launch.m_sigReaderTemp.connect(this, &CBusiness::OnReaderTemp);
	m_launch.m_sigCheck.connect(this, &CBusiness::OnCheckReader);

	memset(m_szAlarmFile, 0, sizeof(m_szAlarmFile));
	memset(m_reader_status, 0, sizeof(m_reader_status));

	m_db.m_sigStatus.connect(this, &CBusiness::OnDbStatus);
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
	DuiLib::CDuiString  strDefault;
	char buf[8192];

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

	g_data.m_cfg->GetConfig(CFG_MAIN_LAYOUT_COLUMNS, g_data.m_CfgData.m_dwLayoutColumns, DEFAULT_MAIN_LAYOUT_COLUMNS);
	g_data.m_cfg->GetConfig(CFG_MAIN_LAYOUT_ROWS, g_data.m_CfgData.m_dwLayoutRows, DEFAULT_MAIN_LAYOUT_ROWS);
	g_data.m_cfg->GetConfig(CFG_AREA_ID_NAME, g_data.m_CfgData.m_dwAreaNo, 0);
	g_data.m_cfg->GetBooleanConfig(CFG_ALARM_VOICE_SWITCH, g_data.m_CfgData.m_bAlarmVoiceOff, DEFAULT_ALARM_VOICE_SWITCH);

	// 皮肤
	g_data.m_cfg->GetConfig(CFG_SKIN, g_data.m_CfgData.m_dwSkinIndex, DEFAULT_SKIN);
	if (g_data.m_CfgData.m_dwSkinIndex > 1) {
		g_data.m_CfgData.m_dwSkinIndex = 0;
	}
	g_data.m_skin.SetSkin( (CMySkin::ENUM_SKIN)g_data.m_CfgData.m_dwSkinIndex );
	// end of 皮肤

	// 发射器端口
	g_data.m_cfg->GetConfig("launch com port", g_data.m_szLaunchPort, sizeof(g_data.m_szLaunchPort), "");

	// 温度字体大小
	g_data.m_cfg->GetConfig(CFG_TEMP_FONT, g_data.m_CfgData.m_dwTempFont, DEFAULT_TEMP_FONT );

	g_data.m_cfg->GetConfig("max printing points", g_dwPrintExcelMaxPointsCnt, PRINT_EXCEL_MAX_POINTS_COUNT);
	if (g_dwPrintExcelMaxPointsCnt <= PRINT_EXCEL_MAX_POINTS_COUNT) {
		g_dwPrintExcelMaxPointsCnt = PRINT_EXCEL_MAX_POINTS_COUNT;
	}
		
	for (int i = 0; i < MAX_GRID_COUNT; i++) {
		strText.Format("%s %lu", CFG_GRID_SWITCH, i + 1);
		g_data.m_cfg->GetBooleanConfig(strText, g_data.m_CfgData.m_GridCfg[i].m_bSwitch, DEFAULT_READER_SWITCH);

		strText.Format("%s %lu", CFG_COLLECT_INTERVAL, i + 1);
		g_data.m_cfg->GetConfig(strText, g_data.m_CfgData.m_GridCfg[i].m_dwCollectInterval, 0);
		if ( g_data.m_CfgData.m_GridCfg[i].m_dwCollectInterval > 6 ) {
			g_data.m_CfgData.m_GridCfg[i].m_dwCollectInterval = 0;
		}

		strText.Format("%s %lu", CFG_MIN_TEMP, i + 1);
		g_data.m_cfg->GetConfig(strText, g_data.m_CfgData.m_GridCfg[i].m_dwMinTemp, DEFAULT_MIN_TEMP_INDEX);
		if (g_data.m_CfgData.m_GridCfg[i].m_dwMinTemp > 4) {
			g_data.m_CfgData.m_GridCfg[i].m_dwMinTemp = 0;
		}

		strText.Format("%s %lu", CFG_BED_NAME, i + 1);
		strDefault.Format("%02lu", i + 1);
		g_data.m_cfg->GetConfig(strText, g_data.m_CfgData.m_GridCfg[i].m_szBed, MAX_BED_LENGTH, strDefault);

#if !(DB_FLAG)
		strText.Format("%s %lu", CFG_PATIENT_NAME, i + 1);
		g_data.m_cfg->GetConfig(strText, g_data.m_CfgData.m_GridCfg[i].m_szName, MAX_NAME_LENGTH, "--");
#endif

		for ( int j = 0; j < MAX_READERS_PER_GRID; j++ ) {
			strText.Format("%s %lu %lu", CFG_READER_SWITCH, i + 1, j + 1);
			g_data.m_cfg->GetBooleanConfig(strText, g_data.m_CfgData.m_GridCfg[i].m_ReaderCfg[j].m_bSwitch, DEFAULT_READER_SWITCH);

			strText.Format("%s %lu %lu", CFG_LOW_TEMP_ALARM, i + 1, j + 1);
			g_data.m_cfg->GetConfig(strText, g_data.m_CfgData.m_GridCfg[i].m_ReaderCfg[j].m_dwLowTempAlarm, 3500);
			if ( g_data.m_CfgData.m_GridCfg[i].m_ReaderCfg[j].m_dwLowTempAlarm > 4200 ) {
				g_data.m_CfgData.m_GridCfg[i].m_ReaderCfg[j].m_dwLowTempAlarm = 3500;
			}

			strText.Format("%s %lu %lu", CFG_HIGH_TEMP_ALARM, i + 1, j + 1);
			g_data.m_cfg->GetConfig(strText, g_data.m_CfgData.m_GridCfg[i].m_ReaderCfg[j].m_dwHighTempAlarm, 4000);
			if (g_data.m_CfgData.m_GridCfg[i].m_ReaderCfg[j].m_dwHighTempAlarm > 4200) {
				g_data.m_CfgData.m_GridCfg[i].m_ReaderCfg[j].m_dwHighTempAlarm = 4000;
			}

			if (g_data.m_CfgData.m_GridCfg[i].m_ReaderCfg[j].m_dwLowTempAlarm > g_data.m_CfgData.m_GridCfg[i].m_ReaderCfg[j].m_dwHighTempAlarm) {
				g_data.m_CfgData.m_GridCfg[i].m_ReaderCfg[j].m_dwLowTempAlarm = g_data.m_CfgData.m_GridCfg[i].m_ReaderCfg[j].m_dwHighTempAlarm;
			}

			strText.Format("%s %lu %lu", CFG_BED_NO, i + 1, j + 1);
			g_data.m_cfg->GetConfig(strText, g_data.m_CfgData.m_GridCfg[i].m_ReaderCfg[j].m_dwBed, -1);

			strText.Format("%s %lu %lu", CFG_READER_NAME, i + 1, j + 1);
			strDefault.Format("读卡器%02lu%c", i+1, j + 'A');
			g_data.m_cfg->GetConfig(strText, g_data.m_CfgData.m_GridCfg[i].m_ReaderCfg[j].m_szName, 
				     sizeof(g_data.m_CfgData.m_GridCfg[i].m_ReaderCfg[j].m_szName), strDefault);
		}
	}

	// 给默认的床号1, 2, 3
	for (int i = 0; i < MAX_GRID_COUNT; i++) {
		for (int j = 0; j < MAX_READERS_PER_GRID; j++) {
			if ( g_data.m_CfgData.m_GridCfg[i].m_ReaderCfg[j].m_dwBed == -1 ) {

				BOOL bFind = FALSE;
				for (int m = 0; m < MAX_GRID_COUNT; m++) {
					for ( int n = 0; n < MAX_READERS_PER_GRID; n++ ) {
						if (g_data.m_CfgData.m_GridCfg[m].m_ReaderCfg[n].m_dwBed == i * MAX_READERS_PER_GRID  + j + 1) {
							bFind = TRUE;
							break;
						}
					}					
				}

				if (!bFind) {
					g_data.m_CfgData.m_GridCfg[i].m_ReaderCfg[j].m_dwBed = i * MAX_READERS_PER_GRID + j + 1;
				}

			}
		}
	}

	// 校验
	if (   g_data.m_CfgData.m_dwLayoutColumns == 0 || g_data.m_CfgData.m_dwLayoutRows == 0 
		|| g_data.m_CfgData.m_dwLayoutColumns * g_data.m_CfgData.m_dwLayoutRows > MAX_GRID_COUNT) {
		g_data.m_CfgData.m_dwLayoutColumns = DEFAULT_MAIN_LAYOUT_COLUMNS;
		g_data.m_CfgData.m_dwLayoutRows = DEFAULT_MAIN_LAYOUT_ROWS;
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

	if ( g_data.m_CfgData.m_dwAreaNo > 0) {
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
	g_data.m_bAutoScroll = TRUE;

	g_data.m_dwCollectIntervalWidth = DEFAULT_COLLECT_INTERVAL;
	memcpy(g_data.m_argb, g_default_argb, sizeof(ARGB) * MAX_READERS_PER_GRID);

	GetDefaultAlarmFile(m_szAlarmFile, sizeof(m_szAlarmFile));	

	g_data.m_cfg->GetConfig("mysql host",      g_data.m_szDbHost, sizeof(g_data.m_szDbHost), "localhost");
	g_data.m_cfg->GetConfig("mysql user",      g_data.m_szDbUser, sizeof(g_data.m_szDbUser), "root");
	g_data.m_cfg->GetConfig("mysql password",  buf,  sizeof(buf),  "" );

	DWORD  dwPwdLen = sizeof(g_data.m_szDbPwd);
	MyDecrypt( buf, g_data.m_szDbPwd, dwPwdLen );	

	// 线程
	g_thrd_work = new LmnToolkits::PriorityThread();
	if (0 == g_thrd_work) {
		return -1;
	}
	g_thrd_work->Start();

	g_thrd_launch = new LmnToolkits::SimThread();
	if (0 == g_thrd_launch) {
		return -1;
	}
	g_thrd_launch->Start();

	g_thrd_db = new LmnToolkits::Thread();
	if (0 == g_thrd_db) {
		return -1;
	}
	g_thrd_db->Start();

	//ReconnectLaunchAsyn(200);

	return 0;
}

int CBusiness::DeInit() {

	if (g_thrd_work) {
		g_thrd_work->Stop();
		delete g_thrd_work;
		g_thrd_work = 0;
	}

	if (g_thrd_launch) {
		g_thrd_launch->Stop();
		delete g_thrd_launch;
		g_thrd_launch = 0;
	}

	if (g_thrd_db) {
		g_thrd_db->Stop();
		delete g_thrd_db;
		g_thrd_db = 0;
	}

	Clear();	
	return 0;
}

// 调整滑动条
int  CBusiness::UpdateScrollAsyn(int nIndex, DWORD dwDelay /*= 0*/) {
	if (0 == dwDelay) {
		g_thrd_work->PostMessage(this, MSG_UPDATE_SCROLL, new CUpdateScrollParam(nIndex));
	}
	else {
		g_thrd_work->PostDelayMessage(dwDelay, this, MSG_UPDATE_SCROLL, new CUpdateScrollParam(nIndex));
	}
	return 0;
}

int  CBusiness::UpdateScroll(const CUpdateScrollParam * pParam) {
	::PostMessage(g_data.m_hWnd, UM_UPDATE_SCROLL, pParam->m_nIndex, 0 );
	return 0;
}

void   CBusiness::OnUpdateScroll(DWORD dwIndex) {
	UpdateScrollAsyn(dwIndex);
}

int   CBusiness::AlarmAsyn() {
	g_thrd_work->PostMessage( this, MSG_ALARM );
	return 0;
}

int   CBusiness::Alarm() {
	DuiLib::CDuiString strText;

	// 如果关闭报警开关
	if (g_data.m_CfgData.m_bAlarmVoiceOff) {
		return 0;
	}

#if 0
	if ( m_szAlarmFile[0] != '\0' ) {
		// stop
		strText.Format("close %s", m_szAlarmFile);
		mciSendString(strText, NULL, 0, 0);

		// open
		strText.Format("open %s", m_szAlarmFile);
		mciSendString(strText, NULL, 0, 0);

		// play
		strText.Format("play %s", m_szAlarmFile);
		mciSendString(strText, NULL, 0, 0);
	}
#endif

	PlaySound((LPCTSTR)IDR_WAVE1, GetModuleHandle(0), SND_RESOURCE | SND_ASYNC);
	return 0;
}

void  CBusiness::OnAlarm() {
	AlarmAsyn();
}

int   CBusiness::ReconnectLaunchAsyn(DWORD dwDelayTime /*= 0*/) {
	g_thrd_launch->DeleteMessages();

	if (0 == dwDelayTime) {
		g_thrd_launch->PostMessage(this, MSG_RECONNECT_LAUNCH);
	}
	else {
		g_thrd_launch->PostDelayMessage(dwDelayTime, this, MSG_RECONNECT_LAUNCH);
	}
	return 0;
}

int   CBusiness::ReconnectLaunch() {
	m_launch.Reconnect();
	return 0;
}

void   CBusiness::OnReconnect(DWORD dwDelay) {
	ReconnectLaunchAsyn(dwDelay);
}

void  CBusiness::OnStatus(CLmnSerialPort::PortStatus e) {
	::PostMessage( g_data.m_hWnd, UM_LAUNCH_STATUS, (WPARAM)e, 0 );
	if ( e == CLmnSerialPort::OPEN ) {
		DWORD  dwCount = g_data.m_CfgData.m_dwLayoutColumns * g_data.m_CfgData.m_dwLayoutRows;
		DWORD  dwDelay = 200;
		for ( DWORD i = 0; i < dwCount; i++ ) {
			GetGridTemperatureAsyn(i,dwDelay);
			dwDelay += 200;
		}		
		ReadLaunchAsyn(1000);
	}
}

// 打印状态
int   CBusiness::PrintStatusAsyn() {
	g_thrd_work->PostMessage( this, MSG_PRINT_STATUS );
	return 0;
}

int   CBusiness::PrintStatus() {
	JTelSvrPrint("launch status: %s", m_launch.GetStatus() == CLmnSerialPort::OPEN ? "open" : "close");
	JTelSvrPrint("launch messages count: %lu", g_thrd_launch->GetMessagesCount());
	return 0;
}

// 获取温度
int   CBusiness::GetGridTemperatureAsyn(DWORD  dwIndex, DWORD dwDelay /*= 0*/) {
	if (0 == dwDelay) {
		g_thrd_launch->PostMessage(this, MSG_GET_GRID_TEMP, new CGridTempParam(dwIndex));
	}
	else {
		g_thrd_launch->PostDelayMessage(dwDelay, this, MSG_GET_GRID_TEMP, new CGridTempParam(dwIndex));
	}
	return 0;
}

int   CBusiness::GetGridTemperature(const CGridTempParam * pParam) {
	DWORD  dwIndex = pParam->m_dwIndex;
	for ( DWORD j = 0; j < MAX_READERS_PER_GRID; j++ ) {
		m_reader_status[dwIndex][j].m_bChecked = FALSE;
		m_reader_status[dwIndex][j].m_dwLastQueryTick = 0;
		m_reader_status[dwIndex][j].m_dwTryCnt = 0;

		CTemperatureParam a(dwIndex, j);
		GetTemperature(&a);
	}	
	return 0;
}

int   CBusiness::GetTemperatureAsyn(DWORD  dwIndex, DWORD dwSubIndex, DWORD dwDelay /*= 0*/) {
	if (0 == dwDelay) {
		g_thrd_launch->PostMessage(this, MSG_GET_TEMPERATURE, new CTemperatureParam(dwIndex, dwSubIndex));
	}
	else {
		g_thrd_launch->PostDelayMessage( dwDelay, this, MSG_GET_TEMPERATURE, new CTemperatureParam(dwIndex, dwSubIndex));
	}
	
	return 0;
}

int   CBusiness::GetTemperature(CTemperatureParam * pParam) {
	DWORD  dwIndex    = pParam->m_dwIndex;
	DWORD  dwSubIndex = pParam->m_dwSubIndex;

	//JTelSvrPrint("GetTemperature[%lu,%lu]:", dwIndex, dwSubIndex);

	// 如果总开关没有打开
	if ( !g_data.m_CfgData.m_GridCfg[dwIndex].m_bSwitch ) {	
		m_reader_status[dwIndex][dwSubIndex].m_bChecked = TRUE;
		CheckGrid(dwIndex);
		return 0;
	}

	// 如果本Reader开关没有打开或者床号为0
	if ( !g_data.m_CfgData.m_GridCfg[dwIndex].m_ReaderCfg[dwSubIndex].m_bSwitch 
		|| 0 == g_data.m_CfgData.m_GridCfg[dwIndex].m_ReaderCfg[dwSubIndex].m_dwBed ) {
		m_reader_status[dwIndex][dwSubIndex].m_bChecked = TRUE;
		CheckGrid(dwIndex);
		return 0;
	}
	
	// 通知界面, Reader disconnected
	::PostMessage(g_data.m_hWnd, UM_READER_PROCESSING, MAKELONG(dwIndex, dwSubIndex), 0);

	// 如果本Reader开关打开
	m_reader_status[dwIndex][dwSubIndex].m_dwLastQueryTick = LmnGetTickCount();
	m_launch.QueryTemperature( g_data.m_CfgData.m_dwAreaNo, 
		                       g_data.m_CfgData.m_GridCfg[dwIndex].m_ReaderCfg[dwSubIndex].m_dwBed );

	while ( m_launch.GetStatus() == CLmnSerialPort::OPEN ) {
		DWORD  dwCnt = g_data.m_CfgData.m_dwLayoutColumns * g_data.m_CfgData.m_dwLayoutRows;
		if ( dwIndex >= dwCnt ) {
			break;
		}

		m_launch.ReadComData();

		if ( !m_reader_status[dwIndex][dwSubIndex].m_bChecked ) {
			// 一次测量超时结束
			if (m_reader_status[dwIndex][dwSubIndex].m_dwLastQueryTick == 0) {
				m_reader_status[dwIndex][dwSubIndex].m_dwLastQueryTick = LmnGetTickCount();
				m_launch.QueryTemperature(g_data.m_CfgData.m_dwAreaNo,
					g_data.m_CfgData.m_GridCfg[dwIndex].m_ReaderCfg[dwSubIndex].m_dwBed);
			}
		}
		else {
			break;
		}		
		LmnSleep(200);
	}
	
	return 0;
}

// launch 读串口数据
int   CBusiness::ReadLaunchAsyn(DWORD dwDelayTime /*= 0*/) {
	if (0 == dwDelayTime) {
		g_thrd_launch->PostMessage(this, MSG_READ_LAUNCH);
	}
	else {
		g_thrd_launch->PostDelayMessage(dwDelayTime, this, MSG_READ_LAUNCH);
	}
	return 0;
}

int   CBusiness::ReadLaunch() {
	// m_launch.ReadComData();
	ReadLaunchAsyn(READ_INTERVAL_TIME);
	return 0;
}

void  CBusiness::OnReaderTemp(DWORD dwIndex, DWORD dwSubIndex, const LastTemp & t ) {
	if ( !m_reader_status[dwIndex][dwSubIndex].m_bConnected ) {
		m_reader_status[dwIndex][dwSubIndex].m_bConnected = TRUE;
	}

	LastTemp * pTemp = new LastTemp;
	memcpy(pTemp, &t, sizeof(LastTemp));

	// 通知UI温度
	::PostMessage(g_data.m_hWnd, UM_READER_TEMP, MAKELONG(dwIndex, dwSubIndex), (LPARAM)pTemp );

	m_reader_status[dwIndex][dwSubIndex].m_dwLastQueryTick = 0;
	m_reader_status[dwIndex][dwSubIndex].m_dwTryCnt = 0;
	m_reader_status[dwIndex][dwSubIndex].m_dwLastTemp = t.m_dwTemp;
	m_reader_status[dwIndex][dwSubIndex].m_bChecked = TRUE;
	
	CheckGrid(dwIndex);
}

void  CBusiness::CheckGrid(DWORD dwIndex) {
	// 查看是否最后一个
	BOOL  bAllChecked = TRUE;
	for (DWORD j = 0; j < MAX_READERS_PER_GRID; j++) {
		if (!m_reader_status[dwIndex][j].m_bChecked) {
			bAllChecked = FALSE;
			break;
		}
	}

	// 如果格子里的Reader都获取了温度
	if (bAllChecked) {
		for (DWORD j = 0; j < MAX_READERS_PER_GRID; j++) {
			m_reader_status[dwIndex][j].m_dwLastQueryTick = 0;
			m_reader_status[dwIndex][j].m_dwTryCnt = 0;
			m_reader_status[dwIndex][j].m_bChecked = FALSE;
		}
		DWORD dwCollectInterval = GetCollectInterval(g_data.m_CfgData.m_GridCfg[dwIndex].m_dwCollectInterval);
		GetGridTemperatureAsyn(dwIndex, dwCollectInterval * 1000);
	}
}

void  CBusiness::OnCheckReader() {
	DWORD  dwCnt = g_data.m_CfgData.m_dwLayoutColumns * g_data.m_CfgData.m_dwLayoutRows;
	DWORD  dwCurTick = LmnGetTickCount();
	for ( DWORD i = 0; i < dwCnt; i++ ) {
		for (DWORD j = 0; j < MAX_READERS_PER_GRID; j++) {
			if ( m_reader_status[i][j].m_dwLastQueryTick > 0 ) {
				// 如果超时
				if ( dwCurTick - m_reader_status[i][j].m_dwLastQueryTick >= GET_TEMPERATURE_TIMEOUT) {
					// 如果超时不超过3次，重新请求
					if ( m_reader_status[i][j].m_dwTryCnt < 3 ) {
						m_reader_status[i][j].m_dwLastQueryTick = 0;
						m_reader_status[i][j].m_dwTryCnt++;
						// GetTemperatureAsyn(i, j, 100);
					}
					// 认为失败
					else {
						// 通知界面, Reader disconnected
						::PostMessage(g_data.m_hWnd, UM_READER_DISCONNECTED, MAKELONG(i, j), 0);

						assert(!m_reader_status[i][j].m_bChecked);
						m_reader_status[i][j].m_bChecked = TRUE;
						m_reader_status[i][j].m_dwLastQueryTick = 0;
						m_reader_status[i][j].m_dwTryCnt = 0;
						CheckGrid(i);
					}
				}
			}
					
		}
	}
}

int   CBusiness::CheckLaunchStatusAsyn() {
	g_thrd_launch->PostMessage(this, MSG_CHECK_LAUNCH_STATUS, 0, TRUE);
	return 0;
}

int   CBusiness::CheckLaunchStatus() {
	m_launch.CheckStatus();
	return 0;
}

// 连接数据库
int   CBusiness::ReconnectDbAsyn(DWORD dwDelay /*= 0*/) {
	g_thrd_db->DeleteMessages();
	if (0 == dwDelay) {
		g_thrd_db->PostMessage( this, MSG_RECONNECT_DB );
	}
	else {
		g_thrd_db->PostDelayMessage( dwDelay, this, MSG_RECONNECT_DB);
	}
	return 0;
}

int   CBusiness::ReconnectDb() {
	m_db.Reconnect();
	return 0;
}

void   CBusiness::OnDbStatus(int nDbStatus) {
	::PostMessage(g_data.m_hWnd, UM_DB_STATUS, (WPARAM)nDbStatus, 0);
}


// 消息处理
void CBusiness::OnMessage(DWORD dwMessageId, const  LmnToolkits::MessageData * pMessageData) {
	switch ( dwMessageId )
	{
	case MSG_UPDATE_SCROLL:
	{
		CUpdateScrollParam * pParam = (CUpdateScrollParam *)pMessageData;
		UpdateScroll(pParam);
	}
	break;

	case MSG_ALARM:
	{
		Alarm();
	}
	break;

	case MSG_RECONNECT_LAUNCH:
	{
		ReconnectLaunch();
	}
	break;

	case MSG_PRINT_STATUS:
	{
		PrintStatus();
	}
	break;

	case MSG_GET_TEMPERATURE:
	{
		CTemperatureParam * pParam = (CTemperatureParam *)pMessageData;
		GetTemperature(pParam);
	}
	break;

	case MSG_READ_LAUNCH:
	{
		ReadLaunch();
	}
	break;

	case MSG_GET_GRID_TEMP:
	{
		CGridTempParam * pParam = (CGridTempParam *)pMessageData;
		GetGridTemperature(pParam);
	}
	break;

	case MSG_CHECK_LAUNCH_STATUS:
	{
		CheckLaunchStatus();
	}
	break;

	case MSG_RECONNECT_DB: 
	{
		ReconnectDb();
	}
	break;

	default:
		break;
	}
}

CMyDb::CMyDb() {
	//初始化mysql结构
	mysql_init(&m_mysql);
	m_nStatus = 0;
}

CMyDb::~CMyDb() {
	//释放数据库
	mysql_close(&m_mysql);
}

int CMyDb::Reconnect() {

	if (!mysql_real_connect(&m_mysql, g_data.m_szDbHost, g_data.m_szDbUser, g_data.m_szDbPwd, "surgery", 3306, NULL, 0))
	{
		m_nStatus = 0;
		g_data.m_log->Output(ILog::LOG_SEVERITY_ERROR, "连接数据库失败\n");
	}
	else {
		m_nStatus = 1;
		char reconnect = 1;
		mysql_options( &m_mysql, MYSQL_OPT_RECONNECT, (char *)&reconnect );
	}

	m_sigStatus.emit(m_nStatus);

	return 0;
}