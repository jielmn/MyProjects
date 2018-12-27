#include "business.h"
#include "LmnTelSvr.h"
#include "resource.h"
#include <time.h>
#include "LmnFile.h"

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

	m_bExcelSupport = CExcelEx::IfExcelInstalled();
	m_excel = 0;

	memset(m_excel_row,          0, sizeof(m_excel_row));
	memset(m_excel_tag_id,       0, sizeof(m_excel_tag_id));
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
	ClearVector(g_vBodyParts);
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
	g_data.m_cfg->GetBooleanConfig("utf8", g_data.m_bUtf8, TRUE);
	g_data.m_cfg->GetBooleanConfig("auto save excel", g_data.m_CfgData.m_bAutoSaveExcel, FALSE);	
	g_data.m_cfg->GetBooleanConfig("cross anchor", g_data.m_CfgData.m_bCrossAnchor, FALSE);
	g_data.m_cfg->GetBooleanConfig("curve thread", g_data.m_bCurve, FALSE);

	// Ƥ��
	g_data.m_cfg->GetConfig(CFG_SKIN, g_data.m_CfgData.m_dwSkinIndex, DEFAULT_SKIN);
	if (g_data.m_CfgData.m_dwSkinIndex > 1) {
		g_data.m_CfgData.m_dwSkinIndex = 0;
	}
	g_data.m_skin.SetSkin( (CMySkin::ENUM_SKIN)g_data.m_CfgData.m_dwSkinIndex );
	// end of Ƥ��

	// �������˿�
	g_data.m_cfg->GetConfig("launch com port", g_data.m_szLaunchPort, sizeof(g_data.m_szLaunchPort), "");

	// �¶������С
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
		if (g_data.m_CfgData.m_GridCfg[i].m_dwMinTemp > 5) {
			g_data.m_CfgData.m_GridCfg[i].m_dwMinTemp = 0;
		}

		strText.Format("%s %lu", CFG_MAX_TEMP, i + 1);
		g_data.m_cfg->GetConfig(strText, g_data.m_CfgData.m_GridCfg[i].m_dwMaxTemp, DEFAULT_MAX_TEMP_INDEX);
		if (g_data.m_CfgData.m_GridCfg[i].m_dwMaxTemp > 4) {
			g_data.m_CfgData.m_GridCfg[i].m_dwMaxTemp = 0;
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
			strDefault.Format("������%02lu%c", i+1, j + 'A');
			g_data.m_cfg->GetConfig(strText, g_data.m_CfgData.m_GridCfg[i].m_ReaderCfg[j].m_szName, 
				     sizeof(g_data.m_CfgData.m_GridCfg[i].m_ReaderCfg[j].m_szName), strDefault);
		}
	}

	// ��Ĭ�ϵĴ���1, 2, 3
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

	// У��
	if (   g_data.m_CfgData.m_dwLayoutColumns == 0 || g_data.m_CfgData.m_dwLayoutRows == 0 
		|| g_data.m_CfgData.m_dwLayoutColumns * g_data.m_CfgData.m_dwLayoutRows > MAX_GRID_COUNT) {
		g_data.m_CfgData.m_dwLayoutColumns = DEFAULT_MAIN_LAYOUT_COLUMNS;
		g_data.m_CfgData.m_dwLayoutRows = DEFAULT_MAIN_LAYOUT_ROWS;
	}

	// �����б�
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

		// ���g_dwAreaNo����g_vArea֮��
		if (it == g_vArea.end()) {
			TArea * pArea = new TArea;
			pArea->dwAreaNo = g_data.m_CfgData.m_dwAreaNo;
			SNPRINTF(pArea->szAreaName, sizeof(pArea->szAreaName), "(��ţ�%lu)", g_data.m_CfgData.m_dwAreaNo);
			g_vArea.push_back(pArea);
		}
	}
	// END OF ����

	// ���岿λ( tag name )
	for (int i = 0; i < 20; i++) {
		BodyPart item;
		strText.Format("body part %d", i + 1);
		g_data.m_cfg->GetConfig(strText, item.m_szName, sizeof(item.m_szName), "");
		if (item.m_szName[0] == '\0') {
			break;
		}		

		BodyPart * pNewItem = new BodyPart;
		memcpy(pNewItem, &item, sizeof(BodyPart));
		if (g_data.m_bUtf8) {
			Utf8ToAnsi(pNewItem->m_szName, sizeof(pNewItem->m_szName), item.m_szName);
		}
		g_vBodyParts.push_back(pNewItem);
	}

	g_data.m_bAutoScroll = TRUE;

	g_data.m_dwCollectIntervalWidth = DEFAULT_COLLECT_INTERVAL;
	memcpy(g_data.m_argb, g_default_argb, sizeof(ARGB) * MAX_READERS_PER_GRID);

	GetDefaultAlarmFile(m_szAlarmFile, sizeof(m_szAlarmFile));	

	g_data.m_cfg->GetConfig("mysql host",      g_data.m_szDbHost, sizeof(g_data.m_szDbHost), "localhost");
	g_data.m_cfg->GetConfig("mysql user",      g_data.m_szDbUser, sizeof(g_data.m_szDbUser), "root");
	g_data.m_cfg->GetConfig("mysql password",  buf,  sizeof(buf),  "" );

	DWORD  dwPwdLen = sizeof(g_data.m_szDbPwd);
	MyDecrypt( buf, g_data.m_szDbPwd, dwPwdLen );	

	// �߳�
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

	SaveExcelAsyn();
	if (m_excel) {
		m_excel->Quit();
		delete m_excel;
		m_excel = 0;
	}
	
	return 0;
}

