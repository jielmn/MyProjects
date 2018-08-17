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

	if (g_cfg_area) {
		g_cfg_area->Deinit();
		delete g_cfg_area;
		g_cfg_area = 0;
	}

	ClearVector(g_vArea);
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

	g_cfg_area = new FileConfigEx();
	if (0 == g_cfg_area) {
		return -1;
	}
	g_cfg_area->Init(AREA_CFG_FILE_NAME);

	DWORD  dwCfgValue = 0;

	g_cfg->GetConfig(CFG_MAIN_LAYOUT_COLUMNS, g_dwLayoutColumns,  DEFAULT_MAIN_LAYOUT_COLUMNS);
	g_cfg->GetConfig(CFG_MAIN_LAYOUT_ROWS,    g_dwLayoutRows,     DEFAULT_MAIN_LAYOUT_ROWS);
	g_cfg->GetConfig(CFG_TIME_UNIT_WIDTH,     g_dwTimeUnitWidth,  DEFAULT_TIME_UNIT_WIDTH);
	g_cfg->GetConfig(CFG_MAX_POINTS_COUNT,    g_dwMaxPointsCount, DEFAULT_MAX_POINTS_COUNT);
	if (g_dwMaxPointsCount < DEFAULT_MAX_POINTS_COUNT) {
		g_dwMaxPointsCount = DEFAULT_MAX_POINTS_COUNT;
	}
	else if (g_dwMaxPointsCount > 3 * DEFAULT_MAX_POINTS_COUNT) {
		g_dwMaxPointsCount = 3 * DEFAULT_MAX_POINTS_COUNT;
	}
	g_cfg->GetConfig(CFG_AREA_ID_NAME, g_dwAreaNo, 0);
	

	g_cfg->GetConfig(CFG_MYIMAGE_LEFT_BLANK,  g_dwMyImageLeftBlank,  DEFALUT_MYIMAGE_LEFT_BLANK);
	g_cfg->GetConfig(CFG_MYIMAGE_RIGHT_BLANK, g_dwMyImageRightBlank, DEFALUT_MYIMAGE_RIGHT_BLANK);
	g_cfg->GetConfig(CFG_MYIMAGE_TIME_TEXT_OFFSET_X, g_dwMyImageTimeTextOffsetX, DEFAULT_MYIMAGE_TIME_TEXT_OFFSET_X);
	g_cfg->GetConfig(CFG_MYIMAGE_TIME_TEXT_OFFSET_Y, g_dwMyImageTimeTextOffsetY, DEFAULT_MYIMAGE_TIME_TEXT_OFFSET_Y);
	g_cfg->GetConfig(CFG_MYIMAGE_TEMP_TEXT_OFFSET_X, g_dwMyImageTempTextOffsetX, DEFAULT_MYIMAGE_TEMP_TEXT_OFFSET_X);
	g_cfg->GetConfig(CFG_MYIMAGE_TEMP_TEXT_OFFSET_Y, g_dwMyImageTempTextOffsetY, DEFAULT_MYIMAGE_TEMP_TEXT_OFFSET_Y);
	g_cfg->GetConfig(CFG_ALARM_VOICE_SWITCH, dwCfgValue, DEFAULT_ALARM_VOICE_SWITCH);
	g_bAlarmVoiceOff = dwCfgValue;
	g_cfg->GetConfig(CFG_SKIN, g_dwSkinIndex, DEFAULT_SKIN);
	g_skin.SetSkin(g_dwSkinIndex);

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

		//strText.Format(CFG_COM_PORT " %d", i + 1);
		//g_cfg->GetConfig(strText, g_szComPort[i], MAX_COM_PORT_LENGTH, "");

		strText.Format(CFG_BED_NO " %d", i + 1);
		g_cfg->GetConfig(strText, g_dwBedNo[i], 0);

		strText.Format(CFG_LAST_BED_NAME " %d", i + 1);
		g_cfg->GetConfig(strText, g_szLastBedName[i], MAX_BED_NAME_LENGTH, "--");

		strText.Format(CFG_LAST_PATIENT_NAME " %d", i + 1);
		g_cfg->GetConfig(strText, g_szLastPatientName[i], MAX_PATIENT_NAME_LENGTH, "--");
	}

	DWORD  dwCount = g_dwLayoutColumns * g_dwLayoutRows;
	// 给默认的床号1, 2, 3
	for (int i = 0; i < MAX_GRID_COUNT; i++) {
		if ( g_dwBedNo[i] == 0 ) {

			int j = 0;
			for ( j = 0; j < MAX_GRID_COUNT; j++) {
				if ( g_dwBedNo[j] == i + 1 ) {
					break;
				}
			}

			if ( j >= MAX_GRID_COUNT ) {
				g_dwBedNo[i] = i + 1;
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
		if (0 == area.dwAreaNo || area.dwAreaNo > MAX_AREA_ID ) {
			break;
		}

		TArea * pArea = new TArea;
		memcpy(pArea, &area, sizeof(TArea));
		g_vArea.push_back(pArea);
	}

	if (g_dwAreaNo > 0) {
		std::vector<TArea *>::iterator it;
		for (it = g_vArea.begin(); it != g_vArea.end(); ++it) {
			TArea * pArea = *it;
			if (pArea->dwAreaNo == g_dwAreaNo) {
				break;
			}
		}

		// 如果g_dwAreaNo不在g_vArea之内
		if (it == g_vArea.end()) {
			TArea * pArea = new TArea;
			pArea->dwAreaNo = g_dwAreaNo;
			SNPRINTF(pArea->szAreaName, sizeof(pArea->szAreaName), "(编号：%lu)", g_dwAreaNo );
			g_vArea.push_back(pArea);
		}
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

	g_cfg->GetConfig(CFG_LAUNCH_COM_PORT, g_szLaunchComPort, sizeof(g_szLaunchComPort), "");

	g_cfg->GetConfig(CFG_LAUNCH_WRITE_INTERVAL, g_dwLaunchWriteInterval, DEFAULT_LAUNCH_WRITE_INTERVAL);
	

	if ( g_dwLayoutRows * g_dwLayoutColumns > MAX_GRID_COUNT ) {
		g_log->Output(ILog::LOG_SEVERITY_ERROR, "too much columns * rows(%lu * %lu) \n", g_dwLayoutColumns, g_dwLayoutRows );
		return -1;
	}

	//g_thrd_db = new LmnToolkits::Thread();
	//if (0 == g_thrd_db) {
	//	return -1;
	//}
	//g_thrd_db->Start();

	g_thrd_work = new LmnToolkits::Thread();
	if (0 == g_thrd_work) {
		return -1;
	}
	g_thrd_work->Start();

	g_thrd_launch = new LmnToolkits::Thread();
	if (0 == g_thrd_launch) {
		return -1;
	}
	g_thrd_launch->Start();
	
	ReconnectLaunchAsyn(200);

	return 0;
}

int CBusiness::DeInit() {

	//if (g_thrd_db) {
	//	g_thrd_db->Stop();
	//	delete g_thrd_db;
	//	g_thrd_db = 0;
	//}

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

	Clear();
	return 0;
}

// 调整滑动条
int  CBusiness::UpdateScrollAsyn(int nIndex, DWORD dwDelay /*= 0*/) {
	if (0 == dwDelay) {
		g_thrd_work->PostMessage( this, MSG_UPDATE_SCROLL, new CUpdateScrollParam(nIndex) );
	}
	else {
		g_thrd_work->PostDelayMessage(dwDelay, this, MSG_UPDATE_SCROLL, new CUpdateScrollParam(nIndex) );
	}
	return 0;
}

int  CBusiness::UpdateScroll(const CUpdateScrollParam * pParam) {
	::PostMessage(g_hWnd, UM_UPDATE_SCROLL, 0, 0);
	return 0;
}

int   CBusiness::AlarmAsyn(const char * szAlarmFile) {
	g_thrd_work->PostMessage(this, MSG_ALARM, new CAlarmParam(szAlarmFile));
	return 0;
}

int   CBusiness::Alarm(const CAlarmParam * pParam) {
	DuiLib::CDuiString strText;

	// 如果关闭报警开关
	if ( g_bAlarmVoiceOff ) {
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

int   CBusiness::ReconnectLaunchAsyn(DWORD dwDelayTime /*= 0*/) {

	g_thrd_launch->DeleteMessages();

	if (0 == dwDelayTime) {
		g_thrd_launch->PostMessage(this, MSG_RECONNECT_LAUNCH );
	}
	else {
		g_thrd_launch->PostDelayMessage(dwDelayTime, this, MSG_RECONNECT_LAUNCH );
	}
	return 0;
}

int   CBusiness::ReconnectLaunch() {
	m_launch.Reconnect();
	return 0;
}

int  CBusiness::CheckLaunchStatusAsyn() {
	g_thrd_launch->PostMessage(this, MSG_CHECK_LAUNCH_STATUS);
	return 0;
}

int   CBusiness::CheckLaunchStatus() {
	m_launch.CheckStatus();
	return 0;
}

// 通知界面Launch状态
int   CBusiness::NotifyUiLaunchStatus(CLmnSerialPort::PortStatus eStatus) {
	::PostMessage(g_hWnd, UM_LAUNCH_STATUS, eStatus, 0);
	return 0;
}

// 通知状态栏其他信息
int   CBusiness::NotifyUiBarTips(int nIndex) {
	::PostMessage(g_hWnd, UM_BAR_TIPS, nIndex, 0);
	return 0;
}

// Reader心跳
int   CBusiness::ReaderHeartBeatAsyn(DWORD dwGridIndex, DWORD dwDelayTime /*= 0*/) {
	if (0 == dwDelayTime) {
		g_thrd_launch->PostMessage(this, MSG_READER_HEART_BEAT + dwGridIndex, 
			new CReaderHeartBeatParam(dwGridIndex));
	}
	else {
		g_thrd_launch->PostDelayMessage(dwDelayTime, this, MSG_READER_HEART_BEAT + dwGridIndex, 
			new CReaderHeartBeatParam(dwGridIndex));
	}
	return 0;
}

int   CBusiness::ReaderHeartBeat(const CReaderHeartBeatParam * pParam) {
	m_launch.HeartBeat(pParam);
	return 0;
}

int   CBusiness::QueryTemperatureAsyn(DWORD dwGridIndex, DWORD dwDelayTime /*= 0*/) {
	if (0 == dwDelayTime) {
		g_thrd_launch->PostMessage(this, MSG_GET_TEMPERATURE + dwGridIndex, new CGetTemperatureParam(dwGridIndex) );
	}
	else {
		g_thrd_launch->PostDelayMessage(dwDelayTime, this, MSG_GET_TEMPERATURE + dwGridIndex, new CGetTemperatureParam(dwGridIndex) );
	}
	return 0;
}

int   CBusiness::QueryTemperature(const CGetTemperatureParam * pParam) {
	int ret = m_launch.QueryTemperature(pParam);
	return 0;
}

// launch 读串口数据
int   CBusiness::ReadLaunchAsyn(DWORD dwDelayTime /*= 0*/) {
	if (0 == dwDelayTime) {
		g_thrd_launch->PostMessage(this, MSG_READ_LAUNCH );
	}
	else {
		g_thrd_launch->PostDelayMessage(dwDelayTime, this, MSG_READ_LAUNCH );
	}
	return 0;
}

int   CBusiness::ReadLaunch() {
	m_launch.ReadComData();
	ReadLaunchAsyn(200);
	return 0;
}

// 通知界面温度
// dwTemp如果为0，表示获取温度失败，可能连接不畅
int   CBusiness::NotifyUiTempData(DWORD dwGridIndex, DWORD  dwTemp) {
	::PostMessage(g_hWnd, UM_TEMP_DATA, dwGridIndex, dwTemp);
	return 0;
}

// 通知界面格子相关的Reader在线状态
int   CBusiness::NotifyUiGridReaderStatus(DWORD dwGridIndex, int nStatus) {
	::PostMessage(g_hWnd, UM_GRID_READER_STATUS, dwGridIndex, nStatus);
	return 0;
}




// 消息处理
void CBusiness::OnMessage(DWORD dwMessageId, const  LmnToolkits::MessageData * pMessageData) {
	switch (dwMessageId)
	{
	case MSG_UPDATE_SCROLL:
	{
		CUpdateScrollParam * pParam = (CUpdateScrollParam *)pMessageData;
		UpdateScroll(pParam);
	}
	break;

	case MSG_ALARM: 
	{
		CAlarmParam * pParam = (CAlarmParam *)pMessageData;
		Alarm(pParam);
	}
	break;

	case MSG_RECONNECT_LAUNCH:
	{
		ReconnectLaunch();
	}
	break;

	case MSG_CHECK_LAUNCH_STATUS:
	{
		CheckLaunchStatus();
	}
	break;

	case MSG_READ_LAUNCH:
	{
		ReadLaunch();
	}
	break;

	default:
	{
		if ( dwMessageId >= MSG_GET_TEMPERATURE && dwMessageId < MSG_GET_TEMPERATURE + MAX_GRID_COUNT ) {
			CGetTemperatureParam * pParam = (CGetTemperatureParam *)pMessageData;
			QueryTemperature(pParam);
		}
		else if (dwMessageId >= MSG_READER_HEART_BEAT && dwMessageId < MSG_READER_HEART_BEAT + MAX_GRID_COUNT) {
			CReaderHeartBeatParam * pParam = (CReaderHeartBeatParam *)pMessageData;
			ReaderHeartBeat(pParam);
		}


	}
	break;

	}
}