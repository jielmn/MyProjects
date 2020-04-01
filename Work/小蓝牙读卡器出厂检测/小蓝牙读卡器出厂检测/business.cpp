#include "business.h"

CBusiness * CBusiness::pInstance = 0;

CBusiness *  CBusiness::GetInstance() {
	if (0 == pInstance) {
		pInstance = new CBusiness;
	}
	return pInstance;
}

CBusiness::CBusiness() {

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

	g_data.m_thrd_db = new LmnToolkits::Thread();
	if (0 == g_data.m_thrd_db) {
		return -1;
	}
	g_data.m_thrd_db->Start();

	g_data.m_thrd_com = new LmnToolkits::Thread();
	if (0 == g_data.m_thrd_com) {
		return -1;
	}
	g_data.m_thrd_com->Start();

	return 0;
}

int CBusiness::DeInit() {

	if (g_data.m_thrd_db) {
		g_data.m_thrd_db->Stop();
		delete g_data.m_thrd_db;
		g_data.m_thrd_db = 0;
	}

	if (g_data.m_thrd_com) {
		g_data.m_thrd_com->Stop();
		delete g_data.m_thrd_com;
		g_data.m_thrd_com = 0;
	}

	Clear();
	return 0;
}

void  CBusiness::CheckDevicesAsyn() {
	g_data.m_thrd_com->PostMessage(this, MSG_CHECK_DEVICES);
}

void  CBusiness::CheckDevices() {
	// 如果是串口打开状态
	if ( m_com.GetStatus() == CLmnSerialPort::OPEN ) {
		std::map<int, std::string> m;
		GetAllSerialPorts(m);

		std::map<int, std::string>::iterator it = m.find( m_com.GetPort() );
		// 如果串口已经不存在
		if ( it == m.end() ) {
			m_com.CloseUartPort();
			::PostMessage(g_data.m_hWnd, UM_COM_STATUS, 0, 0);
		}
	}
	// 如果是串口关闭状态
	else {
		std::map<int, std::string> m;
		GetAllSerialPorts(m);
		DWORD  dwLen = strlen(COM_STRING);

		std::map<int, std::string>::iterator it;
		std::map<int, std::string>::iterator it_find;
		int nFindCount = 0;

		for (it = m.begin(); it != m.end(); ++it) {
			std::string & s = it->second;
			if ( 0 == StrNiCmp( COM_STRING, s.c_str(), dwLen ) ) {
				it_find = it;
				nFindCount++;
			}
		}

		if ( nFindCount == 0 ) {
			::PostMessage(g_data.m_hWnd, UM_COM_STATUS, 0, 0);
		}
		else if (nFindCount > 1) {
			::PostMessage(g_data.m_hWnd, UM_COM_STATUS, -1, 0);
		}
		// 如果找到了1个蓝牙模块
		else {
			BOOL bRet = m_com.OpenUartPort(it_find->first);
			::PostMessage(g_data.m_hWnd, UM_COM_STATUS, it_find->first, bRet);
		}
	}
}

void  CBusiness::StartAutoTestAsyn(const char * szMac) {

}

void  CBusiness::StartAutoTest() {

}

void  CBusiness::StopAutoTestAsyn() {

}

void  CBusiness::StopAutoTest() {

}



// 消息处理
void CBusiness::OnMessage(DWORD dwMessageId, const  LmnToolkits::MessageData * pMessageData) {
	switch (dwMessageId)
	{
	case MSG_CHECK_DEVICES:
	{
		CheckDevices();
	}
	break;

	default:
		break;
	}
}