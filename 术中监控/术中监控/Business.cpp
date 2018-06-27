#include <assert.h>
#include "exhCommon.h"
#include "Business.h"
#include "UIlib.h"

CBusiness * CBusiness::pInstance = 0;

CBusiness *  CBusiness::GetInstance() {
	if (0 == pInstance) {
		pInstance = new CBusiness;
	}
	return pInstance;
}

CBusiness::CBusiness() : m_reader(this)  {
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
}

int CBusiness::Init() {
	
	g_log = new FileLog();
	if (0 == g_log) {
		return -1;
	}
	g_log->Init("exhibition.log");

	g_cfg = new FileConfig();
	if (0 == g_cfg) {
		return -1;
	}
	g_cfg->Init("exhibition.cfg");

	g_thrd_reader = new LmnToolkits::Thread();
	if (0 == g_thrd_reader) {
		return -1;
	}
	g_thrd_reader->Start();

	// timer线程
	g_thrd_timer = new LmnToolkits::Thread();
	if (0 == g_thrd_timer) {
		return -1;
	}
	g_thrd_timer->Start();

	// 后台进程
	g_thrd_background = new LmnToolkits::Thread();
	if (0 == g_thrd_background) {
		return -1;
	}
	g_thrd_background->Start();

	
	char buf[8192];
	// 获取Reader协议命令
	g_cfg->GetConfig("prepare command", buf, sizeof(buf), "55 01 01 DD AA");
	TransferReaderCmd(PREPARE_COMMAND, buf);

	g_cfg->GetConfig("read tag data command", buf, sizeof(buf), "55 01 02 DD AA");
	TransferReaderCmd(READ_TAG_DATA_COMMAND, buf);
	
	g_cfg->GetConfig("serial port sleep time", SERIAL_PORT_SLEEP_TIME, 30000 );
	if ( SERIAL_PORT_SLEEP_TIME < 30000 ) {
		 SERIAL_PORT_SLEEP_TIME = 30000;
	}

	// 采集间隔秒
	g_cfg->GetConfig("collect interval", g_dwCollectInterval, 10);

	// 低温报警
	g_cfg->GetConfig("low alarm", g_dwLowTempAlarm, 3500);
	// 高温报警
	g_cfg->GetConfig("high alarm", g_dwHighTempAlarm, 4000);

	g_cfg->GetConfig("min temperature degree", g_dwMinTemp, 32);

	//g_dwLowTempAlarm    = 3500;               // 低温报警，单位1/100摄氏度
	//g_dwHighTempAlarm   = 4000;               // 高温报警，单位1/100摄氏度
	if (g_dwLowTempAlarm < 2000 || g_dwLowTempAlarm > 4200) {
		g_dwLowTempAlarm = 3000;
	}

	if (g_dwHighTempAlarm < 2000 || g_dwHighTempAlarm > 4200) {
		g_dwHighTempAlarm = 4000;
	}

	if (g_dwHighTempAlarm < g_dwLowTempAlarm) {
		g_dwHighTempAlarm = g_dwLowTempAlarm;
	}

	if (g_dwMinTemp < 24) {
		g_dwMinTemp = 20;
	}
	else if (g_dwMinTemp < 28) {
		g_dwMinTemp = 24;
	}
	else if (g_dwMinTemp < 32) {
		g_dwMinTemp = 28;
	}
	else {
		g_dwMinTemp = 32;
	}

	if ((g_dwMinTemp % 2) != 0) {
		g_dwMinTemp--;
	}

	g_cfg->GetConfig("alarm file", buf, sizeof(buf), "");	
	if (buf[0] == '\0') {
		GetModuleFileName(0, buf, sizeof(buf));
		const char * pStr = strrchr(buf, '\\');
		assert(pStr);
		DWORD  dwTemp = pStr - buf;
		memcpy(g_szAlarmFilePath, buf, dwTemp);
		memcpy(g_szAlarmFilePath + dwTemp, DEFAULT_ALARM_FILE_PATH, strlen(DEFAULT_ALARM_FILE_PATH));
	}
	else {
		if ( -1 != GetFileAttributes(buf) )    
		{
			strncpy_s(g_szAlarmFilePath, buf, sizeof(g_szAlarmFilePath));
		}
		else {
			GetModuleFileName(0, buf, sizeof(buf));
			const char * pStr = strrchr(buf, '\\');
			assert(pStr);
			DWORD  dwTemp = pStr - buf;
			memcpy(g_szAlarmFilePath, buf, dwTemp);
			memcpy(g_szAlarmFilePath + dwTemp, DEFAULT_ALARM_FILE_PATH, strlen(DEFAULT_ALARM_FILE_PATH));
		}
	}

	ReconnectReaderAsyn(200);

	return 0;
}

