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

	for (int i = 0; i < MAX_GRID_COUNT; i++) {
		m_reader[i].m_pBusiness = this;
		m_reader[i].m_nIndex = i;
	}

	memset(m_bReaderAvailable, 0, sizeof(m_bReaderAvailable));
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
	char buf[8192];

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

		strText.Format(CFG_COM_PORT " %d", i + 1);
		g_cfg->GetConfig(strText, g_szComPort[i], MAX_COM_PORT_LENGTH, "");

		strText.Format(CFG_LAST_BED_NAME " %d", i + 1);
		g_cfg->GetConfig(strText, g_szLastBedName[i], MAX_BED_NAME_LENGTH, "--");

		strText.Format(CFG_LAST_PATIENT_NAME " %d", i + 1);
		g_cfg->GetConfig(strText, g_szLastPatientName[i], MAX_PATIENT_NAME_LENGTH, "--");
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

	g_cfg->GetConfig("prepare command", buf, sizeof(buf), "55 01 01 DD AA");
	TransferReaderCmd(PREPARE_COMMAND, buf);

	g_cfg->GetConfig("read tag data command", buf, sizeof(buf), "55 01 02 DD AA");
	TransferReaderCmd(READ_TAG_DATA_COMMAND, buf);

	g_cfg->GetConfig("serial port sleep time", SERIAL_PORT_SLEEP_TIME, 30000);
	if (SERIAL_PORT_SLEEP_TIME < 30000) {
		SERIAL_PORT_SLEEP_TIME = 30000;
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

	for (int i = 0; i < MAX_GRID_COUNT; i++) {
		g_thrd_reader[i] = new LmnToolkits::Thread();
		if (0 == g_thrd_reader[i]) {
			return -1;
		}
		g_thrd_reader[i]->Start();
	}

	DWORD  dwRelay = 200;
	for (DWORD i = 0; i < g_dwLayoutRows * g_dwLayoutColumns; i++) {
		m_bReaderAvailable[i] = TRUE;
		ReconnectReaderAsyn(i, dwRelay);
		dwRelay += 200;
	}

	return 0;
}

int CBusiness::DeInit() {

	//if (g_thrd_db) {
	//	g_thrd_db->Stop();
	//	delete g_thrd_db;
	//	g_thrd_db = 0;
	//}

	for (int i = 0; i < MAX_GRID_COUNT; i++) {
		if (g_thrd_reader[i]) {
			m_reader[i].m_bStop = TRUE;
			g_thrd_reader[i]->Stop();
			delete g_thrd_reader[i];
			g_thrd_reader[i] = 0;
		}		
	}

	if (g_thrd_work) {
		if (g_thrd_work) {
			g_thrd_work->Stop();
			delete g_thrd_work;
			g_thrd_work = 0;
		}
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

// 重连Reader
int   CBusiness::ReconnectReaderAsyn(int nIndex, DWORD dwDelayTime /*= 0*/) {
	assert(nIndex >= 0 && nIndex < MAX_GRID_COUNT);
	// 如果被禁止使用
	if (!m_bReaderAvailable[nIndex]) {
		return 0;
	}

	m_reader[nIndex].m_bStop = TRUE;
	g_thrd_reader[nIndex]->DeleteMessages();

	if (0 == dwDelayTime) {
		g_thrd_reader[nIndex]->PostMessage(this, MSG_RECONNECT_READER, new CReconnectReaderParam(nIndex));
	}
	else {
		g_thrd_reader[nIndex]->PostDelayMessage(dwDelayTime, this, MSG_RECONNECT_READER, new CReconnectReaderParam(nIndex));
	}
	return 0;
}

int   CBusiness::ReconnectReader(const CReconnectReaderParam * pParam) {
	int nIndex = pParam->m_nIndex;
	assert(nIndex >= 0 && nIndex < MAX_GRID_COUNT);
	return m_reader[nIndex].Reconnect();
}

// 通知界面连接结果
int   CBusiness::NotifyUiReaderStatus(int nIndex, CTelemedReader::READER_STATUS eStatus) {
	::PostMessage(g_hWnd, UM_SHOW_READER_STATUS, eStatus, nIndex);
	return 0;
}

// 获取状态
CTelemedReader::READER_STATUS  CBusiness::GetReaderStatus(int nIndex) {
	assert(nIndex >= 0 && nIndex < MAX_GRID_COUNT);
	return m_reader[nIndex].GetStatus();
}

// 获取端口号
const char * CBusiness::GetReaderComPort(int nIndex) {
	assert(nIndex >= 0 && nIndex < MAX_GRID_COUNT);
	return m_reader[nIndex].m_szComPort;
}

// 读取Tag温度
int   CBusiness::ReadTagTempAsyn(int nIndex, DWORD dwDelayTime /*= 0*/) {
	assert(nIndex >= 0 && nIndex < MAX_GRID_COUNT);

	if (0 == dwDelayTime) {
		g_thrd_reader[nIndex]->PostMessage(this, MSG_GET_TAG_TEMP, new CReadTempParam(nIndex));
	}
	else {
		g_thrd_reader[nIndex]->PostDelayMessage(dwDelayTime, this, MSG_GET_TAG_TEMP, new CReadTempParam(nIndex));
	}

	return 0;
}

int   CBusiness::ReadTagTemp(const CReadTempParam * pParam) {
	int nIndex = pParam->m_nIndex;
	assert(nIndex >= 0 && nIndex < MAX_GRID_COUNT);

	DWORD  dwTemp = 0;
	int ret = m_reader[nIndex].ReadTagTemp(dwTemp);
	NotifyUiReadTagTemp(nIndex, ret, dwTemp);
	return 0;
}

int   CBusiness::NotifyUiReadTagTemp(int nIndex, int ret, DWORD dwTemp) {
	assert(nIndex >= 0 && nIndex < MAX_GRID_COUNT);

	::PostMessage(g_hWnd, UM_SHOW_READ_TAG_TEMP_RET, ret, (nIndex << 16) | (dwTemp & 0xFFFF));
	return 0;
}

int   CBusiness::StopReaderAsyn(int nIndex) {
	assert(nIndex >= 0 && nIndex < MAX_GRID_COUNT);
	m_bReaderAvailable[nIndex] = FALSE;
	m_reader[nIndex].m_bStop = TRUE;
	g_thrd_reader[nIndex]->DeleteMessages();
	g_thrd_reader[nIndex]->PostMessage(this, MSG_STOP_READER, new CStopReaderParam(nIndex));
	return 0;
}

int   CBusiness::StopReader(const CStopReaderParam * pParam) {
	int nIndex = pParam->m_nIndex;
	m_reader[nIndex].CloseUartPort();
	NotifyUiReaderStatus(nIndex, m_reader[nIndex].GetStatus());
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

	case MSG_RECONNECT_READER:
	{
		CReconnectReaderParam * pParam = (CReconnectReaderParam *)pMessageData;
		ReconnectReader(pParam);
	}
	break;

	case MSG_GET_TAG_TEMP:
	{
		CReadTempParam * pParam = (CReadTempParam *)pMessageData;
		ReadTagTemp(pParam);
	}
	break;

	case MSG_STOP_READER:
	{
		CStopReaderParam * pParam = (CStopReaderParam *)pMessageData;
		StopReader(pParam);
	}
	break;

	default:
		break;
	}
}