// ����������
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

	// ����رձ�������
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

// ��ӡ״̬
int   CBusiness::PrintStatusAsyn() {
	g_thrd_work->PostMessage( this, MSG_PRINT_STATUS );
	return 0;
}

int   CBusiness::PrintStatus() {
	JTelSvrPrint("launch status: %s", m_launch.GetStatus() == CLmnSerialPort::OPEN ? "open" : "close");
	JTelSvrPrint("launch messages count: %lu", g_thrd_launch->GetMessagesCount());
	return 0;
}

// ��ȡ�¶�
int   CBusiness::GetGridTemperatureAsyn(DWORD  dwIndex, DWORD dwDelay /*= 0*/) {
	if (0 == dwDelay) {
		g_thrd_launch->PostMessage(this, MSG_GET_GRID_TEMP + dwIndex, new CGridTempParam(dwIndex), TRUE );
	}
	else {
		g_thrd_launch->PostDelayMessage(dwDelay, this, MSG_GET_GRID_TEMP + dwIndex, new CGridTempParam(dwIndex), TRUE );
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

	// ����ܿ���û�д�
	if ( !g_data.m_CfgData.m_GridCfg[dwIndex].m_bSwitch ) {	
		m_reader_status[dwIndex][dwSubIndex].m_bChecked = TRUE;
		CheckGrid(dwIndex);
		return 0;
	}

	// �����Reader����û�д򿪻��ߴ���Ϊ0
	if ( !g_data.m_CfgData.m_GridCfg[dwIndex].m_ReaderCfg[dwSubIndex].m_bSwitch 
		|| 0 == g_data.m_CfgData.m_GridCfg[dwIndex].m_ReaderCfg[dwSubIndex].m_dwBed ) {
		m_reader_status[dwIndex][dwSubIndex].m_bChecked = TRUE;
		CheckGrid(dwIndex);
		return 0;
	}
	
	// ֪ͨ����, Reader disconnected
	::PostMessage(g_data.m_hWnd, UM_READER_PROCESSING, MAKELONG(dwIndex, dwSubIndex), 0);

	// �����Reader���ش�
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
			// һ�β�����ʱ����
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

// launch ����������
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

	// ֪ͨUI�¶�
	::PostMessage(g_data.m_hWnd, UM_READER_TEMP, MAKELONG(dwIndex, dwSubIndex), (LPARAM)pTemp );

	m_reader_status[dwIndex][dwSubIndex].m_dwLastQueryTick = 0;
	m_reader_status[dwIndex][dwSubIndex].m_dwTryCnt = 0;
	m_reader_status[dwIndex][dwSubIndex].m_dwLastTemp = t.m_dwTemp;
	m_reader_status[dwIndex][dwSubIndex].m_bChecked = TRUE;
	
	CheckGrid(dwIndex);
}

void  CBusiness::CheckGrid(DWORD dwIndex) {
	// �鿴�Ƿ����һ��
	BOOL  bAllChecked = TRUE;
	for (DWORD j = 0; j < MAX_READERS_PER_GRID; j++) {
		if (!m_reader_status[dwIndex][j].m_bChecked) {
			bAllChecked = FALSE;
			break;
		}
	}

	// ����������Reader����ȡ���¶�
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
				// �����ʱ
				if ( dwCurTick - m_reader_status[i][j].m_dwLastQueryTick >= GET_TEMPERATURE_TIMEOUT) {
					// �����ʱ������3�Σ���������
					if ( m_reader_status[i][j].m_dwTryCnt < 3 ) {
						m_reader_status[i][j].m_dwLastQueryTick = 0;
						m_reader_status[i][j].m_dwTryCnt++;
						// GetTemperatureAsyn(i, j, 100);
					}
					// ��Ϊʧ��
					else {
						// ֪ͨ����, Reader disconnected
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

// �������ݿ�
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
	// �������ʧ�ܣ���������
	if ( 0 == nDbStatus ) {
		ReconnectDbAsyn(15000);
	}
	else {
		DbHeartBeatAsyn();
	}
}

int   CBusiness::GetDbStatus() {
	return m_db.GetStatus();
}

// ��ѯ�󶨹�ϵ
int   CBusiness::QueryBindingAsyn(DWORD dwIndex, DWORD dwSubIndex, const char * szTagId) {
	g_thrd_db->PostMessage(this, MSG_QUERY_BINDING + dwIndex * MAX_READERS_PER_GRID + dwSubIndex, 
		new CQueryBindingParam(dwIndex, dwSubIndex, szTagId), TRUE );
	return 0;
}

int   CBusiness::QueryBinding(const CQueryBindingParam * pParam) {
	TagBinding tResult;
	int nRet = m_db.QueryBinding(pParam, &tResult);
	// ���ݿ�OK����ѯ���󶨽��
	if (0 == nRet) {
		TagBinding * pNewRet = new TagBinding;
		memcpy(pNewRet, &tResult, sizeof(TagBinding));
		STRNCPY(pNewRet->m_szTagId, pParam->m_szTagId, sizeof(pNewRet->m_szTagId));
		::PostMessage(g_data.m_hWnd, UM_QUERY_TAG_BINDING_RET,
			MAKELONG(pParam->m_dwIndex, pParam->m_dwSubIndex), (LPARAM)pNewRet);
	}
	return 0;
}

int   CBusiness::DbHeartBeatAsyn(DWORD dwDelay /*= 0*/) {
	if (0 == dwDelay) {
		g_thrd_db->PostMessage(this, MSG_DB_HEARTBEAT);
	}
	else {
		g_thrd_db->PostDelayMessage(dwDelay, this, MSG_DB_HEARTBEAT);
	}
	return 0;
}

int   CBusiness::DbHeartBeat() {
	m_db.DbHeartBeat();
	DbHeartBeatAsyn(15000);
	return 0;
}

int   CBusiness::SaveTempAsyn(DWORD dwIndex, DWORD dwSubIndex, const LastTemp * pTemp) {
	g_thrd_db->PostMessage(this, MSG_SAVE_TEMP, 
		new CSaveTempParam(dwIndex, dwSubIndex, pTemp) );
	return 0;
}

int   CBusiness::SaveTemp(const CSaveTempParam * pParam ) {
	m_db.SaveTemp(pParam);
	return 0;
}

// ��ѯ���в�����Ϣ
int   CBusiness::GetAllPatientsAsyn(HWND  hWnd, DWORD  dwPatientId /*= 0*/) {
	g_thrd_db->PostMessage(this, MSG_GET_ALL_PATIENTS,
		          new CGetPatientsParam(hWnd, dwPatientId));
	return 0;
}

int   CBusiness::GetAllPatients(const CGetPatientsParam * pParam) {
	std::vector<Patient *> * pvRet = new std::vector<Patient *>;
	int nRet = m_db.GetAllPatients(*pvRet, pParam->m_dwPatientId);

	::PostMessage(pParam->m_hWnd, UM_GET_ALL_PATIENTS_RET, (WPARAM)pvRet, 0 );
	return 0;
}

// ��tag
int   CBusiness::SetBindingAsyn( HWND hWnd, DWORD dwPatientId, 
	const TagBinding_1 * pTags, DWORD dwTagsCnt ) {
	g_thrd_db->PostMessage(this, MSG_BIND_TAGS,
		new CBindTagsParam( hWnd, dwPatientId, pTags, dwTagsCnt));
	return 0;
}

int   CBusiness::SetBinding(const CBindTagsParam * pParam) {
	int nRet = m_db.SetBinding(pParam);
	::PostMessage(pParam->m_hWnd, UM_BIND_TAGS_RET, (WPARAM)nRet, 0);
	return 0;
}

// ����excel
int    CBusiness::SaveExcelAsyn() {

	// �����֧��excel
	if ( !m_bExcelSupport) {
		return 0;
	}

	// ������Զ�����
	if ( !g_data.m_CfgData.m_bAutoSaveExcel ) {
		return 0;
	}

	if (0 == m_excel) {
		m_excel = new CExcelEx;
	}

	char szPath[256];
	GetModuleFileName(0, szPath, sizeof(szPath));
	const char * pStr = strrchr(szPath, '\\');
	assert(pStr);
	DWORD  dwTemp = pStr - szPath;
	szPath[dwTemp] = '\0';

	const char * szFolder = "auto_save_excel";
	char buf[256];
	SNPRINTF(buf, sizeof(buf), "%s\\%s", szPath, szFolder );
	LmnCreateFolder( buf );

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

	return 0;
}

// 
int  CBusiness::ExcelTempAsyn(DWORD dwIndex, DWORD  dwSubIndex, const LastTemp * pTemp) {
	// �����֧��excel
	if (!m_bExcelSupport) {
		return 0;
	}

	// ������Զ�����
	if (!g_data.m_CfgData.m_bAutoSaveExcel) {
		return 0;
	}

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
	if ( 0 != strcmp(m_excel_tag_id[dwIndex][dwSubIndex], pTemp->m_szTagId) ) {
		bTagOrNameChanged = TRUE;		

		STRNCPY(m_excel_tag_id[dwIndex][dwSubIndex], pTemp->m_szTagId, sizeof(m_excel_tag_id[dwIndex][dwSubIndex]));
	}

	int nCol = dwSubIndex * 3;
	// ���tag���������ı�
	if ( bTagOrNameChanged ) {
		// ��һ��
		if (m_excel_row[dwIndex][dwSubIndex] > 0) {
			m_excel_row[dwIndex][dwSubIndex]++;
		}

		m_excel->WriteGridEx(dwIndex + 1, m_excel_row[dwIndex][dwSubIndex], nCol, "tag id");
		m_excel->WriteGridEx(dwIndex + 1, m_excel_row[dwIndex][dwSubIndex], nCol + 1, pTemp->m_szTagId );
		m_excel_row[dwIndex][dwSubIndex]++;

		m_excel->WriteGridEx(dwIndex + 1, m_excel_row[dwIndex][dwSubIndex], nCol, "��������");
		m_excel->WriteGridEx(dwIndex + 1, m_excel_row[dwIndex][dwSubIndex], nCol + 1, g_data.m_CfgData.m_GridCfg[dwIndex].m_szName);
		m_excel_row[dwIndex][dwSubIndex]++;

		m_excel->WriteGridEx(dwIndex + 1, m_excel_row[dwIndex][dwSubIndex], nCol, "ʱ��");
		m_excel->WriteGridEx(dwIndex + 1, m_excel_row[dwIndex][dwSubIndex], nCol + 1, "�¶�");
		m_excel_row[dwIndex][dwSubIndex]++;
	}
	
	char szTime[256];
	Time2String(szTime, sizeof(szTime), &pTemp->m_Time);

	char szTemp[256];
	SNPRINTF(szTemp, sizeof(szTemp), "%.2f", pTemp->m_dwTemp / 100.0);

	// д�¶�
	m_excel->WriteGridEx(dwIndex + 1, m_excel_row[dwIndex][dwSubIndex], nCol, szTime );
	m_excel->WriteGridEx(dwIndex + 1, m_excel_row[dwIndex][dwSubIndex], nCol + 1, szTemp );
	m_excel_row[dwIndex][dwSubIndex]++;
	
	return 0;
}

//
int  CBusiness::ExcelPatientNameChangedAsyn(DWORD dwIndex) {
	// �����֧��excel
	if (!m_bExcelSupport) {
		return 0;
	}

	// ������Զ�����
	if (!g_data.m_CfgData.m_bAutoSaveExcel) {
		return 0;
	}

	if (0 == m_excel) {
		m_excel = new CExcelEx;
	}

	int n = m_excel->GetSheetCount();
	if (n <= (int)dwIndex) {
		for (int i = n; i <= (int)dwIndex; i++) {
			m_excel->AddSheet();
		}
	}

	for (int i = 0; i < MAX_READERS_PER_GRID; i++) {
		if ( m_excel_tag_id[dwIndex][i][0] == '\0' ) {
			continue;
		}

		int nCol = i * 3;
		// ��һ��
		if (m_excel_row[dwIndex][i] > 0) {
			m_excel_row[dwIndex][i]++;
		}

		m_excel->WriteGridEx(dwIndex + 1, m_excel_row[dwIndex][i], nCol, "tag id");
		m_excel->WriteGridEx(dwIndex + 1, m_excel_row[dwIndex][i], nCol + 1, m_excel_tag_id[dwIndex][i]);
		m_excel_row[dwIndex][i]++;

		m_excel->WriteGridEx(dwIndex + 1, m_excel_row[dwIndex][i], nCol, "��������");
		m_excel->WriteGridEx(dwIndex + 1, m_excel_row[dwIndex][i], nCol + 1, g_data.m_CfgData.m_GridCfg[dwIndex].m_szName);
		m_excel_row[dwIndex][i]++;

		m_excel->WriteGridEx(dwIndex + 1, m_excel_row[dwIndex][i], nCol, "ʱ��");
		m_excel->WriteGridEx(dwIndex + 1, m_excel_row[dwIndex][i], nCol + 1, "�¶�");
		m_excel_row[dwIndex][i]++;
	}

	return 0;
}


// ��Ϣ����
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

	case MSG_DB_HEARTBEAT:
	{
		DbHeartBeat();
	}
	break;

	case MSG_SAVE_TEMP:
	{
		CSaveTempParam * pParam = (CSaveTempParam *)pMessageData;
		SaveTemp(pParam);
	}
	break;

	case MSG_GET_ALL_PATIENTS:
	{
		CGetPatientsParam * pParam = (CGetPatientsParam *)pMessageData;
		GetAllPatients(pParam);
	}
	break;
	
	case MSG_BIND_TAGS:
	{
		CBindTagsParam * pParam = (CBindTagsParam *)pMessageData;
		SetBinding(pParam);
	}
	break;

	default:
	{
		if ( dwMessageId >= MSG_QUERY_BINDING && dwMessageId <= MSG_QUERY_BINDING_MAX ) {
			CQueryBindingParam * pParam = (CQueryBindingParam *)pMessageData;
			QueryBinding(pParam);
		}
		else if (dwMessageId >= MSG_GET_GRID_TEMP && dwMessageId <= MSG_GET_GRID_TEMP_MAX) {
			CGridTempParam * pParam = (CGridTempParam *)pMessageData;
			GetGridTemperature(pParam);
		}
	}
	break;

	}
}

CMyDb::CMyDb() {
	//��ʼ��mysql�ṹ
	//mysql_init(&m_mysql);
	m_nStatus = 0;
}

CMyDb::~CMyDb() {
	ClearVector(m_vTemps);

#if DB_FLAG
	//�ͷ����ݿ�
	mysql_close(&m_mysql);
#endif

}

int CMyDb::Reconnect() {

#if DB_FLAG
	mysql_init(&m_mysql);	
	if (!mysql_real_connect(&m_mysql, g_data.m_szDbHost, g_data.m_szDbUser, g_data.m_szDbPwd, "surgery", 3306, NULL, 0))
	{
		m_nStatus = 0;
		g_data.m_log->Output(ILog::LOG_SEVERITY_ERROR, "�������ݿ�ʧ��\n");
	}
	else {
		m_nStatus = 1;
		mysql_set_character_set(&m_mysql, "gbk");

		// ����û�б�����¶�����
		int ret = 0;
		std::vector<TempItem *>::iterator it;
		for (it = m_vTemps.begin(); it != m_vTemps.end(); ) {
			TempItem * pItem = *it;
			ret = SaveTemp(pItem);
			// �����¶�ʧ��
			if (0 != ret) {
				break;
			}
			else {
				delete pItem;
				it = m_vTemps.erase(it);
			}
		}
	}

	m_sigStatus.emit(m_nStatus);
#endif

	return 0;
}

int CMyDb::GetStatus() {
	return m_nStatus;
}

int  CMyDb::QueryBinding(const CQueryBindingParam * pParam, TagBinding * pRet ) {

#if DB_FLAG
	// ���ݿ�û������
	if (0 == m_nStatus) {
		return -1;
	}

	assert(pRet);

	char  szSql[8192] = { 0 };
	SNPRINTF(szSql, sizeof(szSql), 
		"select a.tagid, a.patientid, a.patientpart, b.patientname"
		" from tagbands a inner join patientinfo b on a.patientid = b.id"
		" where a.tagid='%s'", pParam->m_szTagId);

	MYSQL_RES *res = 0;
	MYSQL_ROW row;

	if ( 0 != mysql_query(&m_mysql, szSql) ) {
		return -1;
	}

	res = mysql_store_result(&m_mysql);
	if ( 0 == res ) {
		return -1;
	}

	memset(pRet, 0, sizeof(TagBinding));

	row = mysql_fetch_row(res);
	if ( 0 == row ) {
		mysql_free_result(res);
		return 0;
	}
	
	if (row[2]) {
		STRNCPY(pRet->m_szTagName, row[2], sizeof(pRet->m_szTagName));
	}

	if (row[1]) {
		sscanf(row[1], "%lu", &pRet->m_dwPatientId);
	}

	if (row[3]) {
		STRNCPY(pRet->m_szPatientName, row[3], sizeof(pRet->m_szPatientName));
	}
	

	mysql_free_result(res);

#endif
	return 0;
}

int  CMyDb::DbHeartBeat() {

#if DB_FLAG
	if ( m_nStatus == 0 ) {
		return -1;
	}

	int ret = mysql_ping(&m_mysql);
	if ( 0 != ret ) {
		m_nStatus = 0;
		m_sigStatus.emit(m_nStatus);
	}
#endif

	return 0;
}

void  CMyDb::BuffTemp(const CSaveTempParam * pParam) {
	if (m_vTemps.size() >= 270000) {
		std::vector<TempItem *>::iterator it = m_vTemps.begin();
		TempItem * pData = *it;
		m_vTemps.erase(it);
		delete pData;
	}

	TempItem * pItem = new TempItem;
	pItem->m_dwIndex = pParam->m_dwIndex;
	pItem->m_dwSubIndex = pParam->m_dwSubIndex;
	pItem->m_dwTemp = pParam->m_tTemp.m_dwTemp;
	pItem->m_Time = pParam->m_tTemp.m_Time;
	STRNCPY(pItem->m_szTagId, pParam->m_tTemp.m_szTagId, sizeof(pItem->m_szTagId));
	m_vTemps.push_back(pItem);
}

int  CMyDb::SaveTemp(const CSaveTempParam * pParam) {
#if DB_FLAG
	if (m_nStatus == 0) {
		BuffTemp(pParam);
		return -1;
	}

	char  szSql[8192];
	char  szTime[256];
	SNPRINTF(szSql, sizeof(szSql), 
		"insert into temperature values (null, '%s', '%s', %lu, %lu) ", 
		pParam->m_tTemp.m_szTagId, 
		Date2String_2(szTime, sizeof(szTime), &pParam->m_tTemp.m_Time ), 
		pParam->m_tTemp.m_dwTemp, 
		pParam->m_dwIndex * MAX_READERS_PER_GRID + pParam->m_dwSubIndex );

	int ret = mysql_query(&m_mysql, szSql);
	if ( 0 != ret ) {
		DWORD dwError = mysql_errno(&m_mysql);
		if (2013 == dwError || 2006 == dwError) {
			m_nStatus = 0;
			m_sigStatus.emit(m_nStatus);
		}

		BuffTemp(pParam);
		return -2;
	}
#endif
	return 0;
}

int  CMyDb::SaveTemp(const TempItem * pTemp) {

#if DB_FLAG
	char  szSql[8192];
	char  szTime[256];
	SNPRINTF(szSql, sizeof(szSql),
		"insert into temperature values (null, '%s', '%s', %lu, %lu) ",
		pTemp->m_szTagId,
		Date2String_2(szTime, sizeof(szTime), &pTemp->m_Time),
		pTemp->m_dwTemp,
		pTemp->m_dwIndex * MAX_READERS_PER_GRID + pTemp->m_dwSubIndex);

	int ret = mysql_query(&m_mysql, szSql);
	if (0 != ret) {
		DWORD dwError = mysql_errno(&m_mysql);
		if (2013 == dwError || 2006 == dwError) {
			m_nStatus = 0;
			m_sigStatus.emit(m_nStatus);
		}
		return -1;
	}
#endif

	return 0;
}

int  CMyDb::GetAllPatients(std::vector<Patient *> & vRet, DWORD dwPatiendId /*= 0*/) {

#if DB_FLAG
	if (m_nStatus == 0) {
		return -1;
	}

	char  szSql[8192];
	if ( 0 == dwPatiendId )
		SNPRINTF(szSql, sizeof(szSql),"select * from patientinfo");
	else
		SNPRINTF(szSql, sizeof(szSql), "select * from patientinfo where id = %lu", dwPatiendId );

	MYSQL_RES *res = 0;
	MYSQL_ROW row;

	if (0 != mysql_query(&m_mysql, szSql)) {
		return -1;
	}

	res = mysql_store_result(&m_mysql);
	if (0 == res) {
		return -1;
	}

	while ( row = mysql_fetch_row(res) ) {
		Patient * pItem = new Patient;
		memset(pItem, 0, sizeof(Patient));
		pItem->m_bMale = TRUE;

		sscanf(row[0], "%lu", &pItem->m_dwPatientId);
		if ( row[1] ) {
			STRNCPY(pItem->m_szId, row[1], sizeof(pItem->m_szId));
		}
		if (row[2]) {
			STRNCPY(pItem->m_szName, row[2], sizeof(pItem->m_szName));
		}
		if (row[3]) {
			char ch;
			sscanf(row[3], "%c", &ch);
			if ( ch == '0' ) {
				pItem->m_bMale = FALSE;
			}
		}
		if (row[5]) {
			sscanf(row[5], "%lu", &pItem->m_dwAge);
		}
		vRet.push_back(pItem);
	}

	mysql_free_result(res);

#endif
	return 0;
}

int  CMyDb::SetBinding(const CBindTagsParam * pParam) {

#if DB_FLAG
	if (m_nStatus == 0) {
		return -1;
	}

	char  szSql[8192];
	for (DWORD i = 0; i < pParam->m_dwItemsCnt; i++) {
		SNPRINTF(szSql, sizeof(szSql), "insert into tagbands values('%s', %lu, '%s' ) ",
			pParam->m_items[i].m_szTagId, pParam->m_dwPatientId, pParam->m_items[i].m_szTagName );

		if (0 != mysql_query(&m_mysql, szSql)) {
			return -1;
		}
	}
#endif

	return 0;
}