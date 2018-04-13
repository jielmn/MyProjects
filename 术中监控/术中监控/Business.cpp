#include "exhCommon.h"
#include "Business.h"


CBusiness * CBusiness::pInstance = 0;

CBusiness *  CBusiness::GetInstance() {
	if (0 == pInstance) {
		pInstance = new CBusiness;
	}
	return pInstance;
}

CBusiness::CBusiness() : m_reader(this)  {
	
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

	char buf[8192];
	// 获取Reader协议命令
	g_cfg->GetConfig("prepare command", buf, sizeof(buf), "DD 11 EF 09 00 01 00 D7 B1");
	TransferReaderCmd(PREPARE_COMMAND, buf);

	g_cfg->GetConfig("inventory command", buf, sizeof(buf), "DD 11 EF 09 00 01 00 D7 B1");
	TransferReaderCmd(INVENTORY_COMMAND, buf);

	g_cfg->GetConfig("read tag data command", buf, sizeof(buf), " DD 11 EF 0B 00 23 00 7F 01 17 24");
	TransferReaderCmd(READ_TAG_DATA_COMMAND, buf);
	

	g_cfg->GetConfig("serial port sleep time", SERIAL_PORT_SLEEP_TIME, 500 );

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

	Clear();

	return 0;
}

// 重连Reader
int   CBusiness::ReconnectReaderAsyn(DWORD dwDelayTime /*= 0*/) {
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
int   CBusiness::NotifyUiReaderStatus(CReader::READER_STATUS eStatus) {
	::PostMessage(g_hWnd, UM_SHOW_READER_STATUS, eStatus, 0);
	return 0;
}

// 获取状态
CReader::READER_STATUS  CBusiness::GetReaderStatus() {
	return m_reader.GetStatus();
}

int   CBusiness::GetTagTempAsyn() {
	g_thrd_reader->PostMessage(this, MSG_GET_TAG_TEMP);
	return 0;
}

int   CBusiness::GetTagTemp() {
	int ret = m_reader.GetTagTemperatureData();
	return 0;
}

int   CBusiness::NotifyUiTagTemp() {
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
		GetTagTemp();
	}
	break;

	default:
		break;
	}
}