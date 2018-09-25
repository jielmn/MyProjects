#include "Business.h"

#define RECONNECT_READER_TIME          10000
#define READER_HEARTBEAT_TIME          60000

CBusiness * CBusiness::pInstance = 0;

CBusiness *  CBusiness::GetInstance() {
	if (0 == pInstance) {
		pInstance = new CBusiness;
	}
	return pInstance;
}

CBusiness::CBusiness() {
	m_Reader.m_sigStatusChange.connect(this, &CBusiness::OnReaderStatusChange);
}

CBusiness::~CBusiness() {
	Clear();
}



int CBusiness::Init() {
	g_log = new FileLog();
	if (0 == g_log) {
		return -1;
	}
	g_log->Init("ReaderTool.log");

	g_cfg = new FileConfigEx();
	if (0 == g_cfg) {
		return -1;
	}
	g_cfg->Init("ReaderTool.cfg");

	g_cfg->GetConfig("reader version", g_dwReaderVersion, 3);

	m_thrd_reader.Start();

	ReconnectReaderAsyn(200);

	return 0;
}

int CBusiness::DeInit() {

	Clear();
	return 0;
}

void CBusiness::Clear() {

}

// 重连Reader
int  CBusiness::ReconnectReaderAsyn(DWORD  dwRelayTime /*= 0*/) {
	if (0 == dwRelayTime) {
		m_thrd_reader.PostMessage(this, MSG_RECONNECT_READER);
	}
	else {
		m_thrd_reader.PostDelayMessage(dwRelayTime, this, MSG_RECONNECT_READER);
	}
	return 0;
}

int  CBusiness::ReconnectReader() {
	int ret = m_Reader.Reconnect();
	if (0 != ret) {
		ReconnectReaderAsyn(RECONNECT_READER_TIME);
	}
	return 0;
}

void CBusiness::OnReaderStatusChange(CTelemedReader::TELEMED_READER_STATUS eNewStatus) {
	m_sigReaderStatusChange.emit(eNewStatus);
	if (eNewStatus == CTelemedReader::STATUS_CLOSE) {
		ReconnectReaderAsyn(RECONNECT_READER_TIME);
	}
	// 如果连接成功，则定时心跳检测
	else {
		ReaderHeatbeatAsyn(READER_HEARTBEAT_TIME);
	}
}

// 心跳检测
int  CBusiness::ReaderHeatbeatAsyn(DWORD  dwRelayTime /*= 0*/) {
	if (0 == dwRelayTime) {
		m_thrd_reader.PostMessage(this, MSG_READER_HEARTBEAT);
	}
	else {
		m_thrd_reader.PostDelayMessage(dwRelayTime, this, MSG_READER_HEARTBEAT);
	}
	return 0;
}

int  CBusiness::ReaderHeatbeat() {
	int ret = m_Reader.Heartbeat();
	// 如果心跳检测成功，开启下一次心跳
	if (0 == ret) {
		ReaderHeatbeatAsyn(READER_HEARTBEAT_TIME);
	}
	return 0;
}

// 设置Reader Id
int  CBusiness::SetReaderIdAsyn(int nId, int nVersion) {
	m_thrd_reader.PostMessage(this, MSG_SET_READER_ID, new CReaderIdParam(nId, nVersion) );
	return 0;
}

int  CBusiness::SetReaderId( const CReaderIdParam * pParam ) {
	int ret = m_Reader.SetReaderId(pParam);
	m_sigSetReaderIdRet(ret);
	return 0;
}

// 设置Reader time
int  CBusiness::SetReaderTimeAsyn(time_t tTime) {
	m_thrd_reader.PostMessage(this, MSG_SET_READER_TIME, new CReaderTimeParam(tTime));
	return 0;
}

int  CBusiness::SetReaderTime(const CReaderTimeParam * pParam) {
	int ret = m_Reader.SetReaderTime(pParam);
	m_sigSetReaderTimeRet.emit(ret);
	return 0;
}

// 设置Reader mode
int  CBusiness::SetReaderModeAsyn(int nMode) {
	m_thrd_reader.PostMessage(this, MSG_SET_READER_MODE, new CReaderModeParam(nMode));
	return 0;
}

int  CBusiness::SetReaderMode(const CReaderModeParam * pParam) {
	int ret = m_Reader.SetReaderMode(pParam);
	m_sigSetReaderModeRet.emit(ret);
	return 0;
}

// 清空Reader
int  CBusiness::ClearReaderAsyn() {
	m_thrd_reader.PostMessage(this, MSG_CLEAR_READER );
	return 0;
}

int  CBusiness::ClearReader() {
	int ret = m_Reader.ClearReader();
	m_sigClearReaderRet.emit(ret);
	return 0;
}

// 获取Reader数据
int  CBusiness::GetReaderDataAsyn() {
	m_thrd_reader.PostMessage(this, MSG_GET_READER_DATA);
	return 0;
}

int  CBusiness::GetReaderData() {
	std::vector<TempItem* > vRet;
	int ret = m_Reader.GetReaderData( vRet );
	m_sigGetReaderDataRet.emit(ret,vRet);

	ClearVector(vRet);
	return 0;
}

int CBusiness::SetReaderBluetoothAsyn(BOOL bEnable) {
	m_thrd_reader.PostMessage(this, MSG_SET_READER_BLUE_TOOTH, new CReaderBlueToothParam(bEnable));
	return 0;
}

int CBusiness::SetReaderBluetooth(const CReaderBlueToothParam * pParam) {
	int ret = m_Reader.SetReaderBluetooth(pParam);
	m_sigSetReaderBlueToothRet.emit(ret);
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

	case MSG_READER_HEARTBEAT:
	{
		ReaderHeatbeat();
	}
	break;

	case MSG_SET_READER_ID:
	{
		CReaderIdParam * pParam = (CReaderIdParam *)pMessageData;
		SetReaderId( pParam );
	}
	break;

	case MSG_SET_READER_TIME:
	{
		CReaderTimeParam * pParam = (CReaderTimeParam *)pMessageData;
		SetReaderTime(pParam);
	}
	break;

	case MSG_SET_READER_MODE:
	{
		CReaderModeParam * pParam = (CReaderModeParam *)pMessageData;
		SetReaderMode(pParam);
	}
	break;

	case MSG_CLEAR_READER:
	{
		ClearReader();
	}
	break;

	case MSG_GET_READER_DATA:
	{
		GetReaderData();
	}
	break;

	case MSG_SET_READER_BLUE_TOOTH:
	{
		CReaderBlueToothParam * pParam = (CReaderBlueToothParam *)pMessageData;
		SetReaderBluetooth(pParam);
	}
	break;

	default:
		break;
	}
}