int CBusiness::DeInit() {

	if (g_thrd_reader) {
		g_thrd_reader->Stop();
		delete g_thrd_reader;
		g_thrd_reader = 0;
	}

	if (g_thrd_timer) {
		g_thrd_timer->Stop();
		delete g_thrd_timer;
		g_thrd_timer = 0;
	}

	if (g_thrd_background) {
		g_thrd_background->Stop();
		delete g_thrd_background;
		g_thrd_background = 0;
	}

	Clear();

	return 0;
}

// 重连Reader
int   CBusiness::ReconnectReaderAsyn(DWORD dwDelayTime /*= 0*/) {
	//g_thrd_reader->DeleteMessages();

	if (0 == dwDelayTime) {
		g_thrd_reader->PostMessage(this, MSG_RECONNECT_READER);
	}
	else {
		g_thrd_reader->PostDelayMessage(dwDelayTime, this, MSG_RECONNECT_READER);
	}
	return 0;
}

int   CBusiness::ReconnectReader() {
	return m_reader.Reconnect();
}

// 通知界面连接结果
int   CBusiness::NotifyUiReaderStatus(CTelemedReader::READER_STATUS eStatus) {
	::PostMessage(g_hWnd, UM_SHOW_READER_STATUS, eStatus, 0);
	return 0;
}

// 获取状态
CTelemedReader::READER_STATUS  CBusiness::GetReaderStatus() {
	return m_reader.GetStatus();
}

int   CBusiness::ReadTagTempAsyn(DWORD dwDelayTime /*= 0*/) {
	if (0 == dwDelayTime) {
		g_thrd_reader->PostMessage(this, MSG_GET_TAG_TEMP);
	}
	else {
		g_thrd_reader->PostDelayMessage( dwDelayTime, this, MSG_GET_TAG_TEMP);
	}
	
	return 0;
}

int   CBusiness::ReadTagTemp() {
	DWORD  dwTemp = 0;
	int ret = m_reader.ReadTagTemp(dwTemp);
	NotifyUiReadTagTemp(ret, dwTemp );
	return 0;
}

int   CBusiness::NotifyUiReadTagTemp( int ret, DWORD dwTemp ) {
	::PostMessage(g_hWnd, UM_SHOW_READ_TAG_TEMP_RET, ret, dwTemp);
	return 0;
}

int   CBusiness::AlarmAsyn(const char * szAlarmFile) {
	g_thrd_background->PostMessage(this, MSG_ALARM, new CAlarmParam(szAlarmFile) );
	return 0;
}

int   CBusiness::Alarm(const CAlarmParam * pParam) {
	DuiLib::CDuiString strText;

	if (m_szAlarmFile[0] == '\0') {
		// open
		strText.Format("open %s", pParam->m_szAlarmFile);
		mciSendString( strText, NULL, 0, 0);

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




// 消息处理 
void CBusiness::OnMessage(DWORD dwMessageId, const  LmnToolkits::MessageData * pMessageData) {
	switch (dwMessageId)
	{
	case MSG_RECONNECT_READER:
	{
		ReconnectReader();
	}
	break;

	case MSG_GET_TAG_TEMP:
	{
		ReadTagTemp();
	}
	break;

	case MSG_ALARM:
	{
		CAlarmParam * pParam = (CAlarmParam*)pMessageData;
		Alarm(pParam);
	}
	break;

	default:
		break;
	}
}