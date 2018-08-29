#include "business.h"
#include "LmnTelSvr.h"

CBusiness * CBusiness::pInstance = 0;

CBusiness *  CBusiness::GetInstance() {
	if (0 == pInstance) {
		pInstance = new CBusiness;
	}
	return pInstance;
}

CBusiness::CBusiness() : m_launch(this) {
	memset(m_szAlarmFile, 0, sizeof(m_szAlarmFile));
	m_bFirstHeartBeats = TRUE;
	memset(m_bQueryRet, 0, sizeof(m_bQueryRet));
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
	DWORD  dwCfgValue = 0;

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

		strText.Format(CFG_BED_NO " %d", i + 1);
		g_cfg->GetConfig(strText, g_data.m_dwBedNo[i], -1);

		strText.Format(CFG_READER_SWITCH " %d", i + 1);		
		g_cfg->GetConfig(strText, dwCfgValue, TRUE);
		g_data.m_bReaderSwitch[i] = dwCfgValue;
	}
	
	g_cfg->GetConfig(CFG_SHOWING_LOWEST_TEMP, g_data.m_dwMyImageMinTemp,       DEFAULT_LOWEST_TEMP);
	g_cfg->GetConfig(CFG_MYIMAGE_LEFT_BLANK,  g_data.m_dwMyImageLeftBlank,     DEFAULT_MYIMAGE_LEFT_BLANK);
	g_cfg->GetConfig(CFG_MYIMAGE_RIGHT_BLANK, g_data.m_dwMyImageRightBlank,    DEFAULT_MYIMAGE_RIGHT_BLANK);
	g_cfg->GetConfig(CFG_COLLECT_INTERVAL,    g_data.m_dwCollectInterval,      DEFAULT_COLLECT_INTERVAL);
	g_cfg->GetConfig(CFG_ONCE_COLLECT_WIDTH,  g_data.m_dwCollectIntervalWidth, DEFAULT_COLLECT_INTERVAL_WIDTH);
	g_cfg->GetConfig(CFG_AREA_ID_NAME, g_data.m_dwAreaNo, 0);
	g_cfg->GetConfig(CFG_ALARM_VOICE_SWITCH, dwCfgValue, DEFAULT_ALARM_VOICE_SWITCH);
	g_data.m_bAlarmVoiceOff = dwCfgValue;

	// ��Ĭ�ϵĴ���1, 2, 3
	for (int i = 0; i < MAX_READERS_COUNT; i++) {
		if ( g_data.m_dwBedNo[i] == -1 ) {

			int j = 0;
			for (j = 0; j < MAX_READERS_COUNT; j++) {
				if (g_data.m_dwBedNo[j] == i + 1) {
					break;
				}
			}

			if (j >= MAX_READERS_COUNT) {
				g_data.m_dwBedNo[i] = i + 1;
			}
			else {
				g_data.m_dwBedNo[i] = 0;
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
		if (0 == area.dwAreaNo || area.dwAreaNo > MAX_AREA_ID) {
			break;
		}

		TArea * pArea = new TArea;
		memcpy(pArea, &area, sizeof(TArea));
		g_vArea.push_back(pArea);
	}

	if ( g_data.m_dwAreaNo > 0 ) {
		std::vector<TArea *>::iterator it;
		for (it = g_vArea.begin(); it != g_vArea.end(); ++it) {
			TArea * pArea = *it;
			if (pArea->dwAreaNo == g_data.m_dwAreaNo ) {
				break;
			}
		}

		// ���g_dwAreaNo����g_vArea֮��
		if (it == g_vArea.end()) {
			TArea * pArea = new TArea;
			pArea->dwAreaNo = g_data.m_dwAreaNo;
			SNPRINTF(pArea->szAreaName, sizeof(pArea->szAreaName), "(��ţ�%lu)", g_data.m_dwAreaNo);
			g_vArea.push_back(pArea);
		}
	}

	g_cfg->GetConfig(CFG_ALARM_FILE, g_data.m_szAlarmFilePath, sizeof(g_data.m_szAlarmFilePath), "");
	if (g_data.m_szAlarmFilePath[0] == '\0') {
		GetDefaultAlarmFile(g_data.m_szAlarmFilePath, sizeof(g_data.m_szAlarmFilePath));
	}
	else {
		if (-1 == GetFileAttributes(g_data.m_szAlarmFilePath))
		{
			GetDefaultAlarmFile(g_data.m_szAlarmFilePath, sizeof(g_data.m_szAlarmFilePath));
		}
	}

	g_data.m_bAutoScroll = TRUE;
	memset(g_data.m_szLaunchComPort, 0, sizeof(g_data.m_szLaunchComPort));
	memset(g_data.m_nReaderStatus, 0, sizeof(g_data.m_nReaderStatus));
	memset(g_data.m_nQueryTempRetryTime, 0, sizeof(g_data.m_nQueryTempRetryTime));
	memset(g_data.m_dwLastQueryTick, 0, sizeof(g_data.m_dwLastQueryTick));

	g_thrd_work = new LmnToolkits::PriorityThread();
	if (0 == g_thrd_work) {
		return -1;
	}
	g_thrd_work->Start();

	g_thrd_launch = new LmnToolkits::PriorityThread();
	if (0 == g_thrd_launch) {
		return -1;
	}
	g_thrd_launch->Start();
	
	// ReconnectLaunchAsyn(200);

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

	Clear();
	return 0;
}

// ����������
int    CBusiness::UpdateScrollAsyn(DWORD dwDelay /*= 0*/) {
	if (0 == dwDelay) {
		g_thrd_work->PostMessage(this, MSG_UPDATE_SCROLL );
	}
	else {
		g_thrd_work->PostDelayMessage(dwDelay, this, MSG_UPDATE_SCROLL );
	}
	return 0;
}

int    CBusiness::UpdateScroll() {
	::PostMessage(g_hWnd, UM_UPDATE_SCROLL, 0, 0);
	return 0;
}

// ����
int   CBusiness::AlarmAsyn(const char * szAlarmFile) {
	g_thrd_work->PostMessage(this, MSG_ALARM, new CAlarmParam(szAlarmFile));
	return 0;
}

int   CBusiness::Alarm(const CAlarmParam * pParam) {
	DuiLib::CDuiString strText;

	// ����رձ�������
	if ( g_data.m_bAlarmVoiceOff ) {
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

// ����������
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
	int ret = m_launch.Reconnect();
	// ʧ�ܺ�����
	if (0 != ret) {
		ReconnectLaunchAsyn(RECONNECT_LAUNCH_TIME_INTERVAL);
	}
	else {
		// ֪ͨ�ɹ�
		NotifyUiLaunchStatus( m_launch.GetStatus() );
		ReadLaunchAsyn(READ_LAUNCH_INTERVAL);

		// ��ȡReder����״̬
		DWORD  dwCnt = MAX_READERS_COUNT;
		for (DWORD i = 0; i < dwCnt; i++) {
			ReaderHeartBeatAsyn(i);
		}
	}
	
	return 0;
}

// ֪ͨ����Launch״̬
int   CBusiness::NotifyUiLaunchStatus(CLmnSerialPort::PortStatus eStatus) {
	::PostMessage(g_hWnd, UM_LAUNCH_STATUS, eStatus, 0);
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
	//JTelSvrPrint("launch thread message count = %lu", g_thrd_launch->GetMessagesCount());
	m_launch.ReadComData();
	ReadLaunchAsyn(READ_LAUNCH_INTERVAL);
	return 0;
}

// ֪ͨ���������ص�Reader����״̬
int   CBusiness::NotifyUiReaderStatus(DWORD dwGridIndex, int nStatus) {
	::PostMessage(g_hWnd, UM_GRID_READER_STATUS, dwGridIndex, nStatus);
	return 0;
}

// ֪ͨ�����¶�����
int   CBusiness::NotifyUiTempData(DWORD dwGridIndex, DWORD  dwTemp) {
	::PostMessage(g_hWnd, UM_TEMP_DATA, dwGridIndex, dwTemp);
	return 0;
}

// Reader����
int   CBusiness::ReaderHeartBeatAsyn(DWORD dwGridIndex, DWORD dwDelayTime /*= 0*/) {
	if (0 == dwDelayTime) {
		g_thrd_launch->PostMessage(this, MSG_READER_HEART_BEAT + dwGridIndex,
			new CReaderHeartBeatParam(dwGridIndex), TRUE);
	}
	else {
		g_thrd_launch->PostDelayMessage(dwDelayTime, this, MSG_READER_HEART_BEAT + dwGridIndex,
			new CReaderHeartBeatParam(dwGridIndex), TRUE);
	}
	return 0;
}

int   CBusiness::ReaderHeartBeat(const CReaderHeartBeatParam * pParam) {
	g_data.m_dwLastQueryTick[pParam->m_dwGridIndex] = LmnGetTickCount();
	m_launch.HeartBeat(pParam);
	return 0;
}

// ��ȡ�¶�
int   CBusiness::QueryTemperatureAsyn(DWORD dwGridIndex, DWORD dwDelayTime /*= 0*/) {
	if (0 == dwDelayTime) {
		g_thrd_launch->PostMessage(this, MSG_GET_TEMPERATURE + dwGridIndex, new CGetTemperatureParam(dwGridIndex), TRUE, 10);
	}
	else {
		g_thrd_launch->PostDelayMessage(dwDelayTime, this, MSG_GET_TEMPERATURE + dwGridIndex, new CGetTemperatureParam(dwGridIndex), TRUE, 10);
	}
	return 0;
}

int   CBusiness::QueryTemperature(const CGetTemperatureParam * pParam) {
	g_data.m_dwLastQueryTick[pParam->m_dwGridIndex] = LmnGetTickCount();
	int ret = m_launch.QueryTemperature(pParam);
	return 0;
}

// �ӷ������յ�����ĸ�ʽ
void  CBusiness::OnLaunchError() {
	m_launch.CloseLaunch();
	NotifyUiLaunchStatus(m_launch.GetStatus());
	// ReconnectLaunchAsyn(RECONNECT_LAUNCH_TIME_INTERVAL);
	m_bFirstHeartBeats = TRUE;
	for ( DWORD i = 0; i < MAX_READERS_COUNT; i++) {
		g_data.m_nReaderStatus[i] = READER_STATUS_CLOSE;
		g_data.m_nQueryTempRetryTime[i] = 0;
		g_data.m_dwLastQueryTick[i] = 0;
		NotifyUiReaderStatus(i, READER_STATUS_CLOSE);
	}
}

void  CBusiness::OnHeartBeatRet(DWORD  dwIndex, int nRet) {
	//JTelSvrPrint("OnHeartBeatRet: index,%lu  ret: %d", dwIndex, nRet);

	g_data.m_dwLastQueryTick[dwIndex] = 0;
	g_data.m_nQueryTempRetryTime[dwIndex] = 0;

	if (0 == nRet) {
		g_data.m_nReaderStatus[dwIndex] = READER_STATUS_OPEN;		
		NotifyUiReaderStatus(dwIndex, READER_STATUS_OPEN);
	}
	else {
		g_data.m_nReaderStatus[dwIndex] = READER_STATUS_CLOSE;
		NotifyUiReaderStatus(dwIndex, READER_STATUS_CLOSE);
		// ��һ������
		ReaderHeartBeatAsyn(dwIndex, NEXT_HEART_BEAT_TIME);
	}

	// ����ǵ�һ��ȫ����������
	if (m_bFirstHeartBeats) {
		m_bQueryRet[dwIndex] = TRUE;
		// �������һ�����󶼵õ����
		if ( IsGetAllQueryRet() ) {
			m_bFirstHeartBeats = FALSE;
			QueryRoundTemperatureAsyn();
		}
	}
}

// dwTempΪ-1��ʾʧ�ܣ���ʱ3��û�л�ȡ���¶ȣ�
void  CBusiness::OnTempRet(DWORD dwIndex, DWORD dwTemp) {
	g_data.m_dwLastQueryTick[dwIndex] = 0;
	g_data.m_nQueryTempRetryTime[dwIndex] = 0;

	// ��ȡ�¶ȳɹ�
	if ( dwTemp != -1 ) {
		NotifyUiTempData(dwIndex, dwTemp);
	}
	// ��ȡ�¶�ʧ��
	else {
		g_data.m_nReaderStatus[dwIndex] = READER_STATUS_CLOSE;
		NotifyUiReaderStatus(dwIndex, READER_STATUS_CLOSE);
		// ��һ������
		ReaderHeartBeatAsyn(dwIndex, NEXT_HEART_BEAT_TIME);
	}

	m_bQueryRet[dwIndex] = TRUE;
	// �������һ�����󶼵õ����
	if (IsGetAllQueryRet()) {
		QueryRoundTemperatureAsyn(g_data.m_dwCollectInterval * 1000);
	}

}

// �Ƿ�õ����е������ж�һ�����������
BOOL  CBusiness::IsGetAllQueryRet() {
	DWORD i = 0;
	for ( i = 0; i < MAX_READERS_COUNT; i++) {
		if (!m_bQueryRet[i]) {
			break;
		}
	}
	if (i >= MAX_READERS_COUNT) {
		return TRUE;
	}
	else {
		return FALSE;
	}
}

// ��ȡһ���е��¶�
int   CBusiness::QueryRoundTemperatureAsyn(DWORD dwDelayTime /*= 0*/) {
	if (0 == dwDelayTime) {
		g_thrd_launch->PostMessage(this, MSG_ROUND_TEMP,0,TRUE,5);
	}
	else {
		g_thrd_launch->PostDelayMessage(dwDelayTime, this, MSG_ROUND_TEMP,0,TRUE,5);
	}
	return 0;
}

int   CBusiness::QueryRoundTemperature() {
	memset(m_bQueryRet, 0, sizeof(m_bQueryRet));

	for (DWORD i = 0; i < MAX_READERS_COUNT; i++) {
		if (g_data.m_nReaderStatus[i] == READER_STATUS_OPEN) {
			QueryTemperatureAsyn(i);
		}
		// ���close״̬������Ϊ��ý��
		else {
			m_bQueryRet[i] = TRUE;
		}
	}

	// ���ȫ��������Ϊclose״̬�����һ��ʱ���ȡ��һ���¶�
	if ( IsGetAllQueryRet() ) {
		QueryRoundTemperatureAsyn( g_data.m_dwCollectInterval * 1000 );
	}
	return 0;
}

// Ӳ���Ķ������״̬
int   CBusiness::CheckLaunchStatusAsyn() {
	g_thrd_launch->PostMessage(this, MSG_CHECK_LAUNCH_STATUS, 0, TRUE );
	return 0;
}

int   CBusiness::CheckLaunchStatus() {
	m_launch.CheckStatus();
	return 0;
}

// ��Ϣ����
void CBusiness::OnMessage(DWORD dwMessageId, const  LmnToolkits::MessageData * pMessageData) {
	switch (dwMessageId)
	{
	case MSG_UPDATE_SCROLL:
	{
		UpdateScroll();
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

	case MSG_READ_LAUNCH:
	{
		ReadLaunch();
	}
	break;

	case MSG_ROUND_TEMP:
	{
		QueryRoundTemperature();
	}
	break;

	case MSG_CHECK_LAUNCH_STATUS:
	{
		CheckLaunchStatus();
	}
	break;

	default:
	{
		if (dwMessageId >= MSG_READER_HEART_BEAT && dwMessageId < MSG_READER_HEART_BEAT + MAX_GRID_COUNT) {
			CReaderHeartBeatParam * pParam = (CReaderHeartBeatParam *)pMessageData;
			ReaderHeartBeat(pParam);
		}
		else if (dwMessageId >= MSG_GET_TEMPERATURE && dwMessageId < MSG_GET_TEMPERATURE + MAX_GRID_COUNT) {
			CGetTemperatureParam * pParam = (CGetTemperatureParam *)pMessageData;
			QueryTemperature(pParam);
		}
	}
	break;

	}
}