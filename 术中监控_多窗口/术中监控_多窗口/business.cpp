#include "business.h"
#include "UIlib.h"

CBusiness * CBusiness::pInstance = 0;

CBusiness *  CBusiness::GetInstance() {
	if (0 == pInstance) {
		pInstance = new CBusiness;
	}
	return pInstance;
}

CBusiness::CBusiness() {
	for (int i = 0; i < MYCHART_COUNT; i++) {
		m_reader[i].m_pBusiness = this;
		m_reader[i].m_nIndex = i;
	}
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

	DuiLib::CDuiString  strText;
	for (int i = 0; i < MYCHART_COUNT; i++) {
		strText.Format("low alarm %d", i + 1);
		g_cfg->GetConfig(strText, g_dwLowTempAlarm[i],3500);

		strText.Format("high alarm %d", i + 1);
		g_cfg->GetConfig(strText, g_dwHighTempAlarm[i], 4200);

		strText.Format("collect interval %d", i + 1);
		g_cfg->GetConfig(strText, g_dwCollectInterval[i], 10);

		strText.Format("min temperature degree %d", i + 1);
		g_cfg->GetConfig(strText, g_dwMinTemp[i], 24);

		strText.Format("com port %d", i + 1);
		g_cfg->GetConfig(strText, g_szComPort[i], 32, "");
	}

	g_cfg->GetConfig("alarm file", buf, sizeof(buf), "");
	if (buf[0] == '\0') {
		GetDefaultAlarmFile(g_szAlarmFilePath, sizeof(g_szAlarmFilePath));
	}
	else {
		if (-1 != GetFileAttributes(buf))
		{
			strncpy_s(g_szAlarmFilePath, buf, sizeof(g_szAlarmFilePath));
		}
		else {
			GetDefaultAlarmFile(g_szAlarmFilePath, sizeof(g_szAlarmFilePath));
		}
	}

	g_cfg->GetConfig("prepare command", buf, sizeof(buf), "55 01 01 DD AA");
	TransferReaderCmd(PREPARE_COMMAND, buf);

	g_cfg->GetConfig("read tag data command", buf, sizeof(buf), "55 01 02 DD AA");
	TransferReaderCmd(READ_TAG_DATA_COMMAND, buf);

	g_cfg->GetConfig("serial port sleep time", SERIAL_PORT_SLEEP_TIME, 30000);
	if (SERIAL_PORT_SLEEP_TIME < 30000) {
		SERIAL_PORT_SLEEP_TIME = 30000;
	}


	for (int i = 0; i < MYCHART_COUNT; i++) {
		g_thrd_reader[i] = new LmnToolkits::Thread();
		if (0 == g_thrd_reader[i]) {
			return -1;
		}
		g_thrd_reader[i]->Start();
	}

	for (int i = 0; i < MYCHART_COUNT; i++) {
		ReconnectReaderAsyn(i, 200);
	}
	

	return 0;
}

int CBusiness::DeInit() {

	for (int i = 0; i < MYCHART_COUNT; i++) {
		if (g_thrd_reader[i]) {
			g_thrd_reader[i]->Stop();
			delete g_thrd_reader[i];
			g_thrd_reader[i] = 0;
		}
	}

	Clear();
	return 0;
}

// 调整滑动条
int   CBusiness::UpdateScrollAsyn() {
	//::PostMessage(g_hWnd, UM_UPDATE_SCROLL, 0, 0);
	return 0;
}

int   CBusiness::AlarmAsyn(const char * szAlarmFile) {
	// g_thrd_background->PostMessage(this, MSG_ALARM, new CAlarmParam(szAlarmFile));
	return 0;
}

