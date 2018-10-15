#include "common.h"
#include "launcher.h"


CLaunch::CLaunch() {

}

CLaunch::~CLaunch() {

}

void  CLaunch::CloseLaunch() {
	CloseUartPort();
	m_recv_buf.Clear();
	m_sigStatus.emit(GetStatus());
}

// 重连
int  CLaunch::Reconnect() {
	g_data.m_log->Output(ILog::LOG_SEVERITY_INFO, "launch reconnect \n");

	if (GetStatus() == CLmnSerialPort::OPEN) {
		CloseLaunch();
	}

	DWORD  dwCfgLaunchPort = 0;
	g_data.m_cfg->GetConfig("launch port", dwCfgLaunchPort, 0);

	char  szComPort[256] = { 0 };
	if (dwCfgLaunchPort != 0) {
		SNPRINTF(szComPort, sizeof(szComPort), "com%lu", dwCfgLaunchPort);
	}
	else {
		int nFindCount = GetCh340Count(szComPort, sizeof(szComPort));

		// 太多的ch340
		if (nFindCount > 1) {
			m_sigReconnect.emit(RECONNECT_LAUNCH_TIME_INTERVAL);
			return 0;
		}
		// 没有找到ch340
		else if (nFindCount == 0) {
			m_sigReconnect.emit(RECONNECT_LAUNCH_TIME_INTERVAL);
			return 0;
		}
	}

	BOOL bRet = OpenUartPort(szComPort);
	if (!bRet) {
		m_sigReconnect.emit(RECONNECT_LAUNCH_TIME_INTERVAL);
		return 0;
	}

	// 通知成功
	m_sigStatus.emit(GetStatus());
	m_recv_buf.Clear();

	return 0;
}