int   CBusiness::Alarm(const CAlarmParam * pParam) {
	DuiLib::CDuiString strText;

	//if (m_szAlarmFile[0] == '\0') {
	//	// open
	//	strText.Format("open %s", pParam->m_szAlarmFile);
	//	mciSendString(strText, NULL, 0, 0);

	//	// play
	//	strText.Format("play %s", pParam->m_szAlarmFile);
	//	mciSendString(strText, NULL, 0, 0);
	//}
	//else {
	//	// stop
	//	strText.Format("close %s", m_szAlarmFile);
	//	mciSendString(strText, NULL, 0, 0);

	//	// open
	//	strText.Format("open %s", pParam->m_szAlarmFile);
	//	mciSendString(strText, NULL, 0, 0);

	//	// play
	//	strText.Format("play %s", pParam->m_szAlarmFile);
	//	mciSendString(strText, NULL, 0, 0);

	//}
	//strncpy_s(m_szAlarmFile, pParam->m_szAlarmFile, sizeof(m_szAlarmFile));
	return 0;
}

// 重连Reader
int   CBusiness::ReconnectReaderAsyn(int nIndex, DWORD dwDelayTime /*= 0*/) {
	assert(nIndex >= 0 && nIndex < MYCHART_COUNT);
	if (0 == dwDelayTime) {
		g_thrd_reader[nIndex]->PostMessage(this, MSG_RECONNECT_READER, new CReconnectReaderParam(nIndex) );
	}
	else {
		g_thrd_reader[nIndex]->PostDelayMessage(dwDelayTime, this, MSG_RECONNECT_READER, new CReconnectReaderParam(nIndex) );
	}
	return 0;
}

int   CBusiness::ReconnectReader(const CReconnectReaderParam * pParam) {
	int nIndex = pParam->m_nIndex;
	assert(nIndex >= 0 && nIndex < MYCHART_COUNT);
	return m_reader[nIndex].Reconnect();
}

// 通知界面连接结果
int   CBusiness::NotifyUiReaderStatus(int nIndex, CTelemedReader::READER_STATUS eStatus) {
	::PostMessage(g_hWnd, UM_SHOW_READER_STATUS, eStatus, nIndex);
	return 0;
}

// 获取状态
CTelemedReader::READER_STATUS  CBusiness::GetReaderStatus(int nIndex) {
	assert(nIndex >= 0 && nIndex < MYCHART_COUNT);
	return m_reader[nIndex].GetStatus();
}

// 获取端口号
const char * CBusiness::GetReaderComPort(int nIndex) {
	assert(nIndex >= 0 && nIndex < MYCHART_COUNT);
	return m_reader[nIndex].m_szComPort;
}

// 读取Tag温度
int   CBusiness::ReadTagTempAsyn(int nIndex, DWORD dwDelayTime /*= 0*/) {
	assert(nIndex >= 0 && nIndex < MYCHART_COUNT);

	if (0 == dwDelayTime) {
		g_thrd_reader[nIndex]->PostMessage(this, MSG_GET_TAG_TEMP, new CReadTempParam(nIndex) );
	}
	else {
		g_thrd_reader[nIndex]->PostDelayMessage(dwDelayTime, this, MSG_GET_TAG_TEMP, new CReadTempParam(nIndex) );
	}

	return 0;
}

int   CBusiness::ReadTagTemp(const CReadTempParam * pParam) {
	int nIndex = pParam->m_nIndex;
	assert(nIndex >= 0 && nIndex < MYCHART_COUNT);

	DWORD  dwTemp = 0;
	int ret = m_reader[nIndex].ReadTagTemp(dwTemp);
	NotifyUiReadTagTemp(nIndex, ret, dwTemp);
	return 0;
}

int   CBusiness::NotifyUiReadTagTemp(int nIndex, int ret, DWORD dwTemp) {
	assert(nIndex >= 0 && nIndex < MYCHART_COUNT);

	::PostMessage(g_hWnd, UM_SHOW_READ_TAG_TEMP_RET, ret, ( nIndex << 16 ) | (dwTemp & 0xFFFF) );
	return 0;
}

// 消息处理
void CBusiness::OnMessage(DWORD dwMessageId, const  LmnToolkits::MessageData * pMessageData) {
	switch (dwMessageId)
	{
	case MSG_RECONNECT_READER:
	{
		CReconnectReaderParam * pParam = (CReconnectReaderParam*)pMessageData;
		ReconnectReader(pParam);
	}
	break;

	case MSG_GET_TAG_TEMP:
	{
		CReadTempParam * pParam = (CReadTempParam*)pMessageData;
		ReadTagTemp(pParam);
	}
	break;

	default:
		break;
	}
}